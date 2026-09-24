# ProcessPilot: Linux Service Manager and Process Supervisor

ProcessPilot is a lightweight, high-performance Linux service management and process supervision system built in modern C++17.

## Features
- **Process Supervision**: Spawns and manages child background processes via POSIX `fork()`, `execvp()`, and `waitpid()`.
- **Auto-Recovery**: Automatic crash detection (`SIGCHLD`) and restart policy execution (`on-failure`, `always`).
- **Dependency Graph (DAG)**: Resolves topological startup order for services with dependencies.
- **Inter-Process Communication (IPC)**: UNIX Domain Socket server/client architecture (`/tmp/processpilot.sock`).
- **Resource Monitoring**: Real-time CPU % and Memory (RSS) tracking via `/proc/[PID]`.
- **CLI Control Utility**: `processpilot_cli` for querying status, starting, stopping, and restarting services.

## Build Instructions
```bash
mkdir build && cd build
cmake ..
make -j4
```

## Running Unit Tests
```bash
cd build
ctest --output-on-failure
```

## Running ProcessPilot Supervisor Daemon
```bash
./build/processpilot_daemon configs/demo.service
```

## CLI Usage Examples
```bash
# Query all service statuses
./build/processpilot_cli status

# Start a specific service
./build/processpilot_cli start demo_app

# Stop a service
./build/processpilot_cli stop demo_app
```
