#include "SCHIP/schip_display.hpp"

#include <vector>

void schip_display::scroll_down(uint8_t n)
{
    scroll(0, n);
}

void schip_display::scroll_right()
{
    scroll(4, 0);
}

void schip_display::scroll_left()
{
    scroll(-4, 0);
}

void schip_display::scroll(uint8_t x_, uint8_t y_)
{
    uint8_t width = get_screen_width();
    uint8_t height = get_screen_height();

    std::vector<uint8_t> temp;
    temp.resize(width * height);

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            uint8_t& pixel = get_pixel(x, y);
            if (pixel != 1) continue;

            uint8_t new_x = x + x_;
            uint8_t new_y = y + y_;

            if (new_x >= width) continue;
            if (new_y >= height) continue;

            temp[new_x + new_y * width] = 1;
        }
    }

    std::ranges::copy(temp, screen);
}
