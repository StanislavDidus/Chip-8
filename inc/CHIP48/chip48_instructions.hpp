#pragma once
#include "instructions.hpp"

class chip48_instructions : public instructions
{
public:
    chip48_instructions(chip8& chip8);
    ~chip48_instructions() override = default;

private:
};