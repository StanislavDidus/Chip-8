#pragma once

#include "instructions.hpp"

class chip8_instructions : public instructions
{
public:
    chip8_instructions(chip8& chip8);
    ~chip8_instructions() override = default;

private:
    void init_table();

    void OP_8XY6(); // Shift right
    void OP_8XYE(); // Shift left

    void OP_BNNN(); // Jump with offset

    void OP_FX55(); // Store memory
    void OP_FX65(); // Load memory
};
