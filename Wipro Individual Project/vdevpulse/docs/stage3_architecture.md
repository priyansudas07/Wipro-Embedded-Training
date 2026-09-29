# Stage 3: Architecture & Design Specification

## 1. System Architecture & UML Documentation

### High-Level System Architecture
The VDevPulse system consists of four primary subsystems:
1. **Config Engine (`Config`)**: Parses runtime JSON policies.
2. **Telemetry Monitor (`TelemetryMonitor`)**: Reads `/proc` system files.
3. **Virtual Device Manager (`DeviceManager`)**: Manages non-blocking file descriptor I/O and POSIX commands.
4. **Logger (`Logger`)**: Centralized thread-safe logging interface.

```
+-----------------------------------------------------------------------+
|                           vdevpulse Daemon                            |
|                                                                       |
|  +------------------+     +-------------------+     +--------------+  |
|  |   Config Engine  |     | Telemetry Monitor |     | Thread Logger|  |
|  |  (vdev_policy)   |     | (/proc/stat, mem) |     | (vdev.log)   |  |
|  +--------+---------+     +---------+---------+     +-------+------+  |
|           |                         |                       |         |
|           +-------------------------+-----------------------+         |
|                                     |                                 |
|                         +-----------v-----------+                     |
|                         | Virtual Device Manager|                     |
|                         |    (/tmp/vdevpulse)   |                     |
|                         +-----------+-----------+                     |
+-------------------------------------|---------------------------------+
                                      |
                           POSIX Read / Write Stream
                                      |
                          +-----------v-----------+
                          | User-Space Applications|
                          +-----------------------+
```

### UML Diagrams

#### UML Class Diagram

```mermaid
classDiagram
    class Logger {
        +static instance() Logger&
        +log(LogLevel level, string message) void
        +set_log_file(string path) void
        -mutex m_mutex
        -ofstream m_file_stream
    }

    class Config {
        +load_from_file(string filepath) bool
        +get_sample_interval_ms() int
        +get_device_path() string
        +get_log_file_path() string
        +get_cpu_alert_threshold() double
        -int m_sample_interval_ms
        -string m_device_path
        -string m_log_file_path
        -double m_cpu_alert_threshold
    }

    class TelemetryData {
        +double cpu_usage_pct
        +double ram_usage_pct
        +uint64_t free_ram_kb
        +uint64_t total_ram_kb
    }

    class TelemetryMonitor {
        +TelemetryMonitor()
        +sample() TelemetryData
        -parse_cpu_ticks() CpuTicks
        -parse_memory() MemoryInfo
        -CpuTicks m_prev_ticks
    }

    class DeviceManager {
        +DeviceManager(string dev_path)
        +initialize() bool
        +read_telemetry_stream(string data) bool
        +process_command(string cmd) string
        +cleanup() void
        -string m_dev_path
        -int m_fd
        -bool m_initialized
    }

    DeviceManager --> TelemetryMonitor : Queries metrics
    DeviceManager --> Logger : Logs events
    TelemetryMonitor --> TelemetryData : Produces
    Config --> Logger : Logs initialization
```

#### Sequence Diagram (Telemetry Sampling & Device I/O)

```mermaid
sequenceDiagram
    autonumber
    participant UserApp as User Application
    participant DevMgr as DeviceManager
    participant TelMon as TelemetryMonitor
    participant ProcFS as /proc Filesystem
    participant Log as Logger

    UserApp->>DevMgr: Open & Read /dev/vdevpulse
    DevMgr->>TelMon: sample()
    TelMon->>ProcFS: Read /proc/stat
    ProcFS-->>TelMon: Return CPU Ticks
    TelMon->>ProcFS: Read /proc/meminfo
    ProcFS-->>TelMon: Return RAM Bytes
    TelMon->>TelMon: Calculate CPU & RAM Usage %
    TelMon-->>DevMgr: TelemetryData Struct
    DevMgr->>Log: log(INFO, "Telemetry sampled")
    DevMgr-->>UserApp: Metric String (JSON/Key-Val)
```

#### State Machine Diagram (Virtual Device Lifecycle)

```mermaid
stateDiagram-v2
    [*] --> Uninitialized
    Uninitialized --> Initializing : initialize()
    Initializing --> DeviceCreated : Create FIFO / Pipe Node
    Initializing --> ErrorState : Failed to Create Device
    DeviceCreated --> ActiveListening : Daemon Loop Started
    ActiveListening --> ProcessingRead : Client Read Request
    ProcessingRead --> ActiveListening : Return Telemetry
    ActiveListening --> ProcessingCommand : Client Write Request
    ProcessingCommand --> ActiveListening : Return Response
    ActiveListening --> CleaningUp : Signal Shutdown (SIGINT)
    CleaningUp --> Uninitialized : Remove FIFO Node
    ErrorState --> [*]
    Uninitialized --> [*]
```

---

## 2. Version Control & Git Commit Tracking

- **SDLC Phase**: Stage 3 - System Architecture & Design
- **Commit Target**: `[Stage 3] System Architecture, UML diagrams & class interfaces`
- **Branch**: `main`
- **Repository Path**: `Wipro Individual Project/vdevpulse/`

---

## 3. Progress Evidence

- Class definitions completed in `include/vdevpulse/` (`logger.hpp`, `config.hpp`, `device_manager.hpp`, `telemetry_monitor.hpp`).
- System interaction flows verified using Sequence and State Machine UML models.
- Header guard specifications and modular separation of interface from implementation achieved.

---

## 4. Demonstration & Presentation Notes

- **Key Takeaway for Mentors**: Walk mentors through the Mermaid Class Diagram, Sequence Diagram, and State Machine Diagram.
- **Demo Focus**: Highlight how `DeviceManager` decouples user-space device requests from low-level `/proc` filesystem reading in `TelemetryMonitor`.

---

## 5. Roadmap for Next Stage (Stage 4)

- Implement core C++ source files (`src/*.cpp`).
- Construct main daemon loop with signal handling (`SIGINT`, `SIGTERM`).
- Configure CMake build pipeline (`CMakeLists.txt`).
