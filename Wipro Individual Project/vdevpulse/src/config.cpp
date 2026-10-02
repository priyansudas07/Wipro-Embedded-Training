#include "vdevpulse/config.hpp"
#include "vdevpulse/logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

bool ConfigParser::loadPolicy(const std::string& filepath, VDevConfig& config) {
    std::string actual_path = filepath;
    if (!fs::exists(actual_path)) {
        std::string fallback = "../" + filepath;
        if (fs::exists(fallback)) {
            actual_path = fallback;
        } else {
            Logger::getInstance().log(LogLevel::WARNING, "Policy configuration file not found: " + filepath + ", using default configuration.");
            return true;
        }
    }

    std::ifstream file(actual_path);
    if (!file.is_open()) return false;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto extractString = [&content](const std::string& key, const std::string& default_val) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos != std::string::npos) {
            size_t colon = content.find(':', pos);
            if (colon != std::string::npos) {
                size_t start = content.find_first_not_of(" \t\n\r\"", colon + 1);
                size_t end = content.find_first_of(",}\n\r\"", start);
                if (start != std::string::npos && end != std::string::npos) {
                    return content.substr(start, end - start);
                }
            }
        }
        return default_val;
    };

    auto extractDouble = [&extractString](const std::string& key, double default_val) -> double {
        std::string val = extractString(key, "");
        if (!val.empty()) {
            try {
                return std::stod(val);
            } catch (...) {}
        }
        return default_val;
    };

    auto extractInt = [&extractString](const std::string& key, int default_val) -> int {
        std::string val = extractString(key, "");
        if (!val.empty()) {
            try {
                return std::stoi(val);
            } catch (...) {}
        }
        return default_val;
    };

    config.device_name = extractString("device_name", config.device_name);
    config.device_path = extractString("device_path", config.device_path);
    config.sampling_rate_ms = extractInt("sampling_rate_ms", config.sampling_rate_ms);
    config.cpu_alert_threshold_pct = extractDouble("cpu_alert_threshold_pct", config.cpu_alert_threshold_pct);
    config.memory_alert_threshold_pct = extractDouble("memory_alert_threshold_pct", config.memory_alert_threshold_pct);
    config.output_format = extractString("output_format", config.output_format);

    Logger::getInstance().log(LogLevel::INFO, "Loaded Virtual Device Policy: Device=" + config.device_path +
                              ", CPU Threshold=" + std::to_string(config.cpu_alert_threshold_pct) + "%" +
                              ", RAM Threshold=" + std::to_string(config.memory_alert_threshold_pct) + "%");
    return true;
}
