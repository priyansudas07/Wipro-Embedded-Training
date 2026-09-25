#ifndef UTILS_HPP
#define UTILS_HPP

#include "sandbox_config.hpp"
#include <string>

class Utils {
public:
    static bool parseConfigFile(const std::string& filepath, SandboxConfig& config);
    static std::string formatBytes(long bytes);
    static bool createDirectoryRecursive(const std::string& path);
    static bool removeDirectoryRecursive(const std::string& path);
    static bool fileExists(const std::string& filepath);
};

#endif // UTILS_HPP
