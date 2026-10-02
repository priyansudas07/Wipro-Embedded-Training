#include "vdevpulse/tui_dashboard.hpp"
#include "vdevpulse/logger.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <sstream>
#include <vector>
#include <deque>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
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

    // Repeat a UTF-8 character string safely
    std::string repeatUtf8(const std::string& pattern, int count) {
        std::string result;
        if (count <= 0) return result;
        result.reserve(pattern.length() * count);
        for (int i = 0; i < count; ++i) {
            result += pattern;
        }
        return result;
    }

    // Calculate visual terminal column width of a string (ignoring ANSI codes & handling UTF-8 characters)
    int visibleWidth(const std::string& str) {
        int width = 0;
        bool in_ansi = false;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\033') {
                in_ansi = true;
            } else if (in_ansi) {
                if (str[i] == 'm') in_ansi = false;
            } else {
                unsigned char c = static_cast<unsigned char>(str[i]);
                // UTF-8 continuation bytes (0x80 to 0xBF) do not advance visual column width
                if ((c & 0xC0) != 0x80) {
                    width++;
                }
            }
        }
        return width;
    }

    // Pad string to exact visual terminal column width
    std::string padToWidth(const std::string& content, int target_width) {
        int vis = visibleWidth(content);
        if (vis < target_width) {
            return content + std::string(target_width - vis, ' ');
        }
        return content;
    }

    // Generate box header with exact 80 visual columns
    std::string makeBoxHeader(const std::string& title, const std::string& color_ansi = ANSI_CYAN) {
        int title_vis = visibleWidth(title);
        // Header format: "┌── [ " + title + " ] " + repeat("─", remaining) + "┐"
        // Total columns = 1 (┌) + 2 (──) + 3 ( [ ) + title_vis + 3 ( ] ) + rem + 1 (┐) = 10 + title_vis + rem = 80
        int rem = 80 - 10 - title_vis;
        if (rem < 1) rem = 1;
        return ANSI_BOLD + color_ansi + "┌── [ " + title + " ] " + repeatUtf8("─", rem) + "┐\n" + ANSI_RESET;
    }

    class TerminalMode {
    public:
        static void setRawMode() {
#ifdef __linux__
            if (!isatty(STDIN_FILENO)) return;
            tcgetattr(STDIN_FILENO, &orig_termios);
            struct termios raw = orig_termios;
            raw.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            raw_active = true;
#endif
        }

        static void restoreMode() {
#ifdef __linux__
            if (raw_active && isatty(STDIN_FILENO)) {
                tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
                raw_active = false;
            }
#endif
        }

        static char readKeyTimeout(int timeout_ms) {
#ifdef __linux__
            if (!isatty(STDIN_FILENO)) {
                char c = 0;
                if (std::cin >> c) return c;
                return 'q';
            }
            struct pollfd pfd;
            pfd.fd = STDIN_FILENO;
            pfd.events = POLLIN;
            int ret = poll(&pfd, 1, timeout_ms);
            if (ret > 0 && (pfd.revents & POLLIN)) {
                char c = 0;
                if (read(STDIN_FILENO, &c, 1) == 1) {
                    return c;
                }
            }
#else
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
#endif
            return 0;
        }

    private:
#ifdef __linux__
        inline static struct termios orig_termios;
        inline static bool raw_active = false;
#endif
    };
}

void TuiDashboard::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void TuiDashboard::renderHeader(const VDevConfig& config) {
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "╔══════════════════════════════════════════════════════════════════════════════╗\n"
              << "║     VDevPulse v1.0 -- Linux Virtual Device & System Telemetry Center         ║\n"
              << "╚══════════════════════════════════════════════════════════════════════════════╝\n"
              << ANSI_RESET;
    
    // Subtitle Bar
    std::ostringstream pbar;
    pbar << "Node: " << config.device_path 
         << " │ CPU Alert: " << std::fixed << std::setprecision(0) << config.cpu_alert_threshold_pct << "%"
         << " │ RAM Alert: " << config.memory_alert_threshold_pct << "%"
         << " │ Rate: " << config.sampling_rate_ms << "ms";
    
    std::cout << ANSI_DIM << ANSI_WHITE << "  " << padToWidth(pbar.str(), 76) << "  " << ANSI_RESET << "\n";
}

