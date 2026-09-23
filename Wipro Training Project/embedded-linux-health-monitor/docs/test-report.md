# 📊 Test Verification Report

## Test Summary

| Test Case | Module | Execution Result | Status |
| :--- | :--- | :--- | :---: |
| `test_cpu` | `CpuMonitor` | CPU usage calculated within [0%, 100%] | PASSED |
| `test_memory` | `MemoryMonitor` | Total & Used RAM parsed via `sysinfo()` | PASSED |
| `test_config` | `ConfigManager` | Parsed `config/health_monitor.json` | PASSED |
| `test_service_monitor` | `ServiceMonitor` | Tested process detection & recovery triggers | PASSED |

## Verification Execution Output

```text
Running tests...
Test project /root/Wipro-Embedded-Training/Wipro Training Project/embedded-linux-health-monitor/build
    Start 1: test_cpu
1/4 Test #1: test_cpu .........................   Passed    0.01 sec
    Start 2: test_memory
2/4 Test #2: test_memory ......................   Passed    0.01 sec
    Start 3: test_config
3/4 Test #3: test_config ......................   Passed    0.01 sec
    Start 4: test_service_monitor
4/4 Test #4: test_service_monitor .............   Passed    0.01 sec

100% tests passed, 0 tests failed out of 4
```
