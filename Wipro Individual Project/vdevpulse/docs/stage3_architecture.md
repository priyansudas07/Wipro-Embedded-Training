# Stage 3: System Design & Architecture

## 3.1 Overall System Architecture

VDevPulse is structured as a modular, single-process Linux systems software daemon with five loosely-coupled subsystems communicating through well-defined Modern C++17 interfaces.

```
╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                vdevpulse — System Daemon Architecture                             ║
║                                                                                                   ║
║  ┌──────────────────┐    ┌───────────────────────────┐    ┌────────────────────┐                  ║
║  │   ConfigParser   │    │    Logger (Singleton)     │    │   History Buffer   │                  ║
║  │ vdev_policy.json │    │  INFO | SUCCESS | WARNING │    │  60-sample ring    │                  ║
║  │   (Thresholds)   │    │  ERROR | DEVICE           │    │    std::deque      │                  ║
║  └────────┬─────────┘    └─────────────┬─────────────┘    └─────────┬──────────┘                  ║
║           │ VDevConfig                 │ std::mutex RAII            │                             ║
║           ▼                            ▼                            ▼                             ║
║  ┌─────────────────────────────────────────────────────────────────────────────┐                  ║
║  │                               DeviceManager                                 │                  ║
║  │   * mkfifo() -> open(O_RDWR|O_NONBLOCK) -> write() -> read()                │                  ║
║  │   * processQueryCommand (GET_CPU, GET_MEM, GET_LOAD, GET_JSON, GET_HEALTH)  │                  ║
║  │   * IOCTL: START(0x8001), STOP(0x8002), RESET(0x8003), STATS(0x8004), RATE  │                  ║
║  │   * DeviceStats: total_bytes_written, total_reads, total_ioctls, queries    │                  ║
║  └─────────────────────────────────────┬───────────────────────────────────────┘                  ║
║                                        │ queries live metrics                                     ║
║                                        ▼                                                          ║
║  ┌─────────────────────────────────────────────────────────────────────────────┐                  ║
║  │                             TelemetryMonitor                                │                  ║
║  │   * /proc/stat    -> CPU tick delta -> cpu_usage_pct (%)                    │                  ║
║  │   * /proc/meminfo -> MemTotal, MemAvailable -> memory_used_mb (MB & %)      │                  ║
║  │   * /proc/loadavg -> 1m, 5m, 15m load averages & active/total threads       │                  ║
║  │   * /proc/uptime  -> System uptime (seconds)                                │                  ║
║  │   * /proc/[PID]/  -> Process memory RSS scanner & top resource consumers    │                  ║
║  │   * Threshold Rule Evaluator -> HEALTHY, WARNING_CPU, WARNING_MEM, CRITICAL │                  ║
║  │   * toJsonString() -> Structured JSON Serialization                         │                  ║
║  └─────────────────────────────────────┬───────────────────────────────────────┘                  ║
║                                        │                                                          ║
║           ┌────────────────────────────┴────────────────────────────┐                             ║
║           ▼                                                         ▼                             ║
║  ┌───────────────────────────────────────────┐    ┌───────────────────────────────────────────┐   ║
║  │        /tmp/vdevpulse (POSIX FIFO)        │    │    TuiDashboard (Non-Blocking TUI Engine) │   ║
║  │  Bidirectional IPC Character Stream Node  │    │  * POSIX poll() & raw termios event loop  │   ║
║  │  cat /tmp/vdevpulse | vdevpulse write MSG │    │  * Live Unicode gauges (██░░) & hotkeys   │   ║
║  │  Synchronous M2M Protocol Dispatcher      │    │  * Traffic bursts, IOCTL, alert tuning    │   ║
║  └───────────────────────────────────────────┘    └───────────────────────────────────────────┘   ║
╚═══════════════════════════════════════════════════════════════════════════════════════════════════╝
```

---

## 3.2 Major System Components & Responsibilities

