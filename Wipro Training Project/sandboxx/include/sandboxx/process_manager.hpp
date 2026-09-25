#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include "sandbox_config.hpp"
#include <sys/types.h>
#include <functional>

class ProcessManager {
public:
    ProcessManager() = default;

    // Spawns child process inside isolated namespace environment
    static pid_t spawnSandboxedProcess(const SandboxConfig& config, std::function<int()> child_entry);
    
    // Wait for child process and return exit code
    static int waitForProcess(pid_t pid);

    // Terminate process safely or forcefully
    static bool killProcess(pid_t pid, int signal_num = 9);
};

#endif // PROCESS_MANAGER_HPP
