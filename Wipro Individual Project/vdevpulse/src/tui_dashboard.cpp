#include "vdevpulse/tui_dashboard.hpp"
#include "vdevpulse/logger.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>
#include <vector>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace {
    // ANSI color codes
    const std::string ANSI_RESET   = "\033[0m";
    const std::string ANSI_BOLD    = "\033[1m";
    const std::string ANSI_DIM     = "\033[2m";
    const std::string ANSI_CYAN    = "\033[36m";
    const std::string ANSI_GREEN   = "\033[32m";
    const std::string ANSI_YELLOW  = "\033[33m";
    const std::string ANSI_RED     = "\033[31m";
    const std::string ANSI_BLUE    = "\033[34m";
    const std::string ANSI_MAGENTA = "\033[35m";
    const std::string ANSI_WHITE   = "\033[37m";

    // Non-blocking key hit for Linux terminal
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

void TuiDashboard::renderHeader(const VDevConfig& config) {
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "     VDevPulse v1.0 -- Linux Virtual Device & System Telemetry Center          \n"
              << "================================================================================\n"
              << ANSI_RESET;
    
    // Policy Bar (80 columns: "  " + 76 chars + "  ")
    std::ostringstream pbar;
    pbar << "Node: " << config.device_path 
         << " | CPU Alert: " << std::fixed << std::setprecision(0) << config.cpu_alert_threshold_pct << "%"
         << " | RAM Alert: " << config.memory_alert_threshold_pct << "%"
         << " | Sample: " << config.sampling_rate_ms << "ms";
    std::string pstr = pbar.str();
    if (pstr.length() < 76) {
        pstr.append(76 - pstr.length(), ' ');
    } else if (pstr.length() > 76) {
        pstr = pstr.substr(0, 76);
    }
    std::cout << ANSI_DIM << ANSI_WHITE << "  " << pstr << "  " << ANSI_RESET << "\n";
}

void TuiDashboard::renderProgressBar(const std::string& label, double pct, int width) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 100.0) pct = 100.0;

    int filled = static_cast<int>((pct / 100.0) * width);
    std::string color = ANSI_GREEN;
    if (pct >= 85.0) color = ANSI_RED;
    else if (pct >= 65.0) color = ANSI_YELLOW;

    std::cout << std::left << std::setw(11) << label << "[";
    std::cout << color << ANSI_BOLD;
    for (int i = 0; i < width; ++i) {
        if (i < filled) std::cout << "=";
        else std::cout << "-";
    }
    std::cout << ANSI_RESET << "] " << std::right << std::setw(5) << std::fixed << std::setprecision(1) << pct << "%";
}

