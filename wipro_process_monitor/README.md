# 🔍 Linux Process Monitor Tool

> **Wipro COE Embedded Systems Training - Assignment 2**  
> A lightweight, efficient C Linux System Programming application that inspects the `/proc` pseudo-filesystem to extract real-time Process details including **PID**, **Process Name**, **State**, and **Parent Process ID (PPID)**.

---

## 📋 Features

- **Process Enumeration**: Scans all active PIDs in Linux `/proc` filesystem.
- **Detailed Attributes**: Displays:
  - `PID` (Process Identification Number)
  - `Process Name` (Executable Command)
  - `State` (`S`: Sleeping, `R`: Running, `Z`: Zombie, `T`: Stopped, `D`: Uninterruptible Disk Sleep)
  - `PPID` (Parent Process ID)
- **Search & Filter**: Search active processes by PID or Name substring.
- **Process Statistics**: Summarizes total count of Running, Sleeping, Stopped, and Zombie processes.

---

## 🛠️ Build & Usage

### Compilation
```bash
make
```

### Execution
```bash
./process_monitor
```

### Cleanup
```bash
make clean
```

---

## 📊 Sample Output Log

```text
======================================================================
            LINUX PROCESS MONITOR TOOL (Wipro COE)                   
======================================================================
PID        Process Name                   State      PPID      
----------------------------------------------------------------------
1          systemd                        S          0         
2          kthreadd                       S          0         
3          pool_workqueue_release         S          2         
450        systemd                        S          1         
721        NetworkManager                 S          1         
850        sshd                           S          1         
1024       bash                           S          850       
1250       gnome-shell                    S          1         
1450       firefox                        S          1250      
1520       code                           S          1250      
----------------------------------------------------------------------
Total Running Processes Found: 10
```
