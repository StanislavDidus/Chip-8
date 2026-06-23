cmake_minimum_required(VERSION 3.15...4.4.0)

project(CHIP8 VERSION 1.0
    DESCRIPTION "Chip-8 emulator written in modern C++"
    LANGUAGES C CXX)

if(MSVC)
    add_compile_options(/MP)
endif(MSVC)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Don't build shared libraries." FORCE)

add_subdirectory(externals/SDL)

set(SOURCE_FILES
        src/main.cpp
        src/SDL/window_renderer.cpp
        src/Chip8.cpp
)

set(HEADER_FILES
        inc/SDL/window_renderer.hpp
        inc/Chip8.hpp
        inc/Config.hpp
)

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(CHIP8 ${ALL_FILES})

target_include_directories(CHIP8 PUBLIC ${CMAKE_SOURCE_DIR}/inc)

target_link_libraries(CHIP8 PUBLIC SDL3::SDL3-static)

target_compile_features(CHIP8 PUBLIC cxx_std_23)
