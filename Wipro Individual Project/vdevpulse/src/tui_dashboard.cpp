#include "vdevpulse/tui_dashboard.hpp"
#include "vdevpulse/logger.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace {
    // ANSI color codes
    const std::string ANSI_RESET   = "\033[0m";
    const std::string ANSI_BOLD    = "\033[1m";
    const std::string ANSI_CYAN    = "\033[36m";
    const std::string ANSI_GREEN   = "\033[32m";
    const std::string ANSI_YELLOW  = "\033[33m";
    const std::string ANSI_RED     = "\033[31m";
    const std::string ANSI_BLUE    = "\033[34m";
    const std::string ANSI_MAGENTA = "\033[35m";
    const std::string ANSI_WHITE   = "\033[37m";

    // Cross-platform non-blocking key check for Linux
    bool kbhit_linux() {
#ifdef __linux__
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return true;
        }
#endif
        return false;
    }
}

void TuiDashboard::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void TuiDashboard::renderHeader() {
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "     VDevPulse — Interactive Virtual Device & Telemetry Control Center          \n"
              << "================================================================================\n"
              << ANSI_RESET;
}

void TuiDashboard::renderProgressBar(const std::string& label, double pct, int width) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 100.0) pct = 100.0;

    int filled = static_cast<int>((pct / 100.0) * width);
    std::string color = ANSI_GREEN;
    if (pct >= 85.0) color = ANSI_RED;
    else if (pct >= 65.0) color = ANSI_YELLOW;

    std::cout << std::left << std::setw(12) << label << " [";
    std::cout << color << ANSI_BOLD;
    for (int i = 0; i < width; ++i) {
        if (i < filled) std::cout << "#";
        else std::cout << "-";
    }
    std::cout << ANSI_RESET << "] " << std::fixed << std::setprecision(1) << pct << "%\n";
}

void TuiDashboard::renderTelemetryCard(const SystemTelemetry& t) {
    std::string health_color = ANSI_GREEN;
    if (t.health_status.find("CRITICAL") != std::string::npos) health_color = ANSI_RED;
    else if (t.health_status.find("WARNING") != std::string::npos) health_color = ANSI_YELLOW;

    std::cout << ANSI_BOLD << "+-- [ LIVE HARDWARE TELEMETRY ] " << std::string(48, '-') << "+\n" << ANSI_RESET;
    std::cout << "| System Health   : " << health_color << ANSI_BOLD << t.health_status << ANSI_RESET << "\n";
    
    std::cout << "| ";
    renderProgressBar("CPU Usage", t.cpu_usage_pct, 28);
    
    std::cout << "| ";
    renderProgressBar("RAM Memory", t.memory_usage_pct, 28);
    std::cout << "|   RAM Allocation: " << t.memory_used_mb << " MB / " << t.memory_total_mb << " MB\n";

    std::cout << "| System Load Avg : " << std::fixed << std::setprecision(2)
              << t.load_1m << " (1m), " << t.load_5m << " (5m), " << t.load_15m << " (15m)\n";
    std::cout << "| Process Threads : " << t.running_processes << " running / " << t.total_processes << " total\n";
    
    long hrs = t.uptime_seconds / 3600;
    long mins = (t.uptime_seconds % 3600) / 60;
    long secs = t.uptime_seconds % 60;
    std::cout << "| System Uptime   : " << hrs << "h " << mins << "m " << secs << "s (" << t.uptime_seconds << "s total)\n";
    std::cout << ANSI_BOLD << "+" << std::string(78, '-') << "+\n" << ANSI_RESET;
}

void TuiDashboard::renderMainMenu() {
    std::cout << ANSI_BOLD << ANSI_BLUE
              << "\n+-- [ INTERACTIVE ACTION MENU ] ---------------------------------------------+\n"
              << ANSI_RESET
              << "| " << ANSI_GREEN << ANSI_BOLD << "[1]" << ANSI_RESET << " Refresh Live Status Snapshot      | "
              << ANSI_GREEN << ANSI_BOLD << "[2]" << ANSI_RESET << " Scan Top Background Processes     |\n"
              << "| " << ANSI_GREEN << ANSI_BOLD << "[3]" << ANSI_RESET << " Interactive Query Protocol (M2M)  | "
              << ANSI_GREEN << ANSI_BOLD << "[4]" << ANSI_RESET << " Virtual Device IOCTL Control      |\n"
              << "| " << ANSI_GREEN << ANSI_BOLD << "[5]" << ANSI_RESET << " View 60-Sample History Buffer     | "
              << ANSI_GREEN << ANSI_BOLD << "[6]" << ANSI_RESET << " Real-Time Auto-Refresh Stream     |\n"
              << "| " << ANSI_GREEN << ANSI_BOLD << "[7]" << ANSI_RESET << " Send Payload to /tmp/vdevpulse    | "
              << ANSI_GREEN << ANSI_BOLD << "[8]" << ANSI_RESET << " Structured JSON Telemetry Export  |\n"
              << "| " << ANSI_RED   << ANSI_BOLD << "[Q]" << ANSI_RESET << " Exit Control Center               |                                  |\n"
              << ANSI_BOLD << ANSI_BLUE
              << "+----------------------------------------------------------------------------+\n"
              << ANSI_RESET;
    std::cout << ANSI_BOLD << "\nSelect an option [1-8, Q]: " << ANSI_RESET;
}

