#include <iostream>

#include "SDL/window_renderer.hpp"
#include <SDL3/SDL.h>

#include "chip8.hpp"

struct Context
{
    window_renderer window_renderer;
    chip8 chip;
};

int init(Context& ctx)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        return -1;

    ctx.window_renderer = std::move(window_renderer{"Standard CHIP-8 screen", 960, 540 ,SDL_WINDOW_RESIZABLE});
    ctx.chip = chip8{"rom/IBM Logo.ch8", &ctx.window_renderer};

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
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        ctx.chip.update(0.0f);
        ctx.chip.render();

    }
    return 0;
}

int shutdown(Context& ctx)
{
    ctx.window_renderer.close();

    return 0;
}

int main()
{
    Context context;

    init(context);

    update(context);

    shutdown(context);

    return 0;
}