cmake_minimum_required(VERSION 3.14)
project(SandBoxX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/sandboxx)

# Core Library Sources
set(CORE_SOURCES
    src/common/logger.cpp
    src/common/utils.cpp
    src/namespace/namespace_manager.cpp
    src/cgroup/cgroup_manager.cpp
    src/filesystem/filesystem_manager.cpp
    src/security/security_manager.cpp
    src/process/process_manager.cpp
    src/monitor/monitor.cpp
    src/runtime/runtime.cpp
    src/cli/cli.cpp
)

add_library(sandboxx_core STATIC ${CORE_SOURCES})

# Executable Binary
add_executable(sandboxx src/main.cpp)
target_link_libraries(sandboxx PRIVATE sandboxx_core)

# Example binaries compilation
add_executable(example_hello examples/hello.cpp)
add_executable(example_cpu_bomb examples/cpu_bomb.cpp)
add_executable(example_memory_bomb examples/memory_bomb.cpp)
add_executable(example_file_test examples/file_test.cpp)
add_executable(example_hostname_test examples/hostname_test.cpp)

# Testing Configuration
enable_testing()

add_executable(namespace_test tests/namespace_test.cpp)
target_link_libraries(namespace_test PRIVATE sandboxx_core)
add_test(NAME namespace_test COMMAND namespace_test)

add_executable(cgroup_test tests/cgroup_test.cpp)
target_link_libraries(cgroup_test PRIVATE sandboxx_core)
add_test(NAME cgroup_test COMMAND cgroup_test)

add_executable(filesystem_test tests/filesystem_test.cpp)
target_link_libraries(filesystem_test PRIVATE sandboxx_core)
add_test(NAME filesystem_test COMMAND filesystem_test)

add_executable(security_test tests/security_test.cpp)
target_link_libraries(security_test PRIVATE sandboxx_core)
add_test(NAME security_test COMMAND security_test)

add_executable(runtime_test tests/runtime_test.cpp)
target_link_libraries(runtime_test PRIVATE sandboxx_core)
add_test(NAME runtime_test COMMAND runtime_test)
