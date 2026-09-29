# Stage 4: Prototype Implementation Details

## 1. Codebase Architecture & Prototype Documentation

### Directory Structure
```
vdevpulse/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── configs/
│   └── vdev_policy.json
├── docs/
│   ├── stage1_introduction.md
│   ├── stage2_requirements_prd.md
│   ├── stage3_architecture.md
│   ├── stage4_prototype.md
│   ├── stage5_testing.md
│   ├── stage6_final_report.md
│   └── project_presentation_guide.md
├── include/
│   └── vdevpulse/
│       ├── config.hpp
│       ├── device_manager.hpp
│       ├── logger.hpp
│       └── telemetry_monitor.hpp
├── src/
│   ├── config.cpp
│   ├── device_manager.cpp
│   ├── logger.cpp
│   ├── main.cpp
│   └── telemetry_monitor.cpp
└── tests/
    ├── device_test.cpp
    └── telemetry_test.cpp
```

### Module Implementations
1. **Thread-Safe Logger (`logger.cpp`)**: Uses `std::mutex` and RAII `std::lock_guard` for concurrent thread logging.
2. **Telemetry Monitor (`telemetry_monitor.cpp`)**: Parses `/proc/stat` and `/proc/meminfo` snapshot deltas to calculate CPU/RAM percentages:
   $$\text{CPU Utilization \%} = \frac{\Delta \text{Active Ticks}}{\Delta \text{Total Ticks}} \times 100$$
3. **Virtual Device Manager (`device_manager.cpp`)**: Manages POSIX FIFO node creation (`mkfifo`) at `/tmp/vdevpulse` and parses control commands (`PING`, `STATUS`, `RESET`).
4. **Policy Engine (`config.cpp`)**: Loads JSON runtime settings and sets fallback default parameters.

---

## 2. Version Control & Git Commit Tracking

- **SDLC Phase**: Stage 4 - Prototype Implementation
- **Commit Target**: `[Stage 4] Prototype implementation of core C++17 system modules`
- **Branch**: `main`
- **Repository Path**: `Wipro Individual Project/vdevpulse/`

---

## 3. Progress Evidence

- All core C++ source files compiled cleanly with standard C++17 compiler (`g++ -std=c++17`).
- Executable binary `vdevpulse_daemon` generated via CMake build system.
- Real-time CPU and RAM telemetry reading verified on POSIX environment.

---

## 4. Demonstration & Presentation Notes

- **Key Takeaway for Mentors**: Demonstrate running `./vdevpulse_daemon ../configs/vdev_policy.json` and interacting with `/tmp/vdevpulse` using standard Linux commands (`cat /tmp/vdevpulse`, `echo "PING" > /tmp/vdevpulse`).
- **Demo Focus**: Show clean program exit when receiving `SIGINT` (Ctrl+C), including device cleanup (`cleanup()`).

---

## 5. Roadmap for Next Stage (Stage 5)

- Construct automated unit test fixtures (`device_test.cpp`, `telemetry_test.cpp`).
- Integrate CMake CTest target suite.
- Execute unit testing and generate verification pass report.
