#include "process_manager.hpp"
#include "logger.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sstream>
#include <vector>
#include <cstring>

bool ProcessManager::addService(const ServiceConfig& config) {
    services_[config.name] = ManagedProcess{config, -1, ServiceStatus::STOPPED, 0};
    return true;
}

bool ProcessManager::startService(const std::string& name) {
    auto it = services_.find(name);
    if (it == services_.end()) return false;
    if (it->second.status == ServiceStatus::RUNNING) return true;

    pid_t pid = fork();
    if (pid < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Fork failed for service: " + name);
        return false;
    }

    if (pid == 0) {
        // Child process execution
        std::stringstream ss(it->second.config.command);
        std::string token;
        std::vector<std::string> args_str;
        while (ss >> token) {
            args_str.push_back(token);
        }

        std::vector<char*> args;
        for (auto& s : args_str) args.push_back(&s[0]);
        args.push_back(nullptr);

        execvp(args[0], args.data());
        _exit(127);
    } else {
        // Parent process tracking
        it->second.pid = pid;
        it->second.status = ServiceStatus::RUNNING;
        Logger::getInstance().log(LogLevel::INFO, "Started service: " + name + " (PID " + std::to_string(pid) + ")");
        return true;
    }
}

bool ProcessManager::stopService(const std::string& name) {
    auto it = services_.find(name);
    if (it == services_.end() || it->second.status != ServiceStatus::RUNNING) return false;

    kill(it->second.pid, SIGTERM);
    usleep(100000);
    int status;
    waitpid(it->second.pid, &status, WNOHANG);

    it->second.status = ServiceStatus::STOPPED;
    it->second.pid = -1;
    Logger::getInstance().log(LogLevel::INFO, "Stopped service: " + name);
    return true;
}

bool ProcessManager::restartService(const std::string& name) {
    stopService(name);
    return startService(name);
}

void ProcessManager::monitorProcesses() {
    for (auto& pair : services_) {
        auto& proc = pair.second;
        if (proc.status == ServiceStatus::RUNNING && proc.pid > 0) {
            int status;
            pid_t res = waitpid(proc.pid, &status, WNOHANG);
            if (res > 0) { // Process exited or crashed
                proc.status = ServiceStatus::FAILED;
                Logger::getInstance().log(LogLevel::WARNING, "Detected failure in service: " + proc.config.name + " (PID " + std::to_string(proc.pid) + ")");
                proc.pid = -1;

                // Auto-recovery execution based on restart policy
                if (proc.config.restart_policy == "always" || proc.config.restart_policy == "on-failure") {
                    proc.restart_count++;
                    Logger::getInstance().log(LogLevel::RECOVERY, "Auto-recovery policy triggered for: " + proc.config.name + " (Attempt " + std::to_string(proc.restart_count) + ")");
                    startService(proc.config.name);
                }
            }
        }
    }
}

ManagedProcess* ProcessManager::getService(const std::string& name) {
    auto it = services_.find(name);
    if (it != services_.end()) return &it->second;
    return nullptr;
}
