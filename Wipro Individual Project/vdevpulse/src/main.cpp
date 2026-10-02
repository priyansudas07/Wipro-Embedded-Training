#include "vdevpulse/device_manager.hpp"
#include "vdevpulse/telemetry_monitor.hpp"
#include "vdevpulse/logger.hpp"
#include "vdevpulse/config.hpp"
#include "vdevpulse/tui_dashboard.hpp"
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
    std::cout << "  vdevpulse menu                       Open interactive visual TUI control center\n";
    std::cout << "  vdevpulse run [policy.json] [--json] Start virtual device & telemetry loop\n";
    std::cout << "  vdevpulse status [--json]            Inspect current system telemetry\n";
    std::cout << "  vdevpulse top [limit]                Inspect top resource-consuming processes\n";
    std::cout << "  vdevpulse history                    Display accumulated telemetry history\n";
    std::cout << "  vdevpulse query <CMD>                Query virtual device (GET_CPU, GET_MEM, GET_LOAD, GET_TOP, GET_JSON, GET_HEALTH, PING)\n";
    std::cout << "  vdevpulse write <message>            Write payload to virtual device node\n";
    std::cout << "  vdevpulse ioctl <start|stop|reset|stats> Send IOCTL command to virtual device\n";
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

    if (cmd == "menu" || cmd == "tui" || cmd == "dashboard" || cmd == "--interactive") {
        VDevConfig config;
        if (argc > 2 && argv[2][0] != '-') {
            ConfigParser::loadPolicy(argv[2], config);
        } else {
            ConfigParser::loadPolicy("configs/vdev_policy.json", config);
        }
        TuiDashboard::runInteractiveLoop(config);
        return 0;
    }

    if (cmd == "run") {
        std::string config_file = "configs/vdev_policy.json";
        bool json_mode = false;
        bool once_mode = false;

        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--json") json_mode = true;
            else if (arg == "--once") once_mode = true;
            else if (arg[0] != '-') config_file = arg;
        }

        VDevConfig config;
        ConfigParser::loadPolicy(config_file, config);
        if (json_mode) config.output_format = "json";

        DeviceManager dev_mgr;
        if (!dev_mgr.initDevice(config)) return 1;
        dev_mgr.openDevice();

        Logger::getInstance().log(LogLevel::INFO, "=== VDevPulse Telemetry & Virtual Device Engine Active ===");
        Logger::getInstance().log(LogLevel::INFO, "Virtual Device Node Created at: " + dev_mgr.getDevicePath());

        while (vdev_running) {
            auto metrics = TelemetryMonitor::collectTelemetry(config);
            TelemetryMonitor::recordHistory(metrics);

            if (config.output_format == "json") {
                std::string json_str = TelemetryMonitor::toJsonString(metrics);
                std::cout << json_str << std::endl;
                dev_mgr.writeData(json_str);
            } else {
                TelemetryMonitor::printTelemetryDashboard(metrics);
                std::string payload = "TELEMETRY_SAMPLE HEALTH=" + metrics.health_status +
                                      " CPU=" + std::to_string(metrics.cpu_usage_pct) +
                                      "% MEM=" + std::to_string(metrics.memory_used_mb) + "MB" +
                                      " LOAD=" + std::to_string(metrics.load_1m);
                dev_mgr.writeData(payload);
            }

            if (once_mode) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(config.sampling_rate_ms));
        }

        Logger::getInstance().log(LogLevel::INFO, "VDevPulse Engine shutting down cleanly.");
        return 0;
    }

    if (cmd == "status") {
        VDevConfig cfg;
        auto metrics = TelemetryMonitor::collectTelemetry(cfg);
        if (argc > 2 && std::string(argv[2]) == "--json") {
            std::cout << TelemetryMonitor::toJsonString(metrics) << std::endl;
        } else {
            TelemetryMonitor::printTelemetryDashboard(metrics);
        }
        return 0;
    }

    if (cmd == "top") {
        auto procs = TelemetryMonitor::getTopProcesses(5);
        std::cout << "======================================================\n";
        std::cout << "        TOP PROCESSES BY MEMORY CONSUMPTION           \n";
        std::cout << "======================================================\n";
        std::cout << "PID     NAME                    RAM (MB)\n";
        std::cout << "------------------------------------------------------\n";
        for (const auto& p : procs) {
            std::cout << p.pid << "\t" << p.name;
            if (p.name.length() < 8) std::cout << "\t\t\t";
            else if (p.name.length() < 16) std::cout << "\t\t";
            else std::cout << "\t";
            std::cout << p.memory_rss_mb << " MB\n";
        }
        std::cout << "------------------------------------------------------\n";
        return 0;
    }

    if (cmd == "history") {
        VDevConfig cfg;
        for (int i = 0; i < 5; ++i) {
            auto m = TelemetryMonitor::collectTelemetry(cfg);
            TelemetryMonitor::recordHistory(m);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        TelemetryMonitor::printHistory();
        return 0;
    }

    if (cmd == "query") {
        if (argc < 3) {
            std::cout << "Usage: vdevpulse query <GET_CPU|GET_MEM|GET_LOAD|GET_TOP|GET_JSON|GET_HEALTH|PING>\n";
            return 1;
        }
        std::string query_cmd = argv[2];
        VDevConfig cfg;
        auto metrics = TelemetryMonitor::collectTelemetry(cfg);
        DeviceManager dev_mgr;
        std::string response = dev_mgr.processQueryCommand(query_cmd, metrics);
        std::cout << response << std::endl;
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
            std::cout << "Error: Usage: vdevpulse ioctl <start|stop|reset|stats>" << std::endl;
            return 1;
        }
        std::string sub = argv[2];
        DeviceManager dev_mgr;
        VDevConfig cfg;
        dev_mgr.initDevice(cfg);

        if (sub == "start") dev_mgr.sendIoctl(VDEV_IOCTL_START);
        else if (sub == "stop") dev_mgr.sendIoctl(VDEV_IOCTL_STOP);
        else if (sub == "reset") dev_mgr.sendIoctl(VDEV_IOCTL_RESET);
        else if (sub == "stats") dev_mgr.sendIoctl(VDEV_IOCTL_GET_STATS);
        else std::cout << "Unknown IOCTL command: " << sub << std::endl;
        return 0;
    }

    printUsage();
    return 0;
}
