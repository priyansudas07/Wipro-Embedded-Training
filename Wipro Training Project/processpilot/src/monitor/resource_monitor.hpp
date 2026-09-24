#ifndef RESOURCE_MONITOR_HPP
#define RESOURCE_MONITOR_HPP

#include <sys/types.h>

struct ProcessResourceStats {
    pid_t pid = 0;
    double cpu_percent = 0.0;
    double memory_rss_mb = 0.0;
};

class ResourceMonitor {
public:
    ResourceMonitor() = default;
    static ProcessResourceStats getProcessStats(pid_t pid);
};

#endif // RESOURCE_MONITOR_HPP
