#include "vdevpulse/device_manager.hpp"
#include "vdevpulse/logger.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <sstream>
#include <algorithm>

namespace fs = std::filesystem;

DeviceManager::~DeviceManager() {
    closeDevice();
}

bool DeviceManager::initDevice(const VDevConfig& config) {
    device_path_ = config.device_path;

    if (fs::exists(device_path_)) {
        fs::remove(device_path_);
    }

    // Create Virtual Character FIFO Device Node
    if (mkfifo(device_path_.c_str(), 0666) < 0) {
        Logger::getInstance().log(LogLevel::WARNING, "Virtual FIFO node creation notice: " + std::string(strerror(errno)));
    }

    state_ = DeviceState::STOPPED;
    Logger::getInstance().log(LogLevel::SUCCESS, "Virtual Character Device initialized at: " + device_path_);
    return true;
}

bool DeviceManager::openDevice() {
    device_fd_ = open(device_path_.c_str(), O_RDWR | O_NONBLOCK);
    if (device_fd_ < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to open virtual device node: " + std::string(strerror(errno)));
        return false;
    }

    state_ = DeviceState::RUNNING;
    Logger::getInstance().log(LogLevel::INFO, "Opened virtual device file descriptor (fd=" + std::to_string(device_fd_) + ")");
    return true;
}

bool DeviceManager::writeData(const std::string& buffer) {
    if (device_fd_ < 0) return false;
    ssize_t bytes = write(device_fd_, buffer.c_str(), buffer.length());
    if (bytes > 0) {
        stats_.total_bytes_written += bytes;
        Logger::getInstance().log(LogLevel::DEVICE, "Wrote " + std::to_string(bytes) + " bytes to " + device_path_);
        return true;
    }
    return false;
}

std::string DeviceManager::readData() {
    if (device_fd_ < 0) return "";
    char buf[1024];
    std::memset(buf, 0, sizeof(buf));
    ssize_t bytes = read(device_fd_, buf, sizeof(buf) - 1);
    if (bytes > 0) {
        stats_.total_reads++;
        return std::string(buf);
    }
    return "";
}

bool DeviceManager::sendIoctl(unsigned long cmd, unsigned long arg) {
    stats_.total_ioctls++;
    switch (cmd) {
        case VDEV_IOCTL_START:
            state_ = DeviceState::RUNNING;
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_START (State: RUNNING)");
            return true;
        case VDEV_IOCTL_STOP:
            state_ = DeviceState::STOPPED;
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_STOP (State: STOPPED)");
            return true;
        case VDEV_IOCTL_RESET:
            state_ = DeviceState::RUNNING;
            stats_ = DeviceStats();
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_RESET (State: RESET, Stats Cleared)");
            return true;
        case VDEV_IOCTL_GET_STATS: {
            std::string stats_str = "STATS: BytesWritten=" + std::to_string(stats_.total_bytes_written) +
                                    ", Reads=" + std::to_string(stats_.total_reads) +
                                    ", IOCTLs=" + std::to_string(stats_.total_ioctls) +
                                    ", Queries=" + std::to_string(stats_.total_queries);
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_GET_STATS -> " + stats_str);
            return true;
        }
        case VDEV_IOCTL_SET_RATE:
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_SET_RATE -> New Interval=" +
                                      std::to_string(arg) + "ms");
            return true;
        default:
            Logger::getInstance().log(LogLevel::WARNING, "Unknown IOCTL Command: 0x" + std::to_string(cmd));
            return false;
    }
}

std::string DeviceManager::processQueryCommand(const std::string& cmd, const SystemTelemetry& telemetry) {
    stats_.total_queries++;
    std::string trimmed = cmd;
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\n'), trimmed.end());
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());

    if (trimmed == "GET_CPU") {
        return "CPU_PCT=" + std::to_string(telemetry.cpu_usage_pct);
    }
    if (trimmed == "GET_MEM") {
        return "MEM_USED=" + std::to_string(telemetry.memory_used_mb) + "MB (" +
               std::to_string(telemetry.memory_usage_pct) + "%)";
    }
    if (trimmed == "GET_LOAD") {
        return "LOAD_AVG=" + std::to_string(telemetry.load_1m) + "," +
               std::to_string(telemetry.load_5m) + "," + std::to_string(telemetry.load_15m);
    }
    if (trimmed == "GET_HEALTH") {
        return "HEALTH=" + telemetry.health_status;
    }
    if (trimmed == "GET_JSON") {
        return TelemetryMonitor::toJsonString(telemetry);
    }
    if (trimmed == "PING") {
        return "PONG";
    }

    return "ERROR: UNRECOGNIZED_COMMAND [" + trimmed + "]";
}

void DeviceManager::closeDevice() {
    if (device_fd_ >= 0) {
        close(device_fd_);
        device_fd_ = -1;
    }
    if (fs::exists(device_path_)) {
        fs::remove(device_path_);
    }
    state_ = DeviceState::STOPPED;
}
