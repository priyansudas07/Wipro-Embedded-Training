#include "sandboxx/cgroup_manager.hpp"
#include "sandboxx/logger.hpp"
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <cstring>

namespace fs = std::filesystem;

CgroupManager::~CgroupManager() {
    cleanup();
}

bool CgroupManager::init(const std::string& sandbox_name) {
    cgroup_name_ = sandbox_name;
    
    // Check for cgroup v2 unified hierarchy first, then v1
    std::string base_v2 = "/sys/fs/cgroup/sandboxx_" + sandbox_name;
    std::string base_v1_cpu = "/sys/fs/cgroup/cpu/sandboxx_" + sandbox_name;
    std::string base_v1_mem = "/sys/fs/cgroup/memory/sandboxx_" + sandbox_name;
    std::string base_v1_pids = "/sys/fs/cgroup/pids/sandboxx_" + sandbox_name;

    try {
        std::error_code ec;
        if (fs::exists("/sys/fs/cgroup/cgroup.controllers")) {
            // Cgroup v2
            cpu_cgroup_path_ = base_v2;
            memory_cgroup_path_ = base_v2;
            pids_cgroup_path_ = base_v2;
            fs::create_directories(base_v2, ec);
            if (ec) {
                Logger::getInstance().log(LogLevel::WARNING, "Non-root Cgroup notice [/sys/fs/cgroup]: " + ec.message());
            }
        } else {
            // Cgroup v1
            cpu_cgroup_path_ = base_v1_cpu;
            memory_cgroup_path_ = base_v1_mem;
            pids_cgroup_path_ = base_v1_pids;
            fs::create_directories(base_v1_cpu, ec);
            fs::create_directories(base_v1_mem, ec);
            fs::create_directories(base_v1_pids, ec);
        }
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::WARNING, "Cgroup initialization exception: " + std::string(e.what()));
    }

    initialized_ = true;
    Logger::getInstance().log(LogLevel::INFO, "Cgroup subsystem initialized for " + sandbox_name);
    return true;
}

bool CgroupManager::applyLimits(const SandboxConfig& config) {
    if (!initialized_) return false;

    try {
        // 1. Memory Limit
        if (config.memory_limit_mb > 0) {
            long bytes = config.memory_limit_mb * 1024 * 1024;
            std::string mem_file = fs::exists(memory_cgroup_path_ + "/memory.max") ?
                                   memory_cgroup_path_ + "/memory.max" :
                                   memory_cgroup_path_ + "/memory.limit_in_bytes";
            std::ofstream f(mem_file);
            if (f.is_open()) {
                f << bytes;
                Logger::getInstance().log(LogLevel::INFO, "Applied Memory limit: " + std::to_string(config.memory_limit_mb) + " MB");
            }
        }

        // 2. CPU Limit
        if (config.cpu_quota_pct > 0 && config.cpu_quota_pct <= 100) {
            long period = 100000; // 100ms
            long quota = (period * config.cpu_quota_pct) / 100;
            if (fs::exists(cpu_cgroup_path_ + "/cpu.max")) {
                std::ofstream f(cpu_cgroup_path_ + "/cpu.max");
                if (f.is_open()) f << quota << " " << period;
            } else if (fs::exists(cpu_cgroup_path_ + "/cpu.cfs_quota_us")) {
                std::ofstream f_quota(cpu_cgroup_path_ + "/cpu.cfs_quota_us");
                std::ofstream f_period(cpu_cgroup_path_ + "/cpu.cfs_period_us");
                if (f_period.is_open()) f_period << period;
                if (f_quota.is_open()) f_quota << quota;
            }
            Logger::getInstance().log(LogLevel::INFO, "Applied CPU limit: " + std::to_string(config.cpu_quota_pct) + "%");
        }

        // 3. PIDs Limit
        if (config.max_pids > 0) {
            std::string pids_file = fs::exists(pids_cgroup_path_ + "/pids.max") ?
                                    pids_cgroup_path_ + "/pids.max" :
                                    pids_cgroup_path_ + "/pids.max";
            std::ofstream f(pids_file);
            if (f.is_open()) {
                f << config.max_pids;
                Logger::getInstance().log(LogLevel::INFO, "Applied Max PIDs limit: " + std::to_string(config.max_pids));
            }
        }
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::WARNING, "Cgroup limits configuration exception: " + std::string(e.what()));
        return false;
    }
}

bool CgroupManager::addProcess(pid_t pid) {
    if (!initialized_) return false;

    auto attachTo = [](const std::string& path, pid_t p) {
        std::ofstream f_procs(path + "/cgroup.procs");
        if (f_procs.is_open()) {
            f_procs << p;
            return true;
        }
        std::ofstream f_tasks(path + "/tasks");
        if (f_tasks.is_open()) {
            f_tasks << p;
            return true;
        }
        return false;
    };

    bool s1 = attachTo(cpu_cgroup_path_, pid);
    bool s2 = attachTo(memory_cgroup_path_, pid);
    bool s3 = attachTo(pids_cgroup_path_, pid);

    if (s1 || s2 || s3) {
        Logger::getInstance().log(LogLevel::SUCCESS, "Attached process PID " + std::to_string(pid) + " to cgroups.");
        return true;
    }
    return false;
}

bool CgroupManager::cleanup() {
    if (!initialized_) return true;
    try {
        if (!cpu_cgroup_path_.empty() && fs::exists(cpu_cgroup_path_)) fs::remove_all(cpu_cgroup_path_);
        if (!memory_cgroup_path_.empty() && fs::exists(memory_cgroup_path_) && memory_cgroup_path_ != cpu_cgroup_path_) fs::remove_all(memory_cgroup_path_);
        if (!pids_cgroup_path_.empty() && fs::exists(pids_cgroup_path_) && pids_cgroup_path_ != cpu_cgroup_path_) fs::remove_all(pids_cgroup_path_);
    } catch (...) {}
    initialized_ = false;
    return true;
}
