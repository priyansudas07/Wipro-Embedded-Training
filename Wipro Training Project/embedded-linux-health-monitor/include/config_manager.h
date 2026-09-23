#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <vector>

struct ConfigData {
    double cpu_threshold = 85.0;
    double memory_threshold = 90.0;
    double disk_threshold = 90.0;
    double temp_threshold = 75.0;
    int poll_interval = 2;
    std::string log_file = "health_monitor.log";
    std::vector<std::string> services = {"cron", "dbus", "rsyslog"};
};

class ConfigManager {
public:
    ConfigManager() = default;
    bool loadConfig(const std::string& filepath);
    const ConfigData& getConfig() const { return config_; }

private:
    ConfigData config_;
};

#endif // CONFIG_MANAGER_H
