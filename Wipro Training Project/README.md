# Wipro Training Projects

Main folder for Wipro COE Embedded Systems training projects.

## Included Projects

### Project 1: Embedded Linux Device Health Monitor & Auto-Recovery Agent
- **Directory**: [`embedded-linux-health-monitor/`](./embedded-linux-health-monitor)
- **Language**: C++ (C++17)
- **Build System**: CMake
- **Description**: Real-time daemon monitoring CPU load, RAM, disk, thermal sensors, and network throughput with an automated service recovery engine (`systemctl` auto-restart). Includes systemd unit file, installation scripts, and unit tests.

### How to Build & Run Project 1

```bash
cd embedded-linux-health-monitor
mkdir build && cd build
cmake ..
make -j4
ctest --output-on-failure
./health_monitor ../config/health_monitor.json
```
