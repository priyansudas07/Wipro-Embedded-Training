#include "network_monitor.h"
#include <fstream>
#include <sstream>

NetworkStats NetworkMonitor::getNetworkStats() {
    NetworkStats stats;
    stats.interface_name = "eth0";

    std::ifstream file("/proc/net/dev");
    if (file.is_open()) {
        std::string line;
        // Skip header lines
        std::getline(file, line);
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string iface;
            ss >> iface;
            if (iface.find("lo:") == std::string::npos) {
                unsigned long long rx, tx, dummy;
                ss >> rx >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> tx;
                stats.interface_name = iface.substr(0, iface.find(':'));
                stats.rx_bytes = rx;
                stats.tx_bytes = tx;
                stats.is_online = true;
                break;
            }
        }
        file.close();
    }
    return stats;
}