void TuiDashboard::showTopProcessesMenu() {
    clearScreen();
    renderHeader();
    std::cout << ANSI_BOLD << ANSI_YELLOW << "\n--- TOP BACKGROUND PROCESSES (MEMORY CONSUMPTION) ---\n" << ANSI_RESET;
    auto topList = TelemetryMonitor::getTopProcesses(10);
    
    std::cout << std::left << std::setw(8) << "PID"
              << std::setw(25) << "PROCESS NAME"
              << std::setw(15) << "RAM (MB)" << "\n";
    std::cout << std::string(50, '-') << "\n";
    
    for (const auto& p : topList) {
        std::cout << std::left << std::setw(8) << p.pid
                  << std::setw(25) << p.name
                  << std::setw(15) << (std::to_string(p.memory_rss_mb) + " MB") << "\n";
    }
    std::cout << "\nPress Enter to return to main menu...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showQueryMenu() {
    clearScreen();
    renderHeader();
    std::cout << ANSI_BOLD << ANSI_CYAN << "\n--- INTERACTIVE SYNCHRONOUS QUERY PROTOCOL ---\n" << ANSI_RESET;
    std::cout << "Available Query Commands:\n";
    std::cout << "  1. GET_CPU    (Query instantaneous CPU usage percentage)\n";
    std::cout << "  2. GET_MEM    (Query physical memory allocation in MB)\n";
    std::cout << "  3. GET_LOAD   (Query 1m, 5m, 15m load averages)\n";
    std::cout << "  4. GET_TOP    (Query top memory consumer process)\n";
    std::cout << "  5. GET_HEALTH (Query policy health evaluation)\n";
    std::cout << "  6. PING       (Send heartbeat ping)\n";
    std::cout << "  7. Custom Query String\n";
    std::cout << "  0. Return to Main Menu\n";
    std::cout << "\nChoose query [0-7]: ";

    int choice = -1;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    std::string queryCmd = "";
    switch (choice) {
        case 1: queryCmd = "GET_CPU"; break;
        case 2: queryCmd = "GET_MEM"; break;
        case 3: queryCmd = "GET_LOAD"; break;
        case 4: queryCmd = "GET_TOP"; break;
        case 5: queryCmd = "GET_HEALTH"; break;
        case 6: queryCmd = "PING"; break;
        case 7: {
            std::cout << "Enter custom query string: ";
            std::cin >> queryCmd;
            break;
        }
        default: return;
    }

    VDevConfig cfg;
    auto t = TelemetryMonitor::collectTelemetry(cfg);
    DeviceManager dev;
    std::string response = dev.processQueryCommand(queryCmd, t);

    std::cout << "\n" << ANSI_GREEN << ANSI_BOLD << ">>> QUERY  : " << ANSI_RESET << queryCmd << "\n";
    std::cout << ANSI_CYAN << ANSI_BOLD  << "<<< RESPONSE: " << ANSI_RESET << response << "\n";

    std::cout << "\nPress Enter to return...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showIoctlMenu() {
    clearScreen();
    renderHeader();
    std::cout << ANSI_BOLD << ANSI_MAGENTA << "\n--- VIRTUAL CHARACTER DRIVER IOCTL CONTROL ---\n" << ANSI_RESET;
    std::cout << "Available IOCTL Operations:\n";
    std::cout << "  1. START     (IOCTL 0x8001: Set device state to RUNNING)\n";
    std::cout << "  2. STOP      (IOCTL 0x8002: Set device state to STOPPED)\n";
    std::cout << "  3. RESET     (IOCTL 0x8003: Reset device state & zero statistics)\n";
    std::cout << "  4. GET_STATS (IOCTL 0x8004: Inspect cumulative I/O bytes & counters)\n";
    std::cout << "  0. Return to Main Menu\n";
    std::cout << "\nChoose IOCTL [0-4]: ";

    int choice = -1;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    DeviceManager dev;
    VDevConfig cfg;
    dev.initDevice(cfg);

    switch (choice) {
        case 1:
            dev.sendIoctl(VDEV_IOCTL_START);
            std::cout << ANSI_GREEN << "\n[SUCCESS] IOCTL 0x8001 (START) dispatched. State: RUNNING\n" << ANSI_RESET;
            break;
        case 2:
            dev.sendIoctl(VDEV_IOCTL_STOP);
            std::cout << ANSI_YELLOW << "\n[SUCCESS] IOCTL 0x8002 (STOP) dispatched. State: STOPPED\n" << ANSI_RESET;
            break;
        case 3:
            dev.sendIoctl(VDEV_IOCTL_RESET);
            std::cout << ANSI_CYAN << "\n[SUCCESS] IOCTL 0x8003 (RESET) dispatched. Counters zeroed.\n" << ANSI_RESET;
            break;
        case 4: {
            dev.sendIoctl(VDEV_IOCTL_GET_STATS);
            auto stats = dev.getStats();
            std::cout << "\n" << ANSI_BOLD << "--- CUMULATIVE IOCTL I/O STATISTICS ---\n" << ANSI_RESET;
            std::cout << "Bytes Written      : " << stats.total_bytes_written << " B\n";
            std::cout << "Total Reads        : " << stats.total_reads << "\n";
            std::cout << "Total Queries      : " << stats.total_queries << "\n";
            std::cout << "IOCTL Operations   : " << stats.total_ioctls << "\n";
            break;
        }
        default: return;
    }

    std::cout << "\nPress Enter to return...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showHistoryMenu() {
    clearScreen();
    renderHeader();
    std::cout << ANSI_BOLD << "\n--- IN-MEMORY HISTORICAL TELEMETRY BUFFER (LAST 60 SAMPLES) ---\n" << ANSI_RESET;
    TelemetryMonitor::printHistory();
    std::cout << "\nPress Enter to return to main menu...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showWritePayloadPrompt() {
    clearScreen();
    renderHeader();
    std::cout << ANSI_BOLD << "\n--- WRITE PAYLOAD TO VIRTUAL DEVICE (/tmp/vdevpulse) ---\n" << ANSI_RESET;
    std::cout << "Enter payload message string: ";
    std::string payload;
    std::cin.ignore(10000, '\n');
    std::getline(std::cin, payload);

    if (!payload.empty()) {
        DeviceManager dev;
        VDevConfig cfg;
        dev.initDevice(cfg);
        dev.openDevice();
        if (dev.writeData(payload)) {
            std::cout << ANSI_GREEN << "\n[SUCCESS] Wrote " << payload.length()
                      << " bytes to /tmp/vdevpulse: \"" << payload << "\"\n" << ANSI_RESET;
        } else {
            std::cout << ANSI_RED << "\n[ERROR] Failed to write to device node.\n" << ANSI_RESET;
        }
    }

    std::cout << "\nPress Enter to return...";
    std::cin.get();
}

