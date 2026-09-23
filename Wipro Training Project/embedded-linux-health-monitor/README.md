# Embedded Linux Device Health Monitor & Auto-Recovery Agent

C++17 daemon for monitoring CPU, memory, disk, thermal sensors, network interfaces, and automated service recovery on Embedded Linux devices.

## Features
- Real-time CPU, RAM, Disk, Temperature, and Network metric monitoring.
- Automated service status checks and self-healing auto-recovery.
- Configurable thresholds via `config/health_monitor.json`.
- Systemd integration for background daemon execution.

## Building with CMake
```bash
mkdir build && cd build
cmake ..
make -j4
```

## Running Unit Tests
```bash
cd build
ctest --output-on-failure
```

## Running Health Monitor Daemon
```bash
./build/health_monitor config/health_monitor.json
```
