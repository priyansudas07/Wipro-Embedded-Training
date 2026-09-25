#ifndef SECURITY_MANAGER_HPP
#define SECURITY_MANAGER_HPP

#include "sandbox_config.hpp"

class SecurityManager {
public:
    SecurityManager() = default;

    static bool dropCapabilities();
    static bool applySeccompFilter();
    static bool setNoNewPrivs();
    static bool applySecurityPolicy(const SandboxConfig& config);
};

#endif // SECURITY_MANAGER_HPP
