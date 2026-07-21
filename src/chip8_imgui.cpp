#include "chip8.hpp"

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

void chip8::render_launch_window()
{
    ImGui::Begin("Launch");

    if (ImGui::BeginMenu("Settings"))
    {
        ImGui::MenuItem("General settings", nullptr, &config.show_general_settings);
        ImGui::MenuItem("Color settings", nullptr, &config.show_color_settings);
        ImGui::MenuItem("Audio settings", nullptr, &config.show_audio_settings);

        ImGui::EndMenu();
    }

    static char path_buffer[256] {};
    std::copy(std::begin(config.rom_path), std::end(config.rom_path), std::begin(path_buffer));

    ImVec2 avail = ImGui::GetContentRegionAvail();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail.x * 0.5f - get_header_size(style, "Welcome to my Chip8 emulator!").x * 0.5f);
    //ImGui::Text("Welcome to my Chip8 emulator!");
    draw_header(style, "Welcome to my Chip8 emulator!", 35.0f);
    ImGui::Spacing();

    if (config.show_intro)
    {
        ImGui::TextWrapped("- This is a revolutionary emulator made for the purpose of playing and creating retro games.\n"
                    "- This emulator can run any Chip8 game as well as games made for other versions of Chip8 (Chip48, Super-Chip, XO-Chip).\n"
                    "- Below you need to specify the path to the ROM you want to run and which version of Chip8 to use. "
                    "For more settings go to the \"settings\" tab.\n"
                    "If you never wish to see this text again uncheck the checkbox in the \"settings\" .\n");
        ImGui::Spacing();
        ImGui::Separator();
    }

    draw_header(style, "Path to ROM");

    ImGui::InputText("##inputtext", path_buffer, sizeof(path_buffer));

    ImGui::SameLine();

    if (ImGui::Button("Open File Browser"))
    {
        file_dialog.Open();
    }

    ImGui::Spacing();

    draw_header(style, "Chip8 version");

    // Choose Chip8 version
    static const char* versions[] {"Chip8", "Chip48", "Super-Chip", "XO-Chip"};
    static int selected_version = config.chip8_version;

    static const char* quirks[] {"Chip8", "Chip48", "Super-Chip", "XO-Chip"};
    static int selected_quirks = config.chip8_quirks;

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

    draw_header(style, "Instruction execution speed");

    // Choose Chip8 instruction executing speed
    if (ImGui::InputInt("##inputint", &config.instructions_per_second))
    {
        config.instructions_per_second = std::clamp(config.instructions_per_second, 0, std::numeric_limits<int32_t>::max());
    }
    if (ImGui::Button("Chip8")) {config.instructions_per_second = CHIP8_INSTRUCTIONS_PER_SECOND; config.instructions_per_frame = CHIP8_INSTRUCTION_PER_FRAME; }
    ImGui::SameLine();
    if (ImGui::Button("Super-Chip")) {config.instructions_per_second = SCHIP_INSTRUCTIONS_PER_SECOND; config.instructions_per_frame = SCHIP_INSTRUCTION_PER_FRAME; }
    ImGui::SameLine();
    if (ImGui::Button("XO-Chip")) {config.instructions_per_second = XOCHIP_INSTRUCTIONS_PER_SECOND; config.instructions_per_frame = XOCHIP_INSTRUCTION_PER_FRAME; }

    ImGui::Spacing();

    // Quirks
    draw_header(style, "Quirks");

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

    draw_header(style, "Launch options");

    // Start game
    if (ImGui::Button("Start"))
    {
        config.chip8_version = selected_version;
        config.chip8_quirks = selected_quirks;
        std::copy(std::begin(path_buffer), std::end(path_buffer), std::begin(config.rom_path));

        setup_from_config();
    }

    ImGui::SameLine();

    // Debug mode Checkbox
    if (ImGui::Checkbox("Debugger", &config.is_debug_mode))
    {

    }

    bool is_disabled_hack = false;
    if (status != chip8_status::PLAYING && status != chip8_status::PAUSED)
    {
        ImGui::BeginDisabled();
        is_disabled_hack = true;
    }

    ImGui::Spacing();

    ImGui::Text("Playtime %.1f", game_timer);

    if (ImGui::Button("Pause/Resume"))
    {
        if (status == chip8_status::PLAYING)
            pause_game();
        else if (status == chip8_status::PAUSED)
            resume_game();
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        stop_game();
    }

    // This hack is needed because stop_game() function sets status to Menu so the if condition
    // gets true and it called EndDisabled() without BeginDisabled being called.
    if (is_disabled_hack)
        ImGui::EndDisabled();

    if (config.is_debug_mode)
    {
        if (status != chip8_status::PAUSED)
            ImGui::BeginDisabled();

        if (ImGui::Button("Move 1 instruction"))
        {
            execute_n_instructions(1);
        }
        ImGui::SameLine();
        if (ImGui::Button("Move 10 instruction"))
        {
            execute_n_instructions(10);
        }

        if (status != chip8_status::PAUSED)
            ImGui::EndDisabled();
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
        memset(path_buffer, 0, sizeof(path_buffer));
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
        ImGui::TextWrapped("%s", log.c_str());
    }

    ImGui::EndChild();

    ImGui::End();
}

