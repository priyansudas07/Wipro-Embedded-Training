#ifndef VDEVPULSE_CONFIG_HPP
#define VDEVPULSE_CONFIG_HPP

#include <string>

struct VDevConfig {
    std::string device_name = "vdevpulse";
    std::string device_path = "/tmp/vdevpulse";
    int sampling_rate_ms = 1000;
    bool enable_cpu_telemetry = true;
    bool enable_memory_telemetry = true;
    double cpu_alert_threshold_pct = 85.0;
    double memory_alert_threshold_pct = 90.0;
    long max_memory_threshold_mb = 4096;
    std::string output_format = "text"; // "text" or "json"
};

class ConfigParser {
public:
    static bool loadPolicy(const std::string& filepath, VDevConfig& config);
};

#endif // VDEVPULSE_CONFIG_HPP
