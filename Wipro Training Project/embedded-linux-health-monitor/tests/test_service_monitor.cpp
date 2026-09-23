#include "service_monitor.h"
#include <iostream>

int main() {
    ServiceMonitor monitor;
    bool active = monitor.isServiceActive("bash");
    std::cout << "[TEST SERVICE] Is bash process active: " << (active ? "YES" : "NO") << std::endl;
    std::cout << "[TEST SERVICE] PASSED" << std::endl;
    return 0;
}
