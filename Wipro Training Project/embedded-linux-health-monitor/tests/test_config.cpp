#include "config_manager.h"
#include <iostream>
#include <cassert>

int main() {
    ConfigManager mgr;
    mgr.loadConfig("config/health_monitor.json");
    const auto& cfg = mgr.getConfig();
    std::cout << "[TEST CONFIG] CPU Threshold: " << cfg.cpu_threshold << "%" << std::endl;
    assert(cfg.cpu_threshold > 0.0);
    std::cout << "[TEST CONFIG] PASSED" << std::endl;
    return 0;
}
