#include "XOCHIP/xochip_display.hpp"

#include <stdexcept>
#include <vector>

uint8_t& xochip_display::get_pixel(int x, int y)
{
    throw std::runtime_error{"This function should not be called now (Undefined behaviour)."};
}

uint8_t xochip_display::get_pixel_value(int x, int y)
{
    uint8_t val1 = screen[x + y * get_screen_width()];
    uint8_t val2 = bitplane[x + y * get_screen_width()];

    if (val1 == 1 && val2 == 0)
        return 1;
    if (val1 == 0 && val2 == 1)
        return 2;
    if (val1 == 1 && val2 == 1)
        return 3;
    return 0;
}

void xochip_display::clear()
{
    if (active_bitplane == Bitplane::FIRST || active_bitplane == Bitplane::BOTH)
        memset(screen, 0, sizeof(screen));
    if (active_bitplane == Bitplane::SECOND || active_bitplane == Bitplane::BOTH)
        memset(bitplane, 0, sizeof(bitplane));
}

void xochip_display::scroll_down(uint8_t n)
{
    if (active_bitplane == Bitplane::FIRST || active_bitplane == Bitplane::BOTH)
        scroll_screen(0, n, get_screen_width(), get_screen_height(), screen);
    if (active_bitplane == Bitplane::SECOND || active_bitplane == Bitplane::BOTH)
        scroll_screen(0, n, get_screen_width(), get_screen_height(), bitplane);
}

void xochip_display::scroll_left()
{
    if (active_bitplane == Bitplane::FIRST || active_bitplane == Bitplane::BOTH)
        scroll_screen(-4, 0, get_screen_width(), get_screen_height(), screen);
    if (active_bitplane == Bitplane::SECOND || active_bitplane == Bitplane::BOTH)
        scroll_screen(-4, 0, get_screen_width(), get_screen_height(), bitplane);
}

void xochip_display::scroll_right()
{
    if (active_bitplane == Bitplane::FIRST || active_bitplane == Bitplane::BOTH)
        scroll_screen(4, 0, get_screen_width(), get_screen_height(), screen);
    if (active_bitplane == Bitplane::SECOND || active_bitplane == Bitplane::BOTH)
        scroll_screen(4, 0, get_screen_width(), get_screen_height(), bitplane);
}

void xochip_display::scroll_up(uint8_t n)
{
    if (active_bitplane == Bitplane::FIRST || active_bitplane == Bitplane::BOTH)
        scroll_screen(0, -n, get_screen_width(), get_screen_height(), screen);
    if (active_bitplane == Bitplane::SECOND || active_bitplane == Bitplane::BOTH)
        scroll_screen(0, -n, get_screen_width(), get_screen_height(), bitplane);
}

bool xochip_display::invert_pixel(uint8_t x, uint8_t y, Bitplane bitplane_)
{
    if (bitplane_ == Bitplane::FIRST)
    {
        uint8_t& p = screen[x + y * get_screen_width()];
        if (p == 1)
        {
            p = 0;
            return true;
        }
        p = 1;
        return false;
    }
    if (bitplane_ == Bitplane::SECOND)
    {
        uint8_t& p = bitplane[x + y * get_screen_width()];
        if (p == 1)
        {
            p = 0;
            return true;
        }
        p = 1;
        return false;
    }

    return false;
}

