#ifndef VDEVPULSE_TELEMETRY_MONITOR_HPP
#define VDEVPULSE_TELEMETRY_MONITOR_HPP

#include <string>

struct SystemTelemetry {
    double cpu_usage_pct = 0.0;
    long memory_total_mb = 0;
    long memory_used_mb = 0;
    long memory_free_mb = 0;
    double memory_usage_pct = 0.0;
    long uptime_seconds = 0;
};

class TelemetryMonitor {
public:
    TelemetryMonitor() = default;

    static SystemTelemetry collectTelemetry();
    static void printTelemetryDashboard(const SystemTelemetry& metrics);
};

#endif // VDEVPULSE_TELEMETRY_MONITOR_HPP
