# Stage 1: Project Overview & Strategic Objectives

## 1. Documentation & Requirements Scope

### Executive Summary
VDevPulse is an embedded system software daemon implemented in modern C++17 designed to provide high-throughput system resource monitoring paired with a Linux character device interface `/dev/vdevpulse`. Embedded systems and edge computing environments require real-time visibility into CPU utilization, memory pressure, and system health while maintaining minimal runtime overhead and operating without heavy third-party runtime frameworks.

VDevPulse fulfills this requirement by directly interacting with Linux kernel abstractions (`/proc` filesystem) and POSIX file streams, enabling low-latency telemetry collection and user-space hardware control.

### Problem Statement
Modern Linux-based edge nodes and autonomous system gateways frequently face resource contention, unexpected memory depletion, and silent CPU throttles. Traditional telemetry tools (such as `top`, `htop`, or heavy Python/Go daemons) introduce significant runtime bloat, high CPU allocation overhead, and external library dependencies unsuitable for resource-constrained embedded targets.

Furthermore, applications in industrial automation need a standard POSIX device interface (`/dev/*`) to query telemetry data and submit device control IOCTLs using native file operations (`open`, `read`, `write`, `close`).

### Project Goals & Vision
1. **POSIX Device Interface Emulation**: Present a virtual character device node `/dev/vdevpulse` capable of handling concurrent read/write streams.
2. **Kernel Telemetry Parser**: Extract low-level CPU tick distributions from `/proc/stat` and system memory allocation metrics from `/proc/meminfo` with zero dynamic heap churn.
3. **Thread-Safe Concurrent Logging**: Provide thread-safe asynchronous logging capabilities across multiple severity channels (DEBUG, INFO, WARN, ERROR).
4. **Policy-Driven Operations**: Allow dynamic runtime configuration loading via JSON specification files without needing binary re-compilation.
5. **Zero External Dependency Footprint**: Built purely using modern C++17 standard libraries and native Linux syscalls.

---

## 2. Version Control & Git Commit Tracking

- **SDLC Phase**: Stage 1 - Concept & Feasibility
- **Commit Target**: `[Stage 1] Initial project proposal, scope & introduction documentation`
- **Branch**: `main`
- **Repository Path**: `Wipro Individual Project/vdevpulse/`

---

## 3. Progress Evidence

- Project repository initialized with standard C++ embedded project hierarchy (`include/`, `src/`, `configs/`, `tests/`, `docs/`).
- Problem statement and high-level feasibility validated against Linux kernel `/proc` filesystem capabilities.
- Target deliverables established: C++17 core daemon binary, CMake build system, CTest suite, and 6-stage SDLC documentation.

---

## 4. Demonstration & Presentation Notes

- **Key Takeaway for Mentors**: Explain why C++17 and kernel `/proc` abstractions were selected over heavy external frameworks to minimize RAM and CPU footprint in embedded environments.
- **Demo Focus**: Walk mentors through the repository layout and explain the core goals of `/dev/vdevpulse` virtual character device I/O.

---

## 5. Roadmap for Next Stage (Stage 2)

- Define quantitative functional requirements (FR-01 to FR-07) and non-functional requirements (NFRs).
- Specify hardware/software operational boundaries and telemetry parsing thresholds.
- Create Product Requirement Document (PRD) specification matrix.