void TuiDashboard::renderLiveTelemetryPanel(const SystemTelemetry& t, const DeviceManager& dev, const VDevConfig& config) {
    (void)config;
    std::string health_badge = ANSI_GREEN + ANSI_BOLD + "[ HEALTHY ]" + ANSI_RESET;
    std::string health_raw = "[ HEALTHY ]";
    if (t.health_status.find("CRITICAL") != std::string::npos) {
        health_badge = ANSI_RED + ANSI_BOLD + "[ CRITICAL ]" + ANSI_RESET;
        health_raw = "[ CRITICAL ]";
    } else if (t.health_status.find("WARNING") != std::string::npos) {
        health_badge = ANSI_YELLOW + ANSI_BOLD + "[ WARNING ]" + ANSI_RESET;
        health_raw = "[ WARNING ]";
    }

    // 1. LIVE HARDWARE TELEMETRY BOX
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "+-- [ LIVE HARDWARE & SYSTEM TELEMETRY ] --------------------------------------+\n"
              << ANSI_RESET;
    
    // Line 1: Health & Uptime (76 chars inner)
    long hrs = t.uptime_seconds / 3600;
    long mins = (t.uptime_seconds % 3600) / 60;
    long secs = t.uptime_seconds % 60;
    std::ostringstream uptime_ss;
    uptime_ss << "System Uptime : " << hrs << "h " << mins << "m " << secs << "s (" << t.uptime_seconds << "s)";
    std::string uptime_str = uptime_ss.str();

    std::cout << "| System Health : " << health_badge;
    int spaces_l1 = 76 - (16 + (int)health_raw.length()) - (int)uptime_str.length();
    if (spaces_l1 < 1) spaces_l1 = 1;
    std::cout << std::string(spaces_l1, ' ') << uptime_str << " |\n";

    // Line 2: CPU Bar & Load Averages
    std::ostringstream load_ss;
    load_ss << "Load (1/5/15) : " << std::fixed << std::setprecision(2)
            << t.load_1m << ", " << t.load_5m << ", " << t.load_15m;
    std::string load_str = load_ss.str();
    
    std::cout << "| ";
    renderProgressBar("CPU Usage", t.cpu_usage_pct, 20); // 11 + 1 + 20 + 2 + 6 = 40 chars
    int spaces_l2 = 76 - 40 - (int)load_str.length();
    if (spaces_l2 < 1) spaces_l2 = 1;
    std::cout << std::string(spaces_l2, ' ') << load_str << " |\n";

    // Line 3: RAM Bar & Thread Count
    std::ostringstream proc_ss;
    proc_ss << "Active Tasks  : " << t.running_processes << " run / " << t.total_processes << " total";
    std::string proc_str = proc_ss.str();

    std::cout << "| ";
    renderProgressBar("RAM Memory", t.memory_usage_pct, 20); // 40 chars
    int spaces_l3 = 76 - 40 - (int)proc_str.length();
    if (spaces_l3 < 1) spaces_l3 = 1;
    std::cout << std::string(spaces_l3, ' ') << proc_str << " |\n";

    // Line 4: RAM Detail
    std::ostringstream ram_ss;
    ram_ss << "RAM Allocation: " << t.memory_used_mb << " MB used / " << t.memory_total_mb << " MB total";
    std::string ram_str = ram_ss.str();
    int spaces_l4 = 76 - (int)ram_str.length();
    if (spaces_l4 < 1) spaces_l4 = 1;
    std::cout << "| " << ram_str << std::string(spaces_l4, ' ') << "|\n";

    std::cout << ANSI_BOLD << ANSI_CYAN
              << "+------------------------------------------------------------------------------+\n"
              << ANSI_RESET;

    // 2. LIVE VIRTUAL DEVICE DRIVER TELEMETRY BOX
    auto stats = dev.getStats();
    std::string dev_state_str = (dev.getState() == DeviceState::RUNNING) ? "RUNNING (Active)" : 
                                (dev.getState() == DeviceState::PAUSED)  ? "PAUSED" : "READY (Standby)";
    std::string dev_color = (dev.getState() == DeviceState::RUNNING) ? ANSI_GREEN : ANSI_YELLOW;

    std::cout << ANSI_BOLD << ANSI_BLUE
              << "+-- [ LIVE VIRTUAL DEVICE TELEMETRY (/tmp/vdevpulse) ] ------------------------+\n"
              << ANSI_RESET;

    std::ostringstream b_ss;
    b_ss << "Bytes Processed : " << stats.total_bytes_written << " Bytes";
    std::string b_str = b_ss.str();

    std::cout << "| Driver State  : " << dev_color << ANSI_BOLD << dev_state_str << ANSI_RESET;
    int spaces_d1 = 76 - (16 + (int)dev_state_str.length()) - (int)b_str.length();
    if (spaces_d1 < 1) spaces_d1 = 1;
    std::cout << std::string(spaces_d1, ' ') << b_str << " |\n";

    std::ostringstream io_ss;
    io_ss << "Driver I/O Ops : " << stats.total_reads << " reads, " << stats.total_queries << " queries";
    std::string io_str = io_ss.str();

    std::ostringstream ioctl_ss;
    ioctl_ss << "IOCTL Operations: " << stats.total_ioctls << " calls";
    std::string ioctl_str = ioctl_ss.str();

    int spaces_d2 = 76 - (int)io_str.length() - (int)ioctl_str.length();
    if (spaces_d2 < 1) spaces_d2 = 1;
    std::cout << "| " << io_str << std::string(spaces_d2, ' ') << ioctl_str << " |\n";

    std::cout << ANSI_BOLD << ANSI_BLUE
              << "+------------------------------------------------------------------------------+\n"
              << ANSI_RESET;
}

