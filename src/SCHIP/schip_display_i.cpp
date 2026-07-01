#include "SCHIP/schip_display.hpp"

#include <algorithm>
#include <vector>

schip_display_i::~schip_display_i()
{
}

void schip_display_i::scroll_down(uint8_t n)
{
    scroll_screen(0, n, get_screen_width(), get_screen_height(), screen);
}

void schip_display_i::scroll_right()
{
    scroll_screen(4, 0, get_screen_width(), get_screen_height(), screen);
}

void schip_display_i::scroll_left()
{
    scroll_screen(-4, 0, get_screen_width(), get_screen_height(), screen);
}

