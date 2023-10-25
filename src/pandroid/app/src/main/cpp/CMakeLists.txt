# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.
cmake_minimum_required(VERSION 3.22.1)
project(pandroid)
set(PANDA_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../..)
set(CMAKE_CXX_STANDARD 20)


# We need to be able to use enable_language(OBJC) on Mac, so we need CMake 3.16 vs the 3.10 we use otherwise. Blame Apple.
if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
    cmake_minimum_required(VERSION 3.16)
else()
    cmake_minimum_required(VERSION 3.10)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 12)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fbracket-depth=4096")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(Alber)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(APPLE)
    enable_language(OBJC)
endif()

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -Wno-format-security")
endif() 

option(DISABLE_PANIC_DEV "Make a build with fewer and less intrusive asserts" ON)
option(GPU_DEBUG_INFO "Enable additional GPU debugging info" OFF)
option(ENABLE_OPENGL "Enable OpenGL rendering backend" ON)
option(ENABLE_VULKAN "Enable Vulkan rendering backend" ON)
option(ENABLE_LTO "Enable link-time optimization" OFF)
option(ENABLE_USER_BUILD "Make a user-facing build. These builds have various assertions disabled, LTO, and more" OFF)
option(ENABLE_HTTP_SERVER "Enable HTTP server. Used for Discord bot support" OFF)
option(ENABLE_DISCORD_RPC "Compile with Discord RPC support (disabled by default)" ON)
option(ENABLE_LUAJIT "Enable scripting with the Lua programming language" OFF)
option(ENABLE_QT_GUI "Enable the Qt GUI. If not selected then the emulator uses a minimal SDL-based UI instead" OFF)
option(BUILD_HYDRA_CORE "Build a Hydra core" on)

include_directories(${PANDA_BASE_DIR}/include/)
include_directories(${PANDA_BASE_DIR}/include/kernel)
include_directories (${FMT_INCLUDE_DIR})
include_directories(${PANDA_BASE_DIR}/third_party/boost/)
include_directories(${PANDA_BASE_DIR}/third_party/elfio/)
include_directories(${PANDA_BASE_DIR}/third_party/imgui/)
include_directories(${PANDA_BASE_DIR}/third_party/dynarmic/src)
include_directories(${PANDA_BASE_DIR}/third_party/cryptopp/)
include_directories(${PANDA_BASE_DIR}/third_party/cityhash/include)
include_directories(${PANDA_BASE_DIR}/third_party/result/include)
include_directories(${PANDA_BASE_DIR}/third_party/xxhash/include)
include_directories(${PANDA_BASE_DIR}/third_party/httplib)
include_directories(${PANDA_BASE_DIR}/third_party/stb)
include_directories(${PANDA_BASE_DIR}/third_party/opengl)
include_directories(${PANDA_BASE_DIR}/third_party/mio/single_include)

add_compile_definitions(NOMINMAX)             # Make windows.h not define min/max macros because third-party deps don't like it
add_compile_definitions(WIN32_LEAN_AND_MEAN)  # Make windows.h not include literally everything
add_compile_definitions(SDL_MAIN_HANDLED)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(${PANDA_BASE_DIR}/third_party/SDL2 ${CMAKE_CURRENT_BINARY_DIR}/SDL2)

add_subdirectory(${PANDA_BASE_DIR}/third_party/toml11 ${CMAKE_CURRENT_BINARY_DIR}/toml11)
include_directories(${PANDA_BASE_DIR}/${SDL2_INCLUDE_DIR})
include_directories(${PANDA_BASE_DIR}/third_party/toml11)
include_directories(${PANDA_BASE_DIR}/third_party/glm)

add_subdirectory(${PANDA_BASE_DIR}/third_party/cmrc ${CMAKE_CURRENT_BINARY_DIR}/cmrc)

set(BOOST_ROOT "${PANDA_BASE_DIR}/third_party/boost")
set(Boost_INCLUDE_DIR "${PANDA_BASE_DIR}/third_party/boost")
set(Boost_NO_SYSTEM_PATHS ON)
add_compile_definitions(BOOST_NO_CXX98_FUNCTION_BASE) # Forbid Boost from using std::unary_function (Fixes MacOS build)

add_library(boost INTERFACE)
target_include_directories(boost SYSTEM INTERFACE ${Boost_INCLUDE_DIR})

