#ifndef VDEVPULSE_DEVICE_MANAGER_HPP
#define VDEVPULSE_DEVICE_MANAGER_HPP

#include "config.hpp"
#include "telemetry_monitor.hpp"
#include <string>
#include <cstdint>

enum class DeviceState {
    STOPPED,
    RUNNING,
    PAUSED
};

// Virtual IOCTL Commands for Linux Character Device Interface
constexpr unsigned long VDEV_IOCTL_START     = 0x8001;
constexpr unsigned long VDEV_IOCTL_STOP      = 0x8002;
constexpr unsigned long VDEV_IOCTL_RESET     = 0x8003;
constexpr unsigned long VDEV_IOCTL_GET_STATS = 0x8004;
constexpr unsigned long VDEV_IOCTL_SET_RATE  = 0x8005;

struct DeviceStats {
    uint64_t total_bytes_written = 0;
    uint64_t total_reads = 0;
    uint64_t total_ioctls = 0;
    uint64_t total_queries = 0;
};

class DeviceManager {
public:
    DeviceManager() = default;
    ~DeviceManager();

    bool initDevice(const VDevConfig& config);
    bool openDevice();
    bool writeData(const std::string& buffer);
    std::string readData();
    bool sendIoctl(unsigned long cmd, unsigned long arg = 0);
    std::string processQueryCommand(const std::string& cmd, const SystemTelemetry& telemetry);
    void closeDevice();

    DeviceState getState() const { return state_; }
    std::string getDevicePath() const { return device_path_; }
    const DeviceStats& getStats() const { return stats_; }

private:
    std::string device_path_;
    int device_fd_ = -1;
    DeviceState state_ = DeviceState::STOPPED;
    DeviceStats stats_;
};

#endif // VDEVPULSE_DEVICE_MANAGER_HPP
