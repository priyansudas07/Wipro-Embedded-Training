# 🏗️ Architecture Documentation

## System Overview

The **Embedded Linux Device Health Monitor & Auto-Recovery Agent** is an event-driven daemon designed to run on resource-constrained Linux edge devices.

```text
+-----------------------------------------------------------------+
|                    MAIN HEALTH MONITOR LOOP                    |
+-----------------------------------------------------------------+
          |              |              |              |
          v              v              v              v
    +-----------+  +-----------+  +-----------+  +-----------+
    |    CPU    |  |  MEMORY   |  |   DISK    |  |   TEMP    |
    |  Monitor  |  |  Monitor  |  |  Monitor  |  |  Monitor  |
    +-----------+  +-----------+  +-----------+  +-----------+
          |              |              |              |
          +--------------+--------------+--------------+
                         |
                         v
              +---------------------+
              | Threshold Check     |
              +---------------------+
                         |
                         v
              +---------------------+
              | SERVICE MONITOR &   |
              | AUTO-RECOVERY AGENT |
              +---------------------+
                         |
                         v
              +---------------------+
              |   LOGGER DAEMON     |
              | (Console / Log file)|
              +---------------------+
```

## Component Roles
- **`CpuMonitor`**: Reads `/proc/stat` delta to compute CPU utilization.
- **`MemoryMonitor`**: Inspects `/proc/meminfo` & `sysinfo()`.
- **`DiskMonitor`**: Reads POSIX `statvfs()` file system stats.
- **`TemperatureMonitor`**: Queries Linux thermal subsystem `/sys/class/thermal/`.
- **`ServiceMonitor`**: Detects failed services & executes auto-recovery (`systemctl restart`).
