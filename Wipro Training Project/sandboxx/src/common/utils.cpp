#include "sandboxx/utils.hpp"
#include "sandboxx/logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

bool Utils::parseConfigFile(const std::string& filepath, SandboxConfig& config) {
    if (!fs::exists(filepath)) {
        Logger::getInstance().log(LogLevel::ERROR, "Config file not found: " + filepath);
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Parse key JSON / text properties from string content
    auto extractString = [&content](const std::string& key, const std::string& default_val) -> std::string {
        size_t pos = content.find("\"" + key + "\"");
        if (pos == std::string::npos) pos = content.find(key);
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

    auto extractLong = [&extractString](const std::string& key, long default_val) -> long {
        std::string val = extractString(key, std::to_string(default_val));
        try {
            return std::stol(val);
        } catch (...) {
            return default_val;
        }
    };

    auto extractBool = [&extractString](const std::string& key, bool default_val) -> bool {
        std::string val = extractString(key, default_val ? "true" : "false");
        return (val == "true" || val == "1");
    };

    config.name = extractString("name", config.name);
    config.hostname = extractString("hostname", config.hostname);
    config.memory_limit_mb = extractLong("memory_limit_mb", config.memory_limit_mb);
    config.cpu_quota_pct = (int)extractLong("cpu_quota_pct", config.cpu_quota_pct);
    config.max_pids = (int)extractLong("max_pids", config.max_pids);
    config.isolate_pid = extractBool("isolate_pid", config.isolate_pid);
    config.isolate_net = extractBool("isolate_net", config.isolate_net);
    config.isolate_mount = extractBool("isolate_mount", config.isolate_mount);
    config.isolate_uts = extractBool("isolate_uts", config.isolate_uts);
    config.isolate_ipc = extractBool("isolate_ipc", config.isolate_ipc);
    config.drop_capabilities = extractBool("drop_capabilities", config.drop_capabilities);
    config.enable_seccomp = extractBool("enable_seccomp", config.enable_seccomp);
    config.command = extractString("command", config.command);

    return true;
}

std::string Utils::formatBytes(long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double count = static_cast<double>(bytes);
    while (count >= 1024 && i < 4) {
        count /= 1024;
        i++;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f %s", count, units[i]);
    return std::string(buf);
}

bool Utils::createDirectoryRecursive(const std::string& path) {
    try {
        return fs::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool Utils::removeDirectoryRecursive(const std::string& path) {
    try {
        fs::remove_all(path);
        return true;
    } catch (...) {
        return false;
    }
}

bool Utils::fileExists(const std::string& filepath) {
    return fs::exists(filepath);
}
