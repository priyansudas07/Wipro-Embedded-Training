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

    bool ioctl_ok = dev_mgr.sendIoctl(VDEV_IOCTL_START);
    assert(ioctl_ok);

    dev_mgr.closeDevice();
    std::cout << "[TEST DEVICE] Virtual character device manager test PASSED." << std::endl;
    return 0;
}
