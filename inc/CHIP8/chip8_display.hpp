#pragma once
#include <cstdint>
#include <display.hpp>

class chip8_display : public display
{
public:
    chip8_display() = default;
    ~chip8_display() override = default;

    // Getters
    [[nodiscard]] uint8_t* get_screen() override { return screen; }
    [[nodiscard]] uint8_t get_screen_width() override { return 64; }
    [[nodiscard]] uint8_t get_screen_height() override { return 32; }
    [[nodiscard]] uint8_t& get_pixel(int x, int y) override { return screen[x + y * 64]; }

    // Setters
    void clear() override { memset(screen, 0, sizeof(screen)); }
private:
    uint8_t screen[64 * 32] {};
};
