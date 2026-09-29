# VDevPulse: Executive Technical Presentation Guide

This guide is prepared for the final mentor evaluation and technical presentation of the VDevPulse capstone project. Use it to structure your 7–10 minute walkthrough.

---

## 1. Project Introduction (1–2 min)

**Project Name**: VDevPulse — Virtual Device Interface & System Telemetry Monitor

**One-line Summary**:
> VDevPulse is a Modern C++17 Linux system daemon that creates a virtual POSIX character device interface at `/tmp/vdevpulse` while monitoring real-time CPU, RAM, and system uptime telemetry directly from Linux kernel `/proc` filesystem interfaces.

---

## 2. Why I Built This Project & Practical Utility

### Why I Built This (Engineering Rationale)
1. **Low-Overhead Embedded Monitoring**: Edge devices (IoT gateways, Raspberry Pi, industrial automation controllers) have constrained CPU and memory. Running a heavy Python daemon or Prometheus agent (50–200 MB RSS) is wasteful. VDevPulse delivers the same core health metrics in under 15 MB RSS.
2. **Standard POSIX Device Abstraction**: Instead of writing a complex REST API or requiring external SDKs, VDevPulse implements the classic UNIX philosophy (*"everything is a file"*). Any process can query metrics by simply reading `/tmp/vdevpulse`.
3. **Hands-On Linux Systems Programming**: Combines POSIX system calls (`mkfifo`, `open`, `read`, `write`, `close`), signal handling, `/proc` virtual filesystem parsing, and thread-safe C++17 OOP design into a cohesive system.
4. **Safe User-Space Emulation**: Developing kernel-space `.ko` modules carries kernel-panic risks and requires root privileges. Emulating character device behavior via user-space named pipes provides safety and 100% portability.

### How It Is Useful & Practical Applications
- **Embedded Health Watchdogs**: Supervisory daemons can poll `/tmp/vdevpulse` to detect runaway processes and trigger auto-recovery before system brownouts occur.
- **Zero-Dependency Diagnostics**: Single standalone binary that runs on any Linux distribution (Kernel &ge; 4.15) with zero third-party library installations.
- **Dynamic Control**: Supports virtual IOCTL commands (`START`, `STOP`, `RESET`) to control data streaming dynamically at runtime.

---

## 3. Architecture Overview (1–2 min)

**Four core modules**:

| Module | Responsibility |
| :--- | :--- |
| `Logger` | Thread-safe, color-coded logging (`std::mutex` + RAII) |
| `ConfigParser` | JSON policy loading with safe fallback to defaults |
| `TelemetryMonitor` | Reads `/proc/stat`, `/proc/meminfo`, `/proc/uptime` |
| `DeviceManager` | POSIX FIFO lifecycle: `mkfifo` → `open` → `read/write` → `ioctl` → `close` |

**Data Flow**:
```
JSON Config → VDevConfig → DeviceManager (mkfifo /tmp/vdevpulse)
                                  ↑
            TelemetryMonitor ← /proc/stat, /proc/meminfo, /proc/uptime
                                  ↓
            Payload written to FIFO → User reads with cat /tmp/vdevpulse
```

---

## 4. Key Technical Concepts to Explain

### Virtual Character Device (`DeviceManager`)
- Uses `mkfifo()` to create a named pipe at `/tmp/vdevpulse`, simulating how Linux character device nodes work under `/dev/`.
- Opens with `O_RDWR | O_NONBLOCK` — avoids blocking the daemon when no reader is attached.
- Supports `writeData()`, `readData()`, and virtual `sendIoctl()` with state tracking (`STOPPED` / `RUNNING` / `PAUSED`).

### CPU Telemetry (`/proc/stat`)
- Reads the `cpu` aggregate line with fields: `user`, `nice`, `system`, `idle`, `iowait`, `irq`, `softirq`, `steal`.
- Formula:
  - `idle_time = idle + iowait`
  - `non_idle_time = user + nice + system + irq + softirq + steal`
  - `cpu_usage_pct = (non_idle_time / total_time) × 100`

### RAM Telemetry (`/proc/meminfo`)
- Extracts `MemTotal`, `MemFree`, `MemAvailable` (in kB).
- Uses `MemAvailable` (preferred — accounts for reclaimable cache) over `MemFree`.
- `memory_used_mb = (MemTotal – MemAvailable) / 1024`

### Thread-Safe Logger
- Singleton pattern: `Logger::getInstance()` returns a static local instance.
- Every `log()` call acquires `std::lock_guard<std::mutex>` before writing — safe for concurrent use.
- ANSI escape codes produce color-coded terminal output by severity level.

