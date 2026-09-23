#ifndef SERVICE_MONITOR_H
#define SERVICE_MONITOR_H

#include <string>
#include <vector>

class ServiceMonitor {
public:
    ServiceMonitor() = default;
    bool isServiceActive(const std::string& service_name);
    bool recoverService(const std::string& service_name);
    void checkAndRecoverServices(const std::vector<std::string>& services);
};

#endif // SERVICE_MONITOR_H
