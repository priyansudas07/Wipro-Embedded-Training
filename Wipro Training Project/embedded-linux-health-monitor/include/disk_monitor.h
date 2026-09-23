#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

#include <string>

struct DiskStats {
    double total_gb = 0.0;
    double free_gb = 0.0;
    double used_gb = 0.0;
    double used_percent = 0.0;
};

class DiskMonitor {
public:
    DiskMonitor() = default;
    DiskStats getDiskStats(const std::string& mount_point = "/");
};

#endif // DISK_MONITOR_H
