#include "sandboxx/filesystem_manager.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    SandboxConfig cfg;
    std::string test_dir = "build/test_sandbox_fs";
    fs::create_directories(test_dir);

    bool ok = FilesystemManager::setupRootfs(cfg, test_dir);
    assert(ok);
    assert(fs::exists(test_dir + "/rootfs/bin"));
    assert(fs::exists(test_dir + "/rootfs/proc"));

    fs::remove_all(test_dir);
    std::cout << "[TEST FILESYSTEM] Filesystem rootfs setup passed." << std::endl;
    return 0;
}
