#include "cpu_monitor.h"
#include <fstream>
#include <string>
#include <sstream>

double CpuMonitor::getCpuUsage() {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) return 0.0;

    std::string line;
    std::getline(file, line);
    file.close();

    if (line.compare(0, 3, "cpu") != 0) return 0.0;

    std::stringstream ss(line);
    std::string cpu_label;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    unsigned long long prev_idle_total = prev_idle_ + prev_iowait_;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user_ + prev_nice_ + prev_system_ + prev_irq_ + prev_softirq_ + prev_steal_;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    unsigned long long total_d = total - prev_total;
    unsigned long long idle_d = idle_total - prev_idle_total;

    prev_user_ = user; prev_nice_ = nice; prev_system_ = system; prev_idle_ = idle;
    prev_iowait_ = iowait; prev_irq_ = irq; prev_softirq_ = softirq; prev_steal_ = steal;

    if (total_d == 0) return 0.0;

    return (static_cast<double>(total_d - idle_d) / total_d) * 100.0;
}
