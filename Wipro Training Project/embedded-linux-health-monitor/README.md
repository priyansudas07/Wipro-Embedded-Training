# Embedded Linux Device Health Monitor & Auto-Recovery Agent

C++17 production-style daemon for monitoring system metrics (CPU, RAM, disk, temperature, network) and executing automated service recovery on Embedded Linux edge devices.

## Project Structure

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

## Quick Start

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

### Run Failure Simulation Script
```bash
./scripts/simulate_failure.sh
```