set(CRYPTOPP_BUILD_TESTING OFF)
add_subdirectory(${PANDA_BASE_DIR}/third_party/cryptopp ${CMAKE_CURRENT_BINARY_DIR}/cryptopp)
add_subdirectory(${PANDA_BASE_DIR}/third_party/glad ${CMAKE_CURRENT_BINARY_DIR}/glad)

# Check for x64
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86-64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(HOST_X64 TRUE)
    add_subdirectory(${PANDA_BASE_DIR}/third_party/xbyak ${CMAKE_CURRENT_BINARY_DIR}/xbyak)
    include_directories(${PANDA_BASE_DIR}/third_party/xbyak)
    add_compile_definitions(PANDA3DS_DYNAPICA_SUPPORTED)
    add_compile_definitions(PANDA3DS_X64_HOST)
else()
    set(HOST_X64 FALSE)
endif()

# Check for arm64
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(HOST_ARM64 TRUE)
    add_compile_definitions(PANDA3DS_ARM64_HOST)
else()
    set(HOST_ARM64 FALSE)
endif()

if(HOST_X64 OR HOST_ARM64)
    set(DYNARMIC_TESTS OFF)
    #set(DYNARMIC_NO_BUNDLED_FMT ON)
    set(DYNARMIC_FRONTENDS "A32" CACHE STRING "")
    add_subdirectory(${PANDA_BASE_DIR}/third_party/dynarmic ${CMAKE_CURRENT_BINARY_DIR}/dynarmic)
    add_compile_definitions(CPU_DYNARMIC)
else()
    message(FATAL_ERROR "Currently unsupported CPU architecture")
endif()

set(SOURCE_FILES ${PANDA_BASE_DIR}/src/emulator.cpp ${PANDA_BASE_DIR}/src/io_file.cpp ${PANDA_BASE_DIR}/src/config.cpp
				 ${PANDA_BASE_DIR}/src/core/CPU/cpu_dynarmic.cpp ${PANDA_BASE_DIR}/src/core/CPU/dynarmic_cycles.cpp
				 ${PANDA_BASE_DIR}/src/core/memory.cpp ${PANDA_BASE_DIR}/src/renderer.cpp ${PANDA_BASE_DIR}/src/core/renderer_null/renderer_null.cpp
				 ${PANDA_BASE_DIR}/src/http_server.cpp ${PANDA_BASE_DIR}/src/stb_image_write.c ${PANDA_BASE_DIR}/src/core/cheats.cpp ${PANDA_BASE_DIR}/src/core/action_replay.cpp
                 ${PANDA_BASE_DIR}/src/discord_rpc.cpp ${PANDA_BASE_DIR}/src/lua.cpp ${PANDA_BASE_DIR}/src/memory_mapped_file.cpp
)
set(CRYPTO_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/crypto/aes_engine.cpp)
set(KERNEL_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/kernel/kernel.cpp ${PANDA_BASE_DIR}/src/core/kernel/resource_limits.cpp
                        ${PANDA_BASE_DIR}/src/core/kernel/memory_management.cpp ${PANDA_BASE_DIR}/src/core/kernel/ports.cpp
                        ${PANDA_BASE_DIR}/src/core/kernel/events.cpp ${PANDA_BASE_DIR}/src/core/kernel/threads.cpp
                        ${PANDA_BASE_DIR}/src/core/kernel/address_arbiter.cpp ${PANDA_BASE_DIR}/src/core/kernel/error.cpp
                        ${PANDA_BASE_DIR}/src/core/kernel/file_operations.cpp ${PANDA_BASE_DIR}/src/core/kernel/directory_operations.cpp
                        ${PANDA_BASE_DIR}/src/core/kernel/idle_thread.cpp ${PANDA_BASE_DIR}/src/core/kernel/timers.cpp
)
set(SERVICE_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/services/service_manager.cpp ${PANDA_BASE_DIR}/src/core/services/apt.cpp ${PANDA_BASE_DIR}/src/core/services/hid.cpp
                         ${PANDA_BASE_DIR}/src/core/services/fs.cpp ${PANDA_BASE_DIR}/src/core/services/gsp_gpu.cpp ${PANDA_BASE_DIR}/src/core/services/gsp_lcd.cpp
                         ${PANDA_BASE_DIR}/src/core/services/ndm.cpp ${PANDA_BASE_DIR}/src/core/services/dsp.cpp ${PANDA_BASE_DIR}/src/core/services/cfg.cpp
                         ${PANDA_BASE_DIR}/src/core/services/ptm.cpp ${PANDA_BASE_DIR}/src/core/services/mic.cpp ${PANDA_BASE_DIR}/src/core/services/cecd.cpp
                         ${PANDA_BASE_DIR}/src/core/services/ac.cpp ${PANDA_BASE_DIR}/src/core/services/am.cpp ${PANDA_BASE_DIR}/src/core/services/boss.cpp
                         ${PANDA_BASE_DIR}/src/core/services/frd.cpp ${PANDA_BASE_DIR}/src/core/services/nim.cpp ${PANDA_BASE_DIR}/src/core/services/mcu/mcu_hwc.cpp
                         ${PANDA_BASE_DIR}/src/core/services/y2r.cpp ${PANDA_BASE_DIR}/src/core/services/cam.cpp ${PANDA_BASE_DIR}/src/core/services/ldr_ro.cpp
                         ${PANDA_BASE_DIR}/src/core/services/act.cpp ${PANDA_BASE_DIR}/src/core/services/nfc.cpp ${PANDA_BASE_DIR}/src/core/services/dlp_srvr.cpp
                         ${PANDA_BASE_DIR}/src/core/services/ir_user.cpp ${PANDA_BASE_DIR}/src/core/services/http.cpp ${PANDA_BASE_DIR}/src/core/services/soc.cpp
                         ${PANDA_BASE_DIR}/src/core/services/ssl.cpp ${PANDA_BASE_DIR}/src/core/services/news_u.cpp ${PANDA_BASE_DIR}/src/core/services/amiibo_device.cpp
                         ${PANDA_BASE_DIR}/src/core/services/csnd.cpp ${PANDA_BASE_DIR}/src/core/services/nwm_uds.cpp
)
set(PICA_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/PICA/gpu.cpp ${PANDA_BASE_DIR}/src/core/PICA/regs.cpp ${PANDA_BASE_DIR}/src/core/PICA/shader_unit.cpp
                      ${PANDA_BASE_DIR}/src/core/PICA/shader_interpreter.cpp ${PANDA_BASE_DIR}/src/core/PICA/dynapica/shader_rec.cpp
                      ${PANDA_BASE_DIR}/src/core/PICA/dynapica/shader_rec_emitter_x64.cpp ${PANDA_BASE_DIR}/src/core/PICA/pica_hash.cpp
)

