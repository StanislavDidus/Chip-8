#pragma once
#include <bitset>
#include <cstdint>
#include <filesystem>

#include "Config.hpp"
#include "SDL/window_renderer.hpp"

class chip8
{
public:
    chip8() = default;
    chip8(const std::filesystem::path& path_to_rom, window_renderer* renderer);
    ~chip8() = default;

    void update(float delta_time);
    void render();
private:
    uint8_t memory[4096] {};
    uint16_t program_counter { STARTING_POINT };
    uint16_t stack_pointer {};
    uint16_t index_register {};
    uint16_t stack[16] {};
    uint8_t delay_timer {};
    uint8_t sound_timer {};
    uint8_t registry[16] {};
    uint8_t screen[64 * 32] {};

    window_renderer* renderer;

    SDL_Surface* surface;
    SDL_Texture* texture;
};
