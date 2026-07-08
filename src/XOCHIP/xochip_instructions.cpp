#include "XOCHIP/xochip_instructions.hpp"

#include <stdexcept>
#include <cstring>

#include "chip8.hpp"
#include "XOCHIP/xochip_audio.hpp"
#include "XOCHIP/xochip_display.hpp"
#include "XOCHIP/xochip_memory.hpp"

void xochip_instructions::init_table()
{
    table[0x0] = [this]{ return Table_0(); };
    table[0xD] = [this]
    {
        return [this]
        {
            if (get_n_value() == 0 && static_cast<xochip_display*>(&owner.get_display())->is_high_res())
                OP_DXY0();
            else
                OP_DXYN();
        };
    };

    table[0x3] = [this] { return [this] { OP_3XNN(); }; };
    table[0x4] = [this] { return [this] { OP_4XNN(); }; };
    table[0x5] = [this] { return Table_5(); };
    table[0x9] = [this] { return [this] { OP_9XY0(); }; };

    table_E[0xE] = [this] { OP_EX9E(); };
    table_E[0x1] = [this] { OP_EXA1(); };

    table_5[0x0] = [this] { OP_5XY0(); };
    table_5[0x2] = [this] { OP_5XY2(); };
    table_5[0x3] = [this] { OP_5XY3(); };

    table_F[0x00] = [this] { OP_F000(); };
    table_F[0x01] = [this] { OP_FN01(); };
    table_F[0x02] = [this] { OP_F002(); };
    table_F[0x3A] = [this] { OP_FX3A(); };
    table_F[0x1E] = [this] { OP_FX1E(); };

    table_0[0xFB] = [this] { OP_00FB(); };
    table_0[0xFC] = [this] { OP_00FC(); };

}

instruction xochip_instructions::Table_5()
{
    return table_5.at(get_n_value());
}

instruction xochip_instructions::Table_0()
{
    uint8_t second_byte = (opcode & 0x00F0) >> 4;

    if (second_byte == 0xD)
    {
        return [this] { OP_00DN(); };
    }
    else if (second_byte == 0xC)
    {
        return [this]{ OP_00CN(); };
    }

    return table_0.at(get_nn_value());
}

void xochip_instructions::OP_5XY2()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint8_t x = get_registry_x_index();
    uint8_t y = get_registry_y_index();

    int offset = 0;
    if (x <= y)
    {
        for (int i = x; i <= y; ++i, ++offset)
        {
            memory.access_memory()[core.get_index_register() + offset] = core.V(i);
        }
    }
    else
    {
        for (int i = x; i >= y; --i, ++offset)
        {
            memory.access_memory()[core.get_index_register() + offset] = core.V(i);
        }
    }
}

void xochip_instructions::OP_5XY3()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint8_t x = get_registry_x_index();
    uint8_t y = get_registry_y_index();

    int offset = 0;
    if (x <= y)
    {
        for (int i = x; i <= y; ++i, ++offset)
        {
            core.V(i) = memory.access_memory()[core.get_index_register() + offset];
        }
    }
    else
    {
        for (int i = x; i >= y; --i, ++offset)
        {
            core.V(i) = memory.access_memory()[core.get_index_register() + offset];
        }
    }
}

void xochip_instructions::OP_F000()
{
    memory& memory = owner.get_memory();
    core& core = owner.get_core();

    uint16_t two_byte_address = memory.get_opcode(core.get_pc());
    core.set_index_register(two_byte_address);
    core.skip_next();
}

void xochip_instructions::OP_FN01()
{
    core& core = owner.get_core();

    uint8_t x = get_registry_x_index();

    if (x > 3)
        throw std::runtime_error{"Error in instruction FN01: plane value cannot be bigger than 3."};

    static_cast<xochip_display*>(&owner.get_display())->set_bitplane(x);
}

