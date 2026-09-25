#ifndef NAMESPACE_MANAGER_HPP
#define NAMESPACE_MANAGER_HPP

#include "sandbox_config.hpp"

class NamespaceManager {
public:
    NamespaceManager() = default;
    
    // Returns flags mask for clone() / unshare() based on config
    static int getNamespaceFlags(const SandboxConfig& config);

    // Apply namespace configurations to the current process context
    static bool applyNamespaces(const SandboxConfig& config);
};

#endif // NAMESPACE_MANAGER_HPP
