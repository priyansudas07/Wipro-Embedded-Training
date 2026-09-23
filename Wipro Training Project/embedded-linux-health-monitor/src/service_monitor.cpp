#include "service_monitor.h"
#include "logger.h"
#include <cstdlib>
#include <iostream>

bool ServiceMonitor::isServiceActive(const std::string& service_name) {
    std::string cmd = "pgrep -x " + service_name + " > /dev/null 2>&1";
    int res = std::system(cmd.c_str());
    return (res == 0);
}

bool ServiceMonitor::recoverService(const std::string& service_name) {
    Logger::getInstance().log(LogLevel::RECOVERY, "Initiating auto-recovery for service: " + service_name);
    
    std::string cmd = "systemctl restart " + service_name + " > /dev/null 2>&1";
    int res = std::system(cmd.c_str());
    if (res == 0) {
        Logger::getInstance().log(LogLevel::INFO, "Auto-recovery successful for service: " + service_name);
        return true;
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Auto-recovery failed for service: " + service_name);
        return false;
    }
}

void ServiceMonitor::checkAndRecoverServices(const std::vector<std::string>& services) {
    for (const auto& service : services) {
        if (!isServiceActive(service)) {
            Logger::getInstance().log(LogLevel::WARNING, "Service inactive or failed: " + service);
            recoverService(service);
        }
    }
}
