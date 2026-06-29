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
    [[nodiscard]] bool is_high_resolution() const { return high_resolution_screen; }

    // Setters
    void clear() override { memset(screen, 0, sizeof(screen)); }
    void enable_high_resolution_scree() { high_resolution_screen = true; clear(); }
    void disable_high_resolution_scree() { high_resolution_screen = false; clear(); }
    void scroll_down(uint8_t n);
    void scroll_right();
    void scroll_left();
private:
    uint8_t screen[128 * 64] {};
    bool high_resolution_screen = false;

    void scroll(uint8_t x_, uint8_t y_);
};
