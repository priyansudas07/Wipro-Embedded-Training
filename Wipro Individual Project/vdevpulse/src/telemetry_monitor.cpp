#include "vdevpulse/telemetry_monitor.hpp"
#include "vdevpulse/logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

std::deque<SystemTelemetry> TelemetryMonitor::history_buffer_;

std::vector<ProcessInfo> TelemetryMonitor::getTopProcesses(size_t limit) {
    std::vector<ProcessInfo> procs;

    if (!fs::exists("/proc")) return procs;

    for (const auto& entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory()) continue;

        std::string filename = entry.path().filename().string();
        if (filename.empty() || !std::all_of(filename.begin(), filename.end(), ::isdigit)) {
            continue;
        }

        int pid = 0;
        try {
            pid = std::stoi(filename);
        } catch (...) {
            continue;
        }

        ProcessInfo p;
        p.pid = pid;

        // 1. Read process name from /proc/[pid]/comm
        std::ifstream comm_file(entry.path() / "comm");
        if (comm_file.is_open()) {
            std::getline(comm_file, p.name);
        }

        // 2. Read physical memory RSS from /proc/[pid]/status
        std::ifstream status_file(entry.path() / "status");
        if (status_file.is_open()) {
            std::string line;
            while (std::getline(status_file, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {
                    std::stringstream ss(line.substr(6));
                    long rss_kb = 0;
                    ss >> rss_kb;
                    p.memory_rss_mb = rss_kb / 1024;
                    break;
                }
            }
        }

        if (p.memory_rss_mb > 0) {
            procs.push_back(p);
        }
    }

    // Sort descending by memory consumption
    std::sort(procs.begin(), procs.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.memory_rss_mb > b.memory_rss_mb;
    });

    if (procs.size() > limit) {
        procs.resize(limit);
    }

    return procs;
}

SystemTelemetry TelemetryMonitor::collectTelemetry(const VDevConfig& config) {
    SystemTelemetry t;

    // 1. Read RAM memory from /proc/meminfo
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        long total_kb = 0, free_kb = 0, available_kb = 0;
        while (std::getline(meminfo, line)) {
            if (line.rfind("MemTotal:", 0) == 0) {
                std::stringstream ss(line.substr(9));
                ss >> total_kb;
            } else if (line.rfind("MemFree:", 0) == 0) {
                std::stringstream ss(line.substr(8));
                ss >> free_kb;
            } else if (line.rfind("MemAvailable:", 0) == 0) {
                std::stringstream ss(line.substr(13));
                ss >> available_kb;
            }
        }
        if (total_kb > 0) {
            t.memory_total_mb = total_kb / 1024;
            long usable_free_kb = (available_kb > 0) ? available_kb : free_kb;
            t.memory_free_mb = usable_free_kb / 1024;
            t.memory_used_mb = t.memory_total_mb - t.memory_free_mb;
            t.memory_usage_pct = (static_cast<double>(t.memory_used_mb) / t.memory_total_mb) * 100.0;
        }
    }

    // 2. Read CPU ticks from /proc/stat
    std::ifstream proc_stat("/proc/stat");
    if (proc_stat.is_open()) {
        std::string cpu_label;
        long user, nice, system, idle, iowait, irq, softirq, steal;
        proc_stat >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        long idle_time = idle + iowait;
        long non_idle_time = user + nice + system + irq + softirq + steal;
        long total_time = idle_time + non_idle_time;
        if (total_time > 0) {
            t.cpu_usage_pct = (static_cast<double>(non_idle_time) / total_time) * 100.0;
        }
    }

    // 3. Read System Uptime from /proc/uptime
    std::ifstream proc_uptime("/proc/uptime");
    if (proc_uptime.is_open()) {
        double uptime_sec;
        proc_uptime >> uptime_sec;
        t.uptime_seconds = static_cast<long>(uptime_sec);
    }

    // 4. Read System Load Averages & Process Counts from /proc/loadavg
    std::ifstream proc_loadavg("/proc/loadavg");
    if (proc_loadavg.is_open()) {
        std::string proc_counts;
        proc_loadavg >> t.load_1m >> t.load_5m >> t.load_15m >> proc_counts;
        size_t slash_pos = proc_counts.find('/');
        if (slash_pos != std::string::npos) {
            try {
                t.running_processes = std::stoi(proc_counts.substr(0, slash_pos));
                t.total_processes = std::stoi(proc_counts.substr(slash_pos + 1));
            } catch (...) {}
        }
    }

    // 5. Ingest Top Resource Consuming Processes
    t.top_processes = getTopProcesses(3);

    // 6. Automated Health & Threshold Rule Evaluation
    t.health_status = "HEALTHY";
    if (t.cpu_usage_pct > config.cpu_alert_threshold_pct) {
        t.health_status = "WARNING_CPU_OVERLOAD";
        Logger::getInstance().log(LogLevel::WARNING, "[THRESHOLD ALERT] CPU usage at " +
                                  std::to_string(t.cpu_usage_pct) + "% (Threshold: " +
                                  std::to_string(config.cpu_alert_threshold_pct) + "%)");
    }
    if (t.memory_usage_pct > config.memory_alert_threshold_pct) {
        if (t.health_status == "HEALTHY") {
            t.health_status = "WARNING_MEMORY_PRESSURE";
        } else {
            t.health_status = "CRITICAL_RESOURCE_PRESSURE";
        }
        Logger::getInstance().log(LogLevel::WARNING, "[THRESHOLD ALERT] RAM usage at " +
                                  std::to_string(t.memory_usage_pct) + "% (Threshold: " +
                                  std::to_string(config.memory_alert_threshold_pct) + "%)");
    }

    return t;
}

