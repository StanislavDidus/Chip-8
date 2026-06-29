#pragma once

#include <cstdint>

#include "memory.hpp"

class schip_memory : public memory
{
public:
    schip_memory() = default;
    ~schip_memory() override = default;

    uint8_t* access_memory() override { return memory; }
private:
    uint8_t memory[4096] {};
};
