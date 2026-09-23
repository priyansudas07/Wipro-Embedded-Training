#include "cpu_monitor.h"
#include <iostream>
#include <cassert>

int main() {
    CpuMonitor monitor;
    double usage = monitor.getCpuUsage();
    std::cout << "[TEST CPU] Initial CPU Usage: " << usage << "%" << std::endl;
    assert(usage >= 0.0 && usage <= 100.0);
    std::cout << "[TEST CPU] PASSED" << std::endl;
    return 0;
}
