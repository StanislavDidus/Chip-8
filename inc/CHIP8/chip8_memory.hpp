#pragma once

#include <cstdint>

#include "memory.hpp"

class chip8_memory : public memory
{
public:
    chip8_memory() = default;
    ~chip8_memory() override = default;

    uint8_t* access_memory() override { return memory; }
private:
    uint8_t memory[4096] {};
};
