#include "chip8.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "imgui_internal.h"

#include "CHIP8/chip8_audio.hpp"
#include "CHIP8/chip8_display.hpp"
#include "CHIP8/chip8_memory.hpp"

#include "CHIP48/chip48_instructions.hpp"
#include "CHIP48/chip48_quirks.hpp"
#include "CHIP8/chip8_quirks.hpp"

#include "SCHIP/schip_display.hpp"
#include "SCHIP/schip_instructions.hpp"
#include "SCHIP/schip_quirks.hpp"

#include "XOCHIP/xochip_audio.hpp"
#include "XOCHIP/xochip_display.hpp"
#include "XOCHIP/xochip_instructions.hpp"
#include "XOCHIP/xochip_memory.hpp"
#include "XOCHIP/xochip_quirks.hpp"

chip8::chip8(window_renderer& renderer, const application_style& style, const chip8_config& config)
    : renderer(renderer)
    , style(style)
    , config(config)
{
    // Init ImGui file dialog
    file_dialog.SetTitle("File Browser");
    file_dialog.SetTypeFilters({".ch8"});
}

chip8::~chip8()
{
    if (texture) SDL_DestroyTexture(texture);
    if (surface) SDL_DestroySurface(surface);
}

void chip8::setup_chip8(uint8_t version)
{
    if (version == 0)
    {
        m_display = std::make_unique<chip8_display>();
        m_instructions = std::make_unique<chip48_instructions>(*this);
        m_memory = std::make_unique<chip8_memory>();
        m_audio = std::make_unique<chip8_audio>();
        m_core = core{};
    }
    else if (version == 1)
    {
        m_display = std::make_unique<chip8_display>();
        m_instructions = std::make_unique<chip48_instructions>(*this);
        m_memory = std::make_unique<chip8_memory>();
        m_audio = std::make_unique<chip8_audio>();
        m_core = core{};
    }
    else if (version == 2)
    {
        m_display = std::make_unique<schip_display>();
        m_instructions = std::make_unique<schip_instructions>(*this);
        m_memory = std::make_unique<chip8_memory>();
        m_audio = std::make_unique<chip8_audio>();
        m_core = core{};
    }
    else if (version == 3)
    {
        m_display = std::make_unique<xochip_display>();
        m_instructions = std::make_unique<xochip_instructions>(*this);
        m_memory = std::make_unique<xochip_memory>();
        m_audio = std::make_unique<xochip_audio>();
        m_core = core{};
    }
    else
    {
        throw std::runtime_error{"Wrong Chip8 version."};
    }

    config.instructions_per_frame = static_cast<int32_t>(static_cast<float>(config.instructions_per_second) / 60.0f);

    m_audio->set_volume(config.volume);

    init_keys();
    init_font();
    init_render_texture();
}

void chip8::load_rom(const std::filesystem::path& path_to_rom)
{
    // std::ios::ate makes the cursor move to the end of the file
    std::ifstream file{path_to_rom, std::ios::binary | std::ios::ate};
    if (!file)
       throw std::runtime_error{"Could not open file for reading."};

    // Get actual file size
    std::streamsize size = file.tellg();
    std::streamsize max_size = m_memory->get_size() - STARTING_POINT;
    if (size > max_size)
        throw std::runtime_error{"Rom size exceeds the size of the available memory."};

    // Move the pointer back to the beginning
    file.seekg(0, std::ios::beg);

    // Write into memory starting from address 200
    file.read(reinterpret_cast<char*>(m_memory->access_memory() + STARTING_POINT), size);

    if (file.bad())
        throw std::runtime_error{"Failed to read the file."};

    // Save the ROM's name without the extension
    rom_name = path_to_rom.stem().string();

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

    uint8_t high_res_characters[10 * 16] = {
        0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00, // 0
        0x0C, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00, // 1
        0x3E, 0x63, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x61, 0x7F, 0x00, // 2
        0x3E, 0x63, 0x03, 0x03, 0x3E, 0x03, 0x03, 0x63, 0x3E, 0x00, // 3
        0x06, 0x0E, 0x1E, 0x36, 0x66, 0x7F, 0x06, 0x06, 0x0F, 0x00, // 4
        0x7F, 0x60, 0x60, 0x60, 0x7C, 0x03, 0x03, 0x63, 0x3E, 0x00, // 5
        0x1C, 0x30, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x66, 0x3C, 0x00, // 6
        0x7F, 0x63, 0x03, 0x06, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00, // 7
        0x3C, 0x66, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x3C, 0x00, // 8
        0x3C, 0x66, 0x66, 0x66, 0x3E, 0x03, 0x03, 0x06, 0x3C, 0x00, // 9
        0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x66, 0x00, // A
        0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00, // B
        0x3C, 0x66, 0x60, 0x60, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00, // C
        0x78, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00, // D
        0x7F, 0x62, 0x64, 0x68, 0x78, 0x68, 0x64, 0x62, 0x7F, 0x00, // E
        0x7F, 0x62, 0x64, 0x68, 0x78, 0x68, 0x60, 0x60, 0xF0, 0x00  // F
    };

    memcpy(m_memory->access_memory() + LOW_RES_FONT_MEMORY_LOCATION, characters, sizeof(characters));
    memcpy(m_memory->access_memory() + HIGH_RES_FONT_MEMORY_LOCATION, high_res_characters, sizeof(high_res_characters));

    std::cout << "Initialized font." << std::endl;
}


