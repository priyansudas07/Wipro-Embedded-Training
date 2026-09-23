#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

struct MemoryStats {
    double total_ram_mb = 0.0;
    double used_ram_mb = 0.0;
    double free_ram_mb = 0.0;
    double used_percent = 0.0;
};

class MemoryMonitor {
public:
    MemoryMonitor() = default;
    MemoryStats getMemoryStats();
};

#endif // MEMORY_MONITOR_H
