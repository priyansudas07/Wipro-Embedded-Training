# VDevPulse: Executive Technical Presentation Guide

This guide is prepared for the final mentor evaluation and technical presentation of the VDevPulse capstone project. Use it to structure your 7–10 minute walkthrough.

---

## 1. Project Introduction (1–2 min)

**Project Name**: VDevPulse — Virtual Device Interface & System Telemetry Monitor

**One-line Summary**:
> VDevPulse is a Modern C++17 Linux system software application that creates a virtual POSIX character device interface at `/tmp/vdevpulse` while monitoring real-time CPU utilization, RAM pressure, system load averages, and uptime directly from Linux kernel `/proc` filesystem interfaces, featuring an automated threshold alert engine and JSON export capabilities.

---

## 2. Why I Built This Project & Practical Utility

### Why I Built This (Engineering Rationale)
1. **Low-Overhead Embedded Monitoring**: Edge devices (IoT gateways, Raspberry Pi, industrial automation controllers) have constrained CPU and memory. Running a heavy Python daemon or Prometheus agent (50–200 MB RSS) is wasteful. VDevPulse delivers the same core health metrics in under 15 MB RSS.
2. **Standard POSIX Device Abstraction**: Instead of writing a complex REST API or requiring external SDKs, VDevPulse implements the classic UNIX philosophy (*"everything is a file"*). Any process can query metrics by simply reading `/tmp/vdevpulse`.
3. **Hands-On Linux Systems Programming**: Combines POSIX system calls (`mkfifo`, `open`, `read`, `write`, `close`), signal handling, `/proc` virtual filesystem parsing (`/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, `/proc/uptime`), and thread-safe C++17 OOP design into a cohesive system.
4. **Safe User-Space Emulation**: Developing kernel-space `.ko` modules carries kernel-panic risks and requires root privileges. Emulating character device behavior via user-space named pipes provides safety and 100% portability.

### How It Is Useful & Practical Applications
- **Embedded Health Watchdogs**: Supervisory daemons can poll `/tmp/vdevpulse` to detect runaway processes and trigger auto-recovery before system brownouts occur.
- **Zero-SDK Diagnostic Integration**: Any external application (written in C, C++, Python, Rust, or Bash) can query live metrics simply by reading the device node.
- **Dynamic Control & Query Protocol**: Supports virtual IOCTL commands (`START`, `STOP`, `RESET`, `GET_STATS`) and text queries (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_JSON`, `GET_HEALTH`).

---

## 3. Architecture Overview (1–2 min)

**Four core modules**:

| Module | Responsibility |
| :--- | :--- |
| `Logger` | Thread-safe, color-coded logging (`std::mutex` + RAII) |
| `ConfigParser` | JSON policy loading with threshold definitions and safe fallback |
| `TelemetryMonitor` | Reads `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, `/proc/uptime`, JSON serialization, and history buffer |
| `DeviceManager` | POSIX FIFO lifecycle, query command protocol, IOCTL state machine, and I/O statistics |

**Data Flow**:
```
JSON Policy → VDevConfig → DeviceManager (mkfifo /tmp/vdevpulse)
                                  ↑
      TelemetryMonitor ← /proc/stat, /proc/meminfo, /proc/loadavg, /proc/uptime
                                  ↓
      Stream/JSON payload written to FIFO → Client reads with cat /tmp/vdevpulse
