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

    if (b >= a)
        registry[0xF] = 1;
    else if (a > b)
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

void chip8::OP_BNNN()
{
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t x = (opcode & 0x0F00) >> 8;

    if constexpr (!CHIP48_IMPLEMENTATION)
    {
        program_counter = nnn + registry[0x0];
    }
    else
    {
        program_counter = nnn + registry[x];
    }
}


// TODO: Move it from here
std::random_device rng{};
std::uniform_int_distribution<int> dist{0, 255};

void chip8::OP_CXNN()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;

    auto rand = static_cast<uint8_t>(dist(rng));

    registry[x] = rand & nn;
}

void chip8::OP_EX9E()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = registry[x];

    if (key <= 0xF)
        if (keys[key])
            program_counter += 2;
}

void chip8::OP_EXA1()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = registry[x];

    if (key <= 0xF)
        if (!keys[key])
            program_counter += 2;
}

void chip8::OP_FX07()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    registry[x] = delay_timer;
}

void chip8::OP_FX15()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    delay_timer = registry[x];
}

void chip8::OP_FX18()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    sound_timer = registry[x];
}

void chip8::OP_FX1E()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    uint16_t a = index_register;
    uint8_t b = registry[x];

    index_register = a + b;

    if constexpr(AMIGA_INDEX_IMPLEMENTATION)
        if (a > 0 && b > 0 && index_register < 0)
            registry[0xF] = 1;
}

void chip8::OP_FX0A()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i < 16; ++i)
    {
        if (keys[i])
        {
            registry[x] = i;
            return;
        }
    }

    program_counter -= 2;
}

void chip8::OP_FX29()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t last_nibble = registry[x] & 0x0F;

    index_register = FONT_MEMORY_LOCATION + 5 * last_nibble;
}

void chip8::OP_FX33()
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t number = registry[x];

    memory[index_register] = number / 100;
    memory[index_register + 1] = (number % 100) / 10;
    memory[index_register + 2] = number % 10;

}

void chip8::OP_FX55()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i)
    {
        memory[index_register + i] = registry[i];
    }

    if constexpr (!CHIP48_IMPLEMENTATION)
        index_register += x;
}

void chip8::OP_FX65()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i)
    {
        registry[i] = memory[index_register + i];
    }

    if constexpr (!CHIP48_IMPLEMENTATION)
        index_register += x;
}
