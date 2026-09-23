#include "config_manager.h"
#include "logger.h"
#include <fstream>
#include <iostream>
#include <sstream>

bool ConfigManager::loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().log(LogLevel::WARNING, "Config file not found: " + filepath + ". Using default thresholds.");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    auto find_double = [&](const std::string& key, double default_val) {
        size_t pos = content.find("\"" + key + "\"");
        if (pos != std::string::npos) {
            size_t colon = content.find(":", pos);
            if (colon != std::string::npos) {
                try {
                    return std::stod(content.substr(colon + 1));
                } catch (...) {}
            }
        }
        return default_val;
    };

    auto find_int = [&](const std::string& key, int default_val) {
        size_t pos = content.find("\"" + key + "\"");
        if (pos != std::string::npos) {
            size_t colon = content.find(":", pos);
            if (colon != std::string::npos) {
                try {
                    return std::stoi(content.substr(colon + 1));
                } catch (...) {}
            }
        }
        return default_val;
    };

    config_.cpu_threshold = find_double("cpu_threshold_pct", 85.0);
    config_.memory_threshold = find_double("memory_threshold_pct", 90.0);
    config_.disk_threshold = find_double("disk_threshold_pct", 90.0);
    config_.temp_threshold = find_double("temperature_threshold_celsius", 75.0);
    config_.poll_interval = find_int("poll_interval_seconds", 2);

    Logger::getInstance().log(LogLevel::INFO, "Loaded configuration from " + filepath);
    return true;
}
