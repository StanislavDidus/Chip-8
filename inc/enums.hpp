#pragma once

enum class chip8_status
{
    NONE,
    MENU, // Game is not running
    PLAYING, // Game is running
    PAUSED, // Game is paused
};