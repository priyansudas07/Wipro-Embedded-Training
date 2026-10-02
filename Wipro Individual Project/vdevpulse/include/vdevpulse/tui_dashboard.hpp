#pragma once

#include "vdevpulse/config.hpp"
#include "vdevpulse/telemetry_monitor.hpp"
#include "vdevpulse/device_manager.hpp"
#include <string>

class TuiDashboard {
public:
    static void runInteractiveLoop(const VDevConfig& config = VDevConfig());

private:
    static void clearScreen();
    static void renderHeader();
    static void renderTelemetryCard(const SystemTelemetry& t);
    static void renderProgressBar(const std::string& label, double pct, int width = 25);
    static void renderMainMenu();
    
    // Sub-screens
    static void showTopProcessesMenu();
    static void showQueryMenu();
    static void showIoctlMenu();
    static void showHistoryMenu();
    static void showLiveStreamMode(const VDevConfig& config);
    static void showWritePayloadPrompt();
};
