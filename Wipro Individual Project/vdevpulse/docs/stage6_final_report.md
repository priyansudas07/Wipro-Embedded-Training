# Stage 6: Final Implementation & Presentation

## 6.1 Project Completion Summary

VDevPulse has successfully completed all six stages of the Software Development Life Cycle. The final deliverable is a fully functional, zero-dependency C++17 Linux system daemon providing a virtual character device interface and real-time kernel telemetry monitoring.

### SDLC Stage Completion Status

| Stage | Title | Status |
| :--- | :--- | :--- |
| **Stage 1** | Project Introduction — Problem, Scope, Objectives | Complete |
| **Stage 2** | Requirements & Development Plan — PRD, NFRs, Milestones | Complete |
| **Stage 3** | System Design & Architecture — UML, Data Structures, Toolchain | Complete |
| **Stage 4** | Implementation & Prototype — All modules, CLI, Signal handling | Complete |
| **Stage 5** | Testing, Integration & Improvement — CTest 100% pass, issues log | Complete |
| **Stage 6** | Final Delivery — System demo, deployment, achievements & limitations | Complete |

---

## 6.2 Final System Architecture Summary

```
CLI Entry: ./vdevpulse run|status|write|ioctl
              │
              ▼
          main.cpp
     ┌────────────────────────────────────────┐
     │  Signal Handler: SIGINT/SIGTERM        │
     │  std::atomic<bool> vdev_running        │
     └────────┬───────────────────────────────┘
              │
     ┌────────▼──────────┐    ┌───────────────────────┐
     │   ConfigParser    │    │       Logger           │
     │  vdev_policy.json │    │  5-level severity      │
     │  → VDevConfig     │    │  mutex + ANSI colors   │
     └────────┬──────────┘    └───────────────────────┘
              │
     ┌────────▼──────────┐    ┌───────────────────────┐
     │   DeviceManager   │◄───│  TelemetryMonitor      │
     │  mkfifo()         │    │  /proc/stat  → CPU%    │
     │  O_RDWR|O_NONBLOCK│    │  /proc/meminfo → RAM%  │
     │  IOCTL state      │    │  /proc/uptime → secs   │
     └────────┬──────────┘    └───────────────────────┘
              │
    /tmp/vdevpulse  (POSIX FIFO Node)
              │
    User-space: cat /tmp/vdevpulse
```

---

## 6.3 Complete System Demonstration

### 6.3.1 Build from Source
```bash
git clone https://github.com/priyansudas07/Wipro-Embedded-Training.git
cd "Wipro-Embedded-Training/Wipro Individual Project/vdevpulse"
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### 6.3.2 Run Unit Tests
```bash
ctest --output-on-failure
# Expected: 100% tests passed, 0 tests failed out of 2
```

### 6.3.3 Launch the Full Daemon
```bash
./vdevpulse run ../configs/vdev_policy.json
```
**Expected Terminal Output**:
```
[2026-09-29 23:10:01] [SUCCESS] Virtual Character Device initialized at: /tmp/vdevpulse
[2026-09-29 23:10:01] [INFO   ] Opened virtual device file descriptor (fd=3)
[2026-09-29 23:10:01] [INFO   ] === VDevPulse Telemetry & Virtual Device Engine Active ===
======================================================
        VDEVPULSE SYSTEM TELEMETRY DASHBOARD
======================================================
  CPU Usage        : 14.37 %
  RAM Memory Usage : 7142 MB / 15966 MB (44.7 %)
  System Uptime    : 42837 seconds
------------------------------------------------------
[2026-09-29 23:10:01] [DEVICE ] Wrote 42 bytes to /tmp/vdevpulse
```

### 6.3.4 Read Telemetry from Virtual Device
```bash
# In a second terminal:
cat /tmp/vdevpulse
# Output: TELEMETRY_SAMPLE CPU=14.373100 MEM=7142MB
```

### 6.3.5 IOCTL Commands
```bash
./vdevpulse ioctl start     # → [DEVICE] IOCTL Command: VDEV_IOCTL_START (State: RUNNING)
./vdevpulse ioctl stop      # → [DEVICE] IOCTL Command: VDEV_IOCTL_STOP  (State: STOPPED)
./vdevpulse ioctl reset     # → [DEVICE] IOCTL Command: VDEV_IOCTL_RESET (State: RESET)
```

### 6.3.6 Write Custom Payload
```bash
./vdevpulse write "HEALTH_CHECK_SIGNAL"
# → [DEVICE] Wrote 20 bytes to /tmp/vdevpulse
```

### 6.3.7 Single Status Snapshot
```bash
./vdevpulse status
```

### 6.3.8 Graceful Shutdown
```bash
Ctrl+C
# → [INFO] VDevPulse Engine shutting down cleanly.
# → DeviceManager destructor: close(fd), fs::remove("/tmp/vdevpulse")
```

---

## 6.4 Production Deployment — systemd Service

For deployment on a production Linux system as a managed background service:

**Create**: `/etc/systemd/system/vdevpulse.service`
```ini
[Unit]
Description=VDevPulse Linux Virtual Device & System Telemetry Daemon
After=network.target local-fs.target

