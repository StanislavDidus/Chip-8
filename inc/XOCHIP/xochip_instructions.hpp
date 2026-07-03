#pragma once

#include <cstdint>

#include "SCHIP/schip_instructions_i.hpp"

class xochip_instructions : public schip_instructions_i
{
public:
    xochip_instructions(chip8& chip8) : schip_instructions_i(chip8) { init_table(); }
    ~xochip_instructions() override = default;

private:
    void init_table();

    instruction Table_5();
    instruction Table_0();

    instruction_table table_5;

    // New instructions
    void OP_5XY2(); // save registers to memory i
    void OP_5XY3(); // load to registers from memory i
    void OP_F000(); // load i with a 16-bit address
    void OP_FN01(); // Select drawing planes
    void OP_F002(); // Store audio pattern buffer that is located at i
    void OP_FX3A(); // set audio pattern playback
    void OP_00DN(); // scroll-up
    void OP_00CN(); // scroll-down
    void OP_00FB(); // Scroll right
    void OP_00FC(); // Scroll left

    // Overridden instructions
    void OP_DXYN();
    void OP_DXY0();

    void OP_BNNN(); // Jump
    void OP_8XY6(); // Shift
    void OP_8XYE();
    void OP_FX55();
    void OP_FX65(); // Load and Store
    void OP_FX1E(); // Add to I

    // Skip conditionally overridden to skip 4 byte instruction
    void OP_3XNN(); // Skip if equal
    void OP_4XNN(); // Skip if not equal
    void OP_5XY0(); // Skip if equal
    void OP_9XY0(); // Skip if not equal
    void OP_EX9E(); // Skip if key is pressed
    void OP_EXA1(); // SKip if key is not pressed

    void draw_sprite(uint8_t bytes_per_row, uint8_t width, uint8_t height);
};