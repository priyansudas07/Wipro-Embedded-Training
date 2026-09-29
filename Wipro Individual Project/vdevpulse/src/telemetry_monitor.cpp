#include "vdevpulse/telemetry_monitor.hpp"
#include "vdevpulse/logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <filesystem>

namespace fs = std::filesystem;

SystemTelemetry TelemetryMonitor::collectTelemetry() {
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

    return t;
}

void TelemetryMonitor::printTelemetryDashboard(const SystemTelemetry& metrics) {
    std::cout << "======================================================" << std::endl;
    std::cout << "        VDEVPULSE SYSTEM TELEMETRY DASHBOARD          " << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << "  CPU Usage        : " << std::fixed << std::setprecision(2) << metrics.cpu_usage_pct << " %" << std::endl;
    std::cout << "  RAM Memory Usage : " << metrics.memory_used_mb << " MB / " << metrics.memory_total_mb << " MB ("
              << std::setprecision(1) << metrics.memory_usage_pct << " %)" << std::endl;
    std::cout << "  System Uptime    : " << metrics.uptime_seconds << " seconds" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
}