| Component | Class / Struct | Responsibility |
| :--- | :--- | :--- |
| **Logger** | `Logger` (Singleton) | Thread-safe color-coded log output to stdout and file |
| **Policy Engine** | `ConfigParser`, `VDevConfig` | Load runtime settings, thresholds, and format from `vdev_policy.json` |
| **Telemetry Engine** | `TelemetryMonitor`, `SystemTelemetry`, `ProcessInfo` | Parse `/proc` kernel FS; compute CPU%, RAM%, loadavg, top processes; JSON serialization; history buffer |
| **Device Manager** | `DeviceManager`, `DeviceState`, `DeviceStats` | Create/open/read/write/close FIFO; process interactive query commands; handle IOCTL state & stats |
| **TUI Control Center** | `TuiDashboard` | Interactive Unicode/ANSI terminal control center with real-time non-blocking `poll()` loop & hotkey dispatch |
| **Daemon Entry Point** | `main()` | CLI dispatch (`menu`, `run`, `status`, `top`, `history`, `query`, `write`, `ioctl`), signal handling |

---

## 3.3 Core Data Structures

### 3.3.1 `VDevConfig` — Device Policy Configuration
```cpp
// include/vdevpulse/config.hpp
struct VDevConfig {
    std::string device_name                 = "vdevpulse";
    std::string device_path                 = "/tmp/vdevpulse";
    int         sampling_rate_ms            = 1000;
    bool        enable_cpu_telemetry        = true;
    bool        enable_memory_telemetry     = true;
    double      cpu_alert_threshold_pct     = 85.0;
    double      memory_alert_threshold_pct  = 90.0;
    long        max_memory_threshold_mb     = 4096;
    std::string output_format               = "text"; // "text" or "json"
};
```

### 3.3.2 `SystemTelemetry` & `ProcessInfo` — Live Telemetry Snapshot
```cpp
// include/vdevpulse/telemetry_monitor.hpp
struct ProcessInfo {
    int           pid           = 0;
    std::string   name          = "";
    long          memory_rss_mb = 0;
    unsigned long cpu_ticks     = 0;
};

struct SystemTelemetry {
    double                  cpu_usage_pct       = 0.0;
    long                    memory_total_mb     = 0;
    long                    memory_used_mb      = 0;
    long                    memory_free_mb      = 0;
    double                  memory_usage_pct    = 0.0;
    long                    uptime_seconds      = 0;
    double                  load_1m             = 0.0;
    double                  load_5m             = 0.0;
    double                  load_15m            = 0.0;
    int                     running_processes   = 0;
    int                     total_processes     = 0;
    std::string             health_status       = "HEALTHY";
    std::vector<ProcessInfo> top_processes;
};
```

### 3.3.3 `DeviceStats` & IOCTL Command Enums
```cpp
// include/vdevpulse/device_manager.hpp
enum class DeviceState { STOPPED, RUNNING, PAUSED };

constexpr unsigned long VDEV_IOCTL_START     = 0x8001;
constexpr unsigned long VDEV_IOCTL_STOP      = 0x8002;
constexpr unsigned long VDEV_IOCTL_RESET     = 0x8003;
constexpr unsigned long VDEV_IOCTL_GET_STATS = 0x8004;
constexpr unsigned long VDEV_IOCTL_SET_RATE  = 0x8005;

struct DeviceStats {
    uint64_t total_bytes_written = 0;
    uint64_t total_reads         = 0;
    uint64_t total_ioctls        = 0;
    uint64_t total_queries       = 0;
};
```

---

## 3.4 UML Diagrams

### 3.4.1 Comprehensive Class Diagram