```

---

## 4. Key Technical Concepts to Explain

### Virtual Character Device (`DeviceManager`)
- Uses `mkfifo()` to create a named pipe at `/tmp/vdevpulse`, simulating how Linux character device nodes work under `/dev/`.
- Opens with `O_RDWR | O_NONBLOCK` — avoids blocking the daemon when no reader is attached.
- Supports `writeData()`, `readData()`, `processQueryCommand()`, and virtual `sendIoctl()` with state tracking.

### System Load & Multi-Source Telemetry (`TelemetryMonitor`)
- **CPU Utilization**: Parsed from `/proc/stat` tick distribution.
- **RAM Pressure**: Parsed from `/proc/meminfo` prioritizing `MemAvailable`.
- **System Load Averages**: Parsed from `/proc/loadavg` reporting 1m, 5m, 15m load averages and active/total thread counts.
- **System Uptime**: Parsed from `/proc/uptime`.

### Automated Threshold Alert Engine
- Evaluates CPU and RAM utilization against configurable thresholds in `vdev_policy.json`.
- Dynamically sets health status (`HEALTHY`, `WARNING_CPU_OVERLOAD`, `WARNING_MEMORY_PRESSURE`, `CRITICAL_RESOURCE_PRESSURE`) and logs diagnostic warnings.

### Thread-Safe Logger & History Buffer
- Singleton pattern: `Logger::getInstance()` returns a static local instance.
- Every `log()` call acquires `std::lock_guard<std::mutex>` before writing.
- In-memory circular buffer (`std::deque`) records the last 60 telemetry snapshots for historical analysis.

---

## 5. Live Demonstration Script (3 min)

```bash
# Step 1: Build & Run Tests
cd "Wipro Individual Project/vdevpulse/build"
cmake .. && make -j$(nproc)
ctest --output-on-failure
# Expected: 100% tests passed, 0 tests failed out of 2

# Step 2: Show Instantaneous Telemetry Dashboard
./vdevpulse status

# Step 3: Show Structured JSON Output Mode
./vdevpulse status --json

# Step 4: Show Historical Telemetry Buffer
./vdevpulse history

# Step 5: Test Interactive Device Queries
./vdevpulse query GET_CPU
./vdevpulse query GET_LOAD
./vdevpulse query GET_HEALTH
./vdevpulse query PING

# Step 6: Query Device IOCTL Statistics
./vdevpulse ioctl stats

# Step 7: Launch Continuous Daemon Loop
./vdevpulse run ../configs/vdev_policy.json

# (In a second terminal) Read live stream:
cat /tmp/vdevpulse

# Stop daemon with Ctrl + C (clean shutdown)
```

---

## 6. Common Mentor Questions & Answers

### Q1: Why did you make this project?
**A**: To solve real-time resource visibility challenges on constrained embedded Linux hardware using pure C++17 and POSIX system calls, without incurring the heavy RAM overhead or external dependencies of tools like Python daemons or Prometheus.

### Q2: Why use `/proc` instead of a kernel driver or eBPF?
**A**: Reading `/proc` provides 100% portability across all Linux kernels ≥ 4.15 without needing root privileges for module insertion or fighting eBPF verifier restrictions. It retrieves hardware metrics with sub-millisecond latency and near-zero CPU overhead.

### Q3: How does the Threshold Alert Engine work?
**A**: `vdev_policy.json` defines alert thresholds (e.g., CPU > 85%, RAM > 90%). On every sampling cycle, `TelemetryMonitor` evaluates live metrics against these limits and updates `health_status`, logging warning alerts whenever resource limits are exceeded.

### Q4: How are System Load Averages extracted?
**A**: We parse `/proc/loadavg`, which provides the exponential moving average load over 1, 5, and 15 minutes along with the count of currently running threads over total scheduled threads.

### Q5: How does the query protocol work over the device node?
**A**: `DeviceManager::processQueryCommand()` parses command strings (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_JSON`, `GET_HEALTH`, `PING`) and returns targeted key-value strings or JSON data structures without requiring any custom network protocol.

---

## 7. Project Achievements at a Glance

| Feature | Implementation |
| :--- | :--- |
| Virtual device interface | `mkfifo("/tmp/vdevpulse")` with `O_RDWR\|O_NONBLOCK` |
| Multi-source telemetry | `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, `/proc/uptime` |
| Threshold Alert Engine | Policy-based rule evaluation with dynamic health status |
| Query protocol | `GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_JSON`, `GET_HEALTH`, `PING` |
| JSON output stream | Compact structured JSON serialization |
| History ring buffer | In-memory circular buffer preserving recent samples |
| Extended IOCTL suite | `START`, `STOP`, `RESET`, `GET_STATS`, `SET_RATE` |
| Thread-safe logger | Singleton + `std::mutex` + ANSI colors + file output |
| Automated tests | 2 CTest suites; 100% pass; 14 test assertions |
| SDLC documentation | Full 6-stage engineering docs and PDF report |
