#include "sandboxx/namespace_manager.hpp"
#include "sandboxx/logger.hpp"
#include <sched.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int NamespaceManager::getNamespaceFlags(const SandboxConfig& config) {
    int flags = 0;
    if (config.isolate_pid)   flags |= CLONE_NEWPID;
    if (config.isolate_net)   flags |= CLONE_NEWNET;
    if (config.isolate_mount) flags |= CLONE_NEWNS;
    if (config.isolate_uts)   flags |= CLONE_NEWUTS;
    if (config.isolate_ipc)   flags |= CLONE_NEWIPC;
    return flags;
}

bool NamespaceManager::applyNamespaces(const SandboxConfig& config) {
    int flags = getNamespaceFlags(config);

    if (unshare(flags) < 0) {
        Logger::getInstance().log(LogLevel::WARNING, "unshare failed (may require root or CAP_SYS_ADMIN): " + std::string(strerror(errno)));
        return false;
    }

    if (config.isolate_uts && !config.hostname.empty()) {
        if (sethostname(config.hostname.c_str(), config.hostname.length()) < 0) {
            Logger::getInstance().log(LogLevel::WARNING, "Failed to set isolated UTS hostname: " + std::string(strerror(errno)));
        } else {
            Logger::getInstance().log(LogLevel::INFO, "Isolated UTS hostname set to: " + config.hostname);
        }
    }

    Logger::getInstance().log(LogLevel::SUCCESS, "Linux Namespaces successfully applied.");
    return true;
}
