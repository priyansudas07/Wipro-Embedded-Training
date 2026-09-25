#include "sandboxx/runtime.hpp"
#include <iostream>
#include <cassert>

int main() {
    SandboxConfig cfg;
    cfg.name = "unit_test_sandbox";
    cfg.command = "echo 'Unit test execution inside sandbox'";
    cfg.isolate_pid = false; // Disable PID unshare for unprivileged unit test execution
    cfg.isolate_net = false;
    cfg.isolate_mount = false;
    cfg.isolate_uts = false;
    cfg.isolate_ipc = false;

    SandboxRuntime runtime;
    bool ok = runtime.run(cfg);
    assert(ok);

    std::cout << "[TEST RUNTIME] Runtime engine test PASSED." << std::endl;
    return 0;
}
