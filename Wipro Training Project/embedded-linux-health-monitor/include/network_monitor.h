#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <string>

struct NetworkStats {
    std::string interface_name;
    unsigned long long rx_bytes = 0;
    unsigned long long tx_bytes = 0;
    bool is_online = true;
};

class NetworkMonitor {
public:
    NetworkMonitor() = default;
    NetworkStats getNetworkStats();
};

#endif // NETWORK_MONITOR_H
