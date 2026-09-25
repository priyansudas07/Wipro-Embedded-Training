# SandBoxX Requirements Document

## Functional Requirements

1. **Process Isolation**: Isolate process execution using Linux PID, Mount, Network, IPC, and UTS namespaces.
2. **Resource Boundaries**: Limit CPU utilization percentage, memory limit in MB, and total allowed PIDs via Control Groups.
3. **Security Policies**: Prevent capability escalation using `PR_SET_NO_NEW_PRIVS` and drop dangerous capabilities.
4. **CLI Suite**: Provide command-line options `run`, `list`, `inspect`, and `stop`.
5. **Logging & Monitoring**: Log execution events with timestamps and provide real-time process statistics.
