#pragma once

#include <cstdint>

#include "memory.hpp"

class chip48_memory : public memory
{
public:
    chip48_memory() = default;
    ~chip48_memory() override = default;

    uint8_t* access_memory() override { return memory; }
private:
    uint8_t memory[4096] {};
};