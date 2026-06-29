#pragma once
#include "instructions.hpp"

class schip_instructions : public instructions
{
public:
    schip_instructions(chip8& chip8);
    ~schip_instructions() override = default;

private:
    void init_table();

    instruction Table_0();

    void OP_00E0(); // Clear screen
    void OP_00EE(); // Call
    void OP_00FF(); // Set high resolution screen
    void OP_00FE(); // Set low resolution screen
    void OP_00CN(); // Scroll down
    void OP_00FB(); // Scroll right
    void OP_00FC(); // Scroll left

    void OP_DXY0(); // Draw 16x16
    void OP_DXYN(); // Draw 8xN

    void OP_FX30(); // Get font character
    void OP_FX75(); // load to disk
    void OP_FX85(); // Save to disk

    void OP_00FD(); // Exit
};
