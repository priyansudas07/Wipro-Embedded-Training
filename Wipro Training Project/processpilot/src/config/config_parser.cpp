#include "config_parser.hpp"
#include "logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<ServiceConfig> ConfigParser::parseFile(const std::string& filepath) {
    std::vector<ServiceConfig> services;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to open config file: " + filepath);
        return services;
    }

    std::string line;
    ServiceConfig current;
    bool in_section = false;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "[Service]") {
            if (in_section && !current.name.empty()) {
                services.push_back(current);
            }
            current = ServiceConfig();
            in_section = true;
            continue;
        }

        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = trim(line.substr(0, eq));
            std::string val = trim(line.substr(eq + 1));

            if (key == "Name") current.name = val;
            else if (key == "Command") current.command = val;
            else if (key == "RestartPolicy") current.restart_policy = val;
            else if (key == "Dependencies") {
                std::stringstream ss(val);
                std::string dep;
                while (std::getline(ss, dep, ',')) {
                    dep = trim(dep);
                    if (!dep.empty()) current.dependencies.push_back(dep);
                }
            }
        }
    }

    if (in_section && !current.name.empty()) {
        services.push_back(current);
    }

    file.close();
    Logger::getInstance().log(LogLevel::INFO, "Parsed " + std::to_string(services.size()) + " service configurations from " + filepath);
    return services;
}
