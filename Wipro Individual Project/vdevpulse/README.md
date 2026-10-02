# VDevPulse: Linux Virtual Device Interface & System Telemetry Monitor

VDevPulse is a high-performance C++17 system daemon that bridges Linux POSIX virtual character device I/O with kernel hardware telemetry monitoring (`/proc`).

## Architecture & Features

- **POSIX Virtual Device Driver Emulation**: Creates and manages FIFO stream channels simulating `/dev/vdevpulse` for non-blocking asynchronous user-space communication.
- **Kernel Telemetry Parser**: Monitors CPU utilization, RAM consumption, and uptime in real-time from kernel synthetic filesystems (`/proc/stat`, `/proc/meminfo`, `/proc/uptime`).
- **System Load & Process Tracking**: Extracts 1m, 5m, 15m load averages and active/total thread counts from `/proc/loadavg`.
- **Top Resource-Consuming Process Scanner**: Scans `/proc/[PID]/comm` and `/proc/[PID]/status` to isolate the top memory/CPU consumer daemons in real time.
- **Automated Threshold Alert Engine**: Policy-driven rule evaluation firing alerts and tagging system health state (`HEALTHY`, `WARNING_CPU_OVERLOAD`, `WARNING_MEMORY_PRESSURE`).
- **Interactive Device Query Protocol**: Supports targeted query commands (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING`) over device streams.
- **Structured JSON Streaming**: Export live metrics as compact JSON objects for direct integration with web dashboards or analytics tools.
- **Historical Ring Buffer**: In-memory circular buffer preserving recent telemetry snapshots.
- **Extended IOCTL Suite**: Virtual IOCTL commands (`START`, `STOP`, `RESET`, `GET_STATS`, `SET_RATE`) and runtime statistics accounting.
- **Thread-Safe Logging**: Synchronized multi-threaded logging system supporting debug, info, warning, error, and device severity levels.
- **Unit Testing**: Suite powered by standard C++ test fixtures with 100% CTest pass rate.

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

### 1. Launch Interactive Visual TUI Control Center
Opens the interactive terminal dashboard with real-time gauges, top process inspector, query dispatcher, and IOCTL control buttons in your terminal:
```bash
./vdevpulse menu
```
*(Also accessible via `./vdevpulse tui` or `./vdevpulse dashboard`)*

### 2. Launch Continuous Daemon Loop
Starts the live telemetry monitor and creates the virtual character device at `/tmp/vdevpulse`:
```bash
./vdevpulse run ../configs/vdev_policy.json
```
*(Supports `--json` for JSON output format. Press `Ctrl + C` for graceful shutdown)*

### 3. Read Telemetry Stream (Client Terminal)
While the daemon is running, open a separate terminal to read live telemetry directly from the virtual device node:
```bash
cat /tmp/vdevpulse
# Output: TELEMETRY_SAMPLE HEALTH=HEALTHY CPU=14.37% MEM=7142MB LOAD=0.45
```

### 4. Query Single Telemetry Snapshot
```bash
./vdevpulse status
# Or output in JSON format:
./vdevpulse status --json
```

### 5. Inspect Top Resource-Consuming Processes
```bash
./vdevpulse top 5
```

### 6. Interactive Device Queries
```bash
./vdevpulse query GET_CPU      # Output: CPU_PCT=14.37
./vdevpulse query GET_MEM      # Output: MEM_USED=7142MB (44.7%)
./vdevpulse query GET_LOAD     # Output: LOAD_AVG=0.45,0.30,0.15
./vdevpulse query GET_TOP      # Output: TOP_PROCESSES=[PID:229 unattended-upgr 31MB]
./vdevpulse query GET_HEALTH   # Output: HEALTH=HEALTHY
./vdevpulse query GET_JSON     # Outputs formatted JSON snapshot
./vdevpulse query PING         # Output: PONG
```

### 7. Inspect Historical Telemetry Buffer
```bash
./vdevpulse history
```

### 8. Send Virtual IOCTL Control Commands
```bash
./vdevpulse ioctl start    # Set virtual device state to RUNNING
./vdevpulse ioctl stop     # Set virtual device state to STOPPED
./vdevpulse ioctl reset    # Reset device state and statistics
./vdevpulse ioctl stats    # Query cumulative I/O statistics
```

### 9. Write Payload to Virtual Device
```bash
./vdevpulse write "PING_DIAGNOSTIC_SIGNAL"
```

### CLI Command Summary

| Command | Description |
| :--- | :--- |
| `./vdevpulse` | Displays usage and help menu |
| `./vdevpulse menu` | Launches the interactive visual TUI control center with button menus |
| `./vdevpulse run [policy.json] [--json]` | Starts the continuous live daemon loop (text or JSON format) |
| `./vdevpulse status [--json]` | Displays an instantaneous CPU, RAM, Load, Top Processes, and Health snapshot |
| `./vdevpulse top` | Displays the top 5 memory-consuming processes |
| `./vdevpulse history` | Displays the circular ring buffer of recent telemetry samples |
| `./vdevpulse query <CMD>` | Queries targeted metrics (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING`) |
| `./vdevpulse ioctl <start\|stop\|reset\|stats>` | Sends an IOCTL state or stats command |
| `./vdevpulse write <message>` | Writes a custom payload string to the virtual device node |
| `cat /tmp/vdevpulse` | Reads real-time telemetry stream from the device node |

## Execution Screenshots & Live Demonstration

### 1. Continuous Daemon Streaming & Telemetry Dashboard
![Continuous Daemon Streaming](docs/screenshots/vdevpulse_daemon_stream.png)

### 2. Instantaneous JSON Export (`status --json`)
![Structured JSON Output](docs/screenshots/vdevpulse_status_json.png)

### 3. Top Background Process Inspector (`top 5`)
![Top Background Process Scanner](docs/screenshots/vdevpulse_top_processes.png)

### 4. Interactive Synchronous Query Protocol (`query <CMD>`)
![Interactive Query Protocol](docs/screenshots/vdevpulse_queries.png)

### 5. In-Memory Historical Telemetry Ring Buffer (`history`)
![Historical Ring Buffer](docs/screenshots/vdevpulse_history.png)

## Documentation

- [Project Report - Complete Technical Defense (PDF - 6 Pages)](docs/VDevPulse_Project_Documentation.pdf)
- [Simple Documentation - Executive Summary (PDF - 3 Pages)](docs/VDevPulse_Simple_Documentation.pdf)

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
