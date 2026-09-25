#ifndef FILESYSTEM_MANAGER_HPP
#define FILESYSTEM_MANAGER_HPP

#include "sandbox_config.hpp"
#include <string>

class FilesystemManager {
public:
    FilesystemManager() = default;

    static bool setupRootfs(const SandboxConfig& config, const std::string& sandbox_dir);
    static bool prepareContainerMounts(const std::string& target_rootfs);
    static bool isolateFilesystem(const std::string& target_rootfs);
    static bool cleanupMounts(const std::string& sandbox_dir);
};

#endif // FILESYSTEM_MANAGER_HPP
