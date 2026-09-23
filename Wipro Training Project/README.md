# Wipro Training Projects

Main folder for Wipro COE Embedded Systems training projects.

## Included Projects

### Project 1: Embedded Linux Device Health Monitor & Auto-Recovery Agent

#### Objective
Build a C++ program that runs on Linux continuously in the background and behaves like a health supervisor for the device. The program monitors the Linux system and automatically reacts when something becomes unhealthy.

#### Live Execution Dashboard Screenshot
![Embedded Linux Device Health Dashboard](./embedded-linux-health-monitor/docs/dashboard_screenshot.png)

#### Monitored System Metrics (6 Health Monitors):
1. **CPU Usage**: Real-time `/proc/stat` CPU load computation.
2. **RAM Usage**: Memory & Swap utilization monitoring (`/proc/meminfo`, `sysinfo()`).
3. **Disk Usage**: File system storage utilization via POSIX `statvfs()`.
4. **CPU/System Temperature**: Reads thermal sensor values (`/sys/class/thermal/`).
5. **Network Status**: Traffic statistics (RX/TX bytes) and connection status (`/proc/net/dev`).
6. **Critical Linux Service/Process**: Process health checking with automated service restart auto-recovery.

- **Directory**: [`embedded-linux-health-monitor/`](./embedded-linux-health-monitor)
- **Language**: C++ (C++17)
- **Build System**: CMake

### How to Build & Run Project 1

```bash
cd embedded-linux-health-monitor
mkdir build && cd build
cmake ..
make -j4
./health_monitor ../config/health_monitor.json
```
