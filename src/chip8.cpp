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

chip8::chip8(window_renderer& renderer)
    : renderer(renderer)
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

    /*if (config.chip8_quirks == 0)
        m_quirks = chip8_quirks;
    else if (config.chip8_quirks == 1)
        m_quirks = chip48_quirks;
    else if (config.chip8_quirks == 2)
        m_quirks = schip_quirks;
    else if (config.chip8_quirks == 3)
        m_quirks = xochip_quirks;
    else
        throw std::runtime_error{"Wrong Chip8 quirk."};*/

    instructions_per_frame = static_cast<int32_t>(static_cast<float>(instructions_per_second) / 60.0f);

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
        m_logger.log(std::format("Error occured during Chip8 initialization: {}", e.what()));
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

            for (int i = 0; i < instructions_per_frame; ++i)
            {
                // Fetch
                uint16_t opcode = fetch();

                // Increase PC
                m_core.skip_next();

                // Decode
                instruction inst = m_instructions->decode(opcode);

                // Execute
                std::cout << "Executing instruction: " << std::hex << opcode << " at: " << std::hex << m_core.get_pc() << std::endl;
                inst();

                // Stop update look after 00DFD instruction
                if (status == chip8_status::MENU)
                    break;
            }

            // Update timers
            if (m_core.get_delay_timer_value() > 0)
                m_core.decrease_delay_timer();
            if (m_core.get_sound_timer_value() > 0)
            {
                m_audio->play_sound();
                m_core.decrease_sound_timer();
            }
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

    if (status == chip8_status::PLAYING)
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
    ImGui::ShowDemoWindow();
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

