#pragma once

constexpr int STARTING_POINT = 0x200;
constexpr int LOW_RES_FONT_MEMORY_LOCATION = 0x50;
constexpr int HIGH_RES_FONT_MEMORY_LOCATION = 0xA0;

constexpr int CHIP8_INSTRUCTIONS_PER_SECOND = 700;
constexpr int CHIP48_INSTRUCTIONS_PER_SECOND = 700;
constexpr int SCHIP_INSTRUCTIONS_PER_SECOND = 1800;
constexpr int XOCHIP_INSTRUCTIONS_PER_SECOND = 10000;
constexpr int CHIP8_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(CHIP8_INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr int CHIP48_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(CHIP48_INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr int SCHIP_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(SCHIP_INSTRUCTIONS_PER_SECOND) / 60.0f);
constexpr int XOCHIP_INSTRUCTION_PER_FRAME = static_cast<int>(static_cast<float>(XOCHIP_INSTRUCTIONS_PER_SECOND) / 60.0f);

constexpr const char* GAMESAVES_LOCATION = "gamesaves/";

constexpr uint32_t color_0 = 0xFF000000;
constexpr uint32_t color_1 = 0xFFFF00FF;
constexpr uint32_t color_2 = 0xFF00FFFF;
constexpr uint32_t color_3 = 0xFFFFFFFF;

struct chip8_config
{
    uint8_t chip8_version = 0;
    std::string rom_path {};
    bool show_color_settings = false;

    float color_0[4];
    float color_1[4];
    float color_2[4];
    float color_3[4];
};