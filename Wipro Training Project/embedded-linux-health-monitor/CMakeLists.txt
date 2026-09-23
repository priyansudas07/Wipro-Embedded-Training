cmake_minimum_required(VERSION 3.10)
project(embedded_linux_health_monitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

include_directories(include)

file(GLOB SOURCES
    src/config_manager.cpp
    src/logger.cpp
    src/cpu_monitor.cpp
    src/memory_monitor.cpp
    src/disk_monitor.cpp
    src/temperature_monitor.cpp
    src/network_monitor.cpp
    src/service_monitor.cpp
)

# Main Executable
add_executable(health_monitor src/main.cpp ${SOURCES})

# Unit Tests
enable_testing()

add_executable(test_cpu tests/test_cpu.cpp ${SOURCES})
add_test(NAME test_cpu COMMAND test_cpu)

add_executable(test_memory tests/test_memory.cpp ${SOURCES})
add_test(NAME test_memory COMMAND test_memory)

add_executable(test_config tests/test_config.cpp ${SOURCES})
add_test(NAME test_config COMMAND test_config)

add_executable(test_service_monitor tests/test_service_monitor.cpp ${SOURCES})
add_test(NAME test_service_monitor COMMAND test_service_monitor)
