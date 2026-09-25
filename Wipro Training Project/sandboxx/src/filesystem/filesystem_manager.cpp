#include "sandboxx/filesystem_manager.hpp"
#include "sandboxx/logger.hpp"
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>
#include <cstring>
#include <iostream>

namespace fs = std::filesystem;

bool FilesystemManager::setupRootfs(const SandboxConfig& config, const std::string& sandbox_dir) {
    try {
        std::string rootfs = sandbox_dir + "/rootfs";
        fs::create_directories(rootfs + "/bin");
        fs::create_directories(rootfs + "/lib");
        fs::create_directories(rootfs + "/lib64");
        fs::create_directories(rootfs + "/usr");
        fs::create_directories(rootfs + "/tmp");
        fs::create_directories(rootfs + "/proc");
        fs::create_directories(rootfs + "/dev");
        fs::create_directories(rootfs + "/sys");
        fs::create_directories(rootfs + "/etc");

        Logger::getInstance().log(LogLevel::INFO, "Created sandbox rootfs structure at: " + rootfs);
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to create rootfs structure: " + std::string(e.what()));
        return false;
    }
}

bool FilesystemManager::prepareContainerMounts(const std::string& target_rootfs) {
    std::string proc_target = target_rootfs + "/proc";
    std::string sys_target = target_rootfs + "/sys";
    std::string dev_target = target_rootfs + "/dev";

    // Mount proc
    if (fs::exists(proc_target)) {
        mount("proc", proc_target.c_str(), "proc", MS_NOSUID | MS_NOEXEC | MS_NODEV, NULL);
    }

    // Mount tmpfs on /dev and /tmp
    std::string tmp_target = target_rootfs + "/tmp";
    if (fs::exists(tmp_target)) {
        mount("tmpfs", tmp_target.c_str(), "tmpfs", 0, "size=16M");
    }

    Logger::getInstance().log(LogLevel::INFO, "Prepared container pseudo-filesystems (proc, tmp).");
    return true;
}

bool FilesystemManager::isolateFilesystem(const std::string& target_rootfs) {
    if (!fs::exists(target_rootfs)) return false;

    // Make mount propagation private
    mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);

    // Bind mount rootfs to itself to turn it into a mount point
    if (mount(target_rootfs.c_str(), target_rootfs.c_str(), "bind", MS_BIND | MS_REC, NULL) < 0) {
        Logger::getInstance().log(LogLevel::WARNING, "Rootfs self bind mount warning: " + std::string(strerror(errno)));
    }

    // Change root directory
    if (chroot(target_rootfs.c_str()) < 0) {
        Logger::getInstance().log(LogLevel::WARNING, "chroot failed: " + std::string(strerror(errno)));
        return false;
    }

    if (chdir("/") < 0) {
        Logger::getInstance().log(LogLevel::WARNING, "chdir '/' failed: " + std::string(strerror(errno)));
        return false;
    }

    Logger::getInstance().log(LogLevel::SUCCESS, "Filesystem chroot isolation established at container root /.");
    return true;
}

bool FilesystemManager::cleanupMounts(const std::string& sandbox_dir) {
    std::string rootfs = sandbox_dir + "/rootfs";
    umount2((rootfs + "/proc").c_str(), MNT_DETACH);
    umount2((rootfs + "/sys").c_str(), MNT_DETACH);
    umount2((rootfs + "/tmp").c_str(), MNT_DETACH);
    umount2(rootfs.c_str(), MNT_DETACH);
    return true;
}
