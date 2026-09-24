cmake_minimum_required(VERSION 3.10)
project(processpilot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread -D_GNU_SOURCE")

include_directories(
    src
    src/daemon
    src/process
    src/config
    src/monitor
    src/dependency
    src/ipc
    src/logging
)

file(GLOB LIB_SOURCES
    src/logging/logger.cpp
    src/config/config_parser.cpp
    src/dependency/dependency_graph.cpp
    src/monitor/resource_monitor.cpp
    src/process/process_manager.cpp
    src/ipc/unix_socket.cpp
    src/daemon/daemon.cpp
)

# Supervisor Daemon Executable
add_executable(processpilot_daemon src/main.cpp ${LIB_SOURCES})

# User CLI Executable
add_executable(processpilot_cli cli/processpilot_cli.cpp src/ipc/unix_socket.cpp src/logging/logger.cpp)

# Unit Tests
enable_testing()

add_executable(process_tests tests/process_tests.cpp ${LIB_SOURCES})
add_test(NAME process_tests COMMAND process_tests)

add_executable(config_tests tests/config_tests.cpp ${LIB_SOURCES})
add_test(NAME config_tests COMMAND config_tests)

add_executable(dependency_tests tests/dependency_tests.cpp ${LIB_SOURCES})
add_test(NAME dependency_tests COMMAND dependency_tests)
