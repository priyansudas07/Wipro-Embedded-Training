# 📊 Linux System Resource Monitor (C++)

> **Wipro COE Embedded Systems Training - Assignment 3**  
> A modern C++17 system utility built using `<filesystem>`, `<fstream>`, and `/proc` pseudo-filesystem inspection to monitor processes, query process details by PID, and export system reports.

---

## 📋 Features

1. **Show Running Processes**: Displays the top active processes (PID & Executable Name).
2. **Show Process Details by PID**: Extracts `Name:`, `State:`, `Pid:`, and `PPid:` attributes from `/proc/[PID]/status`.
3. **Count Total Processes**: Dynamically counts all active PIDs in the system.
4. **Search Process by PID**: Verifies process directory existence in `/proc`.
5. **Generate Process Report**: Exports a complete active process snapshot to `process_report.txt`.

---

## 🛠️ Build & Execution Instructions

### Prerequisites
- C++17 compatible compiler (`g++ 7+` or `clang++`).

### Compilation
```bash
make
```

### Run Application
```bash
./resource_monitor
```

### Cleanup
```bash
make clean
```

---

## 📑 Output Log Example

```text
====================================
        RUNNING PROCESSES
====================================
PID	PROCESS
-----------------------------
1	systemd
2	kthreadd
7	init
44	systemd-journald
79	systemd-resolved
88	systemd-udevd
105	chronyd-starter
106	cron
107	dbus-daemon
139	systemd-logind
359	bash
575	resource_monitor
```
