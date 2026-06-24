#pragma once
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <random>
#include <unordered_map>

#include "config.hpp"
#include "SDL/window_renderer.hpp"

class chip8
{
public:
    chip8() = default;
    chip8(const std::filesystem::path& path_to_rom, window_renderer* renderer);
    ~chip8() = default;

    void update(float delta_time);
    void render(window_renderer& renderer);

    void key_pressed(SDL_Scancode scancode);
    void key_released(SDL_Scancode scancode);
private:
    void initKeys();
    void initFont();

    // Instructions
    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();
    void OP_2NNN();
    void OP_6XNN();
    void OP_7XNN();
    void OP_ANNN();
    void OP_DXYN();

    // Skip conditionally
    void OP_3XNN();
    void OP_4XNN();
    void OP_5XY0();
    void OP_9XY0();

    // Logical and Arithmetic instructions
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY7();
    void OP_8XY6();
    void OP_8XYE();

    void OP_BNNN(); // Jump with offset
    void OP_CXNN(); // Random
    void OP_EX9E(); // Skip if key
    void OP_EXA1(); // Skip if key

    // Timers
    void OP_FX07(); // Read delay timer
    void OP_FX15(); // Set delay timer
    void OP_FX18(); // set sound timer

    void OP_FX1E(); // Add to index
    void OP_FX0A(); // Get key
    void OP_FX29(); // Font character
    void OP_FX33(); // Binary-coded decimal conversion
    void OP_FX55(); // Store memory
    void OP_FX65(); // Load memory

    uint8_t memory[4096] {};
    uint16_t program_counter { STARTING_POINT };
    uint16_t stack_pointer {};
    uint16_t index_register {};
    uint16_t stack[16] {};
    uint8_t delay_timer {};
    uint8_t sound_timer {};
    uint8_t registry[16] {};
    uint8_t screen[64 * 32] {};
    uint16_t opcode {};

    SDL_Surface* surface;
    SDL_Texture* texture;

    // Input
    std::unordered_map<SDL_Scancode, uint8_t> keymap {};
    std::bitset<16> keys;

};