void chip8::init_render_texture()
{
    if (texture) SDL_DestroyTexture(texture);
    if (surface) SDL_DestroySurface(surface);

    surface = SDL_CreateSurface(128,64, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) std::cerr << "Could not create surface: " << SDL_GetError() << std::endl;
    texture = SDL_CreateTextureFromSurface(renderer.get_renderer(), surface);
    if (!texture) std::cerr << "Could not create texture: " << SDL_GetError() << std::endl;
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    std::cout << "Initialized texture target." << std::endl;
}

void chip8::setup_from_config()
{
    try
    {
        setup_chip8(config.chip8_version);
        load_rom(config.rom_path);

        status = chip8_status::PLAYING;
    }
    catch (std::exception& e)
    {
        m_logger.log(std::format("Error occurred during Chip8 initialization: {}", e.what()));
    }
}

void chip8::execute_n_instructions(int number)
{
    for (int i = 0; i < number; ++i)
    {
        // Fetch
        uint16_t opcode = fetch();

        // Increase PC
        m_core.skip_next();

        // Decode
        instruction inst = m_instructions->decode(opcode);

        // Execute
        //std::cout << "Executing instruction: " << std::hex << opcode << " at: " << std::hex << m_core.get_pc() << std::endl;
        inst();

        // Stop update look after 00DFD instruction
        if (status == chip8_status::MENU)
            break;
    }

    frame_counter += static_cast<float>(number) / static_cast<float>(config.instructions_per_frame);
    while (frame_counter >= 1.0f)
    {
        update_timers();
        frame_counter -= 1.0f;
    }

}

void chip8::update_timers()
{
    if (m_core.get_delay_timer_value() > 0)
        m_core.decrease_delay_timer();
    if (m_core.get_sound_timer_value() > 0)
    {
        m_audio->play_sound();
        m_core.decrease_sound_timer();
    }
}

uint16_t chip8::fetch() const
{
    uint16_t pc = m_core.get_pc();
    uint8_t first_byte = m_memory->access_memory()[pc];
    uint8_t second_byte = m_memory->access_memory()[pc + 1];
    return first_byte << 8 | second_byte;
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

void chip8::pause_game()
{
    status = chip8_status::PAUSED;
}

void chip8::resume_game()
{
    status = chip8_status::PLAYING;
}

void chip8::stop_game()
{
    game_timer = 0.0f;
    status = chip8_status::MENU;
}

void chip8::update(float delta_time)
{
    try
    {
        if (status == chip8_status::PLAYING)
        {
            game_timer += delta_time;

            execute_n_instructions(config.instructions_per_frame);
        }
    }
    catch (std::exception& e)
    {
        m_logger.log(std::format("Error encountered when running: {}", e.what()));
        m_logger.log(std::format("Stopping the game", e.what()));
        stop_game();
    }
}

uint32_t get_color_value(float color[3])
{
    uint32_t a = 0xFF;
    uint32_t r = static_cast<uint32_t>(color[0] * 255.0f);
    uint32_t g = static_cast<uint32_t>(color[1] * 255.0f);
    uint32_t b = static_cast<uint32_t>(color[2] * 255.0f);
    return a << 24 | r << 16 | g << 8 | b;
}

void chip8::render(window_renderer& renderer)
{
    SDL_Renderer* sdl_renderer = renderer.get_renderer();

    if (m_display && texture)
    {
        // Create a texture from a screen buffer
        uint8_t screen_width = m_display->get_screen_width();
        uint8_t screen_height = m_display->get_screen_height();
        std::vector<uint32_t> pixels(screen_width * screen_height);

        for (int y = 0; y < screen_height; ++y)
        {
            for (int x = 0; x < screen_width; ++x)
            {
                uint8_t pixel_value = m_display->get_pixel_value(x, y);
                uint32_t color = 0;

                if (pixel_value == 0) color = get_color_value(config.color_0);
                if (pixel_value == 1) color = get_color_value(config.color_1);
                if (pixel_value == 2) color = get_color_value(config.color_2);
                if (pixel_value == 3) color = get_color_value(config.color_3);

                pixels[x + y * screen_width] = color;
            }
        }

        SDL_FRect src {0.0f, 0.0f, static_cast<float>(m_display->get_screen_width()), static_cast<float>(m_display->get_screen_height())};
        SDL_Rect src_i {0, 0, static_cast<int>(m_display->get_screen_width()), static_cast<int>(m_display->get_screen_height())};

        SDL_UpdateTexture(texture, &src_i, pixels.data(), screen_width * sizeof(uint32_t));
    }

    // Start new ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Draw ImGui stuff here
    ImGui::DockSpaceOverViewport();
    //ImGui::ShowDemoWindow();
    render_launch_window();
    render_log_window();
    render_viewport_window();
    render_debug_windows();
    render_additional_windows();

    // Render ImGui
    ImGui::Render();

    SDL_RenderClear(sdl_renderer);

    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer.get_renderer());

    SDL_RenderPresent(sdl_renderer);

    // Multi viewports
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

