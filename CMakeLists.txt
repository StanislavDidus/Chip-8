cmake_minimum_required(VERSION 3.15...4.4.0)

project(CHIP8 VERSION 1.0
    DESCRIPTION "Chip-8 emulator written in modern C++"
    LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif(MSVC)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

add_subdirectory(externals/SDL)

set(CHIP8_SOURCE_FILES
        src/CHIP8/chip8_instructions.cpp
)

set(CHIP48_SOURCE_FILES
        src/CHIP48/chip48_instructions.cpp
)

set(SCHIP_SOURCE_FILES
        src/SCHIP/schip_display.cpp
        src/SCHIP/schip_instructions.cpp)

set(OTHER_SOURCE_FILES
        src/main.cpp
        src/chip8.cpp
        src/SDL/window_renderer.cpp
        src/Audio/audio.cpp
        src/instructions.cpp
)

set(SOURCE_FILES ${CHIP8_SOURCE_FILES} ${CHIP48_SOURCE_FILES} ${SCHIP_SOURCE_FILES} ${OTHER_SOURCE_FILES})

set (CHIP8_HEADER_FILES
        inc/CHIP8/chip8_display.hpp
        inc/CHIP8/chip8_instructions.hpp
        inc/CHIP8/chip8_memory.hpp
)

set (CHIP48_HEADER_FILES
        inc/CHIP48/chip48_instructions.hpp
)

set (SCHIP_HEADER_FILES
        inc/SCHIP/schip_display.hpp
        inc/SCHIP/schip_instructions.hpp)

set (OTHER_HEADER_FILES
        inc/chip8.hpp
        inc/config.hpp
        inc/SDL/window_renderer.hpp
        inc/Audio/audio.hpp

        inc/display.hpp
        inc/instructions.hpp
        inc/memory.hpp
        inc/core.hpp
)

set(HEADER_FILES ${CHIP8_HEADER_FILES} ${CHIP48_HEADER_FILES} ${SCHIP_HEADER_FILES} ${OTHER_HEADER_FILES})

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES}
        inc/SCHIP/schip_display.hpp)

add_executable(CHIP8 ${ALL_FILES})

target_include_directories(CHIP8 PUBLIC ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(CHIP8 PUBLIC SDL3::SDL3-static)

target_compile_features(CHIP8 PUBLIC cxx_std_23)
