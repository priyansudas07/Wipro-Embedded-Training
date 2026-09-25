# SandBoxX Architecture Specification

## Overview

SandBoxX implements a modular container runtime architecture divided into four primary subsystems:

1. **Isolation Engine**: Interfaces with Linux `unshare` and `clone` system calls to create isolated PID, Network, Mount, UTS, and IPC namespaces.
2. **Resource Controller**: Interacts with kernel Control Groups (cgroups v1/v2) to set memory upper limits, CPU bandwidth quotas, and process limits.
3. **Filesystem Manager**: Prepares container rootfs layout, pseudo-filesystems (`/proc`, `/tmp`), and chroot boundaries.
4. **Security & Runtime Supervisor**: Drops administrative Linux capabilities, sets `PR_SET_NO_NEW_PRIVS`, tracks active sandboxes, and records logs.

---

## Component Diagram

```text
+-------------------------------------------------------------+
|                        SandBoxX CLI                         |
+------------------------------+------------------------------+
                               |
                               v
+-------------------------------------------------------------+
|                      Sandbox Runtime                        |
+--------+------------------+------------------+--------------+
         |                  |                  |
         v                  v                  v
+------------------+ +--------------+ +------------------+
| Namespace Mgr    | | Cgroup Mgr   | | Filesystem Mgr   |
| (PID/NET/UTS/NS) | | (CPU/Mem/PIDs| | (chroot, /proc)  |
+------------------+ +--------------+ +------------------+
         |                  |                  |
         +------------------+------------------+
                            |
                            v
+-------------------------------------------------------------+
|                 Sandboxed Child Process                     |
+-------------------------------------------------------------+
```