void chip8::render_launch_window()
{
    ImGui::Begin("Launch");

    if (ImGui::BeginMenu("Settings"))
    {
        ImGui::MenuItem("Color settings", nullptr, &config.show_color_settings);
        ImGui::MenuItem("Audio settings", nullptr, &config.show_audio_settings);

        ImGui::EndMenu();
    }

    static char path_buffer[128] {};
    ImVec2 avail = ImGui::GetContentRegionAvail();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail.x * 0.5f - ImGui::CalcTextSize("Welcome to my Chip8 emulator!").x * 0.5f);
    ImGui::Text("Welcome to my Chip8 emulator!");

    ImGui::TextWrapped("- This is a revolutionary emulator made for the purpose of playing and creating retro games.\n"
                "- This emulator can run any Chip8 game as well as games made for other versions of Chip8 (Chip48, Super-Chip, XO-Chip).\n"
                "- Below you need to specify the path to the ROM you want to run and which version of Chip8 to use. "
                "For more settings go to the \"settings\" tab.\n"
                "If you never wish to see this text again uncheck the checkbox in the \"settings\" .\n");

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Path to ROM");

    ImGui::InputText("##inputtext", path_buffer, sizeof(path_buffer));

    ImGui::SameLine();

    if (ImGui::Button("Open File Browser"))
    {
        file_dialog.Open();
    }

    ImGui::Spacing();

    ImGui::Text("Chip8 version");

    // Choose Chip8 version
    static const char* versions[] {"Chip8", "Chip48", "Super-Chip", "XO-Chip"};
    static int selected_version = 0;

    static const char* quirks[] {"Chip8", "Chip48", "Super-Chip", "XO-Chip"};
    static int selected_quirks = 0;

    if (ImGui::Combo("##checkbox", &selected_version, versions, IM_ARRAYSIZE(versions)))
    {
        selected_quirks = selected_version;

        if (selected_quirks == 0)
            m_quirks = chip8_quirks;
        if (selected_quirks == 1)
            m_quirks = chip48_quirks;
        if (selected_quirks == 2)
            m_quirks = schip_quirks;
        if (selected_quirks == 3)
            m_quirks = xochip_quirks;
    }

    ImGui::Spacing();

    ImGui::Text("Instruction execution speed");

    // Choose Chip8 instruction executing speed
    if (ImGui::InputInt("##inputint", &instructions_per_second))
    {
        instructions_per_second = std::clamp(instructions_per_second, 0, std::numeric_limits<int32_t>::max());
    }
    if (ImGui::Button("Chip8")) {instructions_per_second = CHIP8_INSTRUCTIONS_PER_SECOND; instructions_per_frame = CHIP8_INSTRUCTION_PER_FRAME; }
    ImGui::SameLine();
    if (ImGui::Button("Super-Chip")) {instructions_per_second = SCHIP_INSTRUCTIONS_PER_SECOND; instructions_per_frame = SCHIP_INSTRUCTION_PER_FRAME; }
    ImGui::SameLine();
    if (ImGui::Button("XO-Chip")) {instructions_per_second = XOCHIP_INSTRUCTIONS_PER_SECOND; instructions_per_frame = XOCHIP_INSTRUCTION_PER_FRAME; }

    ImGui::Spacing();

    // Quirks
    ImGui::Text("Quirks");

    if (ImGui::Combo("##checkbox_quirks", &selected_quirks, quirks, IM_ARRAYSIZE(quirks)))
    {
        if (selected_quirks == 0)
            m_quirks = chip8_quirks;
        if (selected_quirks == 1)
            m_quirks = chip48_quirks;
        if (selected_quirks == 2)
            m_quirks = schip_quirks;
        if (selected_quirks == 3)
            m_quirks = xochip_quirks;
    }

    ImGui::SameLine();

    if (ImGui::Button("Configure"))
    {
        config.show_quirks_settings = true;
    }

    ImGui::Spacing();

    ImGui::Text("Launch options");

    // Start game
    if (ImGui::Button("Start"))
    {
        config.chip8_version = selected_version;
        config.chip8_quirks = selected_quirks;
        config.rom_path = path_buffer;

        setup_from_config();
    }

    ImGui::SameLine();

    // Debug mode Checkbox
    if (ImGui::Checkbox("Debugger", &config.is_debug_mode))
    {

    }

    if (status == chip8_status::PLAYING || status == chip8_status::PAUSED)
    {
       ImGui::Spacing();

        ImGui::Text("Playtime %.1f", game_timer);

        if (status == chip8_status::PLAYING)
        {
            if (ImGui::Button("Pause"))
            {
                pause_game();
            }
        }
        else if (status == chip8_status::PAUSED)
        {
            if (ImGui::Button("Resume"))
            {
                resume_game();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Stop"))
        {
            stop_game();
        }
    }

    ImGui::Spacing();

    ImGui::End();

    file_dialog.Display();

    if (file_dialog.HasSelected())
    {
        //buffer = file_dialog.GetSelected();
        //strncpy_s(const_cast<char*>(buffer.string().c_str()), sizeof(buffer) - 1, path_buffer, 0);
        //std::cout << "File selected: " << buffer.c_str() << std::endl;
        std::string selected_path = std::filesystem::relative(file_dialog.GetSelected()).string();
        //strncpy_s(path_buffer, sizeof(path_buffer), selected_path.c_str(), MAX);
        std::copy_n(selected_path.c_str(), std::min(strlen(selected_path.c_str()), sizeof(path_buffer) - 1), path_buffer);
        file_dialog.ClearSelected();
    }
}

void chip8::render_log_window()
{

    ImGui::Begin("Log");

    ImGui::Text("Log information will be displayed here");
    if (ImGui::Button("Clear"))
    {
        m_logger.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Copy"))
    {
        m_logger.copy();
    }
    ImGui::Spacing();

    ImGui::BeginChild("Child", ImGui::GetContentRegionAvail());

    const auto& logs = m_logger.get_logs();
    for (const auto& log : logs)
    {
        ImGui::TextWrapped(log.c_str());
    }

    ImGui::EndChild();

    ImGui::End();
}

void chip8::render_viewport_window()
{
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (texture && (status == chip8_status::PLAYING || status == chip8_status::PAUSED))
    {
        ImVec2 avail_size = ImGui::GetContentRegionAvail();

        float scale_x = avail_size.x / texture->w;
        float scale_y = avail_size.y / texture->h;

        float final_scale = scale_x < scale_y ? scale_x : scale_y;

        ImVec2 viewport_size;
        viewport_size.x = texture->w * final_scale;
        viewport_size.y = texture->h * final_scale;

        float start_x = avail_size.x * 0.5f - viewport_size.x * 0.5f;
        float start_y = avail_size.y * 0.5f - viewport_size.y * 0.5f;

        if (start_x < 0.0f)
            start_x = 0.0f;
        if (start_y < 0.0f)
            start_y = 0.0f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + start_x);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + start_y);

        ImVec2 uv  = ImVec2{1.0f, 1.0f};

        if (!m_display->is_high_res())
        {
            uv.x = 0.5f;
            uv.y = 0.5f;
        }

        ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(texture)), viewport_size, ImVec2{0.0f, 0.0f}, uv);
    }

    ImGui::End();
}


