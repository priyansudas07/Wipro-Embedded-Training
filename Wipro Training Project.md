# Wipro Embedded Systems Training Project

This repository contains all the assignments, lab programs, and hands-on projects completed during the Wipro Center of Excellence (CoE) Embedded Systems training program.

## Overview of Completed Assignments

### 1. Linux System Information Tool
- **Directory**: `wipro_system_info_tool/`
- **Language**: C (C99)
- **Description**: Displays system information including OS distribution, kernel release, uptime, CPU specifications, and memory usage by parsing Linux `/proc` files and system calls (`uname`, `sysinfo`).
- **Build & Run**:
  ```bash
  cd wipro_system_info_tool
  make
  ./sysinfo_tool
  ```

### 2. Linux Process Monitor
- **Directory**: `wipro_process_monitor/`
- **Language**: C (C99)
- **Description**: Scans the `/proc` directory to list running processes along with their PID, Process Name, State (Sleeping, Running, Zombie), and Parent PID (PPID).
- **Build & Run**:
  ```bash
  cd wipro_process_monitor
  make
  ./process_monitor
  ```

### 3. Linux System Resource Monitor
- **Directory**: `wipro_system_resource_monitor/`
- **Language**: C++ (C++17)
- **Description**: Monitors system processes using C++17 `<filesystem>` and `<fstream>`, allowing process search, PID detail inspection, and exporting a process report (`process_report.txt`).
- **Build & Run**:
  ```bash
  cd wipro_system_resource_monitor
  make
  ./resource_monitor
  ```

## Development Environment
- OS: Linux (Ubuntu on WSL2)
- Compilers: `gcc`, `g++`
- Build Utility: `make`
