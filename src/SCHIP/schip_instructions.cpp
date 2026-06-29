#include "SCHIP/schip_instructions.hpp"

#include <fstream>
#include <memory>

#include "chip8.hpp"
#include "SCHIP/schip_display.hpp"

schip_instructions::schip_instructions(chip8& chip8)
    : instructions(chip8)
{
    init_table();
}

void schip_instructions::init_table()
{
    table[0x0] = [this]{ return Table_0(); };
    table[0xD] = [this]
    {
        return [this]
        {
            if (get_n_value() == 0 && static_cast<schip_display*>(&owner.get_display())->is_high_resolution())
                OP_DXY0();
            else
                OP_DXYN();
        };
    };

    table_0.clear();

    table_0[0xE0] = [this] { OP_00E0(); };
    table_0[0xEE] = [this] { OP_00EE(); };
    table_0[0xFF] = [this] { OP_00FF(); };
    table_0[0xFE] = [this] { OP_00FE(); };
    table_0[0xFB] = [this] { OP_00FB(); };
    table_0[0xFC] = [this] { OP_00FC(); };
    table_0[0xFD] = [this] { OP_00FD(); };

    table_F[0x30] = [this] { OP_FX30(); };
    table_F[0x75] = [this] { OP_FX75(); };
    table_F[0x85] = [this] { OP_FX85(); };
}

instruction schip_instructions::Table_0()
{
    uint8_t second_byte = (opcode & 0x00F0) >> 4;
    if (second_byte == 0xC)
    {
        return [this]{ OP_00CN(); };
    }

    return table_0.at(get_nn_value());
}

void schip_instructions::OP_00E0()
{
    for (int i = 0; i < 64 * 32; ++i) owner.get_display().clear();
}

void schip_instructions::OP_00EE()
{
    core& core = owner.get_core();
    core.stack_pop();
}

void schip_instructions::OP_00FF()
{
    static_cast<schip_display*>(&owner.get_display())->enable_high_resolution_scree();
}

void schip_instructions::OP_00FE()
{
    static_cast<schip_display*>(&owner.get_display())->disable_high_resolution_scree();
}

void schip_instructions::OP_00CN()
{
    static_cast<schip_display*>(&owner.get_display())->scroll_down(get_n_value());
}

void schip_instructions::OP_00FB()
{
    static_cast<schip_display*>(&owner.get_display())->scroll_right();
}

void schip_instructions::OP_00FC()
{
    static_cast<schip_display*>(&owner.get_display())->scroll_left();
}

void schip_instructions::OP_DXY0()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    display& display = owner.get_display();

    // Wrap x and y position for a sprite
    uint8_t x_coord = core.get_registry_value(get_registry_x_index()) % 128;
    uint8_t y_coord = core.get_registry_value(get_registry_y_index()) % 64;

    core.set_registry_value(0xF, 0);

    // Else fall back to the standard draw function
    for (int row = 0; row < 16; ++row)
    {
        //uint8_t sprite_data = memory[index_register + row];
        uint8_t first_byte = memory.access_memory()[core.get_index_register() + row * 2];
        uint8_t second_byte = memory.access_memory()[core.get_index_register() + row * 2 + 1];
        uint16_t sprite_data = (first_byte << 8) | second_byte;

        uint8_t target_y = y_coord + row;

        if (target_y >= 64) break;

        for (int i = 0; i < 16; ++i)
        {
            bool is_sprite_bit_set = ((sprite_data >> (15 - i)) & 1) == 1;

            uint8_t target_x = x_coord + i;

            if (target_x >= 128) break;

            uint8_t& screen_pixel = display.get_pixel(target_x, target_y);

            if (is_sprite_bit_set && screen_pixel)
            {
                screen_pixel = 0;
                core.set_registry_value(0xF, 1);
            }
            else if (is_sprite_bit_set && !screen_pixel)
            {
                screen_pixel = 1;
            }
        }
    }
}

void schip_instructions::OP_DXYN()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    display& display = owner.get_display();

    // Wrap x and y position for a sprite
    uint8_t x_coord = core.get_registry_value(get_registry_x_index()) % 128;
    uint8_t y_coord = core.get_registry_value(get_registry_y_index()) % 64;

    core.set_registry_value(0xF, 0);

    // Else fall back to the standard draw function
    for (int row = 0; row < get_n_value(); ++row)
    {
        //uint8_t sprite_data = memory[index_register + row];
        uint8_t sprite_data = memory.access_memory()[core.get_index_register() + row];

        uint8_t target_y = y_coord + row;

        if (target_y >= 64) break;

        for (int i = 0; i < 8; ++i)
        {
            bool is_sprite_bit_set = ((sprite_data >> (7 - i)) & 1) == 1;

            uint8_t target_x = x_coord + i;

            if (target_x >= 128) break;

            uint8_t& screen_pixel = display.get_pixel(target_x, target_y);

            if (is_sprite_bit_set && screen_pixel)
            {
                screen_pixel = 0;
                core.set_registry_value(0xF, 1);
            }
            else if (is_sprite_bit_set && !screen_pixel)
            {
                screen_pixel = 1;
            }
        }
    }
}

void schip_instructions::OP_FX30()
{
    core& core = owner.get_core();

    uint8_t last_nibble = core.V(get_registry_x_index()) & 0x0F;
    core.set_index_register(HIGH_RES_FONT_MEMORY_LOCATION + 10 * last_nibble);
}

void schip_instructions::OP_FX75()
{
    uint8_t x = get_registry_x_index();

    core& core = owner.get_core();

    if (x > 7) return;

    std::string full_file_path = GAMESAVES_LOCATION + owner.get_rom_name() + ".sav";
    std::ofstream save(full_file_path, std::ios::binary);
    if (!save)
    {
        std::cerr << "Could not open " << full_file_path << " for writing." << std::endl;
        return;
    }

    save.write(reinterpret_cast<const char*>(x), sizeof(x));

    uint8_t* reg = &core.V(0);
    save.write(reinterpret_cast<const char*>(reg), x * sizeof(uint8_t));

    if (save.bad())
    {
        std::cerr << "Failed writing data to .sav file." << std::endl;
        return;
    }

    save.close();
}

void schip_instructions::OP_FX85()
{
    uint8_t x = get_registry_x_index();

    core& core = owner.get_core();

    if (x > 7) return;

    std::string full_file_path = GAMESAVES_LOCATION + owner.get_rom_name() + ".sav";
    std::ifstream save(full_file_path, std::ios::binary);
    if (!save)
    {
        std::cerr << "Could not open " << full_file_path << " for reading." << std::endl;
        return;
    }

    uint8_t* reg = &core.V(0);
    save.read(reinterpret_cast<char*>(reg), x * sizeof(uint8_t));

    if (save.bad())
    {
        std::cerr << "Failed reading data to .sav file." << std::endl;
        return;
    }

    save.close();
}

void schip_instructions::OP_00FD()
{
    std::exit(0);
}