[Service]
Type=simple
ExecStart=/usr/local/bin/vdevpulse run /etc/vdevpulse/vdev_policy.json
Restart=on-failure
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

**Commands**:
```bash
sudo cp build/vdevpulse /usr/local/bin/
sudo systemctl daemon-reload
sudo systemctl enable vdevpulse
sudo systemctl start vdevpulse
sudo systemctl status vdevpulse
```

---

## 6.5 Project Achievements

| Achievement | Detail |
| :--- | :--- |
| Zero external dependencies | Built entirely with C++17 standard library and POSIX syscalls |
| POSIX Character Device interface | Named FIFO (`/tmp/vdevpulse`) with standard `open/read/write` semantics |
| 3-source kernel telemetry | `/proc/stat` (CPU), `/proc/meminfo` (RAM), `/proc/uptime` (uptime) |
| Virtual IOCTL emulation | Commands `0x8001`, `0x8002`, `0x8003` with `DeviceState` tracking |
| Thread-safe logging | `std::mutex` + RAII `std::lock_guard` with 5 ANSI color-coded levels |
| 100% automated test pass | CTest: 2/2 tests passed; device + telemetry suites verified |
| Graceful signal handling | `SIGINT` / `SIGTERM` → `std::atomic<bool>` → clean shutdown + RAII cleanup |
| Full 6-stage SDLC | Complete engineering documentation across all 6 stages |

---

## 6.6 Known Limitations

| Limitation | Description | Potential Fix |
| :--- | :--- | :--- |
| CPU% is aggregate (not delta) | Uses total tick ratio from boot, not interval delta | Store previous tick snapshot; compute `ΔActive / ΔTotal` each interval |
| User-space FIFO (not kernel module) | `/tmp/vdevpulse` is a named pipe, not a true kernel character device | Implement as Linux kernel module using `cdev_add`, `file_operations` |
| Single device node | Only one virtual device node supported per daemon instance | Add multi-device registry map in `DeviceManager` |
| No multi-threading | Main loop is single-threaded; telemetry blocks I/O writes | Add separate telemetry and device threads with a shared queue |

---

## 6.7 Future Improvements & Roadmap

1. **Linux Kernel Character Driver**: Port to native `.ko` kernel module using `cdev_add`, `file_operations`, `copy_to_user`, and real kernel `ioctl` registrations.
2. **Delta CPU Tick Calculation**: Implement two-sample snapshot for accurate per-interval CPU utilization.
3. **gRPC Telemetry Export**: Expose `SystemTelemetry` data over a gRPC stream for remote monitoring dashboards.
4. **eBPF Probes**: Add eBPF kprobe hooks for per-process syscall frequency monitoring.
5. **Multi-Device Support**: Extend `DeviceManager` to manage a map of named virtual device nodes.

---

## 6.8 Final Submission Checklist

| Deliverable | Status |
| :--- | :--- |
| Source code — `src/*.cpp`, `include/**/*.hpp` | Submitted |
| Build system — `CMakeLists.txt` | Submitted |
| Runtime config — `configs/vdev_policy.json` | Submitted |
| Unit tests — `tests/device_test.cpp`, `tests/telemetry_test.cpp` | Submitted |
| Stage 1–6 SDLC documentation — `docs/stage*.md` | Submitted |
| Presentation guide — `docs/project_presentation_guide.md` | Submitted |
| Git repository with staged commit history | Submitted |
| `README.md` with build instructions | Submitted |
| `LICENSE` (MIT) | Submitted |

---

## 6.9 Version Control & Progress Evidence

- **SDLC Phase**: Stage 6 — Final Delivery & Presentation
- **Git Commit**: `[Stage 6] Final deployment guide, presentation guide & project delivery`
- **Repository**: [github.com/priyansudas07/Wipro-Embedded-Training](https://github.com/priyansudas07/Wipro-Embedded-Training)
- **Commit History**: Sequential `[Stage 1]` through `[Stage 6]` commits demonstrate continuous development progress.
