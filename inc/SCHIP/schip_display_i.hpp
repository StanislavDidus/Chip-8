#pragma once

#pragma once

#include <cstdint>
#include <display.hpp>
#include <vector>
#include <algorithm>


static void scroll_screen(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t* buffer)
{
    std::vector<uint8_t> temp;
    temp.resize(width * height);

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            uint8_t& pixel = buffer[i + j * width];

            if (pixel != 1) continue;

            uint8_t new_x = x + i;
            uint8_t new_y = y + j;

            if (new_x >= width) continue;
            if (new_y >= height) continue;

            temp[new_x + new_y * width] = 1;
        }
    }

    std::ranges::copy(temp, buffer);
}

class schip_display_i : public display
{
public:
    schip_display_i() = default;
    ~schip_display_i() override = 0;

    // Getters
    [[nodiscard]] uint8_t* get_screen() override { return screen; }
    [[nodiscard]] uint8_t get_screen_width() override { return high_resolution_screen ? 128 : 64; }
    [[nodiscard]] uint8_t get_screen_height() override { return high_resolution_screen ? 64 : 32; }
    [[nodiscard]] uint8_t& get_pixel(int x, int y) override { return screen[x + y * get_screen_width()]; }
    [[nodiscard]] uint8_t get_pixel_value(int x, int y) override { return get_pixel(x, y); }
    [[nodiscard]] bool is_high_resolution() const { return high_resolution_screen; }

    // Setters
    void clear() override { memset(screen, 0, sizeof(screen)); }
    void enable_high_resolution_scree() { high_resolution_screen = true; clear(); }
    void disable_high_resolution_scree() { high_resolution_screen = false; clear(); }
    virtual void scroll_down(uint8_t n);
    virtual void scroll_right();
    virtual void scroll_left();

protected:
    uint8_t screen[128 * 64] {};
    bool high_resolution_screen = false;
};
