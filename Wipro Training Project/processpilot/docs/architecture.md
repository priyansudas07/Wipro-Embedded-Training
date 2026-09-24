# 🏗️ ProcessPilot Architecture Documentation

ProcessPilot is an architecture designed for high-reliability Linux service supervision and automated recovery.

```text
+-----------------------------------------------------------------+
|                    PROCESSPILOT DAEMON                          |
+-----------------------------------------------------------------+
|                                                                 |
|  +--------------------+        +-----------------------------+  |
|  | Config Parser      | ---->  | Dependency Graph (DAG)      |  |
|  +--------------------+        +-----------------------------+  |
|                                                |                |
|                                                v                |
|  +--------------------+        +-----------------------------+  |
|  | Resource Monitor   | <----> | Process Manager             |  |
|  | (/proc/[PID])      |        | (fork/exec/waitpid/recovery)|  |
|  +--------------------+        +-----------------------------+  |
|                                                |                |
+------------------------------------------------|----------------+
                                                 v
                                  +-----------------------------+
                                  | UNIX Socket IPC Server      |
                                  | (/tmp/processpilot.sock)    |
                                  +-----------------------------+
                                                 ^
                                                 |
                                  +-----------------------------+
                                  | ProcessPilot CLI Tool       |
                                  +-----------------------------+
```

## Core Modules
1. **`ProcessManager`**: Handles process spawning via POSIX `fork()` & `execvp()`, non-blocking status checking (`waitpid(WNOHANG)`), and auto-recovery execution.
2. **`DependencyGraph`**: Computes topological startup order using Kahn's DAG algorithm.
3. **`ResourceMonitor`**: Queries CPU load & RSS memory via Linux `/proc/[PID]`.
4. **`UnixSocket`**: Provides IPC communication between client CLI and supervisor daemon.