void TuiDashboard::showLiveStreamMode(const VDevConfig& config) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN << "Starting live stream auto-refresh (sampling every "
              << config.sampling_rate_ms << "ms)..." << ANSI_RESET << "\n";
    std::cout << "Press [Enter] to return to menu.\n\n";
    
    for (int i = 0; i < 15; ++i) {
        clearScreen();
        renderHeader();
        auto t = TelemetryMonitor::collectTelemetry(config);
        TelemetryMonitor::recordHistory(t);
        renderTelemetryCard(t);

        std::cout << "\n" << ANSI_YELLOW << "Live Streaming Frame #" << (i + 1)
                  << " | Streaming to /tmp/vdevpulse | Press Enter anytime to exit." << ANSI_RESET << "\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config.sampling_rate_ms));
        if (kbhit_linux()) {
            std::cin.ignore(10000, '\n');
            break;
        }
    }

    std::cout << "\nStream paused. Press Enter to return to main menu...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::runInteractiveLoop(const VDevConfig& config) {
    bool running = true;
    while (running) {
        clearScreen();
        renderHeader();
        
        auto currentTelemetry = TelemetryMonitor::collectTelemetry(config);
        TelemetryMonitor::recordHistory(currentTelemetry);
        renderTelemetryCard(currentTelemetry);

        renderMainMenu();

        std::string choice;
        if (!(std::cin >> choice)) {
            break;
        }

        if (choice == "1") {
            continue;
        } else if (choice == "2") {
            showTopProcessesMenu();
        } else if (choice == "3") {
            showQueryMenu();
        } else if (choice == "4") {
            showIoctlMenu();
        } else if (choice == "5") {
            showHistoryMenu();
        } else if (choice == "6") {
            showLiveStreamMode(config);
        } else if (choice == "7") {
            showWritePayloadPrompt();
        } else if (choice == "8") {
            clearScreen();
            renderHeader();
            std::cout << ANSI_BOLD << "\n--- STRUCTURED JSON TELEMETRY PAYLOAD ---\n" << ANSI_RESET;
            std::cout << TelemetryMonitor::toJsonString(currentTelemetry) << "\n";
            std::cout << "\nPress Enter to return to main menu...";
            std::cin.ignore(10000, '\n');
            std::cin.get();
        } else if (choice == "q" || choice == "Q" || choice == "0") {
            running = false;
        }
    }

    clearScreen();
    std::cout << ANSI_BOLD << ANSI_GREEN << "Exiting VDevPulse Interactive Control Center. Goodbye!\n" << ANSI_RESET;
}
