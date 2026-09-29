#include "vdevpulse/telemetry_monitor.hpp"
#include <iostream>
#include <cassert>

int main() {
    auto metrics = TelemetryMonitor::collectTelemetry();
    assert(metrics.memory_total_mb > 0);
    assert(metrics.memory_used_mb >= 0);
    assert(metrics.uptime_seconds >= 0);

    TelemetryMonitor::printTelemetryDashboard(metrics);

    std::cout << "[TEST TELEMETRY] Linux /proc system telemetry monitor test PASSED." << std::endl;
    return 0;
}
