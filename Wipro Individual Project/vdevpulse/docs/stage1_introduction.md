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

## 1.2 Problem Statement

Modern embedded Linux systems — including industrial automation controllers, edge computing nodes, and autonomous gateway devices — have an increasing need for **real-time hardware resource telemetry** and **device control interfaces** operating under strict resource constraints.

Existing tools present significant practical drawbacks:

| Existing Tool | Problem |
| :--- | :--- |
| `top`, `htop` | Interactive only, not automatable, no device I/O interface |
| Python/Go telemetry daemons | High memory overhead (50–300 MB RSS), large dependency graphs |
| Heavy observability stacks (Prometheus, Grafana) | Require additional processes, network stacks, time-series databases |
| Direct kernel modules | Require elevated root privileges and kernel build infrastructure |

There is no lightweight, self-contained, C++ native solution that combines:
- A POSIX character device interface (`/dev/*` paradigm)
- Real-time kernel telemetry collection via `/proc`
- Thread-safe structured logging with configurable policy

VDevPulse directly addresses this gap.

---

## 1.3 Project Scope

### In Scope
- Virtual POSIX character device creation and lifecycle management using named FIFO pipes (`mkfifo`)
- Real-time parsing of three kernel `/proc` interfaces:
  - `/proc/stat` — CPU tick accounting
  - `/proc/meminfo` — RAM allocation and availability
  - `/proc/uptime` — System uptime duration
- Virtual IOCTL command interface (`VDEV_IOCTL_START`, `VDEV_IOCTL_STOP`, `VDEV_IOCTL_RESET`)
- Color-coded terminal telemetry dashboard output
- JSON policy configuration loading
- Thread-safe structured logging with severity levels: `INFO`, `SUCCESS`, `WARNING`, `ERROR`, `DEVICE`
- Automated CMake CTest unit testing suite
- Full 6-stage SDLC documentation

### Out of Scope
- Native Linux Kernel Module (`.ko` file) insertion requiring `insmod`
- eBPF tracepoint or kprobe kernel hooks
- Networked telemetry export (gRPC, REST API)
- Multi-node distributed monitoring

---

## 1.4 Expected Outcome & Application

**Expected Outcome**: A fully functional, single-binary daemon `vdevpulse` that runs on any C++17-compatible Linux system without external library installation, providing real-time system telemetry through both terminal output and a virtual device node interface.

**Application Domains**:
- Embedded Linux edge nodes (Raspberry Pi, NXP i.MX series, BeagleBone)
- Industrial automation system monitoring
- Automotive embedded ECU resource management
- Telecom gateway health monitoring

---

## 1.5 Version Control & Progress Evidence

- **SDLC Phase**: Stage 1 — Project Introduction
- **Git Commit**: `[Stage 1] Initial project proposal, scope & introduction documentation`
- **Branch**: `main`

---

## 1.6 Roadmap for Next Stage (Stage 2)

- Formally document all functional requirements (FR matrix)
- Define non-functional requirements with quantified performance targets
- Design project development timeline with phased milestones
