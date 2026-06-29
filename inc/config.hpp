#pragma once

constexpr int STARTING_POINT = 0x200;
constexpr int LOW_RES_FONT_MEMORY_LOCATION = 0x50;
constexpr int HIGH_RES_FONT_MEMORY_LOCATION = 0xA0;

constexpr int CHIP8_INSTRUCTIONS_PER_SECOND = 700;
constexpr int CHIP48_INSTRUCTIONS_PER_SECOND = 700;
constexpr int SCHIP_INSTRUCTIONS_PER_SECOND = 1800;
constexpr int CHIP8_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(CHIP8_INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr int CHIP48_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(CHIP48_INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr int SCHIP_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(SCHIP_INSTRUCTIONS_PER_SECOND) / 60.0f);

constexpr const char* GAMESAVES_LOCATION = "gamesaves/";