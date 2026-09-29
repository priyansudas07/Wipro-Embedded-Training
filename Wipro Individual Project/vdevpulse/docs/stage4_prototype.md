# Stage 4: Implementation & Prototype

## 4.1 Directory Structure (Final Layout)

```
vdevpulse/
├── CMakeLists.txt                          ← CMake 3.14+ build & CTest config
├── README.md                               ← Project overview and quick-start guide
├── LICENSE                                 ← MIT License
├── configs/
│   └── vdev_policy.json                    ← Runtime device & telemetry policy
├── docs/
│   ├── stage1_introduction.md
│   ├── stage2_requirements_prd.md
│   ├── stage3_architecture.md
│   ├── stage4_prototype.md
│   ├── stage5_testing.md
│   ├── stage6_final_report.md
│   └── project_presentation_guide.md
├── include/
│   └── vdevpulse/
│       ├── logger.hpp                      ← Logger Singleton interface
│       ├── config.hpp                      ← VDevConfig struct + ConfigParser
│       ├── telemetry_monitor.hpp           ← SystemTelemetry struct + TelemetryMonitor
│       └── device_manager.hpp             ← DeviceState enum, IOCTL constants, DeviceManager
└── src/
│   ├── logger.cpp                          ← Logger implementation (mutex, ANSI, file output)
│   ├── config.cpp                          ← JSON policy string extractor
│   ├── telemetry_monitor.cpp              ← /proc FS parsers + dashboard renderer
│   ├── device_manager.cpp                 ← mkfifo, open, write, read, sendIoctl, close
│   └── main.cpp                           ← CLI dispatcher + signal handler + daemon loop
└── tests/
    ├── device_test.cpp                     ← DeviceManager unit test
    └── telemetry_test.cpp                  ← TelemetryMonitor unit test
```

---

## 4.2 Module Implementation Details

### 4.2.1 Logger — `logger.hpp` / `logger.cpp`

**Design Pattern**: Singleton (thread-safe via static local variable + `std::mutex`)

**Key Implementation Details**:
- `getInstance()` returns a static-local `Logger` object — guaranteed single initialization in C++11 and beyond.
- Every call to `log()` acquires `std::lock_guard<std::mutex>` before formatting or writing, ensuring no concurrent write corruption.
- Five `LogLevel` variants are mapped to distinct ANSI escape codes for color-coded terminal output:

| LogLevel | ANSI Code | Terminal Color |
| :--- | :--- | :--- |
| `INFO` | `\033[36m` | Cyan |
| `SUCCESS` | `\033[32m` | Green |
| `WARNING` | `\033[33m` | Yellow |
| `ERROR` | `\033[31m` | Red |
| `DEVICE` | `\033[35m` | Magenta |

- Log entry format: `[YYYY-MM-DD HH:MM:SS] [LEVEL] Message`
- Destructor closes `std::ofstream log_file_` if open (RAII).

---

### 4.2.2 ConfigParser & VDevConfig — `config.hpp` / `config.cpp`

**Approach**: Lightweight JSON string extraction using C++ lambda (no external JSON library dependency).

**`VDevConfig` Default Values**:
```json
{
  "device_name":              "vdevpulse",
  "device_path":              "/tmp/vdevpulse",
  "sampling_rate_ms":         1000,
  "enable_cpu_telemetry":     true,
  "enable_memory_telemetry":  true,
  "max_memory_threshold_mb":  4096
}
```

**`loadPolicy()` Behaviour**:
1. Checks file existence using `std::filesystem::exists()`.
2. If file is missing: logs a `WARNING` and returns `true` — daemon continues with compiled-in defaults.
3. If file exists: reads entire content into `std::string`, then uses `extractString()` lambda to locate key-value pairs by searching for `"key"` → `:` → value.

---

### 4.2.3 TelemetryMonitor — `telemetry_monitor.hpp` / `telemetry_monitor.cpp`

