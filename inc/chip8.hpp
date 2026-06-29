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
#include "quirks.hpp"

#include "Audio/audio.hpp"
#include "SDL/window_renderer.hpp"

// Variants
// CHIP8-Classic, CHIP8-48, SUPER-CHIP, XO-CHIP

class chip8
{
public:
    chip8(window_renderer& renderer, uint32_t instructions_per_frame);
    ~chip8() = default;

    void setup_chip8(
        std::unique_ptr<display> display,
        std::unique_ptr<quirks> quirks,
        std::unique_ptr<instructions> instructions,
        std::unique_ptr<memory> memory
        );

    void load_rom(const std::filesystem::path& path_to_rom);

    void update();
    void render(window_renderer& renderer);

    void key_pressed(SDL_Scancode scancode);
    void key_released(SDL_Scancode scancode);

    // Getters
    display& get_display() { return *m_display; }
    quirks& get_quirks() { return *m_quirks; }
    instructions& get_instructions() { return *m_instructions; }
    memory& get_memory() { return *m_memory; }
    core& get_core() { return m_core; }
    const std::string& get_rom_name() const { return rom_name; }

    [[nodiscard]] bool is_key_pressed(uint8_t key) { return keys[key]; }
private:
    // Init
    void init_keys();
    void init_font();
    void init_render_texture();

    // ROM
    std::string rom_name {};
    uint32_t instructions_per_frame = 0;
    uint16_t fetch() const;

    // Components
    std::unique_ptr<display> m_display;
    std::unique_ptr<quirks> m_quirks;
    std::unique_ptr<instructions> m_instructions;
    std::unique_ptr<memory> m_memory;
    core m_core {};

    // Image
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

    // Input
    std::unordered_map<SDL_Scancode, uint8_t> keymap {};
    std::bitset<16> keys;

    // Audio
    audio audio_play {};

    // Renderer
    window_renderer& renderer;
};
