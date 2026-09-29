# Stage 2: System Requirements & Product Requirement Document (PRD)

## 1. Functional & Non-Functional Requirements Documentation

### Functional Requirements Matrix

| Req ID | Module | Description | Priority |
| :--- | :--- | :--- | :--- |
| **FR-01** | Device Manager | Must create and maintain a named FIFO/character pipe node at `/tmp/vdevpulse` or `/dev/vdevpulse`. | High |
| **FR-02** | Device Manager | Must accept user-space read operations and return current system telemetry formatted as key-value metrics. | High |
| **FR-03** | Device Manager | Must accept user-space write operations to parse operational commands (`RESET_TELEMETRY`, `SET_SAMPLE_RATE`). | Medium |
| **FR-04** | Telemetry Monitor | Must read `/proc/stat` and compute aggregate CPU usage percentage based on user, system, idle, and iowait ticks. | High |
| **FR-05** | Telemetry Monitor | Must read `/proc/meminfo` and extract `MemTotal`, `MemFree`, `MemAvailable`, computing RAM usage percentage. | High |
| **FR-06** | Policy Config Engine | Must load configuration options (log file path, polling interval in milliseconds, alert thresholds) from JSON. | Medium |
| **FR-07** | Logger | Must write timestamped log entries to stdout and log file with thread safety (`std::mutex`). | High |

### Non-Functional Requirements (NFRs)
- **Performance**: Telemetry parsing cycle must complete within < 5ms per sampling interval.
- **Memory Footprint**: Resident Set Size (RSS) memory consumption must stay under 15 MB.
- **Portability**: Must build and run on any C++17 POSIX-compliant Linux OS (Kernel >= 4.15).
- **Reliability**: Graceful handling of missing files, invalid device paths, and bad JSON input without throwing unhandled exceptions.
- **Maintainability**: Modular OOP structure adhering to C++ Core Guidelines and SOLID principles.

### Hardware & Software Requirements
- **Operating System**: Linux (Ubuntu, Debian, RedHat, or WSL2)
- **Compiler**: GCC 9.0+ or Clang 10.0+ supporting `-std=c++17`
- **Build System**: CMake 3.14+
- **Version Control**: Git 2.25+

---

## 2. Version Control & Git Commit Tracking

- **SDLC Phase**: Stage 2 - Requirements & PRD
- **Commit Target**: `[Stage 2] Formalized Product Requirement Document (PRD) & specification matrix`
- **Branch**: `main`
- **Repository Path**: `Wipro Individual Project/vdevpulse/`

---

## 3. Progress Evidence

- Functional requirements FR-01 through FR-07 validated against stakeholder expectations.
- Operational boundaries established for system resources (RSS < 15MB, latency < 5ms).
- JSON configuration schema designed for `configs/vdev_policy.json`.

---

## 4. Demonstration & Presentation Notes

- **Key Takeaway for Mentors**: Present the PRD requirements matrix highlighting how low latency (< 5ms) and small footprint (< 15MB) drive design choices.
- **Demo Focus**: Show how each requirement maps directly to software modules (`DeviceManager`, `TelemetryMonitor`, `Config`, `Logger`).

---

## 5. Roadmap for Next Stage (Stage 3)

- Design system architecture and component interactions.
- Produce UML Class, Sequence, and State Machine diagrams using Mermaid markdown syntax.
- Define C++ class interfaces (`.hpp` headers) for core modules.