void TelemetryMonitor::printTelemetryDashboard(const SystemTelemetry& metrics) {
    std::cout << "======================================================" << std::endl;
    std::cout << "        VDEVPULSE SYSTEM TELEMETRY DASHBOARD          " << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << "  System Health    : " << metrics.health_status << std::endl;
    std::cout << "  CPU Usage        : " << std::fixed << std::setprecision(2) << metrics.cpu_usage_pct << " %" << std::endl;
    std::cout << "  RAM Memory Usage : " << metrics.memory_used_mb << " MB / " << metrics.memory_total_mb << " MB ("
              << std::setprecision(1) << metrics.memory_usage_pct << " %)" << std::endl;
    std::cout << "  Load Averages    : " << std::setprecision(2) << metrics.load_1m << " (1m), "
              << metrics.load_5m << " (5m), " << metrics.load_15m << " (15m)" << std::endl;
    std::cout << "  Active Processes : " << metrics.running_processes << " running / " << metrics.total_processes << " total" << std::endl;
    
    if (!metrics.top_processes.empty()) {
        std::cout << "  Top Consumers    :" << std::endl;
        for (size_t i = 0; i < metrics.top_processes.size(); ++i) {
            std::cout << "    " << (i + 1) << ". [PID " << metrics.top_processes[i].pid << "] "
                      << std::left << std::setw(16) << metrics.top_processes[i].name
                      << " (RAM: " << metrics.top_processes[i].memory_rss_mb << " MB)" << std::endl;
        }
    }

    std::cout << "  System Uptime    : " << metrics.uptime_seconds << " seconds" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
}

std::string TelemetryMonitor::toJsonString(const SystemTelemetry& metrics) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"health_status\": \"" << metrics.health_status << "\",\n";
    ss << "  \"cpu_usage_pct\": " << std::fixed << std::setprecision(2) << metrics.cpu_usage_pct << ",\n";
    ss << "  \"memory_used_mb\": " << metrics.memory_used_mb << ",\n";
    ss << "  \"memory_total_mb\": " << metrics.memory_total_mb << ",\n";
    ss << "  \"memory_usage_pct\": " << std::fixed << std::setprecision(2) << metrics.memory_usage_pct << ",\n";
    ss << "  \"load_1m\": " << std::fixed << std::setprecision(2) << metrics.load_1m << ",\n";
    ss << "  \"load_5m\": " << std::fixed << std::setprecision(2) << metrics.load_5m << ",\n";
    ss << "  \"load_15m\": " << std::fixed << std::setprecision(2) << metrics.load_15m << ",\n";
    ss << "  \"running_processes\": " << metrics.running_processes << ",\n";
    ss << "  \"total_processes\": " << metrics.total_processes << ",\n";
    ss << "  \"uptime_seconds\": " << metrics.uptime_seconds << ",\n";
    ss << "  \"top_processes\": [\n";
    for (size_t i = 0; i < metrics.top_processes.size(); ++i) {
        ss << "    {\"pid\": " << metrics.top_processes[i].pid
           << ", \"name\": \"" << metrics.top_processes[i].name << "\""
           << ", \"memory_rss_mb\": " << metrics.top_processes[i].memory_rss_mb << "}"
           << (i + 1 < metrics.top_processes.size() ? ",\n" : "\n");
    }
    ss << "  ]\n";
    ss << "}";
    return ss.str();
}

void TelemetryMonitor::recordHistory(const SystemTelemetry& metrics) {
    if (history_buffer_.size() >= MAX_HISTORY_SAMPLES) {
        history_buffer_.pop_front();
    }
    history_buffer_.push_back(metrics);
}

const std::deque<SystemTelemetry>& TelemetryMonitor::getHistory() {
    return history_buffer_;
}

void TelemetryMonitor::printHistory() {
    std::cout << "======================================================" << std::endl;
    std::cout << "      VDEVPULSE HISTORICAL TELEMETRY BUFFER           " << std::endl;
    std::cout << "======================================================" << std::endl;
    if (history_buffer_.empty()) {
        std::cout << "  No historical records accumulated yet." << std::endl;
        std::cout << "------------------------------------------------------" << std::endl;
        return;
    }

    std::cout << std::left << std::setw(8) << "Sample"
              << std::setw(12) << "CPU (%)"
              << std::setw(16) << "RAM (MB/%)"
              << std::setw(12) << "Load (1m)"
              << std::setw(12) << "Health" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    int idx = 1;
    for (const auto& h : history_buffer_) {
        std::string ram_str = std::to_string(h.memory_used_mb) + "M/" + std::to_string(static_cast<int>(h.memory_usage_pct)) + "%";
        std::cout << std::left << std::setw(8) << idx++
                  << std::setw(12) << std::fixed << std::setprecision(1) << h.cpu_usage_pct
                  << std::setw(16) << ram_str
                  << std::setw(12) << std::setprecision(2) << h.load_1m
                  << std::setw(12) << h.health_status << std::endl;
    }
    std::cout << "------------------------------------------------------" << std::endl;
}
