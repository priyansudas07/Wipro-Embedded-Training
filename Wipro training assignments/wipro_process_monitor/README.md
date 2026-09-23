# Linux Process Monitor (C++)

C++ program utilizing `<filesystem>`, `<iostream>`, and `<vector>` to list running processes from `/proc`, displaying PID, process name, state, and parent PID.

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
