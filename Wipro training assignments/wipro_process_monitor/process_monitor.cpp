#include "process_monitor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

void ProcessMonitor::printBanner() {
    std::cout << "======================================================================\n";
    std::cout << "            LINUX PROCESS MONITOR TOOL (Wipro COE)                   \n";
    std::cout << "======================================================================\n";
}

bool ProcessMonitor::readProcessInfo(int pid, ProcessInfo& info) {
    std::string status_path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream file(status_path);
    if (!file.is_open()) return false;

    info.pid = pid;
    info.name = "Unknown";
    info.state = "U";
    info.ppid = 0;

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("Name:", 0) == 0) {
            std::string val = line.substr(5);
            size_t first = val.find_first_not_of(" \t");
            if (first != std::string::npos) val = val.substr(first);
            info.name = val;
        } else if (line.rfind("State:", 0) == 0) {
            std::string val = line.substr(6);
            size_t first = val.find_first_not_of(" \t");
            if (first != std::string::npos) {
                info.state = std::string(1, val[first]);
            }
        } else if (line.rfind("PPid:", 0) == 0) {
            std::string val = line.substr(5);
            info.ppid = std::stoi(val);
        }
    }
    file.close();
    return true;
}

void ProcessMonitor::listAllProcesses() {
    std::vector<ProcessInfo> processes;

    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            std::string dirname = entry.path().filename().string();
            bool is_num = !dirname.empty() && std::all_of(dirname.begin(), dirname.end(), ::isdigit);
            if (is_num) {
                int pid = std::stoi(dirname);
                ProcessInfo info;
                if (readProcessInfo(pid, info)) {
                    processes.push_back(info);
                }
            }
        }
    } catch (...) {}

    std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.pid < b.pid;
    });

    std::cout << std::left << std::setw(10) << "PID"
              << std::setw(30) << "Process Name"
              << std::setw(10) << "State"
              << std::setw(10) << "PPID" << "\n";
    std::cout << "----------------------------------------------------------------------\n";

    for (const auto& proc : processes) {
        std::cout << std::left << std::setw(10) << proc.pid
                  << std::setw(30) << proc.name
                  << std::setw(10) << proc.state
                  << std::setw(10) << proc.ppid << "\n";
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Total Running Processes Found: " << processes.size() << "\n";
}

void ProcessMonitor::searchProcess(const std::string& query) {
    int query_pid = 0;
    try { query_pid = std::stoi(query); } catch (...) {}

    int found_count = 0;

    std::cout << "\n" << std::left << std::setw(10) << "PID"
              << std::setw(30) << "Process Name"
              << std::setw(10) << "State"
              << std::setw(10) << "PPID" << "\n";
    std::cout << "----------------------------------------------------------------------\n";

    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            std::string dirname = entry.path().filename().string();
            bool is_num = !dirname.empty() && std::all_of(dirname.begin(), dirname.end(), ::isdigit);
            if (is_num) {
                int pid = std::stoi(dirname);
                ProcessInfo info;
                if (readProcessInfo(pid, info)) {
                    std::string lower_name = info.name;
                    std::string lower_query = query;
                    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
                    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);

                    if ((query_pid > 0 && info.pid == query_pid) || (lower_name.find(lower_query) != std::string::npos)) {
                        std::cout << std::left << std::setw(10) << info.pid
                                  << std::setw(30) << info.name
                                  << std::setw(10) << info.state
                                  << std::setw(10) << info.ppid << "\n";
                        found_count++;
                    }
                }
            }
        }
    } catch (...) {}

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Found " << found_count << " matching process(es) for query: '" << query << "'\n";
}

void ProcessMonitor::showProcessStats() {
    int total = 0, running = 0, sleeping = 0, stopped = 0, zombie = 0;

    try {
        for (const auto& entry : fs::directory_iterator("/proc")) {
            std::string dirname = entry.path().filename().string();
            bool is_num = !dirname.empty() && std::all_of(dirname.begin(), dirname.end(), ::isdigit);
            if (is_num) {
                int pid = std::stoi(dirname);
                ProcessInfo info;
                if (readProcessInfo(pid, info)) {
                    total++;
                    if (info.state == "R") running++;
                    else if (info.state == "S" || info.state == "I") sleeping++;
                    else if (info.state == "Z") zombie++;
                    else if (info.state == "T") stopped++;
                }
            }
        }
    } catch (...) {}

    std::cout << "\n=== PROCESS STATISTICS SUMMARY ===\n";
    std::cout << "  Total Active Processes : " << total << "\n";
    std::cout << "  Running (R)            : " << running << "\n";
    std::cout << "  Sleeping (S/I)        : " << sleeping << "\n";
    std::cout << "  Stopped (T)           : " << stopped << "\n";
    std::cout << "  Zombie (Z)            : " << zombie << "\n";
}
