#include "sandboxx/process_manager.hpp"
#include "sandboxx/namespace_manager.hpp"
#include "sandboxx/logger.hpp"
#include <sys/wait.h>
#include <sched.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <iostream>

pid_t ProcessManager::spawnSandboxedProcess(const SandboxConfig& config, std::function<int()> child_entry) {
    pid_t pid = fork();

    if (pid < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Fork failed: " + std::string(strerror(errno)));
        return -1;
    }

    if (pid == 0) {
        // Child process inside sandbox
        if (config.isolate_pid || config.isolate_net || config.isolate_mount || config.isolate_uts || config.isolate_ipc) {
            NamespaceManager::applyNamespaces(config);
        }

        int ret = child_entry();
        _exit(ret);
    }

    // Parent process returns child PID
    Logger::getInstance().log(LogLevel::INFO, "Spawned sandboxed process PID: " + std::to_string(pid));
    return pid;
}

int ProcessManager::waitForProcess(pid_t pid) {
    if (pid <= 0) return -1;
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        return -1;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        Logger::getInstance().log(LogLevel::INFO, "Sandboxed process PID " + std::to_string(pid) + " exited cleanly with code: " + std::to_string(code));
        return code;
    } else if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        Logger::getInstance().log(LogLevel::WARNING, "Sandboxed process PID " + std::to_string(pid) + " terminated by signal: " + std::to_string(sig));
        return 128 + sig;
    }

    return status;
}

bool ProcessManager::killProcess(pid_t pid, int signal_num) {
    if (pid <= 0) return false;
    if (kill(pid, signal_num) == 0) {
        Logger::getInstance().log(LogLevel::INFO, "Sent signal " + std::to_string(signal_num) + " to sandboxed process PID " + std::to_string(pid));
        return true;
    }
    return false;
}
