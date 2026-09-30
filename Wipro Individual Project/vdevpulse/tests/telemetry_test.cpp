#include "vdevpulse/telemetry_monitor.hpp"
#include <iostream>
#include <cassert>

int main() {
    VDevConfig cfg;
    auto metrics = TelemetryMonitor::collectTelemetry(cfg);
    assert(metrics.memory_total_mb > 0);
    assert(metrics.memory_used_mb >= 0);
    assert(metrics.uptime_seconds >= 0);
    assert(metrics.load_1m >= 0.0);
    assert(metrics.total_processes >= 0);
    assert(!metrics.health_status.empty());

    // Verify JSON serialization
    std::string json = TelemetryMonitor::toJsonString(metrics);
    assert(json.find("\"health_status\"") != std::string::npos);
    assert(json.find("\"load_1m\"") != std::string::npos);

    // Verify History Ring Buffer
    TelemetryMonitor::recordHistory(metrics);
    assert(!TelemetryMonitor::getHistory().empty());

    TelemetryMonitor::printTelemetryDashboard(metrics);

    std::cout << "[TEST TELEMETRY] Linux /proc telemetry, loadavg & JSON export test PASSED." << std::endl;
    return 0;
}
