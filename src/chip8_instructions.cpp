#include "chip8.hpp"

void chip8::OP_00E0()
{
    for (int i = 0; i < 64 * 32; ++i) screen[i] = 0;
}

void chip8::OP_00EE()
{
    program_counter = stack[--stack_pointer];
}

void chip8::OP_1NNN()
{
    uint16_t address = opcode & 0x0FFF;
    program_counter = address;
}

void chip8::OP_2NNN()
{
    uint16_t address = opcode & 0x0FFF;
    stack[stack_pointer++] = program_counter;
    program_counter = address;
}

void chip8::OP_6XNN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;
    registry[x] = nn;
}

void chip8::OP_7XNN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;
    registry[x] += nn;
}

void chip8::OP_ANNN()
{
    uint16_t address = opcode & 0x0FFF;
    index_register = address;
}

void chip8::OP_DXYN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;

    // Wrap x and y position for a sprite
    uint8_t x_coord = registry[x] % 64;
    uint8_t y_coord = registry[y] % 32;

    registry[0xF] = 0;

    for (int row = 0; row < n; ++row)
    {
        uint8_t sprite_data = memory[index_register + row];

        uint8_t target_y = y_coord + row;

        if (target_y >= 32) break;

        for (int i = 0; i < 8; ++i)
        {
            bool is_sprite_bit_set = ((sprite_data >> (7 - i)) & 1) == 1;

            uint8_t target_x = x_coord + i;

            if (target_x >= 64) break;

            uint8_t& current_screen_coord = screen[target_x + target_y * 64];

            if (is_sprite_bit_set && current_screen_coord)
            {
                current_screen_coord = 0;
                registry[0xF] = 1;
            }
            else if (is_sprite_bit_set && !current_screen_coord)
            {
                current_screen_coord = 1;
            }
        }
    }
}

void chip8::OP_3XNN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (registry[x] == nn)
        program_counter += 2;
}

void chip8::OP_4XNN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    if (registry[x] != nn)
        program_counter += 2;
}

void chip8::OP_5XY0()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (registry[x] == registry[y])
        program_counter += 2;
}

void chip8::OP_9XY0()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if (registry[x] != registry[y])
        program_counter += 2;
}

void chip8::OP_8XY0()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    registry[x] = registry[y];
}

void chip8::OP_8XY1()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    registry[x] = registry[x] | registry[y];
}

void chip8::OP_8XY2()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    registry[x] = registry[x] & registry[y];
}

void chip8::OP_8XY3()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    registry[x] = registry[x] ^ registry[y];
}

void chip8::OP_8XY4()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint8_t a = registry[x];
    uint8_t b = registry[y];

    registry[x] = a + b;

    // Check for overflow
    if (a > 0 && b > 0 && registry[x] < 0)
        registry[0xF] = 1;
}

void chip8::OP_8XY5()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint8_t a = registry[x];
    uint8_t b = registry[y];

    registry[x] = a - b;

    if (a >= b)
        registry[0xF] = 1;
    else if (b > a)
        registry[0xF] = 0;
}

void chip8::OP_8XY7()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint8_t a = registry[x];
    uint8_t b = registry[y];

    registry[x] = b - a;

    if (a >= b)
        registry[0xF] = 1;
    else if (b > a)
        registry[0xF] = 0;
}

void chip8::OP_8XY6()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if constexpr (!CHIP48_IMPLEMENTATION)
    {
        registry[x] = registry[y];
    }

    uint8_t removed_bit = registry[x] & 1;
    registry[x] = registry[x] >> 1;

    registry[0xF] = removed_bit;
}

void chip8::OP_8XYE()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if constexpr (!CHIP48_IMPLEMENTATION)
    {
        registry[x] = registry[y];
    }

    uint8_t removed_bit = registry[x] >> 7;
    registry[x] = registry[x] << 1;

    registry[0xF] = removed_bit;
}
