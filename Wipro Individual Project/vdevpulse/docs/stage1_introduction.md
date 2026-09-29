# Stage 1: Project Introduction

## 1.1 Project Idea & Objective

**Project Name**: VDevPulse — Virtual Device Interface & System Telemetry Monitor

**Objective**: Design and implement a lightweight Linux system software daemon in Modern C++17 that:
1. Creates and manages a **virtual Linux character device node** (`/tmp/vdevpulse`) using POSIX FIFO pipes, emulating the standard Linux `/dev/*` device interface.
2. Continuously **monitors real-time system telemetry** (CPU utilization, RAM usage, system uptime) by parsing Linux kernel's synthetic filesystem interfaces (`/proc/stat`, `/proc/meminfo`, `/proc/uptime`).
3. Provides a **POSIX-standard device I/O interface** supporting `read()`, `write()`, and virtual `ioctl()` command operations.
4. Implements a **thread-safe logging subsystem** for runtime diagnostics.
5. Loads **dynamic runtime policies** from a JSON configuration file without requiring binary recompilation.

---

## 1.2 Why I Built This Project (Engineering Motivation)

During embedded systems engineering, I recognized a recurring challenge: monitoring the hardware health of low-power, resource-constrained Linux boards (such as Raspberry Pi, BeagleBone, and industrial edge controllers) without introducing heavy runtime overhead or complex dependencies.

I built VDevPulse to address this with specific engineering motivations:
1. **Bridge Low-Level Linux Systems Concepts with Modern C++17**: Combine Linux POSIX system calls (`mkfifo`, `open`, `read`, `write`, `close`), virtual procfs parsing, and signal handling with modern C++ OOP principles (RAII, Singleton patterns, and `std::mutex` synchronization).
2. **Eliminate Runtime Bloat in Embedded Observability**: Many modern telemetry daemons rely on Python runtimes, JVMs, or heavy Go binaries consuming 50–200 MB of RAM. VDevPulse runs in under 15 MB RSS with zero third-party dependencies.
3. **Emulate the Classic UNIX "Everything is a File" Philosophy**: By creating `/tmp/vdevpulse`, any standard user-space program, shell script, or diagnostic agent can interact with the telemetry engine using simple file I/O operations (`cat`, `echo`, standard POSIX stream reads) without requiring custom SDKs or network sockets.
4. **Safety and Portability**: Writing in-kernel character drivers (`.ko`) carries kernel panic risks and requires matching kernel headers. VDevPulse achieves character device semantics entirely in user-space, making it safe, portable across any Linux distribution (&ge; 4.15), and runnable inside containers or WSL2.

---

## 1.3 How It Is Useful & Real-World Practical Applications

VDevPulse provides concrete practical utility across several embedded and system software scenarios:

1. **Embedded Edge Node Health Monitoring**:
   - Continuously monitors CPU load and memory pressure on IoT gateways and industrial robots.
   - Detects runaway processes or memory leaks early before they cause hardware watchdog resets or system brownouts.

2. **Zero-SDK Telemetry Integration for Other Processes**:
   - Any external program (written in C, C++, Python, Rust, or Bash) can query real-time system telemetry simply by opening and reading `/tmp/vdevpulse`. No proprietary library or network client is required.

3. **Watchdog and Auto-Recovery Agent Support**:
   - Supervisory processes can read `/tmp/vdevpulse` at periodic intervals. If CPU or memory utilization exceeds safety thresholds defined in `vdev_policy.json`, the watchdog can trigger graceful service throttling or alert operations.

4. **Interactive Hardware Control Emulation (IOCTL)**:
   - Operators can send virtual IOCTL commands (`START`, `STOP`, `RESET`) to dynamically pause or resume the telemetry collection stream without restarting the daemon process.

---

## 1.4 Problem Statement

Modern embedded Linux systems frequently face resource contention and unexpected memory depletion. Traditional observability tools present significant practical drawbacks:

| Existing Tool | Problem in Embedded Systems | VDevPulse Solution |
| :--- | :--- | :--- |
| `top`, `htop` | Interactive terminal only; cannot be read programmatically as a device stream. | Standard named FIFO device node (`/tmp/vdevpulse`) for POSIX file I/O. |
| Python/Go daemons | High memory overhead (50–300 MB RSS), large dependency trees. | Minimal C++17 footprint (&lt; 15 MB RSS) and zero external dependencies. |
| Prometheus agents | Requires network stack overhead, open ports, and time-series infrastructure. | Self-contained local daemon reading kernel `/proc` directly with &lt; 1 ms latency. |
| Kernel Modules (`.ko`) | Requires root module insertion (`insmod`); bugs cause system-wide kernel panics. | User-space POSIX character device emulation; completely safe and crash-isolated. |

---

## 1.5 Project Scope

### In Scope
- Virtual POSIX character device creation and lifecycle management using named FIFO pipes (`mkfifo`).
- Real-time parsing of three kernel `/proc` interfaces: `/proc/stat` (CPU), `/proc/meminfo` (RAM), and `/proc/uptime` (Uptime).
- Virtual IOCTL command interface (`VDEV_IOCTL_START`, `VDEV_IOCTL_STOP`, `VDEV_IOCTL_RESET`).
- Color-coded terminal telemetry dashboard output.
- Dynamic JSON policy configuration loading (`configs/vdev_policy.json`).
- Thread-safe structured logging with 5 severity levels: `INFO`, `SUCCESS`, `WARNING`, `ERROR`, `DEVICE`.
- Automated CMake CTest unit testing suite (`device_test`, `telemetry_test`).
- Full 6-stage SDLC engineering documentation.

### Out of Scope
- Direct kernel module (`.ko`) compilation requiring root `insmod`.
- In-kernel eBPF bytecode loading.
- Networked remote telemetry streaming (gRPC / HTTP).

---

## 1.6 Version Control & Progress Evidence

- **SDLC Phase**: Stage 1 — Project Introduction
- **Git Commit**: `[Stage 1] Initial project proposal, scope & introduction documentation`
- **Branch**: `main`

---

## 1.7 Roadmap for Next Stage (Stage 2)

- Formally document all functional requirements (FR-01 through FR-15).
- Define non-functional requirements with quantified performance targets (latency &lt; 5 ms, RSS &lt; 15 MB).
- Design project development timeline with phased milestones.
