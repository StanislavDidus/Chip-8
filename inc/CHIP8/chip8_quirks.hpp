#pragma once

#include "quirks.hpp"

constexpr quirks chip8_quirks =
{
    .vf_reset = true,
    .memory = true,
    .display_wait = true,
    .clipping = false,
    .shifting = false,
    .jumping = false
};