**All methods are `static`** — `TelemetryMonitor` acts as a stateless utility namespace-equivalent.

#### CPU Utilization Calculation
Reads the first `cpu` aggregate line from `/proc/stat`:
```
cpu  user  nice  system  idle  iowait  irq  softirq  steal  ...
```
CPU usage formula:
```
idle_time     = idle + iowait
non_idle_time = user + nice + system + irq + softirq + steal
total_time    = idle_time + non_idle_time

cpu_usage_pct = (non_idle_time / total_time) × 100.0
```

> **Note**: A production-grade implementation would store `T1` and compute delta ticks `(T2 – T1)` between samples. The current implementation computes instantaneous aggregate tick ratio from boot, which reports stable utilization values.

#### RAM Utilization Calculation
Reads three fields from `/proc/meminfo`:
```
MemTotal:       16384000 kB   → memory_total_mb = 16384000 / 1024
MemFree:         2048000 kB
MemAvailable:    8000000 kB   ← preferred (includes reclaimable caches)

memory_free_mb = MemAvailable / 1024  (falls back to MemFree if MemAvailable == 0)
memory_used_mb = memory_total_mb - memory_free_mb
memory_usage_pct = (memory_used_mb / memory_total_mb) × 100.0
```

#### System Uptime
Reads the first float from `/proc/uptime` (total uptime in fractional seconds), cast to `long` for integer second reporting.

#### Telemetry Dashboard Output
Rendered to `stdout` each sampling cycle:
```
======================================================
        VDEVPULSE SYSTEM TELEMETRY DASHBOARD
======================================================
  CPU Usage        : 14.37 %
  RAM Memory Usage : 7142 MB / 15966 MB (44.7 %)
  System Uptime    : 42837 seconds
------------------------------------------------------
```

---

### 4.2.4 DeviceManager — `device_manager.hpp` / `device_manager.cpp`

**Virtual Character Device Emulation** using POSIX named pipes (`mkfifo`):

#### `initDevice(const VDevConfig& config)`
1. Stores `device_path_` from config.
2. If a node already exists at that path: removes it using `std::filesystem::remove()`.
3. Calls `mkfifo(device_path_.c_str(), 0666)` to create the named pipe with read/write permissions.
4. Sets `state_ = DeviceState::STOPPED` and logs `SUCCESS`.

#### `openDevice()`
1. Calls `open(device_path_, O_RDWR | O_NONBLOCK)` to obtain a file descriptor.
2. `O_NONBLOCK` prevents blocking when no reader is connected on the other end.
3. Stores `device_fd_`. On failure: logs `ERROR` with `strerror(errno)`.
4. Sets `state_ = DeviceState::RUNNING`.

#### `writeData(const std::string& buffer)`
1. Validates `device_fd_ >= 0`.
2. Calls POSIX `write(device_fd_, buffer.c_str(), buffer.length())`.
3. Logs `DEVICE` level entry with byte count.

#### `readData()`
1. Calls POSIX `read(device_fd_, buf, 1023)` into a zero-initialized 1024-byte stack buffer.
2. Returns `std::string(buf)` on success, empty string on failure.

#### `sendIoctl(unsigned long cmd)`
Emulates kernel IOCTL dispatch with a `switch` statement:
| Command | Code | Action |
| :--- | :--- | :--- |
| `VDEV_IOCTL_START` | `0x8001` | Sets `state_ = RUNNING` |
| `VDEV_IOCTL_STOP` | `0x8002` | Sets `state_ = STOPPED` |
| `VDEV_IOCTL_RESET` | `0x8003` | Sets `state_ = RUNNING` (re-initializes) |

#### `closeDevice()` / `~DeviceManager()`
RAII destructor ensures cleanup:
1. Calls POSIX `close(device_fd_)` and resets `device_fd_ = -1`.
2. Removes the FIFO node using `std::filesystem::remove()` if it still exists.
3. Sets `state_ = STOPPED`.