void chip8::render_additional_windows()
{
    if (config.show_color_settings)
    {
        if (ImGui::Begin("Color settings", &config.show_color_settings))
        {
            ImGui::TextWrapped("This is a color settings menu");
            ImGui::TextWrapped("Here you can change the color palette used for rendering.");
            ImGui::Separator();

            ImGui::ColorEdit3("(0) Background color", config.color_0);
            ImGui::ColorEdit3("(1) Fill color", config.color_1);
            ImGui::ColorEdit3("(2) Second Bitplane color", config.color_2);
            ImGui::ColorEdit3("(3) Mixed Bitplane color", config.color_3);

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Colors 2 and 3 are only used when playing XO-Chip games on X0-Chip emulator.");
        }
        ImGui::End();
    }

    if (config.show_quirks_settings)
    {
        ImGui::Begin("Quirks settings", &config.show_quirks_settings);

        ImGui::TextWrapped("This is a quirks settings menu.");
        ImGui::TextWrapped("Here you can turn on/off some quirks.");
        ImGui::TextWrapped("Quirks are automatically set to match the Chip8 version you chose but afterwards you can adjust them yourself.");
        ImGui::Separator();

        if (ImGui::BeginTable("split", 2))
        {
            ImGui::TableNextColumn();
            ImGui::Checkbox("VF Reset", &m_quirks.vf_reset);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Memory", &m_quirks.memory);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Display wait", &m_quirks.display_wait);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Clipping", &m_quirks.clipping);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Shifting", &m_quirks.shifting);

            ImGui::TableNextColumn();
            ImGui::Checkbox("Jumping", &m_quirks.jumping);

            ImGui::EndTable();
        }
        ImGui::SameLine();
        ImGui::End();
    }

    if (config.show_audio_settings)
    {
        if (ImGui::Begin("Audio settings", &config.show_audio_settings))
        {
            ImGui::TextWrapped("This is an audio settings menu.");
            ImGui::TextWrapped("You an adjust the volume and other settings here.");
            ImGui::Separator();

            if (ImGui::SliderFloat("Volume", &config.volume, 0.0f, 1.0f, "%.2f"))
            {
                if (m_audio) m_audio->set_volume(config.volume);
            }
        }
        ImGui::End();
    }
}

