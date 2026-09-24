#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <vector>

struct ServiceConfig {
    std::string name;
    std::string command;
    std::string restart_policy = "on-failure"; // "always", "on-failure", "no"
    std::vector<std::string> dependencies;
};

class ConfigParser {
public:
    ConfigParser() = default;
    static std::vector<ServiceConfig> parseFile(const std::string& filepath);
};

#endif // CONFIG_PARSER_HPP
