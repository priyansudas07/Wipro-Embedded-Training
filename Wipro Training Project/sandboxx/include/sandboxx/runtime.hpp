#ifndef RUNTIME_HPP
#define RUNTIME_HPP

#include "sandbox_config.hpp"
#include "cgroup_manager.hpp"
#include <sys/types.h>
#include <string>
#include <vector>

struct ActiveSandboxInfo {
    std::string id;
    std::string name;
    pid_t pid = -1;
    std::string status;
    std::string command;
    std::string start_time;
};

class SandboxRuntime {
public:
    SandboxRuntime() = default;
    ~SandboxRuntime();

    bool run(const SandboxConfig& config);
    static std::vector<ActiveSandboxInfo> listSandboxes();
    static bool stopSandbox(const std::string& sandbox_id);
    static bool inspectSandbox(const std::string& sandbox_id);

private:
    CgroupManager cgroup_mgr_;
    pid_t child_pid_ = -1;
    std::string sandbox_id_;
};

#endif // RUNTIME_HPP
