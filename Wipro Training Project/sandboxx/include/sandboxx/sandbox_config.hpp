#ifndef SANDBOX_CONFIG_HPP
#define SANDBOX_CONFIG_HPP

#include <string>
#include <vector>

struct SandboxConfig {
    std::string name = "sandbox_instance";
    std::string rootfs_path = "";
    std::string hostname = "sandbox-host";
    
    // Resource Limits
    long memory_limit_mb = 128;       // Memory limit in MB (0 = unlimited)
    int cpu_quota_pct = 50;           // CPU usage percentage limit (1-100)
    int max_pids = 64;                // Maximum allowed process count
    
    // Isolation Options
    bool isolate_pid = true;          // PID namespace
    bool isolate_net = true;          // Network namespace
    bool isolate_mount = true;        // Mount namespace
    bool isolate_uts = true;          // UTS (hostname) namespace
    bool isolate_ipc = true;          // IPC namespace
    
    // Security & Filesystem
    bool drop_capabilities = true;    // Drop administrative Linux capabilities
    bool enable_seccomp = true;       // Enable basic syscall filtering
    std::vector<std::string> readonly_paths;
    std::vector<std::string> bind_mounts;
    
    // Command to execute inside sandbox
    std::string command;
    std::vector<std::string> args;
};

#endif // SANDBOX_CONFIG_HPP
