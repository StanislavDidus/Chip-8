#pragma once

#include <cstdint>
#include <display.hpp>

class schip_display : public display
{
public:
    schip_display() = default;
    ~schip_display() override = default;

    // Getters
    [[nodiscard]] uint8_t* get_screen() override { return screen; }
    [[nodiscard]] uint8_t get_screen_width() override { return high_resolution_screen ? 128 : 64; }
    [[nodiscard]] uint8_t get_screen_height() override { return high_resolution_screen ? 64 : 32; }
    [[nodiscard]] uint8_t& get_pixel(int x, int y) override { return screen[x + y * get_screen_width()]; }

    // Setters
    void clear() override { memset(screen, 0, sizeof(screen)); }
private:
    uint8_t screen[128 * 64] {};
    bool high_resolution_screen = true;
};
