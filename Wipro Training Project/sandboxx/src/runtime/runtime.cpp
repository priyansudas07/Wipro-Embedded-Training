#include "sandboxx/runtime.hpp"
#include "sandboxx/process_manager.hpp"
#include "sandboxx/filesystem_manager.hpp"
#include "sandboxx/security_manager.hpp"
#include "sandboxx/monitor.hpp"
#include "sandboxx/logger.hpp"
#include "sandboxx/utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

SandboxRuntime::~SandboxRuntime() {
    cgroup_mgr_.cleanup();
}

bool SandboxRuntime::run(const SandboxConfig& config) {
    auto now = std::chrono::system_clock::now();
    sandbox_id_ = config.name + "_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());

    std::string sandbox_dir = "runtime/sandboxes/" + sandbox_id_;
    fs::create_directories(sandbox_dir);
    fs::create_directories("runtime/logs");

    Logger::getInstance().init("runtime/logs/" + sandbox_id_ + ".log");
    Logger::getInstance().log(LogLevel::INFO, "Initializing SandBoxX Runtime [" + sandbox_id_ + "]...");

    // 1. Setup Cgroups
    cgroup_mgr_.init(sandbox_id_);
    cgroup_mgr_.applyLimits(config);

    // 2. Setup Filesystem Rootfs
    FilesystemManager::setupRootfs(config, sandbox_dir);

    // Entry point for child process inside sandbox
    auto child_entry = [&config, &sandbox_dir]() -> int {
        SecurityManager::applySecurityPolicy(config);
        
        std::string target_rootfs = sandbox_dir + "/rootfs";
        FilesystemManager::prepareContainerMounts(target_rootfs);
        if (config.isolate_mount) {
            FilesystemManager::isolateFilesystem(target_rootfs);
        }

        Logger::getInstance().log(LogLevel::INFO, "Executing sandboxed command: " + config.command);

        if (!config.command.empty()) {
            int ret = system(config.command.c_str());
            return WEXITSTATUS(ret);
        }
        return 0;
    };

    // 3. Spawn child process
    child_pid_ = ProcessManager::spawnSandboxedProcess(config, child_entry);
    if (child_pid_ <= 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to launch sandboxed process.");
        return false;
    }

    // Attach child to cgroups
    cgroup_mgr_.addProcess(child_pid_);

    // Save runtime metadata info
    std::ofstream meta_file(sandbox_dir + "/meta.json");
    if (meta_file.is_open()) {
        meta_file << "{\n";
        meta_file << "  \"id\": \"" << sandbox_id_ << "\",\n";
        meta_file << "  \"name\": \"" << config.name << "\",\n";
        meta_file << "  \"pid\": " << child_pid_ << ",\n";
        meta_file << "  \"status\": \"RUNNING\",\n";
        meta_file << "  \"command\": \"" << config.command << "\"\n";
        meta_file << "}\n";
    }

    // 4. Monitor & Wait
    ProcessStats stats = Monitor::getProcessStats(child_pid_);
    Monitor::printStatsTable(stats);

    int exit_code = ProcessManager::waitForProcess(child_pid_);
    Logger::getInstance().log(LogLevel::SUCCESS, "Sandbox execution completed with exit code: " + std::to_string(exit_code));

    // Update metadata status
    std::ofstream meta_file_end(sandbox_dir + "/meta.json");
    if (meta_file_end.is_open()) {
        meta_file_end << "{\n";
        meta_file_end << "  \"id\": \"" << sandbox_id_ << "\",\n";
        meta_file_end << "  \"name\": \"" << config.name << "\",\n";
        meta_file_end << "  \"pid\": " << child_pid_ << ",\n";
        meta_file_end << "  \"status\": \"STOPPED\",\n";
        meta_file_end << "  \"command\": \"" << config.command << "\"\n";
        meta_file_end << "}\n";
    }

    FilesystemManager::cleanupMounts(sandbox_dir);
    return (exit_code == 0);
}

std::vector<ActiveSandboxInfo> SandboxRuntime::listSandboxes() {
    std::vector<ActiveSandboxInfo> list;
    std::string base = "runtime/sandboxes";
    if (!fs::exists(base)) return list;

    for (const auto& entry : fs::directory_iterator(base)) {
        if (entry.is_directory()) {
            std::string meta_path = entry.path().string() + "/meta.json";
            if (fs::exists(meta_path)) {
                ActiveSandboxInfo info;
                info.id = entry.path().filename().string();
                std::ifstream f(meta_path);
                std::string line;
                while (std::getline(f, line)) {
                    if (line.find("\"name\":") != std::string::npos) {
                        size_t s = line.find('"', line.find(':')) + 1;
                        size_t e = line.find('"', s);
                        if (s != std::string::npos && e != std::string::npos) info.name = line.substr(s, e - s);
                    } else if (line.find("\"pid\":") != std::string::npos) {
                        size_t colon = line.find(':');
                        if (colon != std::string::npos) info.pid = std::stoi(line.substr(colon + 1));
                    } else if (line.find("\"status\":") != std::string::npos) {
                        size_t s = line.find('"', line.find(':')) + 1;
                        size_t e = line.find('"', s);
                        if (s != std::string::npos && e != std::string::npos) info.status = line.substr(s, e - s);
                    } else if (line.find("\"command\":") != std::string::npos) {
                        size_t s = line.find('"', line.find(':')) + 1;
                        size_t e = line.find('"', s);
                        if (s != std::string::npos && e != std::string::npos) info.command = line.substr(s, e - s);
                    }
                }
                list.push_back(info);
            }
        }
    }
    return list;
}

bool SandboxRuntime::stopSandbox(const std::string& sandbox_id) {
    std::string meta_path = "runtime/sandboxes/" + sandbox_id + "/meta.json";
    if (!fs::exists(meta_path)) {
        std::cout << "Sandbox ID not found: " << sandbox_id << std::endl;
        return false;
    }
    std::ifstream f(meta_path);
    std::string line;
    pid_t pid = -1;
    while (std::getline(f, line)) {
        if (line.find("\"pid\":") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) pid = std::stoi(line.substr(colon + 1));
        }
    }
    if (pid > 0) {
        return ProcessManager::killProcess(pid, 9);
    }
    return false;
}

bool SandboxRuntime::inspectSandbox(const std::string& sandbox_id) {
    std::string meta_path = "runtime/sandboxes/" + sandbox_id + "/meta.json";
    if (!fs::exists(meta_path)) {
        std::cout << "Sandbox ID not found: " << sandbox_id << std::endl;
        return false;
    }
    std::ifstream f(meta_path);
    std::cout << f.rdbuf() << std::endl;
    return true;
}
