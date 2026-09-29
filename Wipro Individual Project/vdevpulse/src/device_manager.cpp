#include "vdevpulse/device_manager.hpp"
#include "vdevpulse/logger.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>

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
        return std::string(buf);
    }
    return "";
}

bool DeviceManager::sendIoctl(unsigned long cmd) {
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
            Logger::getInstance().log(LogLevel::DEVICE, "IOCTL Command: VDEV_IOCTL_RESET (State: RESET)");
            return true;
        default:
            Logger::getInstance().log(LogLevel::WARNING, "Unknown IOCTL Command: 0x" + std::to_string(cmd));
            return false;
    }
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