```mermaid
classDiagram
    class Logger {
        -ofstream log_file_
        -mutex mutex_
        +getInstance() Logger
        +init(log_path string) void
        +log(level LogLevel, message string) void
    }

    class VDevConfig {
        +string device_name
        +string device_path
        +int sampling_rate_ms
        +bool enable_cpu_telemetry
        +bool enable_memory_telemetry
        +double cpu_alert_threshold_pct
        +double memory_alert_threshold_pct
        +long max_memory_threshold_mb
        +string output_format
    }

    class ConfigParser {
        +loadPolicy(filepath string, config VDevConfig) bool
    }

    class ProcessInfo {
        +int pid
        +string name
        +long memory_rss_mb
        +ulong cpu_ticks
    }

    class SystemTelemetry {
        +double cpu_usage_pct
        +long memory_total_mb
        +long memory_used_mb
        +long memory_free_mb
        +double memory_usage_pct
        +long uptime_seconds
        +double load_1m
        +double load_5m
        +double load_15m
        +int running_processes
        +int total_processes
        +string health_status
        +vector~ProcessInfo~ top_processes
    }

    class TelemetryMonitor {
        -deque~SystemTelemetry~ history_buffer_
        +collectTelemetry(config VDevConfig) SystemTelemetry
        +getTopProcesses(limit size_t) vector~ProcessInfo~
        +printTelemetryDashboard(metrics SystemTelemetry) void
        +toJsonString(metrics SystemTelemetry) string
        +recordHistory(metrics SystemTelemetry) void
        +getHistory() deque~SystemTelemetry~
        +printHistory() void
    }

    class DeviceManager {
        -string device_path_
        -int device_fd_
        -DeviceState state_
        -DeviceStats stats_
        +initDevice(config VDevConfig) bool
        +openDevice() bool
        +writeData(buffer string) bool
        +readData() string
        +sendIoctl(cmd ulong, arg ulong) bool
        +processQueryCommand(cmd string, telemetry SystemTelemetry) string
        +closeDevice() void
        +getState() DeviceState
        +getStats() DeviceStats
    }

    class TuiDashboard {
        +runInteractiveLoop(config VDevConfig) void
        -clearScreen() void
        -renderHeader(config VDevConfig) void
        -renderLiveTelemetryPanel(t SystemTelemetry, dev DeviceManager, config VDevConfig) void
        -renderActionControlsMenu() void
        -showTopProcessesMenu() void
        -showQueryMenu() void
        -showIoctlMenu(dev DeviceManager) void
        -showHistoryMenu() void
        -showWritePayloadPrompt(dev DeviceManager) void
        -showPolicyConfigMenu(config VDevConfig) void
    }

    ConfigParser --> VDevConfig : produces
    ConfigParser --> Logger : logs via
    DeviceManager --> VDevConfig : reads config
    DeviceManager --> Logger : logs via
    TelemetryMonitor --> SystemTelemetry : produces
    TelemetryMonitor --> Logger : logs via
    SystemTelemetry *-- ProcessInfo : aggregates
    TuiDashboard --> TelemetryMonitor : samples live data
    TuiDashboard --> DeviceManager : controls driver & IOCTL
    TuiDashboard --> VDevConfig : monitors thresholds
```

---

### 3.4.2 Sequence Diagram — Interactive TUI Non-Blocking Event Loop

```mermaid
sequenceDiagram
    autonumber
    participant User as User / Shell
    participant TUI as TuiDashboard
    participant DEV as DeviceManager
    participant TEL as TelemetryMonitor
    participant PROC as Linux /proc VFS
    participant STDIN as STDIN (poll / termios)

    User->>TUI: execute ./vdevpulse menu
    TUI->>DEV: initDevice(config) & openDevice()
    TUI->>DEV: sendIoctl(VDEV_IOCTL_START)
    TUI->>TUI: clearScreen() & hideCursor()

    loop Every sampling_rate_ms (Non-Blocking Loop)
        TUI->>TUI: moveCursor(0,0) [flicker-free]
        TUI->>TEL: collectTelemetry(current_config)
        TEL->>PROC: read /proc/stat, meminfo, loadavg
        PROC-->>TEL: raw kernel counters
        TEL->>TEL: evaluateAlertMatrix(thresholds)
        TEL-->>TUI: SystemTelemetry
        TUI->>DEV: getStats() & getState()
        DEV-->>TUI: DeviceStats
        TUI->>TUI: renderHeader(), renderLiveGauges(), renderActionMenu()

        TUI->>STDIN: poll(timeout_ms) [Raw Mode]
        
        alt Timeout Expired (No Key Pressed)
            STDIN-->>TUI: timeout (continue loop)
        else Hotkey 'T' (Traffic Burst)
            STDIN-->>TUI: key = 'T'
            loop 10 packets
                TUI->>DEV: writeData(packet)
                TUI->>DEV: processQueryCommand("PING")
            end
        else Hotkey 'S' (Toggle State)
            STDIN-->>TUI: key = 'S'
            TUI->>DEV: sendIoctl(VDEV_IOCTL_STOP / START)
        else Hotkey '+' / '-' (Tune Threshold)
            STDIN-->>TUI: key = '-'
            TUI->>TUI: current_config.cpu_alert_threshold_pct -= 5.0
        else Hotkey '1'..'8' (Sub-screen Menu)
            STDIN-->>TUI: key = '1'
            TUI->>TUI: showTopProcessesMenu()
        else Hotkey 'Q' (Exit)
            STDIN-->>TUI: key = 'Q'
            TUI->>DEV: closeDevice()
            TUI->>User: Restore terminal & exit
        end
    end
```

