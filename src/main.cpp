#include <fstream>
#include <iostream>
#include <functional>

#include "SDL/window_renderer.hpp"
#include <SDL3/SDL.h>

#include "chip8.hpp"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "chip8_imgui_style.hpp"

#include "CHIP48/chip48_instructions.hpp"
#include "CHIP8/chip8_audio.hpp"

#include "CHIP8/chip8_display.hpp"
#include "CHIP8/chip8_memory.hpp"

#include "SCHIP/schip_display.hpp"
#include "SCHIP/schip_instructions.hpp"

#include "XOCHIP/xochip_audio.hpp"
#include "XOCHIP/xochip_display.hpp"
#include "XOCHIP/xochip_instructions.hpp"
#include "XOCHIP/xochip_memory.hpp"

struct Context
{
    char* path_to_rom = nullptr;
    int chip8_version = 0;
    int chip8_hz = 0;
    window_renderer window_renderer_;
    std::unique_ptr<chip8> chip;
};

void init(Context& ctx)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
        throw std::runtime_error {"Error when initializing SDL."};

    ctx.window_renderer_ = std::move(window_renderer{"Chip8 Emulator Window", 960, 540 ,SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY});

    // Setup icon
    SDL_Surface* icon_surface = SDL_LoadPNG("assets/sprites/icon.png");
    SDL_Surface* alternate_image = SDL_ScaleSurface(icon_surface, 64, 64, SDL_SCALEMODE_NEAREST);
    SDL_AddSurfaceAlternateImage(icon_surface, alternate_image);
    SDL_SetWindowIcon(ctx.window_renderer_.get_window(), icon_surface);
    SDL_DestroySurface(icon_surface);
    SDL_DestroySurface(alternate_image);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable multi viewports
    ImGui_ImplSDL3_InitForSDLRenderer(ctx.window_renderer_.get_window(), ctx.window_renderer_.get_renderer());
    ImGui_ImplSDLRenderer3_Init(ctx.window_renderer_.get_renderer());

    ImFont* main_font = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans.ttf");
    ImFont* header_font = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Bold.ttf");
    ImGui::PushFont(main_font, 30.0f);

    ImGui::StyleColorsDark();
    setup_style();

    // Load config if it exists
    chip8_config config {};
    std::ifstream cfg {"config.bin", std::ios::binary};
    if (cfg)
    {
        cfg.read(reinterpret_cast<char*>(&config), sizeof(config));

        if (cfg.bad())
            throw std::runtime_error {"Error when reading from a config.bin file."};
    }

    ctx.chip = std::make_unique<chip8>(ctx.window_renderer_, application_style{main_font, header_font}, config);
}

void update(Context& ctx)
{
    SDL_Event event;
    bool running = true;

    SDL_Renderer* renderer = ctx.window_renderer_.get_renderer();
    if (!renderer)
        throw std::runtime_error {"Error when creating SDL_Renderer."};

    double delta_time = 0.0;

    while (running)
    {
        uint64_t start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                ctx.chip->key_pressed(event.key.scancode);
                break;
            case SDL_EVENT_KEY_UP:
                ctx.chip->key_released(event.key.scancode);
                break;
            }
        }

        ctx.chip->update(16.666 / 1000);

        ctx.chip->render(ctx.window_renderer_);

        // Limit FPS to 60
        uint64_t end = SDL_GetTicks();
        delta_time = static_cast<double>(end - start);
        if (delta_time < 16.666)
            SDL_Delay(16.666 - delta_time);

    }
}

void shutdown(Context& ctx)
{
    ctx.window_renderer_.close();

    std::ofstream cfg("config.bin", std::ios::binary);
    if (!cfg)
    {
        throw std::runtime_error{"Could not open config.bin file for writing."};
    }

    cfg.write(reinterpret_cast<const char*>(&ctx.chip->get_config()), sizeof(chip8_config));

    if (cfg.bad())
        throw std::runtime_error{"Could not write to a config.bin file."};
}

int main()
{
    Context context;

    init(context);

    update(context);

    shutdown(context);

    return 0;
}