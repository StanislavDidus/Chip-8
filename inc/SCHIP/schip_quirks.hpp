#pragma once

#include "quirks.h"

constexpr quirks schip_quirks =
{
    .vf_reset = false,
    .memory = false,
    .display_wait = false,
    .clipping = false,
    .shifting = true,
    .jumping = true
};
