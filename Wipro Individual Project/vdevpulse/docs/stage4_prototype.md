# Stage 4: Implementation & Prototype

## 4.1 Directory Structure (Final Layout)

```
vdevpulse/
├── CMakeLists.txt                          ← CMake 3.14+ build & CTest config
├── README.md                               ← Project overview and quick-start guide
├── LICENSE                                 ← MIT License
├── configs/
│   └── vdev_policy.json                    ← Runtime device, threshold & format policy
├── docs/
│   ├── stage1_introduction.md
│   ├── stage2_requirements_prd.md
│   ├── stage3_architecture.md
│   ├── stage4_prototype.md
│   ├── stage5_testing.md
│   ├── stage6_final_report.md
│   ├── project_presentation_guide.md
│   └── VDevPulse_Project_Documentation.pdf ← Official compiled engineering report
├── include/
│   └── vdevpulse/
│       ├── logger.hpp                      ← Logger Singleton interface
│       ├── config.hpp                      ← VDevConfig struct + ConfigParser
│       ├── telemetry_monitor.hpp           ← SystemTelemetry struct + TelemetryMonitor + History
│       └── device_manager.hpp             ← DeviceState, DeviceStats, IOCTLs, DeviceManager
└── src/
│   ├── logger.cpp                          ← Logger implementation (mutex, ANSI, file output)
│   ├── config.cpp                          ← JSON policy parser with threshold rules
│   ├── telemetry_monitor.cpp              ← /proc FS parsers, health rules, JSON serializer, history
│   ├── device_manager.cpp                 ← mkfifo, open, write, read, IOCTLs, query protocol
│   └── main.cpp                           ← CLI dispatcher (run, status, history, query, write, ioctl)
└── tests/
    ├── device_test.cpp                     ← DeviceManager unit test suite
    └── telemetry_test.cpp                  ← TelemetryMonitor unit test suite
```

---

## 4.2 Module Implementation Details

### 4.2.1 Logger — `logger.hpp` / `logger.cpp`
- **Pattern**: Singleton (thread-safe static local initialization).
- **Synchronization**: `std::lock_guard<std::mutex>` on all logging calls.
- **Severity Channels**: `INFO`, `SUCCESS`, `WARNING`, `ERROR`, `DEVICE` with ANSI terminal color codes and persistent output to `vdevpulse.log`.

### 4.2.2 ConfigParser & VDevConfig — `config.hpp` / `config.cpp`
- Loads device path, sampling rate, CPU/RAM alert thresholds, and output format from `configs/vdev_policy.json`.
- Safely falls back to compiled-in default values if configuration file is missing.

### 4.2.3 TelemetryMonitor — `telemetry_monitor.hpp` / `telemetry_monitor.cpp`
- **CPU Utilization**: Parsed from `/proc/stat` tick distribution.
- **RAM Memory**: Parsed from `/proc/meminfo` prioritizing `MemAvailable`.
- **System Load & Threads**: Parsed from `/proc/loadavg` reporting 1m, 5m, 15m load averages and process thread counts.
- **Threshold Health Rule Evaluator**: Assigns dynamic `health_status` (`HEALTHY`, `WARNING_CPU_OVERLOAD`, `WARNING_MEMORY_PRESSURE`, `CRITICAL_RESOURCE_PRESSURE`).
- **JSON Serializer**: `toJsonString()` outputs compact structured JSON.
- **History Ring Buffer**: `std::deque` storing the last 60 telemetry snapshots for inspection (`vdevpulse history`).

### 4.2.4 DeviceManager — `device_manager.hpp` / `device_manager.cpp`
- **Virtual Character Device**: Creates named FIFO at `/tmp/vdevpulse` via `mkfifo()`.
- **Non-Blocking I/O**: Opens with `O_RDWR | O_NONBLOCK` to allow non-blocking writes.
- **Query Protocol**: `processQueryCommand()` handles targeted queries (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_JSON`, `GET_HEALTH`, `PING`).
- **Extended IOCTLs**: Handles `START`, `STOP`, `RESET`, `GET_STATS`, and `SET_RATE` while maintaining cumulative `DeviceStats`.

---

## 4.3 CLI Command Suite

| Command | Syntax | Behaviour |
| :--- | :--- | :--- |
| `run` | `vdevpulse run [policy.json] [--json]` | Continuous daemon loop streaming telemetry (text or JSON) to `/tmp/vdevpulse` |
| `status` | `vdevpulse status [--json]` | Instantaneous telemetry snapshot (dashboard or JSON format) |
| `history` | `vdevpulse history` | Displays circular ring buffer table of recent telemetry snapshots |
| `query` | `vdevpulse query <CMD>` | Interactive query (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_JSON`, `GET_HEALTH`, `PING`) |
| `write` | `vdevpulse write <message>` | Writes custom diagnostic string payload to the device node |
| `ioctl` | `vdevpulse ioctl <start\|stop\|reset\|stats>` | Sends virtual IOCTL command to manage device state and query I/O stats |

---

## 4.4 Version Control & Progress Evidence

- **SDLC Phase**: Stage 4 — Prototype Implementation
- **Git Commit**: `[Stage 4] Prototype implementation of core C++17 system modules`
- **Evidence**: All C++ source files compile cleanly (`-std=c++17 -Wall -Wextra`).
