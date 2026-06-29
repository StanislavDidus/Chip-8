#pragma once

constexpr int STARTING_POINT = 0x200;
constexpr int LOW_RES_FONT_MEMORY_LOCATION = 0x50;
constexpr int HIGH_RES_FONT_MEMORY_LOCATION = 0xA0;
constexpr int INSTRUCTIONS_PER_SECOND = 700;
constexpr int INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr const char* GAMESAVES_LOCATION = "gamesaves/";