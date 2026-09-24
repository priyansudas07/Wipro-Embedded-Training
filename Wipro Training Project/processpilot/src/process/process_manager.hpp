#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <sys/types.h>
#include "config_parser.hpp"

enum class ServiceStatus { STOPPED, RUNNING, FAILED };

struct ManagedProcess {
    ServiceConfig config;
    pid_t pid = -1;
    ServiceStatus status = ServiceStatus::STOPPED;
    int restart_count = 0;
};

class ProcessManager {
public:
    ProcessManager() = default;
    bool addService(const ServiceConfig& config);
    bool startService(const std::string& name);
    bool stopService(const std::string& name);
    bool restartService(const std::string& name);
    void monitorProcesses();
    ManagedProcess* getService(const std::string& name);
    const std::unordered_map<std::string, ManagedProcess>& getAllServices() const { return services_; }

private:
    std::unordered_map<std::string, ManagedProcess> services_;
};

#endif // PROCESS_MANAGER_HPP
