#ifndef VDEVPULSE_CONFIG_HPP
#define VDEVPULSE_CONFIG_HPP

#include <string>

struct VDevConfig {
    std::string device_name = "vdevpulse";
    std::string device_path = "/tmp/vdevpulse";
    int sampling_rate_ms = 1000;
    bool enable_cpu_telemetry = true;
    bool enable_memory_telemetry = true;
    long max_memory_threshold_mb = 4096;
};

class ConfigParser {
public:
    static bool loadPolicy(const std::string& filepath, VDevConfig& config);
};

#endif // VDEVPULSE_CONFIG_HPP