set(LOADER_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/loader/elf.cpp ${PANDA_BASE_DIR}/src/core/loader/ncsd.cpp ${PANDA_BASE_DIR}/src/core/loader/ncch.cpp ${PANDA_BASE_DIR}/src/core/loader/3dsx.cpp ${PANDA_BASE_DIR}/src/core/loader/lz77.cpp)
set(FS_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/fs/archive_self_ncch.cpp ${PANDA_BASE_DIR}/src/core/fs/archive_save_data.cpp ${PANDA_BASE_DIR}/src/core/fs/archive_sdmc.cpp
                    ${PANDA_BASE_DIR}/src/core/fs/archive_ext_save_data.cpp ${PANDA_BASE_DIR}/src/core/fs/archive_ncch.cpp ${PANDA_BASE_DIR}/src/core/fs/romfs.cpp
                    ${PANDA_BASE_DIR}/src/core/fs/ivfc.cpp ${PANDA_BASE_DIR}/src/core/fs/archive_user_save_data.cpp ${PANDA_BASE_DIR}/src/core/fs/archive_system_save_data.cpp
)

set(APPLET_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/applets/applet.cpp ${PANDA_BASE_DIR}/src/core/applets/mii_selector.cpp ${PANDA_BASE_DIR}/src/core/applets/software_keyboard.cpp ${PANDA_BASE_DIR}/src/core/applets/applet_manager.cpp)
set(RENDERER_SW_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/renderer_sw/renderer_sw.cpp)

cmrc_add_resource_library(
	resources_console_fonts
	NAMESPACE ConsoleFonts
	WHENCE "${PANDA_BASE_DIR}/src/core/services/fonts/"
	"${PANDA_BASE_DIR}/src/core/services/fonts/CitraSharedFontUSRelocated.bin"
)

set(THIRD_PARTY_SOURCE_FILES 
                             ${PANDA_BASE_DIR}/third_party/cityhash/cityhash.cpp
                             ${PANDA_BASE_DIR}/third_party/xxhash/xxhash.c
)


