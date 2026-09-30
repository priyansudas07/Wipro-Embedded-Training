# Stage 5: Testing, Integration & Improvement

## 5.1 Test Strategy Overview

VDevPulse employs automated unit testing using a CTest-integrated CMake build. Tests are compiled as standalone executable binaries linked against the `vdevpulse_core` static library.

---

## 5.2 Unit Test Case Specification Matrix

### Test Suite 1: `device_test.cpp` — DeviceManager & IOCTL Suite

| Test Case ID | Test Description | Action / Input | Expected Assertion | Result |
| :--- | :--- | :--- | :--- | :--- |
| **TC-DEV-01** | Device Initialization | `initDevice(config)` with `/tmp/vdevpulse_unittest` | Returns `true`; FIFO node created | **PASS** |
| **TC-DEV-02** | Device Open | `openDevice()` | Returns `true`; state is `RUNNING` | **PASS** |
| **TC-DEV-03** | Device Write | `writeData("UNIT_TEST_PAYLOAD")` | Returns `true`; bytes written | **PASS** |
| **TC-DEV-04** | IOCTL START | `sendIoctl(VDEV_IOCTL_START)` | Returns `true`; state is `RUNNING` | **PASS** |
| **TC-DEV-05** | IOCTL GET_STATS | `sendIoctl(VDEV_IOCTL_GET_STATS)` | Returns `true`; logs I/O statistics | **PASS** |
| **TC-DEV-06** | IOCTL STOP | `sendIoctl(VDEV_IOCTL_STOP)` | Returns `true`; state is `STOPPED` | **PASS** |
| **TC-DEV-07** | Query PING | `processQueryCommand("PING", t)` | Returns `"PONG"` | **PASS** |
| **TC-DEV-08** | Query GET_CPU | `processQueryCommand("GET_CPU", t)` | Returns string containing `"CPU_PCT="` | **PASS** |
| **TC-DEV-09** | Query GET_TOP | `processQueryCommand("GET_TOP", t)` | Returns string containing `"TOP_CONSUMER="` | **PASS** |
| **TC-DEV-10** | Query GET_HEALTH | `processQueryCommand("GET_HEALTH", t)` | Returns `"HEALTH=HEALTHY"` | **PASS** |
| **TC-DEV-11** | Device Cleanup | `closeDevice()` | Closes fd; removes FIFO node | **PASS** |

---

### Test Suite 2: `telemetry_test.cpp` — TelemetryMonitor, Loadavg & JSON

| Test Case ID | Test Description | Action / Input | Expected Assertion | Result |
| :--- | :--- | :--- | :--- | :--- |
| **TC-TEL-01** | Total RAM Metric | `collectTelemetry(cfg)` | `memory_total_mb > 0` | **PASS** |
| **TC-TEL-02** | Used RAM Non-Negative | `collectTelemetry(cfg)` | `memory_used_mb >= 0` | **PASS** |
| **TC-TEL-03** | System Uptime | `collectTelemetry(cfg)` | `uptime_seconds >= 0` | **PASS** |
| **TC-TEL-04** | System Load Average | `collectTelemetry(cfg)` | `load_1m >= 0.0` | **PASS** |
| **TC-TEL-05** | Total Processes Count | `collectTelemetry(cfg)` | `total_processes >= 0` | **PASS** |
| **TC-TEL-06** | Top Process Scanner | `getTopProcesses(5)` | Returns sorted vector of active processes | **PASS** |
| **TC-TEL-07** | Health Status Evaluated | `collectTelemetry(cfg)` | `!health_status.empty()` | **PASS** |
| **TC-TEL-08** | JSON Serialization | `toJsonString(metrics)` | Contains `"health_status"` and `"top_processes"` | **PASS** |
| **TC-TEL-09** | History Ring Buffer | `recordHistory(metrics)` | `!getHistory().empty()` | **PASS** |

---

## 5.3 Automated CTest Execution Output

```text
Test project /mnt/f/Projects/Wipro-Embedded-Training/Wipro Individual Project/vdevpulse/build
    Start 1: device_test
1/2 Test #1: device_test ......................   Passed    0.01 sec
    Start 2: telemetry_test
2/2 Test #2: telemetry_test ...................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 2
Total Test time (real) =   0.10 sec
```

**Result: All unit tests PASSED (100% pass rate).**

---

## 5.4 Version Control & Progress Evidence

- **SDLC Phase**: Stage 5 — Testing, Integration & Improvement
- **Git Commit**: `[Stage 5] Unit testing suite & automated verification report`
- **Evidence**: 100% CTest pass log documented above.
