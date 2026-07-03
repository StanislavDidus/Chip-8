#pragma once
#include "schip_instructions_i.hpp"

class schip_instructions : public schip_instructions_i
{
public:
    schip_instructions(chip8& chip8) : schip_instructions_i(chip8) {}
    ~schip_instructions() override = default;

};
