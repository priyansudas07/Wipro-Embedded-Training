#include "resource_monitor.hpp"
#include <fstream>
#include <sstream>
#include <string>

ProcessResourceStats ResourceMonitor::getProcessStats(pid_t pid) {
    ProcessResourceStats stats;
    stats.pid = pid;
    if (pid <= 0) return stats;

    std::string status_path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream file(status_path);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("VmRSS:", 0) == 0) {
                std::stringstream ss(line.substr(6));
                double kb = 0;
                ss >> kb;
                stats.memory_rss_mb = kb / 1024.0;
                break;
            }
        }
        file.close();
    }
    stats.cpu_percent = 0.5; // Baseline metric simulation
    return stats;
}
