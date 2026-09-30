# Stage 2: Project Requirements & Development Plan

## 2.1 Functional Requirements

Functional requirements define the specific behaviours and capabilities the system must provide.

| Req ID | Module | Description | Priority |
| :--- | :--- | :--- | :--- |
| **FR-01** | `DeviceManager` | The system shall create a POSIX named FIFO node at the path specified in `vdev_policy.json` (default: `/tmp/vdevpulse`) using `mkfifo(3)`. | High |
| **FR-02** | `DeviceManager` | The system shall open the virtual device node with `O_RDWR | O_NONBLOCK` flags to support non-blocking read and write operations. | High |
| **FR-03** | `DeviceManager` | The system shall support `writeData()` — writing telemetry payload strings to the virtual device node via POSIX `write(2)`. | High |
| **FR-04** | `DeviceManager` | The system shall support `readData()` — reading up to 1023 bytes from the virtual device node via POSIX `read(2)`. | Medium |
| **FR-05** | `DeviceManager` | The system shall support virtual IOCTL commands: `VDEV_IOCTL_START (0x8001)`, `VDEV_IOCTL_STOP (0x8002)`, `VDEV_IOCTL_RESET (0x8003)`, `VDEV_IOCTL_GET_STATS (0x8004)`, `VDEV_IOCTL_SET_RATE (0x8005)`. | High |
| **FR-06** | `DeviceManager` | The system shall track a `DeviceState` enum (`STOPPED`, `RUNNING`, `PAUSED`) updated upon each IOCTL command. | Medium |
| **FR-07** | `TelemetryMonitor` | The system shall parse `/proc/stat` to compute CPU utilization as a percentage of non-idle CPU ticks over total CPU ticks. | High |
| **FR-08** | `TelemetryMonitor` | The system shall parse `/proc/meminfo` for `MemTotal`, `MemFree`, `MemAvailable` to compute RAM usage in MB and percentage. | High |
| **FR-09** | `TelemetryMonitor` | The system shall parse `/proc/uptime` to report system uptime in seconds. | Medium |
| **FR-10** | `TelemetryMonitor` | The system shall parse `/proc/loadavg` and `/proc/[PID]/` to report load averages, thread counts, and top memory/CPU consuming processes. | High |
| **FR-11** | `TelemetryMonitor` | The system shall evaluate CPU and RAM utilization against configurable thresholds in `vdev_policy.json`, dynamically assigning `health_status` (`HEALTHY`, `WARNING_CPU_OVERLOAD`, `WARNING_MEMORY_PRESSURE`). | High |
| **FR-12** | `DeviceManager` | The system shall implement an interactive query protocol responding to commands (`GET_CPU`, `GET_MEM`, `GET_LOAD`, `GET_TOP`, `GET_JSON`, `GET_HEALTH`, `PING`). | High |
| **FR-13** | `TelemetryMonitor` | The system shall support structured JSON serialization (`toJsonString`) for machine-readable streaming. | Medium |
| **FR-14** | `TelemetryMonitor` | The system shall maintain an in-memory circular history buffer storing recent telemetry snapshots for trend inspection (`vdevpulse history`). | Medium |
| **FR-15** | `ConfigParser` | The system shall load device configuration, threshold limits, and output format from a JSON policy file with safe fallback to compiled defaults. | Medium |
| **FR-16** | `Logger` | The system shall provide five log severity levels (`INFO`, `SUCCESS`, `WARNING`, `ERROR`, `DEVICE`) with thread safety (`std::mutex` + RAII). | High |
| **FR-17** | `Main Daemon` | The system shall handle `SIGINT` and `SIGTERM` signals for clean daemon exit and RAII device node cleanup. | High |

---

## 2.2 Non-Functional Requirements (NFRs)

| NFR ID | Category | Description | Target |
| :--- | :--- | :--- | :--- |
| **NFR-01** | Performance | Single telemetry sampling cycle latency | < 5 ms |
| **NFR-02** | Memory Footprint | RSS memory consumption of the daemon process | < 15 MB |
| **NFR-03** | Portability | Must compile and execute on Linux kernel | >= 4.15 |
| **NFR-04** | Reliability | No unhandled exceptions; all error paths return gracefully | 100% |
| **NFR-05** | Maintainability | Each class restricted to a single responsibility (SOLID) | Mandatory |
| **NFR-06** | Dependency-Free | Zero external library dependencies beyond libc and libstdc++ | Mandatory |
| **NFR-07** | Testability | All core modules must have an associated automated unit test case | 100% |
| **NFR-08** | Signal Safety | Daemon must shut down cleanly on `SIGINT` or `SIGTERM` without resource leaks | Mandatory |

---

## 2.3 System Modules & Deliverables

| Module | Source Files | Responsibility |
| :--- | :--- | :--- |
| `Logger` | `logger.hpp`, `logger.cpp` | Singleton thread-safe logging with ANSI color support |
| `ConfigParser` + `VDevConfig` | `config.hpp`, `config.cpp` | JSON policy file parsing with threshold rules |
| `TelemetryMonitor` + `SystemTelemetry` | `telemetry_monitor.hpp`, `telemetry_monitor.cpp` | `/proc` telemetry collection, loadavg, health status, JSON export, and history ring buffer |
| `DeviceManager` | `device_manager.hpp`, `device_manager.cpp` | POSIX FIFO lifecycle, query protocol, IOCTL state machine, and I/O statistics |
| Main Daemon | `main.cpp` | CLI argument dispatch (`run`, `status`, `history`, `query`, `write`, `ioctl`), signal handling |
| Unit Tests | `device_test.cpp`, `telemetry_test.cpp` | CTest-integrated automated verification |
| Policy Config | `configs/vdev_policy.json` | Runtime device, threshold, and output format configuration |
| Build System | `CMakeLists.txt` | CMake 3.14+ build and CTest target configuration |
| Documentation | `docs/*.md`, `docs/*.pdf` | Full 6-stage SDLC engineering documentation and official PDF report |

---

## 2.4 Development Plan & Milestone Timeline

| Milestone | Duration | Deliverables | Status |
| :--- | :--- | :--- | :--- |
| **M1** — Requirements & Design | Week 1 | PRD document, NFR table, module list, development plan | Complete |
| **M2** — Architecture & Interfaces | Week 1-2 | System architecture, UML diagrams, all `.hpp` header files | Complete |
| **M3** — Core Implementation | Week 2-3 | Kernel parsers, Device Manager FIFO I/O, Logger, CLI dispatcher | Complete |
| **M4** — Feature Enhancement | Week 3-4 | Load averages, threshold alert engine, query protocol, JSON export, IOCTL stats | Complete |
| **M5** — Testing & Verification | Week 4 | `device_test.cpp`, `telemetry_test.cpp`, CTest 100% pass | Complete |
| **M6** — Documentation & Delivery | Week 4 | All 6-stage SDLC docs, presentation guide, PDF report, GitHub push | Complete |

---

## 2.5 Version Control & Progress Evidence

- **SDLC Phase**: Stage 2 — Requirements & Development Plan
- **Git Commit**: `[Stage 2] Formalized Product Requirement Document (PRD) & specification matrix`
- **Branch**: `main`

---

## 2.6 Roadmap for Next Stage (Stage 3)

- Design full system architecture with component diagram
- Define C++ data structures (`SystemTelemetry`, `VDevConfig`, `DeviceStats`, `DeviceState`)
- Create UML Class Diagram, Sequence Diagram, and State Machine Diagram
