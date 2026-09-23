#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

#include "logger.h"
#include "config_manager.h"
#include "cpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "temperature_monitor.h"
#include "network_monitor.h"
#include "service_monitor.h"

std::atomic<bool> running(true);

void signalHandler(int signum) {
    (void)signum;
    running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::string config_path = "config/health_monitor.json";
    if (argc > 1) {
        config_path = argv[1];
    }

    ConfigManager config_mgr;
    config_mgr.loadConfig(config_path);
    const auto& config = config_mgr.getConfig();

    Logger::getInstance().init(config.log_file);
    Logger::getInstance().log(LogLevel::INFO, "Embedded Linux Device Health Monitor Daemon started.");

    CpuMonitor cpu_mon;
    MemoryMonitor mem_mon;
    DiskMonitor disk_mon;
    TemperatureMonitor temp_mon;
    NetworkMonitor net_mon;
    ServiceMonitor service_mon;

    // Single pass run option for automated testing/logs
    bool single_run = (argc > 2 && std::string(argv[2]) == "--once");

    int run_count = 0;

    while (running) {
        double cpu_pct = cpu_mon.getCpuUsage();
        auto mem_stats = mem_mon.getMemoryStats();
        auto disk_stats = disk_mon.getDiskStats("/");
        double temp_c = temp_mon.getTemperatureCelsius();
        auto net_stats = net_mon.getNetworkStats();

        std::cout << "\n======================================================\n";
        std::cout << "         EMBEDDED LINUX DEVICE HEALTH DASHBOARD       \n";
        std::cout << "======================================================\n";
        std::cout << "  CPU Usage        : " << cpu_pct << " %\n";
        std::cout << "  Memory Usage     : " << mem_stats.used_ram_mb << " MB / " << mem_stats.total_ram_mb << " MB (" << mem_stats.used_percent << "%)\n";
        std::cout << "  Disk Usage       : " << disk_stats.used_gb << " GB / " << disk_stats.total_gb << " GB (" << disk_stats.used_percent << "%)\n";
        std::cout << "  Temperature      : " << temp_c << " °C\n";
        std::cout << "  Network (" << net_stats.interface_name << ")   : RX " << net_stats.rx_bytes / 1024 << " KB | TX " << net_stats.tx_bytes / 1024 << " KB\n";
        std::cout << "------------------------------------------------------\n";

        // Threshold checks & alerts
        if (cpu_pct > config.cpu_threshold) {
            Logger::getInstance().log(LogLevel::WARNING, "CPU usage threshold exceeded: " + std::to_string(cpu_pct) + "%");
        }
        if (mem_stats.used_percent > config.memory_threshold) {
            Logger::getInstance().log(LogLevel::WARNING, "Memory usage threshold exceeded: " + std::to_string(mem_stats.used_percent) + "%");
        }
        if (disk_stats.used_percent > config.disk_threshold) {
            Logger::getInstance().log(LogLevel::WARNING, "Disk usage threshold exceeded: " + std::to_string(disk_stats.used_percent) + "%");
        }
        if (temp_c > config.temp_threshold) {
            Logger::getInstance().log(LogLevel::WARNING, "Temperature threshold exceeded: " + std::to_string(temp_c) + "°C");
        }

        // Service health check & auto recovery
        service_mon.checkAndRecoverServices(config.services);

        if (single_run || ++run_count >= 1) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.poll_interval));
    }

    Logger::getInstance().log(LogLevel::INFO, "Embedded Linux Device Health Monitor Daemon shutting down cleanly.");
    return 0;
}
