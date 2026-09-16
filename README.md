# 🚀 Wipro COE Embedded Systems Training Assignments

> **Student Repository**  
> Comprehensive repository containing lab assignments, Linux system programming tools, Embedded C/C++ projects, and hardware simulation solutions for the **Wipro Center of Excellence (CoE) Embedded Training Program**.

---

## 📚 Repository Overview

| # | Assignment / Module Name | Tech Stack | Status | Directory Link |
| :---: | :--- | :--- | :---: | :--- |
| **01** | **Linux System Information Tool** | C (POSIX uname, sysinfo, /proc) | ✅ Completed | [wipro_system_info_tool/](./wipro_system_info_tool) |
| **02** | **Linux Process Monitor** | C (/proc directory scanner & parser) | ✅ Completed | [wipro_process_monitor/](./wipro_process_monitor) |
| **03** | **Linux System Resource Monitor** | C++17 (<filesystem>, <fstream>, /proc) | ✅ Completed | [wipro_system_resource_monitor/](./wipro_system_resource_monitor) |

---

## 🖥️ Assignment Breakdown

### 01. Linux System Information Tool
- **Directory**: [wipro_system_info_tool/](./wipro_system_info_tool)
- **Description**: Modular C Command-Line Application that queries real-time Linux system info (OS, Kernel, Uptime), CPU details (Cores, Model, Frequency), and Memory statistics (RAM, Used %, Free, Swap).

### 02. Linux Process Monitor
- **Directory**: [wipro_process_monitor/](./wipro_process_monitor)
- **Description**: Linux System Programming tool that parses /proc to list active processes with PID, Process Name, State, and Parent Process ID (PPID).

### 03. Linux System Resource Monitor
- **Directory**: [wipro_system_resource_monitor/](./wipro_system_resource_monitor)
- **Description**: C++17 application utilizing std::filesystem and standard file streams to inspect /proc, query PID details, and export reports to process_report.txt.

---

## 🛠️ Environment & Tools Used

- **Operating System**: Linux (Ubuntu 26.04 LTS via WSL2)
- **Compilers & Build Tools**: gcc, g++, make, gdb, gcc-arm-none-eabi, cmake
- **Language**: C / C++ (C99 and C++17 standards)
