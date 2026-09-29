#include "vdevpulse/device_manager.hpp"
#include "vdevpulse/telemetry_monitor.hpp"
#include "vdevpulse/logger.hpp"
#include "vdevpulse/config.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

std::atomic<bool> vdev_running(true);

void signalHandler(int signum) {
    (void)signum;
    vdev_running = false;
}

void printUsage() {
    std::cout << "======================================================\n";
    std::cout << "  VDevPulse — Virtual Device & Telemetry Monitor      \n";
    std::cout << "======================================================\n";
    std::cout << "Usage:\n";
    std::cout << "  vdevpulse run [policy.json]        Start virtual device & telemetry loop\n";
    std::cout << "  vdevpulse status                   Inspect system telemetry & device state\n";
    std::cout << "  vdevpulse write <message>          Write payload to virtual device node\n";
    std::cout << "  vdevpulse ioctl <start|stop|reset> Send IOCTL command to virtual device\n";
    std::cout << "------------------------------------------------------\n";
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Logger::getInstance().init("vdevpulse.log");

    if (argc < 2) {
        printUsage();
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "run") {
        std::string config_file = (argc > 2) ? argv[2] : "configs/vdev_policy.json";
        VDevConfig config;
        ConfigParser::loadPolicy(config_file, config);

        DeviceManager dev_mgr;
        if (!dev_mgr.initDevice(config)) return 1;
        dev_mgr.openDevice();

        Logger::getInstance().log(LogLevel::INFO, "=== VDevPulse Telemetry & Virtual Device Engine Active ===");
        Logger::getInstance().log(LogLevel::INFO, "Virtual Device Node Created at: " + dev_mgr.getDevicePath());

        bool once = (argc > 3 && std::string(argv[3]) == "--once");

        while (vdev_running) {
            auto metrics = TelemetryMonitor::collectTelemetry();
            TelemetryMonitor::printTelemetryDashboard(metrics);

            std::string payload = "TELEMETRY_SAMPLE CPU=" + std::to_string(metrics.cpu_usage_pct) +
                                  " MEM=" + std::to_string(metrics.memory_used_mb) + "MB";
            dev_mgr.writeData(payload);

            if (once) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        Logger::getInstance().log(LogLevel::INFO, "VDevPulse Engine shutting down cleanly.");
        return 0;
    }

    if (cmd == "status") {
        auto metrics = TelemetryMonitor::collectTelemetry();
        TelemetryMonitor::printTelemetryDashboard(metrics);
        return 0;
    }

    if (cmd == "write") {
        if (argc < 3) {
            std::cout << "Error: Usage: vdevpulse write <message>" << std::endl;
            return 1;
        }
        std::string msg = argv[2];
        DeviceManager dev_mgr;
        VDevConfig cfg;
        dev_mgr.initDevice(cfg);
        dev_mgr.openDevice();
        dev_mgr.writeData(msg);
        return 0;
    }

    if (cmd == "ioctl") {
        if (argc < 3) {
            std::cout << "Error: Usage: vdevpulse ioctl <start|stop|reset>" << std::endl;
            return 1;
        }
        std::string sub = argv[2];
        DeviceManager dev_mgr;
        VDevConfig cfg;
        dev_mgr.initDevice(cfg);

        if (sub == "start") dev_mgr.sendIoctl(VDEV_IOCTL_START);
        else if (sub == "stop") dev_mgr.sendIoctl(VDEV_IOCTL_STOP);
        else if (sub == "reset") dev_mgr.sendIoctl(VDEV_IOCTL_RESET);
        else std::cout << "Unknown IOCTL command: " << sub << std::endl;
        return 0;
    }

    printUsage();
    return 0;
}
