#pragma once
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"

class display
{
public:
    display() = default;
    virtual ~display() = 0;

    // Getters
    [[nodiscard]] virtual uint8_t* get_screen() = 0;
    [[nodiscard]] virtual uint8_t get_screen_width() = 0;
    [[nodiscard]] virtual uint8_t get_screen_height() = 0;
    [[nodiscard]] virtual uint8_t& get_pixel(int x, int y) = 0;
    [[nodiscard]] virtual uint8_t get_pixel_value(int x, int y) = 0;
    [[nodiscard]] virtual bool is_high_res() = 0;

    // Setters
    virtual void clear() = 0;
};

inline display::~display()
{
}

