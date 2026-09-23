#include "memory_monitor.h"
#include <sys/sysinfo.h>

MemoryStats MemoryMonitor::getMemoryStats() {
    MemoryStats stats;
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        double unit_mb = 1024.0 * 1024.0;
        stats.total_ram_mb = (double)info.totalram * info.mem_unit / unit_mb;
        stats.free_ram_mb = (double)info.freeram * info.mem_unit / unit_mb;
        stats.used_ram_mb = stats.total_ram_mb - stats.free_ram_mb;
        if (stats.total_ram_mb > 0) {
            stats.used_percent = (stats.used_ram_mb / stats.total_ram_mb) * 100.0;
        }
    }
    return stats;
}
