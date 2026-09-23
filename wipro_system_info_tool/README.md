# Linux System Information Tool

C program that fetches Linux system information, CPU details, and memory usage using POSIX system calls (`uname`, `sysinfo`) and `/proc` files.

## Files
- `main.c` - Entry point and user menu
- `sys_info.c` - System info parsing implementation
- `sys_info.h` - Header file
- `Makefile` - Build instructions

## Compilation and Execution
```bash
make
./sysinfo_tool
```
