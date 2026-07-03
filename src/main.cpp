#include <iostream>
#include <functional>

#include "SDL/window_renderer.hpp"
#include <SDL3/SDL.h>

#include "chip8.hpp"

#include "CHIP48/chip48_instructions.hpp"
#include "CHIP8/chip8_audio.hpp"

#include "CHIP8/chip8_display.hpp"
#include "CHIP8/chip8_instructions.hpp"
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
    window_renderer window_renderer;
    std::unique_ptr<chip8> chip;
};

int init(Context& ctx)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
        return -1;

    ctx.window_renderer = std::move(window_renderer{"CHIP-8", 960, 540 ,SDL_WINDOW_RESIZABLE});

    if (ctx.chip8_hz == 1)
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, CHIP8_INSTRUCTION_PER_FRAME);
    else if (ctx.chip8_hz == 2)
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, SCHIP_INSTRUCTION_PER_FRAME);
    else if (ctx.chip8_hz == 3)
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, XOCHIP_INSTRUCTION_PER_FRAME);

    else
    {
        throw std::runtime_error("Wrong CHIP8 hz speed.");
    }

    if (ctx.chip8_version == 1)
    {
        ctx.chip->setup_chip8(
            std::make_unique<chip8_display>(),
            std::make_unique<chip8_instructions>(*ctx.chip),
            std::make_unique<chip8_memory>(),
            std::make_unique<chip8_audio>());
    }
    else if (ctx.chip8_version == 2)
    {
        ctx.chip->setup_chip8(
            std::make_unique<chip8_display>(),
            std::make_unique<chip48_instructions>(*ctx.chip),
            std::make_unique<chip8_memory>(),
            std::make_unique<chip8_audio>());
    }
    else if (ctx.chip8_version == 3)
    {
        ctx.chip->setup_chip8(
            std::make_unique<schip_display>(),
            std::make_unique<schip_instructions>(*ctx.chip),
            std::make_unique<chip8_memory>(),
            std::make_unique<chip8_audio>());
    }
    else if ( ctx.chip8_version == 4)
    {
        ctx.chip->setup_chip8(
            std::make_unique<xochip_display>(),
            std::make_unique<xochip_instructions>(*ctx.chip),
            std::make_unique<xochip_memory>(),
            std::make_unique<xochip_audio>());
    }
    else
    {
        throw std::runtime_error("Wrong CHIP8 version.");
    }

    ctx.chip->load_rom(ctx.path_to_rom);

    return 0;
}

int update(Context& ctx)
{
    SDL_Event event;
    bool running = true;

    SDL_Renderer* renderer = ctx.window_renderer.get_renderer();
    if (!renderer)
        return -1;

    while (running)
    {
        uint64_t start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
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

        ctx.chip->update();
        ctx.chip->render(ctx.window_renderer);

        // Limit FPS to 60
        uint64_t end = SDL_GetTicks();
        double delta_time = static_cast<double>(end - start);
        if (delta_time < 16.666)
            SDL_Delay(16.666 - delta_time);

    }
    return 0;
}

int shutdown(Context& ctx)
{
    ctx.window_renderer.close();

    return 0;
}


#ifndef NDEBUG
//#define TEST
#endif

int main(int argc, char* argv[])
{
    if (argc < 1)
    {
        std::cerr << "Not enough arguments." << std::endl;
        return -1;
    }

#ifdef TEST
    argc = 2;
    char arg0[] = "./CHIP8";

    ///
    // CHANGE THIS PATH TO THE ROM FILE YOU WANT TO RUN
    ///
    char arg1[] = "rom/5-quirks.ch8";

    *argv = new char[2];
    argv[0] = arg0;
    argv[1] = arg1;
#endif

    Context context;

    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    std::cout << "Choose the version of CHIP8 you want to use:" << std::endl;
    std::cout << "1: Chip8 (Standard version that supports older games)" << std::endl;
    std::cout << "2: Chip48 (Modernized version of Chip8)" << std::endl;
    std::cout << "3: Super-Chip (Improved version of Chip48 with bigger screen and new capabilities)" << std::endl;
    std::cout << "4: XO-Chip (Super-Chip advancement that supports multiple colors and more sound effects)" << std::endl;
    std::cout << "Note: Most of the games only support a specific platform" << std::endl;
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    std::cin >> context.chip8_version;

    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    std::cout << "Choose the instructions per second that you want to run the emulator on:" << std::endl;
    std::cout << "1: 700hz (Common speed for Chip8 and Chip48 games)" << std::endl;
    std::cout << "2: 1800hz (Required if you want to run Super-Chip games) " << std::endl;
    std::cout << "3: 4500hz (Extra huge speed, needed for some bih XOChip games) " << std::endl;
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    std::cin >> context.chip8_hz;

    context.path_to_rom = argv[1];

    init(context);

    update(context);

    shutdown(context);

    return 0;
}