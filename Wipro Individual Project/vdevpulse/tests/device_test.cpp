#include "vdevpulse/device_manager.hpp"
#include <iostream>
#include <cassert>

int main() {
    DeviceManager dev_mgr;
    VDevConfig config;
    config.device_path = "/tmp/vdevpulse_unittest";

    bool init_ok = dev_mgr.initDevice(config);
    assert(init_ok);

    bool open_ok = dev_mgr.openDevice();
    assert(open_ok);
    assert(dev_mgr.getState() == DeviceState::RUNNING);

    bool write_ok = dev_mgr.writeData("UNIT_TEST_PAYLOAD");
    assert(write_ok);

    // Test Extended IOCTL Suite
    bool ioctl_start = dev_mgr.sendIoctl(VDEV_IOCTL_START);
    assert(ioctl_start);

    bool ioctl_stats = dev_mgr.sendIoctl(VDEV_IOCTL_GET_STATS);
    assert(ioctl_stats);

    bool ioctl_stop = dev_mgr.sendIoctl(VDEV_IOCTL_STOP);
    assert(ioctl_stop);
    assert(dev_mgr.getState() == DeviceState::STOPPED);

    // Test Query Command Protocol
    SystemTelemetry t;
    t.cpu_usage_pct = 25.5;
    t.memory_used_mb = 1024;
    t.memory_usage_pct = 32.0;
    t.load_1m = 0.45;
    t.health_status = "HEALTHY";

    std::string pong = dev_mgr.processQueryCommand("PING", t);
    assert(pong == "PONG");

    std::string cpu_res = dev_mgr.processQueryCommand("GET_CPU", t);
    assert(cpu_res.find("CPU_PCT=") != std::string::npos);

    std::string health_res = dev_mgr.processQueryCommand("GET_HEALTH", t);
    assert(health_res == "HEALTH=HEALTHY");

    dev_mgr.closeDevice();
    std::cout << "[TEST DEVICE] Virtual character device & IOCTL query test PASSED." << std::endl;
    return 0;
}
