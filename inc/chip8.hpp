#pragma once
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <random>
#include <unordered_map>

#include "config.hpp"
#include "core.hpp"
#include "display.hpp"
#include "instructions.hpp"
#include "memory.hpp"

#include "Audio/audio.hpp"
#include "enums.hpp"
#include "SDL/window_renderer.hpp"

#include "imgui.h"
#include "imfilebrowser.h"
#include "logger.hpp"
#include "quirks.h"
#include "CHIP8/chip8_quirks.hpp"
// Variants
// CHIP8-Classic, CHIP8-48, SUPER-CHIP, XO-CHIP

class chip8
{
public:
    chip8(window_renderer& renderer);
    ~chip8();

    void update(float delta_time);
    void render(window_renderer& renderer);

    void key_pressed(SDL_Scancode scancode);
    void key_released(SDL_Scancode scancode);

    void pause_game();
    void resume_game();
    void stop_game();

    // Getters
    display& get_display() { return *m_display; }
    instructions& get_instructions() { return *m_instructions; }
    memory& get_memory() { return *m_memory; }
    core& get_core() { return m_core; }
    audio& get_audio() { return *m_audio; }
    quirks& get_quirks() { return m_quirks; }
    const std::string& get_rom_name() const { return rom_name; }

    [[nodiscard]] bool is_key_pressed(uint8_t key) { return keys[key]; }
private:
    // Init
    void init_keys();
    void init_font();
    void init_render_texture();

    void setup_from_config();
    void setup_chip8(uint8_t version);
    void load_rom(const std::filesystem::path& path_to_rom);

    // ROM
    std::string rom_name {};
    int32_t instructions_per_second = 0;
    int32_t instructions_per_frame = 0;
    uint16_t fetch() const;

    // Components
    std::unique_ptr<display> m_display;
    std::unique_ptr<instructions> m_instructions;
    std::unique_ptr<memory> m_memory;
    std::unique_ptr<audio> m_audio;
    core m_core {};
    quirks m_quirks = chip8_quirks;

    // Image
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

    // Input
    std::unordered_map<SDL_Scancode, uint8_t> keymap {};
    std::bitset<16> keys;

    // Renderer
    window_renderer& renderer;

    // Logger
    logger m_logger {};

    // ImGui window
    void render_launch_window();
    void render_log_window();
    void render_viewport_window();
    void render_debug_windows();
    void render_additional_windows();

    ImGui::FileBrowser file_dialog;

    chip8_status status = chip8_status::MENU;
    float game_timer = 0.0f;

    chip8_config config {};
};
