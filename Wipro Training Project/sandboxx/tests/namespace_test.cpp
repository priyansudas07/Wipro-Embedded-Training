#include "sandboxx/namespace_manager.hpp"
#include <iostream>
#include <cassert>

int main() {
    SandboxConfig cfg;
    cfg.isolate_pid = true;
    cfg.isolate_net = true;
    cfg.isolate_uts = true;

    int flags = NamespaceManager::getNamespaceFlags(cfg);
    assert(flags & CLONE_NEWPID);
    assert(flags & CLONE_NEWNET);
    assert(flags & CLONE_NEWUTS);

    std::cout << "[TEST NAMESPACE] Namespace flag generation passed." << std::endl;
    return 0;
}
