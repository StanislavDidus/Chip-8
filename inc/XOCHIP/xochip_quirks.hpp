#pragma once

#include "quirks.hpp"

constexpr quirks xochip_quirks =
{
    .vf_reset = false,
    .memory = true,
    .display_wait = false,
    .clipping = false,
    .shifting = false,
    .jumping = false
};
