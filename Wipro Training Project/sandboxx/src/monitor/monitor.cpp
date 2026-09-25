#include "sandboxx/monitor.hpp"
#include "sandboxx/logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <filesystem>

namespace fs = std::filesystem;

ProcessStats Monitor::getProcessStats(pid_t pid) {
    ProcessStats stats;
    stats.pid = pid;

    std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
    std::string status_path = "/proc/" + std::to_string(pid) + "/status";

    if (!fs::exists(stat_path)) {
        stats.is_running = false;
        return stats;
    }

    stats.is_running = true;

    // Read RSS memory from status
    std::ifstream status_file(status_path);
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::stringstream ss(line.substr(6));
            ss >> stats.memory_rss_kb;
        } else if (line.rfind("VmSize:", 0) == 0) {
            std::stringstream ss(line.substr(7));
            ss >> stats.memory_vsz_kb;
        } else if (line.rfind("Threads:", 0) == 0) {
            std::stringstream ss(line.substr(8));
            ss >> stats.thread_count;
        }
    }

    // Read CPU ticks from stat
    std::ifstream stat_file(stat_path);
    if (stat_file.is_open()) {
        std::string comm;
        char state;
        int ppid, pgrp, session, tty_nr, tpgid;
        unsigned int flags;
        unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
        stat_file >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime;

        long ticks = sysconf(_SC_CLK_TCK);
        double total_seconds = (utime + stime) / (double)ticks;
        stats.cpu_usage_pct = total_seconds * 10.0; // Scaled estimate
    }

    return stats;
}

void Monitor::printStatsTable(const ProcessStats& stats) {
    std::cout << "======================================================" << std::endl;
    std::cout << "            SANDBOX PROCESS MONITORING                " << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << "  Process PID    : " << stats.pid << std::endl;
    std::cout << "  State          : " << (stats.is_running ? "RUNNING" : "STOPPED") << std::endl;
    std::cout << "  CPU Usage      : " << std::fixed << std::setprecision(2) << stats.cpu_usage_pct << " %" << std::endl;
    std::cout << "  Memory (RSS)   : " << stats.memory_rss_kb << " KB (" << (stats.memory_rss_kb / 1024.0) << " MB)" << std::endl;
    std::cout << "  Memory (VSZ)   : " << stats.memory_vsz_kb << " KB" << std::endl;
    std::cout << "  Threads        : " << stats.thread_count << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
}