void TuiDashboard::renderActionControlsMenu() {
    std::cout << ANSI_BOLD << ANSI_MAGENTA
              << "+-- [ ACTION CONTROLS & DIAGNOSTICS (NON-LIVE COMMANDS) ] ---------------------+\n"
              << ANSI_RESET
              << "| " << ANSI_YELLOW << ANSI_BOLD << "[1]" << ANSI_RESET << " Scan Top Memory Heavy Processes   | "
              << ANSI_YELLOW << ANSI_BOLD << "[2]" << ANSI_RESET << " Device IOCTL Command Control       |\n"
              << "| " << ANSI_YELLOW << ANSI_BOLD << "[3]" << ANSI_RESET << " Synchronous Query Protocol (M2M)  | "
              << ANSI_YELLOW << ANSI_BOLD << "[4]" << ANSI_RESET << " Write Custom Payload to Device     |\n"
              << "| " << ANSI_YELLOW << ANSI_BOLD << "[5]" << ANSI_RESET << " View 60-Sample History Buffer     | "
              << ANSI_YELLOW << ANSI_BOLD << "[6]" << ANSI_RESET << " Real-Time Auto-Refresh Stream      |\n"
              << "| " << ANSI_YELLOW << ANSI_BOLD << "[7]" << ANSI_RESET << " Structured JSON Telemetry Export   | "
              << ANSI_YELLOW << ANSI_BOLD << "[8]" << ANSI_RESET << " View / Check Policy Configuration  |\n"
              << "| " << ANSI_GREEN  << ANSI_BOLD << "[R]" << ANSI_RESET << " Refresh Live Telemetry Snapshot   | "
              << ANSI_RED    << ANSI_BOLD << "[Q]" << ANSI_RESET << " Exit Control Center to Shell       |\n"
              << ANSI_BOLD << ANSI_MAGENTA
              << "+------------------------------------------------------------------------------+\n"
              << ANSI_RESET;
    std::cout << ANSI_BOLD << "Enter Action Choice [1-8, R, Q]: " << ANSI_RESET;
}

