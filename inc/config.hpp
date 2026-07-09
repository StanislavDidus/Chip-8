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

struct chip8_config
{
    uint8_t chip8_version = 0;
    uint8_t chip8_quirks = 0;
    std::string rom_path {};

    bool show_color_settings = false;
    bool show_quirks_settings = false;
    bool show_audio_settings = false;

    bool is_debug_mode = false;
    bool follow_pc = false;

    float volume = 0.5f;

    float color_0[3] = {0.0f, 0.0f, 0.0f};
    float color_1[3] = {1.0f, 1.0f, 1.0f};
    float color_2[3] = {0.33f, 0.33f, 0.33f};
    float color_3[3] = {0.66f, 0.66f, 0.66f};
};