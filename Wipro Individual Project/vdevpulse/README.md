# VDevPulse: Linux Virtual Device Interface & System Telemetry Monitor

VDevPulse is a high-performance C++17 system daemon that bridges Linux POSIX virtual character device I/O with kernel hardware telemetry monitoring (`/proc`).

---

## Architecture & Features

- **POSIX Virtual Character Driver Emulation**: Creates and manages FIFO stream channels simulating `/dev/vdevpulse` for non-blocking asynchronous user-space communication.
- **Interactive Real-Time TUI Control Center**: Zero-dependency ANSI/Unicode live terminal dashboard (`./vdevpulse menu`) featuring live CPU/RAM progress meters, system health status, and non-blocking instant hotkeys.
- **Kernel Telemetry Parser**: Monitors CPU utilization, RAM consumption, and uptime in real-time from kernel synthetic filesystems (`/proc/stat`, `/proc/meminfo`, `/proc/uptime`).
- **System Load & Process Tracking**: Extracts 1m, 5m, 15m load averages and active/total thread counts from `/proc/loadavg`.
- **Top Resource-Consuming Process Scanner**: Scans `/proc/[PID]/comm` and `/proc/[PID]/status` to isolate top memory/CPU consumer daemons in real time.
- **Automated Threshold Alert Engine**: Policy-driven rule evaluation firing alerts and tagging system health state (`HEALTHY`, `WARNING_CPU_OVERLOAD`, `WARNING_MEMORY_PRESSURE`, `CRITICAL_RESOURCE_PRESSURE`).
- **Dynamic Threshold Tuning**: Instant runtime policy threshold adjustments (`+` / `-` hotkeys) with live health badge transitions.
- **Interactive Device Query Protocol**: Supports targeted query commands (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING`) over device streams.
- **Live Traffic Burst Test Harness**: Built-in traffic injector (`[T]` hotkey) to stress-test FIFO packet pipelines and verify I/O throughput in real time.
- **Driver IOCTL Lifecycle State Machine**: Full driver state control (`RUNNING`, `PAUSED`, `STOPPED`) with state toggling (`[S]` hotkey) and runtime statistics accounting (`VDEV_IOCTL_START`, `STOP`, `RESET`, `GET_STATS`).
- **Historical Ring Buffer & ASCII Sparklines**: In-memory circular buffer with chronological ASCII trend sparklines for CPU and memory telemetry trajectories.
- **Structured JSON Streaming**: Export live metrics as compact JSON objects for direct integration with web dashboards or analytics tools.
- **Thread-Safe Logging Engine**: Synchronized multi-threaded logging system supporting debug, info, warning, error, and device severity levels.
- **Automated Unit Testing Suite**: 100% CTest pass rate across character device simulation and telemetry monitoring test suites.

---

## System Requirements

- Linux OS (Ubuntu 20.04/22.04 LTS, Debian, or WSL2)
- C++17 compliant compiler (`g++` >= 9.0 or `clang++` >= 10.0)
- CMake 3.14+
- Make or Ninja build toolchain

---

## Quick Start & Build Instructions

```bash
# Clone repository and navigate to project directory
git clone https://github.com/priyansudas07/Wipro-Embedded-Training.git
cd "Wipro-Embedded-Training/Wipro Individual Project/vdevpulse"

# Create build directory and run CMake
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Execute automated test suite
ctest --output-on-failure
```

---

## Interactive TUI Control Center (`./vdevpulse menu`)

Launch the real-time non-blocking terminal dashboard:
```bash
./vdevpulse menu
```
*(Also accessible via `./vdevpulse tui` or `./vdevpulse dashboard`)*

### **Live Dashboard Interface**
```text
╔══════════════════════════════════════════════════════════════════════════════╗
║     VDevPulse v1.0 -- Linux Virtual Device & System Telemetry Center         ║
╚══════════════════════════════════════════════════════════════════════════════╝
  Node: /tmp/vdevpulse │ CPU Alert: 85% │ RAM Alert: 90% │ Rate: 1000ms         
┌── [ LIVE HARDWARE & SYSTEM TELEMETRY (AUTO-REFRESHING) ] ────────────────────┐
│ System Health : ● [ HEALTHY ]              System Uptime : 1h 8m 46s (4126s) │
│ CPU Usage  [░░░░░░░░░░░░░░░░░░░░]   0.9%    Load (1/5/15) : 0.44, 0.21, 0.11 │
│ RAM Memory [█░░░░░░░░░░░░░░░░░░░]   7.2%   Active Tasks  : 1 run / 183 total │
│ RAM Allocation: 570 MB used / 7942 MB total                                  │
└──────────────────────────────────────────────────────────────────────────────┘
┌── [ LIVE VIRTUAL DEVICE TELEMETRY (/tmp/vdevpulse) ] ────────────────────────┐
│ Driver State  : RUNNING (Active)                   Bytes Processed : 0 Bytes │
│ Driver I/O Ops : 0 reads, 0 queries                IOCTL Operations: 1 calls │
└──────────────────────────────────────────────────────────────────────────────┘
┌── [ ACTION CONTROLS & DIAGNOSTICS (PRESS HOTKEY INSTANTLY) ] ────────────────┐
│ [1] Scan Top Heavy Processes         │ [2] Device IOCTL Command Control      │
│ [3] Synchronous Query Protocol (M2M) │ [4] Write Custom Payload to Device    │
│ [5] History Buffer & Sparklines      │ [6] Cycle Refresh Rate (500-2000ms)   │
│ [7] Structured JSON Telemetry Export │ [8] View Policy Configuration         │
│ [T] Inject Traffic Burst (10 Pkts)   │ [S] Toggle Driver State (RUN/PAUSE)   │
│ [+]/[-] Adjust CPU Alert Threshold   │ [Q] Exit Control Center to Shell      │
└──────────────────────────────────────────────────────────────────────────────┘
● [LIVE RUNNING] Hotkeys [1-8, T, S, +, -, Q] (No Enter required):
```

### **Instant Hotkey Reference**
| Hotkey | Action | Description |
| :---: | :--- | :--- |
| `[1]` | **Top Processes** | Scans `/proc` and displays top 10 memory-consuming processes. |
| `[2]` | **IOCTL Control** | Dispatches kernel IOCTL commands (`START`, `STOP`, `RESET`, `GET_STATS`). |
| `[3]` | **Query Protocol (M2M)** | Dispatches targeted query opcodes (`GET_CPU`, `GET_MEM`, `PING`, etc.). |
| `[4]` | **Write Payload** | Sends custom payload strings directly to `/tmp/vdevpulse`. |
| `[5]` | **History & Sparklines** | Displays 60-sample history buffer with ASCII trend sparklines. |
| `[6]` | **Cycle Rate** | Cycles live update frequency between **500ms**, **1000ms**, and **2000ms**. |
| `[7]` | **JSON Export** | Dumps structured JSON telemetry snapshot. |
| `[8]` | **Policy Config** | Displays active thresholds and rule evaluation matrix. |
| `[T]` | **Traffic Burst** | Injects a 10-packet burst into the virtual character device pipeline. |
| `[S]` | **Toggle State** | Toggles virtual driver between `RUNNING (Active)` $\longleftrightarrow$ `PAUSED (Standby)`. |
| `[+]` / `[-]` | **Adjust Alert Limit** | Dynamically modifies CPU alert threshold (triggers live `HEALTHY` $\leftrightarrow$ `WARNING`). |
| `[Q]` | **Exit** | Cleanly closes device descriptors and restores terminal cursor. |

---

## CLI Commands Guide

### 1. Launch Continuous Daemon Loop
Starts the live telemetry monitor and creates the virtual character device at `/tmp/vdevpulse`:
```bash
./vdevpulse run ../configs/vdev_policy.json
```
*(Supports `--json` for JSON output format. Press `Ctrl + C` for graceful shutdown)*

### 2. Read Telemetry Stream (Client Terminal)
While the daemon is running, open a separate terminal to read live telemetry directly from the virtual device node:
```bash
cat /tmp/vdevpulse
# Output: TELEMETRY_SAMPLE HEALTH=HEALTHY CPU=14.37% MEM=7142MB LOAD=0.45
```

### 3. Query Single Telemetry Snapshot
```bash
./vdevpulse status
# Or output in JSON format:
./vdevpulse status --json
```

### 4. Inspect Top Resource-Consuming Processes
```bash
./vdevpulse top 5
```

### 5. Interactive Device Queries
```bash
./vdevpulse query GET_CPU      # Output: CPU_PCT=14.37
./vdevpulse query GET_MEM      # Output: MEM_USED=7142MB (44.7%)
./vdevpulse query GET_LOAD     # Output: LOAD_AVG=0.45,0.30,0.15
./vdevpulse query GET_TOP      # Output: TOP_PROCESSES=[PID:229 unattended-upgr 31MB]
./vdevpulse query GET_HEALTH   # Output: HEALTH=HEALTHY
./vdevpulse query GET_JSON     # Outputs formatted JSON snapshot
./vdevpulse query PING         # Output: PONG
```

### 6. Inspect Historical Telemetry Buffer
```bash
./vdevpulse history
```

### 7. Send Virtual IOCTL Control Commands
```bash
./vdevpulse ioctl start    # Set virtual device state to RUNNING
./vdevpulse ioctl stop     # Set virtual device state to STOPPED
./vdevpulse ioctl reset    # Reset device state and statistics
./vdevpulse ioctl stats    # Query cumulative I/O statistics
```

### 8. Write Payload to Virtual Device
```bash
./vdevpulse write "PING_DIAGNOSTIC_SIGNAL"
```

---

## CLI Command Summary

| Command | Description |
| :--- | :--- |
| `./vdevpulse` | Displays usage and help menu |
| `./vdevpulse menu` | Launches the interactive visual TUI control center with non-blocking hotkeys |
| `./vdevpulse run [policy.json] [--json]` | Starts the continuous live daemon loop (text or JSON format) |
| `./vdevpulse status [--json]` | Displays an instantaneous CPU, RAM, Load, Top Processes, and Health snapshot |
| `./vdevpulse top [limit]` | Displays top resource-consuming background processes |
| `./vdevpulse history` | Displays the circular ring buffer of recent telemetry samples |
| `./vdevpulse query <CMD>` | Queries targeted metrics (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING`) |
| `./vdevpulse ioctl <start\|stop\|reset\|stats>` | Sends an IOCTL state or stats command |
| `./vdevpulse write <message>` | Writes a custom payload string to the virtual device node |
| `cat /tmp/vdevpulse` | Reads real-time telemetry stream from the device node |

---

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

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
