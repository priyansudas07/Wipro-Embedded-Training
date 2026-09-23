#include "memory_monitor.h"
#include <iostream>
#include <cassert>

int main() {
    MemoryMonitor monitor;
    auto stats = monitor.getMemoryStats();
    std::cout << "[TEST MEMORY] Total RAM: " << stats.total_ram_mb << " MB, Used %: " << stats.used_percent << "%" << std::endl;
    assert(stats.total_ram_mb >= 0.0);
    std::cout << "[TEST MEMORY] PASSED" << std::endl;
    return 0;
}
