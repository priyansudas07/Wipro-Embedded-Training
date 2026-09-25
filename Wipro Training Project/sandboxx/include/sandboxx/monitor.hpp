#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <sys/types.h>

struct ProcessStats {
    pid_t pid = -1;
    double cpu_usage_pct = 0.0;
    long memory_rss_kb = 0;
    long memory_vsz_kb = 0;
    int thread_count = 0;
    bool is_running = false;
};

class Monitor {
public:
    Monitor() = default;

    static ProcessStats getProcessStats(pid_t pid);
    static void printStatsTable(const ProcessStats& stats);
};

#endif // MONITOR_HPP
