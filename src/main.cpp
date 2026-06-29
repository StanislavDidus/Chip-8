#include <iostream>
#include <functional>

#include "SDL/window_renderer.hpp"
#include <SDL3/SDL.h>

#include "chip8.hpp"

#include "CHIP48/chip48_display.hpp"
#include "CHIP48/chip48_instructions.hpp"
#include "CHIP48/chip48_memory.hpp"
#include "CHIP48/chip48_quirks.hpp"

#include "CHIP8/chip8_display.hpp"
#include "CHIP8/chip8_instructions.hpp"
#include "CHIP8/chip8_memory.hpp"
#include "CHIP8/chip8_quirks.hpp"

#include "SCHIP/schip_display.hpp"
#include "SCHIP/schip_instructions.hpp"
#include "SCHIP/schip_memory.hpp"
#include "SCHIP/schip_quirks.hpp"

struct Context
{
    char* path_to_rom = nullptr;
    int chip8_version = 0;
    window_renderer window_renderer;
    std::unique_ptr<chip8> chip;
};

int init(Context& ctx)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
        return -1;

    ctx.window_renderer = std::move(window_renderer{"CHIP-8", 960, 540 ,SDL_WINDOW_RESIZABLE});

    std::unique_ptr<display> display = nullptr;
    std::unique_ptr<instructions> instructions = nullptr;
    std::unique_ptr<memory> memory = nullptr;
    std::unique_ptr<quirks> quirks = nullptr;
    if (ctx.chip8_version == 1)
    {
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, CHIP8_INSTRUCTION_PER_FRAME);
        ctx.chip->setup_chip8(
            std::move(std::make_unique<chip8_display>()),
            std::move(std::make_unique<chip8_quirks>()),
            std::move(std::make_unique<chip8_instructions>(*ctx.chip)),
            std::move(std::make_unique<chip8_memory>()));
    }
    else if (ctx.chip8_version == 2)
    {
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, CHIP48_INSTRUCTION_PER_FRAME);
        ctx.chip->setup_chip8(
            std::move(std::make_unique<chip48_display>()),
            std::move(std::make_unique<chip48_quirks>()),
            std::move(std::make_unique<chip48_instructions>(*ctx.chip)),
            std::move(std::make_unique<chip48_memory>()));
    }
    else if (ctx.chip8_version == 3)
    {
        ctx.chip = std::make_unique<chip8>(ctx.window_renderer, SCHIP_INSTRUCTION_PER_FRAME);
        ctx.chip->setup_chip8(
            std::move(std::make_unique<schip_display>()),
            std::move(std::make_unique<schip_quirks>()),
            std::move(std::make_unique<schip_instructions>(*ctx.chip)),
            std::move(std::make_unique<schip_memory>()));
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
#define TEST
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
    char arg1[] = "rom/Joust.ch8";
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
    std::cout << "Note: Most of the games only support a specific platform" << std::endl;
    std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;

    std::cin >> context.chip8_version;
    context.path_to_rom = argv[1];

    init(context);

    update(context);

    shutdown(context);

    return 0;
}