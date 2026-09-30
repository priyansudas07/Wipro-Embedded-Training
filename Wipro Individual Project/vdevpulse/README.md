# VDevPulse: Linux Virtual Device Interface & System Telemetry Monitor

VDevPulse is a high-performance C++17 system daemon that bridges Linux POSIX virtual character device I/O with kernel hardware telemetry monitoring (`/proc`).

## Architecture & Features

- **POSIX Virtual Device Driver Emulation**: Creates and manages FIFO stream channels simulating `/dev/vdevpulse` for non-blocking asynchronous user-space communication.
- **Kernel Telemetry Parser**: Monitors CPU utilization and RAM consumption in real-time from kernel synthetic filesystems (`/proc/stat`, `/proc/meminfo`).
- **Policy Engine**: Dynamic JSON rule loader configuring sampling intervals, memory limits, and log destinations.
- **Thread-Safe Logging**: Synchronized multi-threaded logging system supporting debug, info, warning, and error severity levels.
- **Unit Testing**: Suite powered by standard C++ test fixtures.

## System Requirements

- Linux OS (Ubuntu 20.04/22.04 LTS, Debian, or WSL2)
- C++17 compliant compiler (`g++` >= 9.0 or `clang++` >= 10.0)
- CMake 3.14+
- Make or Ninja build toolchain

## Quick Start & Build Instructions

```bash
# Clone repository and navigate to project directory
git clone https://github.com/priyansudas07/Wipro-Embedded-Training.git
cd "Wipro-Embedded-Training/Wipro Individual Project/vdevpulse"

# Create build directory and run CMake
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Execute test suite
ctest --output-on-failure
```

## CLI Usage Guide

### 1. Launch Continuous Daemon Loop
Starts the live telemetry monitor and creates the virtual character device at `/tmp/vdevpulse`:
```bash
./vdevpulse run ../configs/vdev_policy.json
```
*(Press `Ctrl + C` for graceful shutdown)*

### 2. Read Telemetry Stream (Client Terminal)
While the daemon is running, open a separate terminal to read live telemetry directly from the virtual device node:
```bash
cat /tmp/vdevpulse
# Output: TELEMETRY_SAMPLE CPU=14.370000 MEM=7142MB
```

### 3. Query Single Telemetry Snapshot
```bash
./vdevpulse status
```

### 4. Send Virtual IOCTL Control Commands
```bash
./vdevpulse ioctl start    # Set virtual device state to RUNNING
./vdevpulse ioctl stop     # Set virtual device state to STOPPED
./vdevpulse ioctl reset    # Reset virtual device state
```

### 5. Write Payload to Virtual Device
```bash
./vdevpulse write "PING_DIAGNOSTIC_SIGNAL"
```

### CLI Command Summary

| Command | Description |
| :--- | :--- |
| `./vdevpulse` | Displays usage and help menu |
| `./vdevpulse run [policy.json]` | Starts the continuous live daemon loop |
| `./vdevpulse status` | Displays an instantaneous CPU, RAM, and Uptime snapshot |
| `./vdevpulse ioctl <start\|stop\|reset>` | Sends an IOCTL state management command |
| `./vdevpulse write <message>` | Writes a custom payload string to the virtual device node |
| `cat /tmp/vdevpulse` | Reads real-time telemetry stream from the device node |


## Documentation

- [Project Report (PDF)](docs/VDevPulse_Project_Documentation.pdf)

Full SDLC documentation and architecture diagrams are also available across the discrete stage files:

- [Stage 1: Project Overview & Objectives](docs/stage1_introduction.md)
- [Stage 2: Requirements Analysis & PRD](docs/stage2_requirements_prd.md)
- [Stage 3: System Architecture & UML Diagrams](docs/stage3_architecture.md)
- [Stage 4: Prototype Implementation](docs/stage4_prototype.md)
- [Stage 5: Verification & Testing Report](docs/stage5_testing.md)
- [Stage 6: Final Deployment & Maintenance Report](docs/stage6_final_report.md)
- [Executive Technical Presentation Guide](docs/project_presentation_guide.md)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
