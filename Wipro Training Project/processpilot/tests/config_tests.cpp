#include "config_parser.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

int main() {
    std::string configPath = "configs/demo.service";
    if (!std::filesystem::exists(configPath)) {
        if (std::filesystem::exists("../configs/demo.service")) {
            configPath = "../configs/demo.service";
        } else if (std::filesystem::exists("../../configs/demo.service")) {
            configPath = "../../configs/demo.service";
        }
    }
    auto services = ConfigParser::parseFile(configPath);
    std::cout << "[TEST CONFIG] Parsed services count: " << services.size() << std::endl;
    assert(!services.empty());
    assert(services[0].name == "logger_service");
    std::cout << "[TEST CONFIG] PASSED" << std::endl;
    return 0;
}
