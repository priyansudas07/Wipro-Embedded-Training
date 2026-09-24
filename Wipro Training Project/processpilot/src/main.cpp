#include "daemon.hpp"
#include "logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

std::atomic<bool> daemon_running(true);

void daemonSignalHandler(int signum) {
    (void)signum;
    daemon_running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, daemonSignalHandler);
    signal(SIGTERM, daemonSignalHandler);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    std::string config_file = "configs/demo.service";
    if (argc > 1) {
        config_file = argv[1];
    }

    ProcessPilotDaemon daemon;
    if (!daemon.initialize(config_file)) {
        std::cerr << "Failed to initialize ProcessPilot daemon." << std::endl;
        return 1;
    }

    bool once = (argc > 2 && std::string(argv[2]) == "--once");

    Logger::getInstance().log(LogLevel::INFO, "ProcessPilot Daemon is actively supervising services.");

    while (daemon_running) {
        daemon.runSingleLoop();
        if (once) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    Logger::getInstance().log(LogLevel::INFO, "ProcessPilot Daemon shutting down cleanly.");
    return 0;
}