void TuiDashboard::renderProgressBar(const std::string& label, double pct, int width) {
    (void)label;
    (void)pct;
    (void)width;
}

static std::string makeProgressBar(const std::string& label, double pct, int width = 20) {
    if (pct < 0.0) pct = 0.0;
    if (pct > 100.0) pct = 100.0;

    int filled = static_cast<int>((pct / 100.0) * width);
    std::string color = ANSI_GREEN;
    if (pct >= 85.0) color = ANSI_RED;
    else if (pct >= 65.0) color = ANSI_YELLOW;

    std::ostringstream ss;
    ss << std::left << std::setw(11) << label << "[";
    ss << color << ANSI_BOLD;
    for (int i = 0; i < width; ++i) {
        if (i < filled) ss << "█";
        else ss << "░";
    }
    ss << ANSI_RESET << "] " << std::right << std::setw(5) << std::fixed << std::setprecision(1) << pct << "%";
    return ss.str();
}

void TuiDashboard::renderLiveTelemetryPanel(const SystemTelemetry& t, const DeviceManager& dev, const VDevConfig& config) {
    (void)config;
    std::string health_badge = ANSI_GREEN + ANSI_BOLD + "● [ HEALTHY ]" + ANSI_RESET;
    if (t.health_status.find("CRITICAL") != std::string::npos) {
        health_badge = ANSI_RED + ANSI_BOLD + "✖ [ CRITICAL ]" + ANSI_RESET;
    } else if (t.health_status.find("WARNING") != std::string::npos) {
        health_badge = ANSI_YELLOW + ANSI_BOLD + "▲ [ WARNING ]" + ANSI_RESET;
    }

    // 1. LIVE HARDWARE TELEMETRY BOX
    std::cout << makeBoxHeader("LIVE HARDWARE & SYSTEM TELEMETRY (AUTO-REFRESHING)", ANSI_CYAN);
    
    // Row 1: Health & Uptime
    long hrs = t.uptime_seconds / 3600;
    long mins = (t.uptime_seconds % 3600) / 60;
    long secs = t.uptime_seconds % 60;
    std::ostringstream r1_right;
    r1_right << "System Uptime : " << hrs << "h " << mins << "m " << secs << "s (" << t.uptime_seconds << "s)";
    
    std::string r1_left = "System Health : " + health_badge;
    int r1_pad = 76 - visibleWidth(r1_left) - visibleWidth(r1_right.str());
    std::string row1 = r1_left + (r1_pad > 0 ? std::string(r1_pad, ' ') : " ") + r1_right.str();
    std::cout << "│ " << padToWidth(row1, 76) << " │\n";

    // Row 2: CPU Bar & Load Averages
    std::string cpu_bar = makeProgressBar("CPU Usage", t.cpu_usage_pct, 20);
    std::ostringstream r2_right;
    r2_right << "Load (1/5/15) : " << std::fixed << std::setprecision(2)
             << t.load_1m << ", " << t.load_5m << ", " << t.load_15m;
    int r2_pad = 76 - visibleWidth(cpu_bar) - visibleWidth(r2_right.str());
    std::string row2 = cpu_bar + (r2_pad > 0 ? std::string(r2_pad, ' ') : " ") + r2_right.str();
    std::cout << "│ " << padToWidth(row2, 76) << " │\n";

    // Row 3: RAM Bar & Active Tasks
    std::string ram_bar = makeProgressBar("RAM Memory", t.memory_usage_pct, 20);
    std::ostringstream r3_right;
    r3_right << "Active Tasks  : " << t.running_processes << " run / " << t.total_processes << " total";
    int r3_pad = 76 - visibleWidth(ram_bar) - visibleWidth(r3_right.str());
    std::string row3 = ram_bar + (r3_pad > 0 ? std::string(r3_pad, ' ') : " ") + r3_right.str();
    std::cout << "│ " << padToWidth(row3, 76) << " │\n";

    // Row 4: RAM Detail
    std::ostringstream r4;
    r4 << "RAM Allocation: " << t.memory_used_mb << " MB used / " << t.memory_total_mb << " MB total";
    std::cout << "│ " << padToWidth(r4.str(), 76) << " │\n";

    std::cout << ANSI_BOLD << ANSI_CYAN
              << "└" << repeatUtf8("─", 78) << "┘\n"
              << ANSI_RESET;

    // 2. LIVE VIRTUAL DEVICE DRIVER TELEMETRY BOX
    auto stats = dev.getStats();
    std::string dev_state_str = (dev.getState() == DeviceState::RUNNING) ? "RUNNING (Active)" : 
                                (dev.getState() == DeviceState::PAUSED)  ? "PAUSED (Standby)" : "STOPPED (Halted)";
    std::string dev_color = (dev.getState() == DeviceState::RUNNING) ? ANSI_GREEN : ANSI_YELLOW;

    std::cout << makeBoxHeader("LIVE VIRTUAL DEVICE TELEMETRY (/tmp/vdevpulse)", ANSI_BLUE);

    std::string d1_left = "Driver State  : " + dev_color + ANSI_BOLD + dev_state_str + ANSI_RESET;
    std::ostringstream d1_right;
    d1_right << "Bytes Processed : " << stats.total_bytes_written << " Bytes";
    int d1_pad = 76 - visibleWidth(d1_left) - visibleWidth(d1_right.str());
    std::string dev_row1 = d1_left + (d1_pad > 0 ? std::string(d1_pad, ' ') : " ") + d1_right.str();
    std::cout << "│ " << padToWidth(dev_row1, 76) << " │\n";

    std::ostringstream d2_left;
    d2_left << "Driver I/O Ops : " << stats.total_reads << " reads, " << stats.total_queries << " queries";
    std::ostringstream d2_right;
    d2_right << "IOCTL Operations: " << stats.total_ioctls << " calls";
    int d2_pad = 76 - visibleWidth(d2_left.str()) - visibleWidth(d2_right.str());
    std::string dev_row2 = d2_left.str() + (d2_pad > 0 ? std::string(d2_pad, ' ') : " ") + d2_right.str();
    std::cout << "│ " << padToWidth(dev_row2, 76) << " │\n";

    std::cout << ANSI_BOLD << ANSI_BLUE
              << "└" << repeatUtf8("─", 78) << "┘\n"
              << ANSI_RESET;
}

