#pragma once

#include <cstdint>

#include "memory.hpp"

class xochip_memory : public memory
{
public:
    xochip_memory() = default;
    ~xochip_memory() override = default;

    uint8_t* access_memory() override { return memory; }
    uint16_t get_opcode(uint16_t pc) override { return (memory[pc] << 8) | memory[pc + 1]; }
    uint32_t get_size() override { return 65'536; }
private:
    uint8_t memory[65'536] {};
};