static std::optional<ImVec2> viewport_last_position = std::nullopt;
static std::optional<ImVec2> viewport_last_size = std::nullopt;

void chip8::render_viewport_window()
{
    ImGuiWindowFlags flags;
    if (!config.is_immersive_mode)
    {
        flags = ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse;

        if (viewport_last_position)
        {
            ImGui::SetNextWindowPos(viewport_last_position.value());
            viewport_last_position = std::nullopt;
        }
        if (viewport_last_size)
        {
            ImGui::SetNextWindowSize(viewport_last_size.value());
            viewport_last_size = std::nullopt;
        }
    }
    else
    {
        flags = ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
    }

    ImGui::Begin("Viewport", nullptr, flags);

    ImVec2 avail_size = ImGui::GetContentRegionAvail();

    if (ImGui::InvisibleButton("##whywouldyouaddidtoinvisiblebutton", avail_size, ImGuiButtonFlags_PressedOnDoubleClick))
    {
        //Enter fullscreen
        config.is_immersive_mode = !config.is_immersive_mode;
        renderer.toggle_fullscreen();

        // Save windows position and size to then return to it
        if (config.is_immersive_mode)
        {
            viewport_last_position = ImGui::GetWindowPos();
            viewport_last_size = ImGui::GetWindowSize();
        }
    }

    if (texture && (status == chip8_status::PLAYING || status == chip8_status::PAUSED))
    {

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

        ImGui::SetCursorPosX(start_x);
        ImGui::SetCursorPosY(start_y);

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
            ImGui::TextWrapped("This is a color settings menu.");
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

    if (config.show_general_settings)
    {
        if (ImGui::Begin("General setting", &config.show_general_settings))
        {
            ImGui::TextWrapped("This is general settings menu.");

            ImGui::Checkbox("Show introduction text", &config.show_intro);
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

                                if (m_core.get_pc() == byte_index || m_core.get_pc() + 1 == byte_index)
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

            ImGui::SetCursorPosX(avail.x * 0.25f - get_header_size(style, "Register").x * 0.5f);

            draw_header(style, "Register");
            ImGui::SameLine();
            ImGui::SetCursorPosX(avail.x * 0.75f - get_header_size(style, "Stack").x * 0.5f);
            draw_header(style, "Stack");

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
            draw_header(style, "Core");
            ImGui::Text("PC: %02X", m_core.get_pc());
            ImGui::Text("I: %02X", m_core.get_index_register());
            ImGui::Text("SP: %02X", m_core.get_stack_pointer());

            ImGui::Separator();

            draw_header(style, "Timers");
            ImGui::Text("Delay timer: %02X", m_core.get_delay_timer_value());
            ImGui::Text("Sound timer: %02X", m_core.get_sound_timer_value());

            ImGui::Separator();

            draw_header(style, "Advanced Audio");
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