void TuiDashboard::renderActionControlsMenu() {
    std::cout << makeBoxHeader("ACTION CONTROLS & DIAGNOSTICS (PRESS HOTKEY INSTANTLY)", ANSI_MAGENTA);

    auto printMenuRow = [](const std::string& col1, const std::string& col2) {
        std::string c1_padded = padToWidth(col1, 36);
        std::string c2_padded = padToWidth(col2, 37);
        std::cout << "│ " << c1_padded << " │ " << c2_padded << " │\n";
    };

    printMenuRow(ANSI_YELLOW + ANSI_BOLD + "[1]" + ANSI_RESET + " Scan Top Heavy Processes",
                 ANSI_YELLOW + ANSI_BOLD + "[2]" + ANSI_RESET + " Device IOCTL Command Control");

    printMenuRow(ANSI_YELLOW + ANSI_BOLD + "[3]" + ANSI_RESET + " Synchronous Query Protocol (M2M)",
                 ANSI_YELLOW + ANSI_BOLD + "[4]" + ANSI_RESET + " Write Custom Payload to Device");

    printMenuRow(ANSI_YELLOW + ANSI_BOLD + "[5]" + ANSI_RESET + " History Buffer & Sparklines",
                 ANSI_YELLOW + ANSI_BOLD + "[6]" + ANSI_RESET + " Cycle Refresh Rate (500-2000ms)");

    printMenuRow(ANSI_YELLOW + ANSI_BOLD + "[7]" + ANSI_RESET + " Structured JSON Telemetry Export",
                 ANSI_YELLOW + ANSI_BOLD + "[8]" + ANSI_RESET + " View Policy Configuration");

    printMenuRow(ANSI_CYAN   + ANSI_BOLD + "[T]" + ANSI_RESET + " Inject Traffic Burst (10 Pkts)",
                 ANSI_CYAN   + ANSI_BOLD + "[S]" + ANSI_RESET + " Toggle Driver State (RUN/PAUSE)");

    printMenuRow(ANSI_GREEN  + ANSI_BOLD + "[+]" + ANSI_RESET + "/" + ANSI_GREEN + ANSI_BOLD + "[-]" + ANSI_RESET + " Adjust CPU Alert Threshold",
                 ANSI_RED    + ANSI_BOLD + "[Q]" + ANSI_RESET + " Exit Control Center to Shell");

    std::cout << ANSI_BOLD << ANSI_MAGENTA
              << "└" << repeatUtf8("─", 78) << "┘\n"
              << ANSI_RESET;
    std::cout << ANSI_BOLD << ANSI_GREEN << "● [LIVE RUNNING]" << ANSI_RESET 
              << ANSI_BOLD << " Hotkeys [1-8, T, S, +, -, Q] (No Enter required): " << ANSI_RESET << std::flush;
}

