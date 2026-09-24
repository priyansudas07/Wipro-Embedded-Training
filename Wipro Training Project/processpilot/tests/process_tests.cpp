#include "process_manager.hpp"
#include <iostream>
#include <cassert>

int main() {
    ProcessManager pm;
    ServiceConfig cfg;
    cfg.name = "test_sleep";
    cfg.command = "/bin/sleep 2";
    cfg.restart_policy = "on-failure";

    pm.addService(cfg);
    bool started = pm.startService("test_sleep");
    assert(started);
    std::cout << "[TEST PROCESS] Service start: PASSED" << std::endl;

    auto* proc = pm.getService("test_sleep");
    assert(proc != nullptr);
    assert(proc->status == ServiceStatus::RUNNING);
    std::cout << "[TEST PROCESS] Status check: PASSED" << std::endl;

    pm.stopService("test_sleep");
    assert(proc->status == ServiceStatus::STOPPED);
    std::cout << "[TEST PROCESS] Service stop: PASSED" << std::endl;

    return 0;
}
