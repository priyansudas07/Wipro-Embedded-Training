#ifndef VDEVPULSE_TELEMETRY_MONITOR_HPP
#define VDEVPULSE_TELEMETRY_MONITOR_HPP

#include "config.hpp"
#include <string>
#include <vector>
#include <deque>

struct SystemTelemetry {
    double cpu_usage_pct = 0.0;
    long memory_total_mb = 0;
    long memory_used_mb = 0;
    long memory_free_mb = 0;
    double memory_usage_pct = 0.0;
    long uptime_seconds = 0;
    double load_1m = 0.0;
    double load_5m = 0.0;
    double load_15m = 0.0;
    int running_processes = 0;
    int total_processes = 0;
    std::string health_status = "HEALTHY";
};

class TelemetryMonitor {
public:
    TelemetryMonitor() = default;

    static SystemTelemetry collectTelemetry(const VDevConfig& config = VDevConfig());
    static void printTelemetryDashboard(const SystemTelemetry& metrics);
    static std::string toJsonString(const SystemTelemetry& metrics);
    static void recordHistory(const SystemTelemetry& metrics);
    static const std::deque<SystemTelemetry>& getHistory();
    static void printHistory();

private:
    static std::deque<SystemTelemetry> history_buffer_;
    static constexpr size_t MAX_HISTORY_SAMPLES = 60;
};

#endif // VDEVPULSE_TELEMETRY_MONITOR_HPP
