cmake_minimum_required(VERSION 3.22.1)
project(pandroid)
if (${ANDROID_ABI} STREQUAL "arm64-v8a")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8.1-a+crypto")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=format-security")
set(ENABLE_VULKAN OFF CACHE BOOL "" FORCE)
set(BUILD_HYDRA_CORE ON CACHE BOOL "" FORCE)
set(PANDA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_subdirectory(${PANDA_BASE_DIR}/ ${CMAKE_CURRENT_BINARY_DIR}/panda_core)
add_library(${CMAKE_PROJECT_NAME} SHARED pandroid.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME}
    android
    log
    EGL
    Alber
)
# TODO: can panda target_include_directories with PUBLIC so we get them here?
target_include_directories(pandroid PRIVATE ${PANDA_BASE_DIR}/include ${PANDA_BASE_DIR}/include/kernel ${PANDA_BASE_DIR}/third_party/result/include ${PANDA_BASE_DIR}/third_party/glad/include
        ${PANDA_BASE_DIR}/third_party/dynarmic/src ${PANDA_BASE_DIR}/third_party/LuaJIT/src ${PANDA_BASE_DIR}/third_party/cryptopp ${PANDA_BASE_DIR}/third_party/opengl ${PANDA_BASE_DIR}/third_party/boost)
add_compile_definitions(CPU_DYNARMIC)