### Signal Handling
- `signal(SIGINT, signalHandler)` and `signal(SIGTERM, signalHandler)` registered in `main()`.
- Handler flips `std::atomic<bool> vdev_running = false`.
- Main loop exits; `DeviceManager` destructor (`~DeviceManager()`) automatically calls `closeDevice()` — RAII guarantee.

---

## 5. Live Demonstration Script (3 min)

```bash
# Step 1: Build
cd "Wipro Individual Project/vdevpulse/build"
cmake .. && make -j$(nproc)

# Step 2: Run all unit tests
ctest --output-on-failure
# Expected: 100% tests passed, 0 tests failed out of 2

# Step 3: Launch daemon
./vdevpulse run ../configs/vdev_policy.json

# Step 4: In a second terminal — read virtual device
cat /tmp/vdevpulse

# Step 5: Send IOCTL commands
./vdevpulse ioctl start
./vdevpulse ioctl stop
./vdevpulse ioctl reset

# Step 6: Single status snapshot
./vdevpulse status

# Step 7: Graceful shutdown
Ctrl+C    (observe "shutting down cleanly" log message)
```

---

## 6. Common Mentor Questions & Answers

### Q1: Why did you make this project?
**A**: To solve real-time resource visibility challenges on constrained embedded Linux hardware using pure C++17 and POSIX system calls, without incurring the heavy RAM overhead or external dependencies of tools like Python daemons or Prometheus.

### Q2: Why use `/proc` instead of a kernel driver or eBPF?
**A**: Reading `/proc` provides 100% portability across all Linux kernels ≥ 4.15 without needing root privileges for module insertion or fighting eBPF verifier restrictions. It retrieves hardware metrics with sub-millisecond latency and near-zero CPU overhead.

### Q3: Why is this better than just running `top` or a Python script?
**A**: `top` is interactive and cannot be queried as a standard device stream. A Python daemon consumes 50–200 MB RAM and requires an interpreter runtime. VDevPulse runs in under 15 MB RSS with zero runtime dependencies and exposes a standard POSIX device I/O interface.

### Q4: How does thread safety work in the Logger?
**A**: Every call to `Logger::log()` acquires `std::lock_guard<std::mutex>` in its first line. The lock guard is RAII — it locks `mutex_` on construction and automatically releases it when the guard goes out of scope, even if an exception is thrown.

### Q5: How is CPU % calculated from `/proc/stat`?
**A**: We read the `cpu` line from `/proc/stat` which gives cumulative tick counters since boot. We sum idle+iowait as idle time, and user+nice+system+irq+softirq+steal as active time. CPU% = (active / total) × 100.

### Q6: How does the virtual device IOCTL work without a kernel module?
**A**: We simulate kernel IOCTL semantics in user-space using a `switch` statement in `sendIoctl()`. The `VDEV_IOCTL_START/STOP/RESET` constants (`0x8001–0x8003`) mirror real kernel IOCTL command codes. The `DeviceState` enum tracks device state changes, just like a kernel driver would update internal state.

### Q7: How does the daemon shut down cleanly on Ctrl+C?
**A**: `signal(SIGINT, signalHandler)` registers a handler that sets `std::atomic<bool> vdev_running = false`. The `std::atomic` type guarantees the write is visible to the main loop thread without data races. When the loop exits, `DeviceManager`'s destructor runs (RAII), calling `close(fd)` and `std::filesystem::remove()` on the FIFO node.

---

## 7. Project Achievements at a Glance

| Feature | Implementation |
| :--- | :--- |
| Virtual device interface | `mkfifo("/tmp/vdevpulse")` with `O_RDWR|O_NONBLOCK` |
| CPU telemetry | `/proc/stat` tick parser, percentage calculation |
| RAM telemetry | `/proc/meminfo` `MemTotal`/`MemAvailable` parser |
| System uptime | `/proc/uptime` float parser |
| IOCTL commands | `0x8001` start, `0x8002` stop, `0x8003` reset |
| Thread-safe logger | Singleton + `std::mutex` + ANSI colors + file output |
| Signal handling | `SIGINT`/`SIGTERM` → `std::atomic<bool>` → RAII cleanup |
| JSON config | Custom string extractor, graceful fallback to defaults |
| Automated tests | 2 CTest suites; 100% pass; 9 test case assertions |
| SDLC documentation | 6 full stage documents with UML diagrams |
