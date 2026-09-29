#ifndef VDEVPULSE_DEVICE_MANAGER_HPP
#define VDEVPULSE_DEVICE_MANAGER_HPP

#include "config.hpp"
#include <string>

enum class DeviceState {
    STOPPED,
    RUNNING,
    PAUSED
};

// Virtual IOCTL Commands for Linux Character Device Interface
constexpr unsigned long VDEV_IOCTL_START = 0x8001;
constexpr unsigned long VDEV_IOCTL_STOP  = 0x8002;
constexpr unsigned long VDEV_IOCTL_RESET = 0x8003;

class DeviceManager {
public:
    DeviceManager() = default;
    ~DeviceManager();

    bool initDevice(const VDevConfig& config);
    bool openDevice();
    bool writeData(const std::string& buffer);
    std::string readData();
    bool sendIoctl(unsigned long cmd);
    void closeDevice();

    DeviceState getState() const { return state_; }
    std::string getDevicePath() const { return device_path_; }

private:
    std::string device_path_;
    int device_fd_ = -1;
    DeviceState state_ = DeviceState::STOPPED;
};

#endif // VDEVPULSE_DEVICE_MANAGER_HPP