void TuiDashboard::showTopProcessesMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "               DIAGNOSTICS: TOP RESOURCE-CONSUMING PROCESSES                    \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << ANSI_RESET;
    
    auto topList = TelemetryMonitor::getTopProcesses(10);
    
    std::cout << ANSI_BOLD << std::left
              << "  " << std::setw(8) << "PID"
              << std::setw(32) << "PROCESS NAME"
              << std::setw(16) << "PHYSICAL RAM (MB)"
              << "STATUS" << "\n" << ANSI_RESET;
    std::cout << "  " << repeatUtf8("─", 74) << "\n";
    
    for (const auto& p : topList) {
        std::cout << "  " << std::left << std::setw(8) << p.pid
                  << std::setw(32) << p.name
                  << std::setw(16) << (std::to_string(p.memory_rss_mb) + " MB")
                  << ANSI_GREEN << "Active" << ANSI_RESET << "\n";
    }
    std::cout << "\nPress Enter to return to live dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showQueryMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "          MACHINE-TO-MACHINE (M2M) SYNCHRONOUS QUERY PROTOCOL                   \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << ANSI_RESET;
    std::cout << "Select a protocol query opcode to dispatch to the virtual character driver:\n\n";
    std::cout << "  [1] GET_CPU    - Query instantaneous CPU usage percentage\n";
    std::cout << "  [2] GET_MEM    - Query physical memory consumption in MB\n";
    std::cout << "  [3] GET_LOAD   - Query 1m, 5m, 15m kernel load averages\n";
    std::cout << "  [4] GET_TOP    - Query highest memory consuming process\n";
    std::cout << "  [5] GET_HEALTH - Query policy rule evaluation status\n";
    std::cout << "  [6] PING       - Send driver heartbeat ping\n";
    std::cout << "  [7] Custom     - Send custom opcode string\n";
    std::cout << "  [0] Return     - Back to Live Dashboard\n";
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

    std::cout << "\nPress Enter to return to live dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showIoctlMenu(DeviceManager& dev) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_MAGENTA
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "               VIRTUAL CHARACTER DRIVER IOCTL CONTROL PANEL                     \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << ANSI_RESET;
    std::cout << "Available IOCTL Operations:\n\n";
    std::cout << "  [1] START     (IOCTL 0x8001: Transition driver state to RUNNING)\n";
    std::cout << "  [2] STOP      (IOCTL 0x8002: Transition driver state to STOPPED)\n";
    std::cout << "  [3] RESET     (IOCTL 0x8003: Reset driver state & zero I/O statistics)\n";
    std::cout << "  [4] GET_STATS (IOCTL 0x8004: Query internal cumulative counters)\n";
    std::cout << "  [0] Return to Live Dashboard\n";
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

    std::cout << "\nPress Enter to return to live dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showHistoryMenu() {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_CYAN
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "         HISTORICAL TELEMETRY BUFFER & METRIC LOGS (LAST 60 SAMPLES)            \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << ANSI_RESET;

    const auto& hist = TelemetryMonitor::getHistory();

    // Render Sparklines
    std::cout << ANSI_BOLD << ANSI_YELLOW << "\n─── TELEMETRY TREND SPARKLINES (CHRONOLOGICAL) ───\n" << ANSI_RESET;
    std::cout << "CPU Trajectory : [";
    for (const auto& sample : hist) {
        double p = sample.cpu_usage_pct;
        if (p < 10.0) std::cout << ANSI_GREEN << " " << ANSI_RESET;
        else if (p < 30.0) std::cout << ANSI_GREEN << "▂" << ANSI_RESET;
        else if (p < 60.0) std::cout << ANSI_YELLOW << "▄" << ANSI_RESET;
        else if (p < 85.0) std::cout << ANSI_YELLOW << "▆" << ANSI_RESET;
        else std::cout << ANSI_RED << "█" << ANSI_RESET;
    }
    std::cout << "]\n";

    std::cout << "RAM Trajectory : [";
    for (const auto& sample : hist) {
        double p = sample.memory_usage_pct;
        if (p < 20.0) std::cout << ANSI_GREEN << "▂" << ANSI_RESET;
        else if (p < 50.0) std::cout << ANSI_GREEN << "▄" << ANSI_RESET;
        else if (p < 80.0) std::cout << ANSI_YELLOW << "▆" << ANSI_RESET;
        else std::cout << ANSI_RED << "█" << ANSI_RESET;
    }
    std::cout << "]\n";
    std::cout << "Legend:  =Idle(0-10%)  ▂=Low(10-30%)  ▄=Med(30-60%)  ▆=High(60-85%)  █=Alert(>85%)\n\n";

    TelemetryMonitor::printHistory();
    std::cout << "\nPress Enter to return to live dashboard...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}

