#include "sandboxx/security_manager.hpp"
#include <iostream>

int main() {
    SandboxConfig cfg;
    cfg.drop_capabilities = true;
    cfg.enable_seccomp = true;

    SecurityManager::setNoNewPrivs();
    SecurityManager::dropCapabilities();
    SecurityManager::applySeccompFilter();

    std::cout << "[TEST SECURITY] Security policy enforcement test completed." << std::endl;
    return 0;
}
