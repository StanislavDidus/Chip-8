#include "chip8.hpp"

#include <fstream>
#include <iostream>
#include <cstring>


void chip8::setup_chip8(std::unique_ptr<display> display, std::unique_ptr<quirks> quirks,
                        std::unique_ptr<instructions> instructions, std::unique_ptr<memory> memory)
{
    m_display = std::move(display);
    m_quirks = std::move(quirks);
    m_instructions = std::move(instructions);
    m_memory = std::move(memory);

    init_keys();
    init_font();
    init_render_texture();

    std::cout << "Initialized Chip8." << std::endl;
}

void chip8::load_rom(const std::filesystem::path& path_to_rom)
{
    // std::ios::ate makes the cursor move to the end of the file
    std::ifstream file{path_to_rom, std::ios::binary | std::ios::ate};
    if (!file)
        std::cerr << "Could not open file for reading." << std::endl;

    // Get actual file size
    std::streamsize size = file.tellg();

    // Move the pointer back to the beginning
    file.seekg(0, std::ios::beg);

    // Write into memory starting from address 200
    file.read(reinterpret_cast<char*>(m_memory->access_memory() + STARTING_POINT), size);

    if (file.bad())
        std::cerr << "Failed to read the file." << std::endl;

    // Init texture that we will draw to


    std::cout << "Loaded ROM." << std::endl;
}

void chip8::init_keys()
{
    keymap[SDL_SCANCODE_1] = 0x1;
    keymap[SDL_SCANCODE_2] = 0x2;
    keymap[SDL_SCANCODE_3] = 0x3;
    keymap[SDL_SCANCODE_4] = 0xC;
    keymap[SDL_SCANCODE_Q] = 0x4;
    keymap[SDL_SCANCODE_W] = 0x5;
    keymap[SDL_SCANCODE_E] = 0x6;
    keymap[SDL_SCANCODE_R] = 0xD;
    keymap[SDL_SCANCODE_A] = 0x7;
    keymap[SDL_SCANCODE_S] = 0x8;
    keymap[SDL_SCANCODE_D] = 0x9;
    keymap[SDL_SCANCODE_F] = 0xE;
    keymap[SDL_SCANCODE_Z] = 0xA;
    keymap[SDL_SCANCODE_X] = 0x0;
    keymap[SDL_SCANCODE_C] = 0xB;
    keymap[SDL_SCANCODE_V] = 0xF;

    std::cout << "Initialized key map." << std::endl;
}

void chip8::init_font()
{
    uint8_t characters[5 * 16] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(m_memory->access_memory() + FONT_MEMORY_LOCATION, characters, 5 * 16 * sizeof(uint8_t));

    std::cout << "Initialized font." << std::endl;
}

chip8::chip8(window_renderer& renderer)
    : renderer(renderer)
{

}

void chip8::init_render_texture()
{
    surface = SDL_CreateSurface(64,32, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) std::cerr << "Could not create surface: " << SDL_GetError() << std::endl;
    texture = SDL_CreateTextureFromSurface(renderer.get_renderer(), surface);
    if (!texture) std::cerr << "Could not create texture: " << SDL_GetError() << std::endl;
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void chip8::key_pressed(SDL_Scancode scancode)
{
    if (keymap.contains(scancode))
        keys.set(keymap[scancode], true);
}

void chip8::key_released(SDL_Scancode scancode)
{
    if (keymap.contains(scancode))
        keys.set(keymap[scancode], false);
}

void chip8::update()
{
    for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
    {
        m_instructions->execute_instruction();
    }

    // Update timers
    if (m_core.get_delay_timer_value() > 0)
        m_core.decrease_delay_timer();
    if (m_core.get_sound_timer_value() > 0)
    {
        audio_play.play_sound();
        m_core.decrease_sound_timer();
    }
}

void chip8::render(window_renderer& renderer)
{
    SDL_Renderer* sdl_renderer = renderer.get_renderer();

    uint8_t screen_width = m_display->get_screen_width();
    uint8_t screen_height = m_display->get_screen_height();
    std::vector<uint32_t> pixels(screen_width * screen_height);

    for (int y = 0; y < screen_height; ++y)
    {
        for (int x = 0; x < screen_width; ++x)
        {
            uint32_t color = (m_display->get_pixel(x, y) == 1) ? 0xFFFFFFFF : 0xFF000000;
            pixels[x + y * screen_width] = color;
        }
    }

    SDL_UpdateTexture(texture, nullptr, pixels.data(), 64 * sizeof(uint32_t));

    //SDL_SetRenderDrawColor(sdl_renderer, 0,0,0,255);
    SDL_RenderClear(sdl_renderer);

    //SDL_SetRenderDrawColor(sdl_renderer, 255,255,255,255);
    SDL_RenderTexture(sdl_renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(sdl_renderer);
}

