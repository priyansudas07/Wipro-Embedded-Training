#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

class CpuMonitor {
public:
    CpuMonitor() = default;
    double getCpuUsage();

private:
    unsigned long long prev_user_ = 0, prev_nice_ = 0, prev_system_ = 0, prev_idle_ = 0;
    unsigned long long prev_iowait_ = 0, prev_irq_ = 0, prev_softirq_ = 0, prev_steal_ = 0;
};

#endif // CPU_MONITOR_H