void TuiDashboard::showTopProcessesMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "               DIAGNOSTICS: TOP RESOURCE-CONSUMING PROCESSES                    \n"
              << "================================================================================\n"
              << ANSI_RESET;
    
    auto topList = TelemetryMonitor::getTopProcesses(10);
    
    std::cout << ANSI_BOLD << std::left
              << "  " << std::setw(8) << "PID"
              << std::setw(32) << "PROCESS NAME"
              << std::setw(16) << "PHYSICAL RAM (MB)"
              << "STATUS" << "\n" << ANSI_RESET;
    std::cout << "  " << std::string(74, '-') << "\n";
    
    for (const auto& p : topList) {
        std::cout << "  " << std::left << std::setw(8) << p.pid
                  << std::setw(32) << p.name
                  << std::setw(16) << (std::to_string(p.memory_rss_mb) + " MB")
                  << ANSI_GREEN << "Active" << ANSI_RESET << "\n";
    }
    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showQueryMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "          MACHINE-TO-MACHINE (M2M) SYNCHRONOUS QUERY PROTOCOL                   \n"
              << "================================================================================\n"
              << ANSI_RESET;
    std::cout << "Select a protocol query opcode to dispatch to the virtual character driver:\n\n";
    std::cout << "  [1] GET_CPU    - Query instantaneous CPU usage percentage\n";
    std::cout << "  [2] GET_MEM    - Query physical memory consumption in MB\n";
    std::cout << "  [3] GET_LOAD   - Query 1m, 5m, 15m kernel load averages\n";
    std::cout << "  [4] GET_TOP    - Query highest memory consuming process\n";
    std::cout << "  [5] GET_HEALTH - Query policy rule evaluation status\n";
    std::cout << "  [6] PING       - Send driver heartbeat ping\n";
    std::cout << "  [7] Custom     - Send custom opcode string\n";
    std::cout << "  [0] Return     - Back to Main Dashboard\n";
    std::cout << "\nChoose Query Opcode [0-7]: ";

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

    std::cout << "\n" << ANSI_GREEN << ANSI_BOLD << ">>> QUERY DISPATCHED : " << ANSI_RESET << queryCmd << "\n";
    std::cout << ANSI_CYAN << ANSI_BOLD  << "<<< DRIVER RESPONSE  : " << ANSI_RESET << response << "\n";

    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showIoctlMenu(DeviceManager& dev) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_MAGENTA
              << "================================================================================\n"
              << "               VIRTUAL CHARACTER DRIVER IOCTL CONTROL PANEL                     \n"
              << "================================================================================\n"
              << ANSI_RESET;
    std::cout << "Available IOCTL Operations:\n\n";
    std::cout << "  [1] START     (IOCTL 0x8001: Transition driver state to RUNNING)\n";
    std::cout << "  [2] STOP      (IOCTL 0x8002: Transition driver state to STOPPED)\n";
    std::cout << "  [3] RESET     (IOCTL 0x8003: Reset driver state & zero I/O statistics)\n";
    std::cout << "  [4] GET_STATS (IOCTL 0x8004: Query internal cumulative counters)\n";
    std::cout << "  [0] Return to Main Dashboard\n";
    std::cout << "\nChoose IOCTL [0-4]: ";

    int choice = -1;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return;
    }

    switch (choice) {
        case 1:
            dev.sendIoctl(VDEV_IOCTL_START);
            std::cout << ANSI_GREEN << "\n[SUCCESS] IOCTL 0x8001 (START) executed. Driver State: RUNNING\n" << ANSI_RESET;
            break;
        case 2:
            dev.sendIoctl(VDEV_IOCTL_STOP);
            std::cout << ANSI_YELLOW << "\n[SUCCESS] IOCTL 0x8002 (STOP) executed. Driver State: STOPPED\n" << ANSI_RESET;
            break;
        case 3:
            dev.sendIoctl(VDEV_IOCTL_RESET);
            std::cout << ANSI_CYAN << "\n[SUCCESS] IOCTL 0x8003 (RESET) executed. Counters zeroed.\n" << ANSI_RESET;
            break;
        case 4: {
            dev.sendIoctl(VDEV_IOCTL_GET_STATS);
            auto stats = dev.getStats();
            std::cout << "\n" << ANSI_BOLD << "--- CUMULATIVE IOCTL DRIVER STATISTICS ---\n" << ANSI_RESET;
            std::cout << "Total Bytes Written : " << stats.total_bytes_written << " Bytes\n";
            std::cout << "Total Read Calls    : " << stats.total_reads << "\n";
            std::cout << "Total Query Ops     : " << stats.total_queries << "\n";
            std::cout << "IOCTL Invocations   : " << stats.total_ioctls << "\n";
            break;
        }
        default: return;
    }

    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showHistoryMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "         HISTORICAL TELEMETRY BUFFER & METRIC LOGS (LAST 60 SAMPLES)            \n"
              << "================================================================================\n"
              << ANSI_RESET;
    TelemetryMonitor::printHistory();
    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showWritePayloadPrompt(DeviceManager& dev) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_BLUE
              << "================================================================================\n"
              << "            WRITE CUSTOM PAYLOAD TO VIRTUAL DEVICE NODE                         \n"
              << "================================================================================\n"
              << ANSI_RESET;
    std::cout << "Target Node: " << dev.getDevicePath() << "\n\n";
    std::cout << "Enter payload message to write: ";
    std::string payload;
    std::cin.ignore(10000, '\n');
    std::getline(std::cin, payload);

    if (!payload.empty()) {
        dev.openDevice();
        if (dev.writeData(payload)) {
            std::cout << ANSI_GREEN << "\n[SUCCESS] Wrote " << payload.length()
                      << " bytes to " << dev.getDevicePath() << ": \"" << payload << "\"\n" << ANSI_RESET;
        } else {
            std::cout << ANSI_RED << "\n[ERROR] Failed to write payload to device node.\n" << ANSI_RESET;
        }
    }

    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.get();
}