void xochip_instructions::OP_F002()
{
    audio& audio = owner.get_audio();
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint8_t* dst = &static_cast<xochip_audio*>(&audio)->get_audio_pattern_buffer();
    uint8_t* src = &memory.access_memory()[core.get_index_register()];

    memcpy(dst, src, 16 * sizeof(uint8_t));
}

void xochip_instructions::OP_FX3A()
{
    audio& audio = owner.get_audio();
    core& core = owner.get_core();

    uint8_t x = get_registry_x_index();

    static_cast<xochip_audio*>(&audio)->set_pitch(core.V(x));

}

void xochip_instructions::OP_00DN()
{
    display& display = owner.get_display();
    static_cast<xochip_display*>(&display)->scroll_up(get_n_value());
}

void xochip_instructions::OP_00CN()
{
    display& display = owner.get_display();
    static_cast<xochip_display*>(&display)->scroll_down(get_n_value());
}

void xochip_instructions::OP_00FB()
{
    display& display = owner.get_display();
    static_cast<xochip_display*>(&display)->scroll_right();
}

void xochip_instructions::OP_00FC()
{
    display& display = owner.get_display();
    static_cast<xochip_display*>(&display)->scroll_left();
}


void xochip_instructions::OP_DXY0()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    display& display = owner.get_display();
    quirks& quirks = owner.get_quirks();

    // Get sprite x and y coords
    uint8_t x_coord = core.get_registry_value(get_registry_x_index());
    uint8_t y_coord = core.get_registry_value(get_registry_y_index());
    if (!quirks.clipping)
    {
        x_coord %= 128;
        y_coord %= 64;
    }

    auto* xo_display = static_cast<xochip_display*>(&display);
    xochip_display::Bitplane active_bitplane = xo_display->get_active_bitplane();
    bool nullified = false;

    core.V(0xF) = 0;

    // Else fall back to the standard draw function
    for (int row = 0; row < 16; ++row)
    {
        //uint8_t sprite_data = memory[index_register + row];

        uint8_t first_byte = memory.access_memory()[core.get_index_register() + row * 2];
        uint8_t second_byte = memory.access_memory()[core.get_index_register() + row * 2 + 1];
        uint16_t sprite_data = (first_byte << 8) | second_byte;

        uint8_t first_byte1 = memory.access_memory()[core.get_index_register() + row * 2 + 32];
        uint8_t second_byte1 = memory.access_memory()[core.get_index_register() + row * 2 + 1 + 32];
        uint16_t sprite_data1 = (first_byte1 << 8) | second_byte1;

        uint8_t target_y = (y_coord + row) % 64;

        for (int i = 0; i < 16; ++i)
        {
           uint8_t target_x = (x_coord + i) % 128;

            bool is_sprite_bit_set = ((sprite_data >> (15 - i)) & 1) == 1;

            if (active_bitplane == xochip_display::Bitplane::FIRST)
            {
                if (is_sprite_bit_set)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::FIRST);
            }
            else if (active_bitplane == xochip_display::Bitplane::SECOND)
            {
                if (is_sprite_bit_set)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::SECOND);
            }
            else if (active_bitplane == xochip_display::Bitplane::BOTH)
            {
                bool is_sprite_bit_set1 = ((sprite_data1 >> (15 - i)) & 1) == 1;

                if (is_sprite_bit_set)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::FIRST);
                if (is_sprite_bit_set1)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::SECOND);
            }
        }
    }

    if (nullified)
        core.V(0xF) = 1;
}

