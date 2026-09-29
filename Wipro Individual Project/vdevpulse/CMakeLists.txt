cmake_minimum_required(VERSION 3.14)
project(VDevPulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/vdevpulse)

set(CORE_SOURCES
    src/logger.cpp
    src/config.cpp
    src/device_manager.cpp
    src/telemetry_monitor.cpp
)

add_library(vdevpulse_core STATIC ${CORE_SOURCES})

add_executable(vdevpulse src/main.cpp)
target_link_libraries(vdevpulse PRIVATE vdevpulse_core)

enable_testing()

add_executable(device_test tests/device_test.cpp)
target_link_libraries(device_test PRIVATE vdevpulse_core)
add_test(NAME device_test COMMAND device_test)

add_executable(telemetry_test tests/telemetry_test.cpp)
target_link_libraries(telemetry_test PRIVATE vdevpulse_core)
add_test(NAME telemetry_test COMMAND telemetry_test)