source_group("Source Files\\Core" FILES ${SOURCE_FILES})
source_group("Source Files\\Core\\Crypto" FILES ${CRYPTO_SOURCE_FILES})
source_group("Source Files\\Core\\Filesystem" FILES ${FS_SOURCE_FILES})
source_group("Source Files\\Core\\Kernel" FILES ${KERNEL_SOURCE_FILES})
source_group("Source Files\\Core\\Loader" FILES ${LOADER_SOURCE_FILES})
source_group("Source Files\\Core\\Services" FILES ${SERVICE_SOURCE_FILES})
source_group("Source Files\\Core\\Applets" FILES ${APPLET_SOURCE_FILES})
source_group("Source Files\\Core\\PICA" FILES ${PICA_SOURCE_FILES})
source_group("Source Files\\Core\\Software Renderer" FILES ${RENDERER_SW_SOURCE_FILES})
source_group("Source Files\\Third Party" FILES ${THIRD_PARTY_SOURCE_FILES})

set(RENDERER_GL_SOURCE_FILES "") # Empty by default unless we are compiling with the GL renderer
set(RENDERER_VK_SOURCE_FILES "") # Empty by default unless we are compiling with the VK renderer

if(ENABLE_OPENGL)
    # This may look weird but opengl.hpp is our header even if it's in the third_party folder

	set(RENDERER_GL_SOURCE_FILES ${PANDA_BASE_DIR}/src/core/renderer_gl/renderer_gl.cpp
        ${PANDA_BASE_DIR}/src/core/renderer_gl/textures.cpp ${PANDA_BASE_DIR}/src/core/renderer_gl/etc1.cpp
        ${PANDA_BASE_DIR}/src/core/renderer_gl/gl_state.cpp ${PANDA_BASE_DIR}/src/host_shaders/opengl_display.frag
        ${PANDA_BASE_DIR}/src/host_shaders/opengl_display.vert ${PANDA_BASE_DIR}/src/host_shaders/opengl_vertex_shader.vert
        ${PANDA_BASE_DIR}/src/host_shaders/opengl_fragment_shader.frag
	)
    source_group("Source Files\\Core\\OpenGL Renderer" FILES ${RENDERER_GL_SOURCE_FILES})

	cmrc_add_resource_library(
		resources_renderer_gl
		NAMESPACE RendererGL
		WHENCE "${PANDA_BASE_DIR}/src/host_shaders/"
		"${PANDA_BASE_DIR}/src/host_shaders/opengl_display.frag"
		"${PANDA_BASE_DIR}/src/host_shaders/opengl_display.vert"
		"${PANDA_BASE_DIR}/src/host_shaders/opengl_vertex_shader.vert"
		"${PANDA_BASE_DIR}/src/host_shaders/opengl_fragment_shader.frag"
	)
endif()

source_group("Header Files\\Core" FILES ${HEADER_FILES})
set(ALL_SOURCES ${SOURCE_FILES} ${FRONTEND_SOURCE_FILES} ${FS_SOURCE_FILES} ${CRYPTO_SOURCE_FILES} ${KERNEL_SOURCE_FILES} 
    ${LOADER_SOURCE_FILES} ${SERVICE_SOURCE_FILES} ${APPLET_SOURCE_FILES} ${RENDERER_SW_SOURCE_FILES} ${PICA_SOURCE_FILES} ${THIRD_PARTY_SOURCE_FILES})

if(ENABLE_OPENGL)
    # Add the OpenGL source files to ALL_SOURCES
    set(ALL_SOURCES ${ALL_SOURCES} ${RENDERER_GL_SOURCE_FILES})
endif()

add_library(Alber STATIC ${ALL_SOURCES})

target_link_libraries(Alber PRIVATE dynarmic SDL2-static cryptopp glad resources_console_fonts)

if(ENABLE_OPENGL)
    target_compile_definitions(Alber PUBLIC "PANDA3DS_ENABLE_OPENGL=1")
    target_link_libraries(Alber PRIVATE resources_renderer_gl)
endif()





add_library(pandroid SHARED
    android.cpp
)
target_link_libraries(pandroid
    android
    log
    EGL
    Alber
)
target_include_directories(pandroid PRIVATE ${PANDA_BASE_DIR}/include ${PANDA_BASE_DIR}/include/renderer_gl ${PANDA_BASE_DIR}/third_party/glad/include)