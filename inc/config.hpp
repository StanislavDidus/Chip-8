#pragma once

constexpr int STARTING_POINT = 0x200;
constexpr int FONT_MEMORY_LOCATION = 0x50;
constexpr int INSTRUCTIONS_PER_SECOND = 700;
constexpr int INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(INSTRUCTIONS_PER_SECOND) / 60.0f);

#ifdef ALTERNATIVE_KEY_MAPPING
constexpr bool ALTERNATIVE_KEY_MAPPING = false; // 0123 4567 89AB CDEF
#endif

constexpr bool CHIP48_IMPLEMENTATION = true;
constexpr bool AMIGA_INDEX_IMPLEMENTATION = true;
