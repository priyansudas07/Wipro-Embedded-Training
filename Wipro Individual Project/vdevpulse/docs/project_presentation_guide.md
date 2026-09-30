# VDevPulse: Executive Technical Presentation Guide

This guide is prepared for the final mentor evaluation and technical presentation of the VDevPulse capstone project. Use it to structure your 7–10 minute walkthrough.

---

## 1. Project Introduction (1–2 min)

**Project Name**: VDevPulse — Virtual Device Interface & System Telemetry Monitor

**One-line Summary**:
> VDevPulse is a Modern C++17 Linux system software application that creates a virtual POSIX character device interface at `/tmp/vdevpulse` while monitoring real-time CPU utilization, RAM pressure, system load averages, and top resource-consuming background processes directly from Linux kernel `/proc` filesystem interfaces.

---

## 2. Why I Built This Project & Practical Utility

### Why I Built This (Engineering Rationale)
1. **Low-Overhead Embedded Monitoring**: Edge devices (IoT gateways, Raspberry Pi, industrial automation controllers) have constrained CPU and memory. Running a heavy Python daemon or Prometheus agent (50–200 MB RSS) is wasteful. VDevPulse delivers the same core health metrics in under 15 MB RSS.
2. **Standard POSIX Device Abstraction**: Instead of writing a complex REST API or requiring external SDKs, VDevPulse implements the classic UNIX philosophy (*"everything is a file"*). Any process can query metrics by simply reading `/tmp/vdevpulse`.
3. **Pinpoint Runaway Processes**: Beyond high-level CPU/RAM percentages, VDevPulse scans `/proc/[PID]/` to isolate the exact background processes causing resource bottlenecks.
4. **Safe User-Space Emulation**: Developing kernel-space `.ko` modules carries kernel-panic risks and requires root privileges. Emulating character device behavior via user-space named pipes provides safety and 100% portability.

### How It Is Useful & Practical Applications
- **Embedded Health Watchdogs**: Supervisory daemons can poll `/tmp/vdevpulse` to detect runaway processes and trigger auto-recovery before system brownouts occur.
- **Zero-SDK Diagnostic Integration**: Any external application (written in C, C++, Python, Rust, or Bash) can query live metrics simply by reading the device node.
- **Dynamic Control & Query Protocol**: Supports virtual IOCTL commands (`START`, `STOP`, `RESET`, `GET_STATS`) and text queries (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`).

---

## 3. Architecture Overview (1–2 min)

**Four core modules**:

| Module | Responsibility |
| :--- | :--- |
| `Logger` | Thread-safe, color-coded logging (`std::mutex` + RAII) |
| `ConfigParser` | JSON policy loading with threshold definitions and safe fallback |
| `TelemetryMonitor` | Reads `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, `/proc/uptime`, `/proc/[PID]/`, JSON serialization, and history buffer |
| `DeviceManager` | POSIX FIFO lifecycle, query command protocol, IOCTL state machine, and I/O statistics |

---

## 4. Live Demonstration Script (3 min)

```bash
# Step 1: Build & Run Tests
cd "Wipro Individual Project/vdevpulse/build"
cmake .. && make -j$(nproc)
ctest --output-on-failure
# Expected: 100% tests passed, 0 tests failed out of 2

# Step 2: Show Instantaneous Telemetry Dashboard (includes Top Consumers)
./vdevpulse status

# Step 3: Inspect Top Resource Consuming Processes
./vdevpulse top

# Step 4: Show Structured JSON Output Mode
./vdevpulse status --json

# Step 5: Test Interactive Device Queries
./vdevpulse query GET_CPU
./vdevpulse query GET_LOAD
./vdevpulse query GET_TOP
./vdevpulse query GET_HEALTH

# Step 6: Query Device IOCTL Statistics
./vdevpulse ioctl stats

# Step 7: Launch Continuous Daemon Loop
./vdevpulse run ../configs/vdev_policy.json

# (In a second terminal) Read live stream:
cat /tmp/vdevpulse

# Stop daemon with Ctrl + C (clean shutdown)
```

---

## 5. Common Mentor Questions & Answers

### Q1: Why did you make this project?
**A**: To solve real-time resource visibility challenges on constrained embedded Linux hardware using pure C++17 and POSIX system calls, without incurring the heavy RAM overhead or external dependencies of tools like Python daemons or Prometheus.

### Q2: How does VDevPulse detect which background app is putting pressure on the CPU or RAM?
**A**: It iterates over active numeric PID subdirectories in `/proc`, extracting the process name from `/proc/[PID]/comm` and physical memory from `/proc/[PID]/status` (`VmRSS`). It sorts them in descending order to identify the top resource-consuming processes in real time.

### Q3: Why use `/proc` instead of a kernel driver or eBPF?
**A**: Reading `/proc` provides 100% portability across all Linux kernels ≥ 4.15 without needing root privileges for module insertion or fighting eBPF verifier restrictions. It retrieves hardware metrics with sub-millisecond latency and near-zero CPU overhead.

### Q4: How does the Threshold Alert Engine work?
**A**: `vdev_policy.json` defines alert thresholds (e.g., CPU > 85%, RAM > 90%). On every sampling cycle, `TelemetryMonitor` evaluates live metrics against these limits and updates `health_status`, logging warning alerts whenever resource limits are exceeded.

### Q5: How does the virtual device IOCTL work without a kernel driver?
**A**: We simulate kernel IOCTL semantics in user-space using a `switch` command dispatch in `DeviceManager::sendIoctl()`. The `VDEV_IOCTL_*` constants (`0x8001–0x8005`) mirror kernel IOCTL command codes, and the `DeviceState` enum tracks state transitions just like a real kernel character device.

---

## 6. Project Achievements at a Glance

| Feature | Implementation |
| :--- | :--- |
| Virtual device interface | `mkfifo("/tmp/vdevpulse")` with `O_RDWR\|O_NONBLOCK` |
| Multi-source telemetry | `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, `/proc/uptime` |
| Top process scanner | `/proc/[PID]/comm` and `/proc/[PID]/status` (VmRSS) scanner |
| Threshold Alert Engine | Policy-based rule evaluation with dynamic health status |
| Query protocol | `GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING` |
| JSON output stream | Compact structured JSON serialization |
| History ring buffer | In-memory circular buffer preserving recent samples |
| Extended IOCTL suite | `START`, `STOP`, `RESET`, `GET_STATS`, `SET_RATE` |
| Thread-safe logger | Singleton + `std::mutex` + ANSI colors + file output |
| Automated tests | 2 CTest suites; 100% pass; 14 test assertions |
| SDLC documentation | Full 6-stage engineering docs and PDF report |
