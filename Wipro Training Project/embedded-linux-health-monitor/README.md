# Embedded Linux Device Health Monitor & Auto-Recovery Agent

## Objective
Build a C++ program that runs on Linux continuously in the background and behaves like a health supervisor for the device. The program monitors the Linux system and automatically reacts when something becomes unhealthy.

## Monitored Health Metrics (6 System Monitors)
1. **CPU Usage**: Measures total CPU load percentage.
2. **RAM Usage**: Monitors Total, Free, and Used memory.
3. **Disk Usage**: Tracks disk space utilization on mounted file systems.
4. **CPU/System Temperature**: Reads system thermal zone sensors.
5. **Network Status**: Monitors network interface traffic (RX/TX) and connectivity.
6. **Critical Linux Service/Process**: Monitors system processes/services and triggers automated service recovery if inactive.

---

## Project Directory Structure

```text
embedded-linux-health-monitor/
├── CMakeLists.txt
├── config/
│   └── health_monitor.json
├── include/
│   ├── cpu_monitor.h
│   ├── memory_monitor.h
│   ├── disk_monitor.h
│   ├── temperature_monitor.h
│   ├── network_monitor.h
│   ├── service_monitor.h
│   ├── logger.h
│   └── config_manager.h
├── src/
│   ├── main.cpp
│   ├── cpu_monitor.cpp
│   ├── memory_monitor.cpp
│   ├── disk_monitor.cpp
│   ├── temperature_monitor.cpp
│   ├── network_monitor.cpp
│   ├── service_monitor.cpp
│   ├── logger.cpp
│   └── config_manager.cpp
├── systemd/
│   └── device-health-monitor.service
├── scripts/
│   ├── install.sh
│   ├── uninstall.sh
│   └── simulate_failure.sh
├── tests/
│   ├── test_cpu.cpp
│   ├── test_memory.cpp
│   ├── test_config.cpp
│   └── test_service_monitor.cpp
└── docs/
    ├── architecture.md
    └── test-report.md
```

---

## Build & Execution Instructions

### Build with CMake
```bash
mkdir build && cd build
cmake ..
make -j4
```

### Run Unit Tests
```bash
cd build
ctest --output-on-failure
```

### Run Health Monitor Daemon
```bash
./build/health_monitor config/health_monitor.json
```
