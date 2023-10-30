cmake_minimum_required(VERSION 3.22.1)
project(pandroid)
set(PANDA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")

option(ENABLE_LUAJIT "" OFF)
option(ENABLE_VULKAN "" OFF)

add_subdirectory(${PANDA_BASE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Alber)

add_library(pandroid SHARED
    android.cpp
)
target_link_libraries(pandroid
    android
    log
    EGL
    Alber
)
# Optimally Albers cmake would target_compile_definitions instead of add_compile_definitions but
# peach doesn't like his code tampered with too much so I'm afraid to change it
target_compile_definitions(pandroid PRIVATE CPU_DYNARMIC)
