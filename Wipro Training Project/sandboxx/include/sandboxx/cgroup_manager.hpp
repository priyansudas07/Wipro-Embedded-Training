#ifndef CGROUP_MANAGER_HPP
#define CGROUP_MANAGER_HPP

#include "sandbox_config.hpp"
#include <sys/types.h>
#include <string>

class CgroupManager {
public:
    CgroupManager() = default;
    ~CgroupManager();

    bool init(const std::string& sandbox_name);
    bool applyLimits(const SandboxConfig& config);
    bool addProcess(pid_t pid);
    bool cleanup();

private:
    std::string cgroup_name_;
    std::string cpu_cgroup_path_;
    std::string memory_cgroup_path_;
    std::string pids_cgroup_path_;
    bool initialized_ = false;
};

#endif // CGROUP_MANAGER_HPP
