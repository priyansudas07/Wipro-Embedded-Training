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
    static void renderHeader(const VDevConfig& config);
    static void renderLiveTelemetryPanel(const SystemTelemetry& t, const DeviceManager& dev, const VDevConfig& config);
    static void renderProgressBar(const std::string& label, double pct, int width = 28);
    static void renderActionControlsMenu();
    
    // Sub-screens (Non-live Action Controls)
    static void showTopProcessesMenu();
    static void showQueryMenu();
    static void showIoctlMenu(DeviceManager& dev);
    static void showHistoryMenu();
    static void showLiveStreamMode(const VDevConfig& config, DeviceManager& dev);
    static void showWritePayloadPrompt(DeviceManager& dev);
    static void showPolicyConfigMenu(const VDevConfig& config);
};

