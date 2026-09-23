# Linux Process Monitor (C++)

C++ program utilizing `<filesystem>`, `<iostream>`, and `<vector>` to list running processes from `/proc`, displaying PID, process name, state, and parent PID.

---

## 📊 Execution Output Screenshots

### Full System & Hardware Summary
![Full System Summary](./docs/full_summary.png)

### System Information Output
![System Information](./docs/sys_info.png)

### CPU Information Output
![CPU Information](./docs/cpu_info.png)

### Memory Information Output
![Memory Information](./docs/memory_info.png)

---

## Files
- `main.cpp` - Interactive menu and CLI
- `process_monitor.cpp` - `/proc` directory reader and status parser
- `process_monitor.hpp` - Header file
- `Makefile` - Build file (`g++ -std=c++17`)

## Compilation and Execution
```bash
make
./process_monitor
```