void TuiDashboard::showPolicyConfigMenu(const VDevConfig& config) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "================================================================================\n"
              << "                  VIRTUAL DEVICE POLICY CONFIGURATION                           \n"
              << "================================================================================\n"
              << ANSI_RESET;
    std::cout << "Active Policy Parameters:\n\n";
    std::cout << "  Device Node Path         : " << config.device_path << "\n";
    std::cout << "  Device Name              : " << config.device_name << "\n";
    std::cout << "  Sampling Rate            : " << config.sampling_rate_ms << " ms\n";
    std::cout << "  CPU Alert Threshold      : " << config.cpu_alert_threshold_pct << " %\n";
    std::cout << "  Memory Alert Threshold   : " << config.memory_alert_threshold_pct << " %\n";
    std::cout << "  Output Format            : " << config.output_format << "\n\n";
    std::cout << "Rule Evaluation Matrix:\n";
    std::cout << "  - Status is WARNING  if CPU >= " << config.cpu_alert_threshold_pct << "% OR RAM >= " << config.memory_alert_threshold_pct << "%\n";
    std::cout << "  - Status is CRITICAL if CPU >= 95% OR RAM >= 95%\n";
    std::cout << "  - Status is HEALTHY  otherwise\n";

    std::cout << "\nPress Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showLiveStreamMode(const VDevConfig& config, DeviceManager& dev) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN << "Starting real-time live telemetry stream (rate: "
              << config.sampling_rate_ms << "ms)..." << ANSI_RESET << "\n";
    std::cout << "Press [Enter] anytime to pause stream and return to control center.\n\n";
    
    for (int frame = 1; frame <= 30; ++frame) {
        clearScreen();
        renderHeader(config);
        auto t = TelemetryMonitor::collectTelemetry(config);
        TelemetryMonitor::recordHistory(t);
        renderLiveTelemetryPanel(t, dev, config);

        std::cout << "\n" << ANSI_YELLOW << ANSI_BOLD
                  << ">>> LIVE STREAMING (Frame #" << frame << " / 30) | Press Enter to return to menu..." 
                  << ANSI_RESET << "\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config.sampling_rate_ms));
        if (kbhit_linux()) {
            std::cin.ignore(10000, '\n');
            break;
        }
    }

    std::cout << "\nStream paused. Press Enter to return to main dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::runInteractiveLoop(const VDevConfig& config) {
    DeviceManager dev;
    dev.initDevice(config);
    dev.openDevice();

    bool running = true;
    while (running) {
        clearScreen();
        renderHeader(config);
        
        auto currentTelemetry = TelemetryMonitor::collectTelemetry(config);
        TelemetryMonitor::recordHistory(currentTelemetry);
        renderLiveTelemetryPanel(currentTelemetry, dev, config);

        renderActionControlsMenu();

        std::string choice;
        if (!(std::cin >> choice)) {
            break;
        }

        if (choice == "r" || choice == "R") {
            continue;
        } else if (choice == "1") {
            showTopProcessesMenu();
        } else if (choice == "2") {
            showIoctlMenu(dev);
        } else if (choice == "3") {
            showQueryMenu();
        } else if (choice == "4") {
            showWritePayloadPrompt(dev);
        } else if (choice == "5") {
            showHistoryMenu();
        } else if (choice == "6") {
            showLiveStreamMode(config, dev);
        } else if (choice == "7") {
            clearScreen();
            std::cout << ANSI_BOLD << ANSI_CYAN
                      << "================================================================================\n"
                      << "                   STRUCTURED JSON TELEMETRY PAYLOAD                            \n"
                      << "================================================================================\n"
                      << ANSI_RESET;
            std::cout << TelemetryMonitor::toJsonString(currentTelemetry) << "\n";
            std::cout << "\nPress Enter to return to main dashboard...";
            std::cin.ignore(10000, '\n');
            std::cin.get();
        } else if (choice == "8") {
            showPolicyConfigMenu(config);
        } else if (choice == "q" || choice == "Q" || choice == "0") {
            running = false;
        }
    }

    clearScreen();
    std::cout << ANSI_BOLD << ANSI_GREEN << "Exiting VDevPulse Control Center. Virtual device closed cleanly. Goodbye!\n" << ANSI_RESET;
}
