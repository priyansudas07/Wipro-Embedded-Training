# VDevPulse: Linux Virtual Device Interface & System Telemetry Monitor

VDevPulse is a high-performance C++17 system daemon that bridges Linux POSIX virtual character device I/O with kernel hardware telemetry monitoring (`/proc`).

## Architecture & Features

- **POSIX Virtual Device Driver Emulation**: Creates and manages FIFO stream channels simulating `/dev/vdevpulse` for non-blocking asynchronous user-space communication.
- **Kernel Telemetry Parser**: Monitors CPU utilization and RAM consumption in real-time from kernel synthetic filesystems (`/proc/stat`, `/proc/meminfo`).
- **Policy Engine**: Dynamic JSON rule loader configuring sampling intervals, memory limits, and log destinations.
- **Thread-Safe Logging**: Synchronized multi-threaded logging system supporting debug, info, warning, and error severity levels.
- **Unit Testing**: Suite powered by standard C++ test fixtures.

## System Requirements

- Linux OS (Ubuntu 20.04/22.04 LTS, Debian, or WSL2)
- C++17 compliant compiler (`g++` >= 9.0 or `clang++` >= 10.0)
- CMake 3.14+
- Make or Ninja build toolchain

## Quick Start & Build Instructions

```bash
# Clone repository and navigate to project directory
git clone https://github.com/priyansudas07/Wipro-Embedded-Training.git
cd "Wipro-Embedded-Training/Wipro Individual Project/vdevpulse"

# Create build directory and run CMake
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# Execute test suite
ctest --output-on-failure

# Launch VDevPulse daemon
./vdevpulse run ../configs/vdev_policy.json
```

## Documentation

- [Project Report (PDF)](docs/VDevPulse_Project_Documentation.pdf)
- [Project Report (HTML)](docs/VDevPulse_Project_Documentation.html)

Full SDLC documentation and architecture diagrams are also available across the discrete stage files:

- [Stage 1: Project Overview & Objectives](docs/stage1_introduction.md)
- [Stage 2: Requirements Analysis & PRD](docs/stage2_requirements_prd.md)
- [Stage 3: System Architecture & UML Diagrams](docs/stage3_architecture.md)
- [Stage 4: Prototype Implementation](docs/stage4_prototype.md)
- [Stage 5: Verification & Testing Report](docs/stage5_testing.md)
- [Stage 6: Final Deployment & Maintenance Report](docs/stage6_final_report.md)
- [Executive Technical Presentation Guide](docs/project_presentation_guide.md)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