void chip8::render_debug_windows()
{
    if (config.is_debug_mode)
    {
        ImGui::Begin("Memory Layout");

        ImVec2 avail = ImGui::GetContentRegionAvail();

        ImVec2 slot_size {ImGui::CalcTextSize("00 ")};

        int columns = static_cast<int>(avail.x / slot_size.x);

        if (m_memory && columns > 0)
        {
            if (ImGui::BeginChild("Child1", avail))
            {
                if (ImGui::BeginTable("split", columns))
                {
                    int rows = static_cast<int>(m_memory->get_size() / columns);

                    ImGuiListClipper clipper;
                    clipper.Begin(rows);

                    while (clipper.Step())
                    {
                        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
                        {
                            ImGui::TableNextRow();

                            for (int col = 0; col < columns; ++col)
                            {
                                ImGui::TableNextColumn();

                                int byte_index = row * columns + col;
                                uint8_t value = m_memory->access_memory()[byte_index];

                                ImVec4 color{1.0f, 1.0f, 1.0f, 1.0f};

                                if (byte_index < STARTING_POINT)
                                    color = {1.0f, 0.0f, 0.0f, 1.0};
                                if (byte_index >= LOW_RES_FONT_MEMORY_LOCATION && byte_index < HIGH_RES_FONT_MEMORY_LOCATION + 160)
                                    color = {0.0f, 0.0f, 1.0f, 1.0f};

                                if (m_core.get_pc() == byte_index || m_core.get_pc() - 1 == byte_index)
                                {
                                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 0xFF00FF00);
                                    color = {0.0f, 0.0f, 0.0f, 1.0f};
                                }
                                ImGui::TextColored(color, "%02X", value);
                            }
                        }
                    }
                    clipper.End();
                }
                ImGui::EndTable();
            }
            ImGui::EndChild();
        }

        ImGui::End();
    }

    if (config.is_debug_mode)
    {
        if (ImGui::Begin("Register & Stack"))
        {
            ImVec2 avail = ImGui::GetContentRegionAvail();

            ImGui::SetCursorPosX(avail.x * 0.25f - ImGui::CalcTextSize("Register").x * 0.5f);

            ImGui::Text("Register");
            ImGui::SameLine();
            ImGui::SetCursorPosX(avail.x * 0.75f - ImGui::CalcTextSize("Stack").x * 0.5f);
            ImGui::Text("Stack");

            if (ImGui::BeginTable("Table_register", 3, ImGuiTableFlags_Borders, ImVec2{avail.x * 0.5f, avail.y}))
            {
                /*int total_rows = 6;

                for (int r = 0; r < total_rows; ++r)
                {
                    ImGui::TableNextRow();
                }*/

                for (int i = 0; i < 16; ++i)
                {
                    uint8_t value = m_core.get_registry_value(i);

                    ImGui::TableNextColumn();
                    ImGui::Text("V%X: %02X", i, value);
                }

            }
            ImGui::EndTable();

            ImGui::SameLine();

            //ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail.x * 0.5f - ImGui::CalcTextSize("Stack").x * 0.5f);
            if (ImGui::BeginTable("Table_stack", 3, ImGuiTableFlags_Borders, ImVec2{avail.x * 0.5f, avail.y}))
            {
                for (int i = 0; i < 16; ++i)
                {
                    uint8_t value = m_core.get_stack_value(i);

                    ImGui::TableNextColumn();
                    ImGui::Text("S%i : %02X", i, value);
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }

    if (config.is_debug_mode)
    {
        if (ImGui::Begin("Core Info"))
        {
            ImGui::Text("Core");
            ImGui::Text("PC: %02X", m_core.get_pc());
            ImGui::Text("I: %02X", m_core.get_index_register());
            ImGui::Text("SP: %02X", m_core.get_stack_pointer());

            ImGui::Spacing();

            ImGui::Text("Timers");
            ImGui::Text("Delay timer: %02X", m_core.get_delay_timer_value());
            ImGui::Text("Sound timer: %02X", m_core.get_sound_timer_value());

            ImGui::Spacing();

            ImGui::Text("Advanced Audio");
            ImGui::TextColored(ImVec4{0.5f, 0.5f, 0.5f, 1.0f}, "Only supported in XO-Chip.");

            auto* advanced_audio = dynamic_cast<xochip_audio*>(m_audio.get());
            if (advanced_audio != nullptr)
            {
                ImGui::Text("Sound pattern buffer: ");

                auto* sound_pattern_buffer = &advanced_audio->get_audio_pattern_buffer();
                if (ImGui::BeginTable("Table_sound_pattern_buffer", 8))
                {
                    for (int i = 0; i < 16; ++i)
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text("%02X", sound_pattern_buffer[i]);
                    }
                }
                ImGui::EndTable();
            }
            else
            {
                ImGui::Text("Chosen Chip8 version does not support advanced audio.");
            }
        }

        ImGui::End();
    }
}
