#include "disk_monitor.h"
#include <sys/statvfs.h>

DiskStats DiskMonitor::getDiskStats(const std::string& mount_point) {
    DiskStats stats;
    struct statvfs stat;
    if (statvfs(mount_point.c_str(), &stat) == 0) {
        double unit_gb = 1024.0 * 1024.0 * 1024.0;
        stats.total_gb = (double)stat.f_blocks * stat.f_frsize / unit_gb;
        stats.free_gb = (double)stat.f_bavail * stat.f_frsize / unit_gb;
        stats.used_gb = stats.total_gb - stats.free_gb;
        if (stats.total_gb > 0) {
            stats.used_percent = (stats.used_gb / stats.total_gb) * 100.0;
        }
    }
    return stats;
}
