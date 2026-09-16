# 🚀 Wipro COE Embedded Systems Training Assignments

> **Student Repository**  
> Comprehensive repository containing lab assignments, Linux system programming tools, Embedded C projects, and hardware simulation solutions for the **Wipro Center of Excellence (CoE) Embedded Training Program**.

---

## 📚 Repository Overview

| # | Assignment / Module Name | Tech Stack | Status | Directory Link |
| :---: | :--- | :--- | :---: | :--- |
| **01** | **Linux System Information Tool** | C (POSIX `uname`, `sysinfo`, `/proc`) | ✅ Completed | [`wipro_system_info_tool/`](./wipro_system_info_tool) |

---

## 🖥️ Assignment Breakdown

### 01. Linux System Information Tool
- **Directory**: [`wipro_system_info_tool/`](./wipro_system_info_tool)
- **Description**: Modular C Command-Line Application that queries real-time Linux system info (OS, Kernel, Uptime), CPU details (Cores, Model, Frequency), and Memory statistics (RAM, Used %, Free, Swap).
- **Key APIs**: `sys/utsname.h`, `sys/sysinfo.h`, `unistd.h`, `/proc/cpuinfo`.
- **Build & Run**:
  ```bash
  cd wipro_system_info_tool
  make
  ./sysinfo_tool
  ```

---

## 🛠️ Environment & Tools Used

- **Operating System**: Linux (Ubuntu 26.04 LTS via WSL2)
- **Compilers & Build Tools**: `gcc`, `make`, `gdb`, `gcc-arm-none-eabi`, `cmake`
- **Language**: C / C++ (C99 standard)