---

### 4.2.5 Main Daemon — `main.cpp`

#### Signal Handling
```cpp
std::atomic<bool> vdev_running(true);
void signalHandler(int signum) { vdev_running = false; }
signal(SIGINT, signalHandler);
signal(SIGTERM, signalHandler);
```
`std::atomic<bool>` ensures the flag update from the signal handler is visible to the main loop thread without data races.

#### CLI Command Dispatch

| Command | Syntax | Behaviour |
| :--- | :--- | :--- |
| `run` | `vdevpulse run [policy.json]` | Full daemon loop: init device → collect telemetry → write to FIFO every 1000ms |
| `status` | `vdevpulse status` | Single telemetry snapshot printed to stdout |
| `write` | `vdevpulse write <message>` | Writes a string payload directly to the virtual device node |
| `ioctl` | `vdevpulse ioctl <start\|stop\|reset>` | Sends a virtual IOCTL command |

#### Main Daemon Loop
```cpp
while (vdev_running) {
    auto metrics = TelemetryMonitor::collectTelemetry();
    TelemetryMonitor::printTelemetryDashboard(metrics);
    std::string payload = "TELEMETRY_SAMPLE CPU=" + std::to_string(metrics.cpu_usage_pct)
                        + " MEM=" + std::to_string(metrics.memory_used_mb) + "MB";
    dev_mgr.writeData(payload);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
```

---

## 4.3 Development Issues & Resolutions Log

| Issue ID | Module | Problem Encountered | Root Cause | Solution Applied |
| :--- | :--- | :--- | :--- | :--- |
| **DEV-01** | `DeviceManager` | `open()` on FIFO blocks indefinitely | `O_RDONLY` / `O_WRONLY` requires both ends connected simultaneously | Changed to `O_RDWR | O_NONBLOCK` — opens without a waiting reader |
| **DEV-02** | `TelemetryMonitor` | CPU reading shows `0%` on first call | No previous tick snapshot for delta calculation | Documented as known limitation; aggregate tick ratio from boot is used |
| **DEV-03** | `ConfigParser` | Daemon crashes on missing policy file | `std::ifstream` opened without existence check | Added `std::filesystem::exists()` guard with graceful fallback |
| **DEV-04** | `Logger` | Log file not flushed on crash | `std::ofstream` not using `std::endl` flush | Changed all file writes to use `std::endl` (flushes on each write) |

---

## 4.4 Version Control & Progress Evidence

- **SDLC Phase**: Stage 4 — Prototype Implementation
- **Git Commit**: `[Stage 4] Prototype implementation of core C++17 system modules`
- **Evidence**: All `src/*.cpp` files, `CMakeLists.txt`, and `configs/vdev_policy.json` committed and verified to compile cleanly (`g++ -std=c++17 -Wall -Wextra`).

---

## 4.5 Demonstration Walkthrough

```bash
# Build the project
cd "Wipro Individual Project/vdevpulse"
mkdir -p build && cd build
cmake .. && make -j$(nproc)

# Run the full daemon
./vdevpulse run ../configs/vdev_policy.json

# In a second terminal — read device telemetry stream
cat /tmp/vdevpulse

# In a second terminal — send IOCTL commands
./vdevpulse ioctl start
./vdevpulse ioctl stop
./vdevpulse ioctl reset

# Single telemetry snapshot
./vdevpulse status

# Write a custom payload to the device
./vdevpulse write "CUSTOM_DIAGNOSTIC_PAYLOAD"

# Stop the daemon
Ctrl+C   ← triggers SIGINT → vdev_running = false → clean shutdown
```

---

## 4.6 Roadmap for Next Stage (Stage 5)

- Execute `device_test` and `telemetry_test` via `ctest --output-on-failure`
- Record full test pass output as progress evidence
- Complete test case specification matrix (TC-01 through TC-06)
- Identify and document any code quality improvements made after testing
