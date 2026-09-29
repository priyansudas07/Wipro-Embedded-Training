#include "vdevpulse/config.hpp"
#include "vdevpulse/logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

bool ConfigParser::loadPolicy(const std::string& filepath, VDevConfig& config) {
    if (!fs::exists(filepath)) {
        Logger::getInstance().log(LogLevel::WARNING, "Policy configuration file not found: " + filepath + ", using default virtual device configuration.");
        return true;
    }

    std::ifstream file(filepath);
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

    config.device_name = extractString("device_name", config.device_name);
    config.device_path = extractString("device_path", config.device_path);
    Logger::getInstance().log(LogLevel::INFO, "Loaded Virtual Device Policy: Device=" + config.device_path);
    return true;
}
