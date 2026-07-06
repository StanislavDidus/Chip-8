#pragma once

#include <cstdint>

#include "memory.hpp"

class chip8_memory : public memory
{
public:
    chip8_memory() = default;
    ~chip8_memory() override = default;

    uint8_t* access_memory() override { return memory; }
    uint16_t get_opcode(uint16_t pc) override { return (memory[pc] << 8) | memory[pc + 1]; }
    uint32_t get_size() override { return 4096; }
private:
    uint8_t memory[4096] {};
};
