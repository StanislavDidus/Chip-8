#pragma once

#include "SCHIP/schip_display_i.hpp"

class xochip_display : public schip_display_i
{
public:
    enum class Bitplane
    {
        NONE = 0,
        FIRST,
        SECOND,
        BOTH,
    };

    xochip_display() = default;
    ~xochip_display() override = default;

    [[nodiscard]] uint8_t& get_pixel(int x, int y) override;
    [[nodiscard]] uint8_t get_pixel_value(int x, int y) override;

    void clear() override;
    void scroll_down(uint8_t n) override;
    void scroll_left() override;
    void scroll_right() override;
    void scroll_up(uint8_t n);

    // Setters
    void set_bitplane(uint8_t number) { active_bitplane = static_cast<Bitplane>(number); }
    // Returns true if any pixel was set from 1 to 0
    bool invert_pixel(uint8_t x, uint8_t y, Bitplane bitplane_);

    // Getters
    [[nodiscard]] Bitplane get_active_bitplane() const { return active_bitplane; }

private:
    uint8_t bitplane[128 * 64] {};
    Bitplane active_bitplane = Bitplane::FIRST;
};