void xochip_instructions::OP_DXYN()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    display& display = owner.get_display();
    quirks& quirks = owner.get_quirks();

    // Wrap x and y position for a sprite
    uint8_t screen_width = display.get_screen_width();
    uint8_t screen_height = display.get_screen_height();


    // Get sprite x and y coords
    uint8_t x_coord = core.get_registry_value(get_registry_x_index());
    uint8_t y_coord = core.get_registry_value(get_registry_y_index());
    if (!quirks.clipping)
    {
        x_coord %= screen_width;
        y_coord %= screen_height;
    }

    core.set_registry_value(0xF, 0);

    //bool big_sprite = get_n_value() == 0;
    //draw_sprite(big_sprite ? 2 : 1, big_sprite ? 16 : 8, big_sprite ? 16 : get_n_value());

    auto* xo_display = static_cast<xochip_display*>(&display);
    xochip_display::Bitplane active_bitplane = xo_display->get_active_bitplane();
    bool nullified = false;

    // Else fall back to the standard draw function
    for (int row = 0; row < get_n_value(); ++row)
    {
        //uint8_t sprite_data = memory[index_register + row];
        uint8_t sprite_data = memory.access_memory()[core.get_index_register() + row];
        uint8_t sprite_data1 = 0;

        // Only read the second block if BOTH planes are actually being drawn
        if (active_bitplane == xochip_display::Bitplane::BOTH)
        {
            sprite_data1 = memory.access_memory()[core.get_index_register() + row + get_n_value()];
        }

        uint8_t target_y = (y_coord + row) % screen_height;

        for (int i = 0; i < 8; ++i)
        {
            bool is_sprite_bit_set = ((sprite_data >> (7 - i)) & 1) == 1;
            bool is_sprite_bit_set1 = ((sprite_data1 >> (7 - i)) & 1) == 1;

            uint8_t target_x = (x_coord + i) % screen_width;

            if (is_sprite_bit_set && active_bitplane == xochip_display::Bitplane::FIRST)
                nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::FIRST);
            else if (is_sprite_bit_set && active_bitplane == xochip_display::Bitplane::SECOND)
                nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::SECOND);
            else if (active_bitplane == xochip_display::Bitplane::BOTH)
            {
                if (is_sprite_bit_set)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::FIRST);
                if (is_sprite_bit_set1)
                    nullified |= xo_display->invert_pixel(target_x, target_y, xochip_display::Bitplane::SECOND);
            }
        }
    }

    if (nullified)
        core.V(0xF) = 1;
}

void xochip_instructions::OP_FX1E()
{
    core& core = owner.get_core();

    core.set_index_register(core.get_index_register() + core.V(get_registry_x_index()));
}


void xochip_instructions::OP_3XNN()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    if (core.V(get_registry_x_index()) == get_nn_value())
    {
        if (next_opcode == 0xF000)
        {
            core.skip_next();
            core.skip_next();
        }
        else
            core.skip_next();
    }
}

void xochip_instructions::OP_4XNN()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    if (core.V(get_registry_x_index()) != get_nn_value())
    {
        if (next_opcode == 0xF000)
        {
            core.skip_next();
            core.skip_next();
        }
        else
            core.skip_next();
    }
}

void xochip_instructions::OP_5XY0()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    if (core.V(get_registry_x_index()) == core.V(get_registry_y_index()))
    {
        if (next_opcode == 0xF000)
        {
            core.skip_next();
            core.skip_next();
        }
        else
            core.skip_next();
    }
}

void xochip_instructions::OP_9XY0()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    if (core.V(get_registry_x_index()) != core.V(get_registry_y_index()))
    {
        if (next_opcode == 0xF000)
        {
            core.skip_next();
            core.skip_next();
        }
        else
            core.skip_next();
    }

}

void xochip_instructions::OP_EX9E()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    uint8_t key = core.V(get_registry_x_index());

    if (key <= 0xF)
    {
        if (owner.is_key_pressed(key))
        {
            if (next_opcode == 0xF000)
            {
                core.skip_next();
                core.skip_next();
            }
            else
            {
                core.skip_next();
            }
        }
    }
}

void xochip_instructions::OP_EXA1()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t next_opcode = memory.get_opcode(core.get_pc());

    uint8_t key = core.V(get_registry_x_index());

    if (key <= 0xF)
    {
        if (!owner.is_key_pressed(key))
        {
            if (next_opcode == 0xF000)
            {
                core.skip_next();
                core.skip_next();
            }
            else
            {
                core.skip_next();
            }
        }
    }
}
