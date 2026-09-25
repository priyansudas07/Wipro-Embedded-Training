#include "sandboxx/cgroup_manager.hpp"
#include <iostream>
#include <cassert>

int main() {
    CgroupManager cgm;
    SandboxConfig cfg;
    cfg.memory_limit_mb = 128;
    cfg.cpu_quota_pct = 50;

    bool init_ok = cgm.init("test_sandbox");
    if (init_ok) {
        cgm.applyLimits(cfg);
        cgm.cleanup();
    }
    std::cout << "[TEST CGROUP] Cgroup manager test completed." << std::endl;
    return 0;
}
