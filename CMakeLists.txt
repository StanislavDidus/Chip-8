cmake_minimum_required(VERSION 3.15...4.4.0)

project(CHIP8 VERSION 1.0
    DESCRIPTION "Chip-8 emulator written in modern C++"
    LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif(MSVC)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

# Add SDL library
add_subdirectory(externals/SDL)

# Add imgui library
add_library(imgui
    externals/imgui/imgui.cpp
    externals/imgui/imgui_draw.cpp
    externals/imgui/imgui_widgets.cpp
    externals/imgui/imgui_tables.cpp
    externals/imgui/imgui_demo.cpp
    externals/imgui/backends/imgui_impl_sdl3.cpp
    externals/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC externals/imgui externals/imgui/backends externals/imgui_filebrowser)
target_link_libraries(imgui PUBLIC SDL3::SDL3-static)

set(CHIP8_SOURCE_FILES
        src/CHIP8/chip8_audio.cpp
)

set(CHIP48_SOURCE_FILES
        src/CHIP48/chip48_instructions.cpp
)

set(SCHIP_SOURCE_FILES
        src/SCHIP/schip_display_i.cpp
        src/SCHIP/schip_instructions_i.cpp
)

set(XOCHIP_SOURCE_FILES
        src/XOCHIP/xochip_instructions.cpp
        src/XOCHIP/xochip_display.cpp
        src/XOCHIP/xochip_audio.cpp

)

set(OTHER_SOURCE_FILES
        src/main.cpp
        src/chip8.cpp
        src/SDL/window_renderer.cpp
        src/Audio/audio.cpp
        src/instructions.cpp
)

set(SOURCE_FILES ${CHIP8_SOURCE_FILES} ${CHIP48_SOURCE_FILES} ${SCHIP_SOURCE_FILES} ${XOCHIP_SOURCE_FILES} ${OTHER_SOURCE_FILES})

set (CHIP8_HEADER_FILES
        inc/CHIP8/chip8_display.hpp
        inc/CHIP8/chip8_memory.hpp
        inc/CHIP8/chip8_audio.hpp
        inc/CHIP8/chip8_quirks.hpp
)

set (CHIP48_HEADER_FILES
        inc/CHIP48/chip48_instructions.hpp
        inc/CHIP48/chip48_quirks.hpp
)

set (SCHIP_HEADER_FILES
        inc/SCHIP/schip_display.hpp
        inc/SCHIP/schip_instructions.hpp
        inc/SCHIP/schip_display_i.hpp
        inc/SCHIP/schip_instructions_i.hpp
        inc/SCHIP/schip_display.hpp
        inc/SCHIP/schip_quirks.hpp
)

set (XOCHIP_HEADER_FILES
        inc/XOCHIP/xochip_display.hpp
        inc/XOCHIP/xochip_memory.hpp
        inc/XOCHIP/xochip_instructions.hpp
        inc/XOCHIP/xochip_audio.hpp
        inc/XOCHIP/xochip_quirks.hpp
)

set (OTHER_HEADER_FILES
        inc/chip8.hpp
        inc/config.hpp
        inc/SDL/window_renderer.hpp
        inc/Audio/audio.hpp

        inc/display.hpp
        inc/instructions.hpp
        inc/memory.hpp
        inc/core.hpp
        inc/enums.hpp
        inc/logger.hpp
        inc/quirks.h
)

set(HEADER_FILES ${CHIP8_HEADER_FILES} ${CHIP48_HEADER_FILES} ${SCHIP_HEADER_FILES} ${XOCHIP_HEADER_FILES} ${OTHER_HEADER_FILES})

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(CHIP8 ${ALL_FILES})

target_include_directories(CHIP8 PUBLIC ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(CHIP8 PUBLIC SDL3::SDL3-static imgui)

target_compile_features(CHIP8 PUBLIC cxx_std_23)