---

### 3.4.3 State Machine Diagram — Virtual Character Driver Lifecycle

```mermaid
stateDiagram-v2
    [*] --> Uninitialized

    Uninitialized --> Initializing : initDevice(config)
    Initializing --> NodeCreated : mkfifo("/tmp/vdevpulse") success
    Initializing --> Uninitialized : mkfifo failure

    NodeCreated --> Stopped : openDevice(O_RDWR | O_NONBLOCK)

    Stopped --> Running : VDEV_IOCTL_START (0x8001) / [S] hotkey
    Running --> Paused : VDEV_IOCTL_STOP (0x8002) / [S] hotkey
    Paused --> Running : VDEV_IOCTL_START (0x8001) / [S] hotkey
    
    Running --> Running : writeData() / readData() / processQueryCommand()
    Running --> Running : VDEV_IOCTL_GET_STATS (0x8004) / VDEV_IOCTL_SET_RATE (0x8005)
    
    Running --> Stopped : VDEV_IOCTL_RESET (0x8003) [Counters Zeroed]
    Paused --> Stopped : VDEV_IOCTL_RESET (0x8003)

    Running --> Cleanup : SIGINT / SIGTERM / [Q] Exit
    Paused --> Cleanup : SIGINT / SIGTERM / [Q] Exit
    Stopped --> Cleanup : SIGINT / SIGTERM / [Q] Exit
    Cleanup --> [*] : close(fd) & unlink FIFO node
```

---

### 3.4.4 Component Flow Diagram — Automated Alerting & Telemetry Pipeline

```mermaid
flowchart TD
    subgraph KernelSpace ["Linux Kernel & Hardware Space"]
        PSTAT["/proc/stat (CPU jiffies)"]
        PMEM["/proc/meminfo (RAM MemAvailable)"]
        PLOAD["/proc/loadavg (1/5/15m load)"]
        PPID["/proc/[PID]/ (comm & status RSS)"]
    end

    subgraph TelemetrySubsystem ["Telemetry Processing Engine"]
        INGEST["Telemetry Ingestion Worker"]
        RULE["Threshold Rule Evaluator<br/>(CPU > 85% | RAM > 90%)"]
        RING["60-Sample Ring Buffer<br/>std::deque"]
        JSON["Structured JSON Serializer"]
    end

    subgraph DriverSubsystem ["Virtual Device & IPC Layer"]
        FIFO["/tmp/vdevpulse<br/>POSIX Named Pipe (FIFO)"]
        IOCTL["IOCTL Command Controller<br/>START | STOP | RESET | STATS"]
        M2M["M2M Query Dispatcher<br/>GET_CPU | GET_MEM | PING"]
    end

    subgraph UserInterface ["User Space Control Interfaces"]
        TUI["Interactive TUI Control Center<br/>./vdevpulse menu"]
        CLI["Command Line Utility<br/>./vdevpulse status | top | query"]
        CLIENT["External Diagnostic Clients<br/>cat /tmp/vdevpulse"]
    end

    PSTAT --> INGEST
    PMEM --> INGEST
    PLOAD --> INGEST
    PPID --> INGEST

    INGEST --> RULE
    RULE --> RING
    RULE --> JSON

    RING --> TUI
    JSON --> FIFO
    RULE --> TUI

    IOCTL <--> TUI
    M2M <--> FIFO
    M2M <--> TUI

    CLI --> INGEST
    CLIENT <--> FIFO
```

---

## 3.5 Version Control & Progress Evidence

- **SDLC Phase**: Stage 3 — System Design & Architecture
- **Git Commit**: `[Stage 3] System Architecture, UML diagrams & class interfaces`
- **Evidence**: Class declarations, UML diagrams, sequence diagrams, and data structures synchronized with code.