void TuiDashboard::showWritePayloadPrompt(DeviceManager& dev) {
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_BLUE
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "            WRITE CUSTOM PAYLOAD TO VIRTUAL DEVICE NODE                         \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
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
              << "════════════════════════════════════════════════════════════════════════════════\n"
              << "                  VIRTUAL DEVICE POLICY CONFIGURATION                           \n"
              << "════════════════════════════════════════════════════════════════════════════════\n"
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
    (void)config;
    (void)dev;
}

void TuiDashboard::runInteractiveLoop(const VDevConfig& config) {
    DeviceManager dev;
    dev.initDevice(config);
    dev.openDevice();
    dev.sendIoctl(VDEV_IOCTL_START); // Start driver in active running state

    VDevConfig current_config = config;

    // Initial clear screen
    clearScreen();
    std::cout << "\033[?25l" << std::flush; // Hide cursor for smooth rendering

    bool running = true;
    while (running) {
        // Redraw smoothly at top-left
        std::cout << "\033[H" << std::flush;
        
        renderHeader(current_config);
        
        auto currentTelemetry = TelemetryMonitor::collectTelemetry(current_config);
        TelemetryMonitor::recordHistory(currentTelemetry);
        renderLiveTelemetryPanel(currentTelemetry, dev, current_config);

        renderActionControlsMenu();

        // Non-blocking wait for keypress with timeout equal to sampling_rate_ms
        TerminalMode::setRawMode();
        char key = TerminalMode::readKeyTimeout(current_config.sampling_rate_ms);
        TerminalMode::restoreMode();

        if (key == 0) {
            // Timeout expired: loop naturally continues and re-renders live telemetry!
            continue;
        }

        // Key was pressed: process action
        std::cout << "\033[?25h" << std::flush; // Show cursor for sub-screens

        if (key == 'r' || key == 'R') {
            continue;
        } else if (key == 't' || key == 'T') {
            // Inject a live traffic burst of 10 telemetry packets
            for (int i = 1; i <= 10; ++i) {
                std::string pkt = "VDEV_PKT#" + std::to_string(i) + " sample_cpu=" + std::to_string(currentTelemetry.cpu_usage_pct) + "%";
                dev.writeData(pkt);
                dev.processQueryCommand("PING", currentTelemetry);
            }
            dev.sendIoctl(VDEV_IOCTL_GET_STATS);
        } else if (key == 's' || key == 'S') {
            // Toggle driver state between RUNNING and PAUSED
            if (dev.getState() == DeviceState::RUNNING) {
                dev.sendIoctl(VDEV_IOCTL_STOP);
            } else {
                dev.sendIoctl(VDEV_IOCTL_START);
            }
        } else if (key == '+' || key == '=') {
            // Increase CPU threshold (up to 100%)
            current_config.cpu_alert_threshold_pct += 5.0;
            if (current_config.cpu_alert_threshold_pct > 100.0) current_config.cpu_alert_threshold_pct = 100.0;
        } else if (key == '-' || key == '_') {
            // Decrease CPU threshold (down to 1%)
            current_config.cpu_alert_threshold_pct -= 5.0;
            if (current_config.cpu_alert_threshold_pct < 1.0) current_config.cpu_alert_threshold_pct = 1.0;
        } else if (key == '1') {
            showTopProcessesMenu();
            clearScreen();
        } else if (key == '2') {
            showIoctlMenu(dev);
            clearScreen();
        } else if (key == '3') {
            showQueryMenu();
            clearScreen();
        } else if (key == '4') {
            showWritePayloadPrompt(dev);
            clearScreen();
        } else if (key == '5') {
            showHistoryMenu();
            clearScreen();
        } else if (key == '6') {
            // Cycle refresh speed: 500ms -> 1000ms -> 2000ms -> 500ms
            if (current_config.sampling_rate_ms == 1000) current_config.sampling_rate_ms = 500;
            else if (current_config.sampling_rate_ms == 500) current_config.sampling_rate_ms = 2000;
            else current_config.sampling_rate_ms = 1000;
            clearScreen();
        } else if (key == '7') {
            clearScreen();
            std::cout << ANSI_BOLD << ANSI_CYAN
                      << "════════════════════════════════════════════════════════════════════════════════\n"
                      << "                   STRUCTURED JSON TELEMETRY PAYLOAD                            \n"
                      << "════════════════════════════════════════════════════════════════════════════════\n"
                      << ANSI_RESET;
            std::cout << TelemetryMonitor::toJsonString(currentTelemetry) << "\n";
            std::cout << "\nPress Enter to return to live dashboard...";
            std::cin.ignore(10000, '\n');
            std::cin.get();
            clearScreen();
        } else if (key == '8') {
            showPolicyConfigMenu(current_config);
            clearScreen();
        } else if (key == 'q' || key == 'Q') {
            running = false;
        }

        std::cout << "\033[?25l" << std::flush; // Hide cursor again for live loop
    }

    // Restore terminal & cursor
    TerminalMode::restoreMode();
    std::cout << "\033[?25h" << std::flush;
    clearScreen();
    std::cout << ANSI_BOLD << ANSI_GREEN << "Exiting VDevPulse Control Center. Virtual device closed cleanly. Goodbye!\n" << ANSI_RESET;
}
