#include "instructions.hpp"
#include "chip8.hpp"

instructions::instructions(chip8& chip8)
    : owner(chip8)
{
    init_table();
}

instructions::~instructions()
{
}

void instructions::execute_instruction()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t pc = core.get_pc();

    uint8_t first_byte = memory.access_memory()[pc];
    uint8_t second_byte = memory.access_memory()[pc + 1];
    opcode = (first_byte << 8) | second_byte;

    core.skip_next();

    std::cout << "Executing instruction: " << std::hex << opcode << std::endl;

    uint8_t last_byte = (opcode & 0xF000) >> 12;

    try
    {
        table.at(last_byte)();
    }
    catch (std::exception&)
    {
        std::cerr << "Unknown exception: " << std::hex << opcode << std::endl;
    }
}

void instructions::init_table()
{
    table[0x0] = [this] { Table_0(); };
    table[0x1] = [this]{ OP_1NNN(); };
    table[0x3] = [this]{ OP_3XNN(); };
    table[0x2] = [this]{ OP_2NNN(); };
    table[0x6] = [this]{ OP_6XNN(); };
    table[0x7] = [this]{ OP_7XNN(); };
    table[0xA] = [this]{ OP_ANNN(); };
    table[0xD] = [this]{ OP_DXYN(); };
    table[0x4] = [this]{ OP_4XNN(); };
    table[0x5] = [this]{ OP_5XY0(); };
    table[0x9] = [this]{ OP_9XY0(); };
    table[0x8] = [this]{ Table_8(); };
    table[0xB] = [this]{ OP_BNNN(); };
    table[0xC] = [this]{ OP_CXNN(); };
    table[0xE] = [this]{ Table_E(); };
    table[0xF] = [this]{ Table_F(); };

    table_0[0x0] = [this]{OP_00E0();};
    table_0[0xE] = [this]{OP_00EE();};

    table_8[0x0] = [this]{OP_8XY0();};
    table_8[0x1] = [this]{OP_8XY1();};
    table_8[0x2] = [this]{OP_8XY2();};
    table_8[0x3] = [this]{OP_8XY3();};
    table_8[0x4] = [this]{OP_8XY4();};
    table_8[0x5] = [this]{OP_8XY5();};
    table_8[0x7] = [this]{OP_8XY7();};
    table_8[0x6] = [this]{OP_8XY6();};
    table_8[0xE] = [this]{OP_8XYE();};

    table_E[0xE] = [this]{OP_EX9E();};
    table_E[0x1] = [this]{OP_EXA1();};

    table_F[0x07] = [this]{OP_FX07();};
    table_F[0x15] = [this]{OP_FX15();};
    table_F[0x18] = [this]{OP_FX18();};
    table_F[0x1E] = [this]{OP_FX1E();};
    table_F[0x0A] = [this]{OP_FX0A();};
    table_F[0x29] = [this]{OP_FX29();};
    table_F[0x33] = [this]{OP_FX33();};
    table_F[0x55] = [this]{OP_FX55();};
    table_F[0x65] = [this]{OP_FX65();};
}

void instructions::Table_0()
{
    uint8_t first_byte = opcode & 0x000F;
    table_0.at(first_byte)();
}

void instructions::Table_8()
{
    uint8_t first_byte = opcode & 0x000F;
    table_8.at(first_byte)();
}

void instructions::Table_E()
{
    uint8_t first_byte = opcode & 0x000F;
    table_E.at(first_byte)();
}

void instructions::Table_F()
{
    uint8_t nn = opcode & 0x00FF;
    table_F.at(nn)();
}

void instructions::OP_00E0()
{
    for (int i = 0; i < 64 * 32; ++i) owner.get_display().clear();
}

void instructions::OP_00EE()
{
    core& core = owner.get_core();
    core.stack_pop();
}

void instructions::OP_1NNN()
{
    core& core = owner.get_core();
    core.set_pc(get_nnn_address());
}

void instructions::OP_2NNN()
{
    core& core = owner.get_core();
    core.stack_push();
    core.set_pc(get_nnn_address());
}

void instructions::OP_6XNN()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) = get_nn_value();
}

void instructions::OP_7XNN()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) += get_nn_value();
}

void instructions::OP_ANNN()
{
    core& core = owner.get_core();
    core.set_index_register(get_nnn_address());
}

void instructions::OP_DXYN()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    display& display = owner.get_display();

    // Wrap x and y position for a sprite
    uint8_t x_coord = core.get_registry_value(get_registry_x_index()) % 64;
    uint8_t y_coord = core.get_registry_value(get_registry_y_index()) % 32;

    core.set_registry_value(0xF, 0);

    for (int row = 0; row < get_n_value(); ++row)
    {
        //uint8_t sprite_data = memory[index_register + row];
        uint8_t sprite_data = memory.access_memory()[core.get_index_register() + row];

        uint8_t target_y = y_coord + row;

        if (target_y >= 32) break;

        for (int i = 0; i < 8; ++i)
        {
            bool is_sprite_bit_set = ((sprite_data >> (7 - i)) & 1) == 1;

            uint8_t target_x = x_coord + i;

            if (target_x >= 64) break;

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

void instructions::OP_3XNN()
{
    core& core = owner.get_core();
    if (core.V(get_registry_x_index()) == get_nn_value())
        core.skip_next();
}

void instructions::OP_4XNN()
{
    core& core = owner.get_core();
    if (core.V(get_registry_x_index()) != get_nn_value())
        core.skip_next();
}

void instructions::OP_5XY0()
{
    core& core = owner.get_core();
    if (core.V(get_registry_x_index()) == core.V(get_registry_y_index()))
        core.skip_next();
}

void instructions::OP_9XY0()
{
    core& core = owner.get_core();
    if (core.V(get_registry_x_index()) != core.V(get_registry_y_index()))
        core.skip_next();
}

void instructions::OP_8XY0()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) = core.V(get_registry_y_index());
}

void instructions::OP_8XY1()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) |= core.V(get_registry_y_index());
}

void instructions::OP_8XY2()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) &= core.V(get_registry_y_index());
}

void instructions::OP_8XY3()
{
    core& core = owner.get_core();
    core.V(get_registry_x_index()) ^= core.V(get_registry_y_index());
}

void instructions::OP_8XY4()
{
    core& core = owner.get_core();

    uint8_t a = core.V(get_registry_x_index());
    uint8_t b = core.V(get_registry_y_index());

    core.V(get_registry_x_index()) = a + b;

    // Check for overflow
    if (a > 0 && b > 0 && core.V(get_registry_x_index()) < 0)
        core.V(0xF) = 1;


}

void instructions::OP_8XY5()
{
    core& core = owner.get_core();
    uint8_t a = core.V(get_registry_x_index());
    uint8_t b = core.V(get_registry_y_index());

    core.V(get_registry_x_index()) = a - b;

    if (a >= b)
        core.V(0xF) = 1;
    else if (b > a)
        core.V(0xF) = 0;
}

void instructions::OP_8XY7()
{
    core& core = owner.get_core();
    uint8_t a = core.V(get_registry_x_index());
    uint8_t b = core.V(get_registry_y_index());

    core.V(get_registry_x_index()) = b - a;

    if (b >= a)
        core.V(0xF) = 1;
    else if (a > b)
        core.V(0xF) = 0;
}

void instructions::OP_8XY6()
{
    /*if constexpr (!CHIP48_IMPLEMENTATION)
    {
        registry[x] = registry[y];
    }*/

    uint8_t x = get_registry_x_index();
    core& core = owner.get_core();

    uint8_t removed_bit = core.V(x) & 1;
    core.V(x) = core.V(x) >> 1;

    core.V(0xF) = removed_bit;
}

void instructions::OP_8XYE()
{
    core& core = owner.get_core();
    uint8_t x = get_registry_x_index();
    uint8_t y = get_registry_y_index();

    /*
    if constexpr (!CHIP48_IMPLEMENTATION)
    {
        registry[x] = registry[y];
    }
    */

    uint8_t removed_bit = core.V(x) >> 7;
    core.V(x) = core.V(x) << 1;

    core.V(0xF) = removed_bit;
}

void instructions::OP_BNNN()
{
    core& core = owner.get_core();

    /*
    if constexpr (!CHIP48_IMPLEMENTATION)
    {
        program_counter = nnn + registry[0x0];
    }
    */

    core.set_pc(get_nnn_address() + core.V(get_registry_x_index()));
}


// TODO: Move it from here
std::random_device rng{};
std::uniform_int_distribution<int> dist{0, 255};

void instructions::OP_CXNN()
{
    core& core = owner.get_core();

    auto rand = static_cast<uint8_t>(dist(rng));

    core.V(get_registry_x_index()) = rand & get_nn_value();
}

void instructions::OP_EX9E()
{
    core& core = owner.get_core();

    uint8_t key = core.V(get_registry_x_index());

    if (key <= 0xF)
        if (owner.is_key_pressed(key))
            core.skip_next();
}

void instructions::OP_EXA1()
{
    core& core = owner.get_core();

    uint8_t key = core.V(get_registry_x_index());

    if (key <= 0xF)
        if (!owner.is_key_pressed(key))
            core.skip_next();
}

void instructions::OP_FX07()
{
    uint8_t x = (opcode & 0x0F00) >> 8;

    core& core = owner.get_core();
    core.V(get_registry_x_index()) = core.get_delay_timer_value();
}

void instructions::OP_FX15()
{
    core& core = owner.get_core();
    core.set_delay_timer(core.V(get_registry_x_index()));
}

void instructions::OP_FX18()
{
    core& core = owner.get_core();
    core.set_sound_timer(core.V(get_registry_x_index()));
}

void instructions::OP_FX1E()
{
    core& core = owner.get_core();

    uint16_t a = core.get_index_register();
    uint8_t b = core.V(get_registry_x_index());

    core.set_index_register(a + b);

    //core.V(0xF) = 0;

    if (a > 0 && b > 0 && core.get_index_register() < 0)
        core.V(0xF) = 1;
}

void instructions::OP_FX0A()
{
    core& core = owner.get_core();
    for (int i = 0; i < 16; ++i)
    {
        if (owner.is_key_pressed(i))
        {
           core.V(get_registry_x_index()) = i;
            return;
        }
    }

    core.return_back();
}

void instructions::OP_FX29()
{
    core& core = owner.get_core();
    uint8_t last_nibble = core.V(get_registry_x_index()) & 0x0F;
    core.set_index_register(LOW_RES_FONT_MEMORY_LOCATION + 5 * last_nibble);
}

void instructions::OP_FX33()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();

    uint16_t I = core.get_index_register();
    uint8_t number = core.V(get_registry_x_index());

    memory.access_memory()[I] = number / 100;
    memory.access_memory()[I + 1] = number % 100 / 10;
    memory.access_memory()[I + 2] = number % 10;
}

void instructions::OP_FX55()
{
    /*
    if constexpr (!CHIP48_IMPLEMENTATION)
        index_register += x;
        */

    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    for (int i = 0; i <= get_registry_x_index(); ++i)
    {
       memory.access_memory()[core.get_index_register() + i] = core.V(i);
    }
}

void instructions::OP_FX65()
{
    /*
    if constexpr (!CHIP48_IMPLEMENTATION)
        index_register += x;
        */

    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    for (int i = 0; i <= get_registry_x_index(); ++i)
    {
        core.V(i) = memory.access_memory()[core.get_index_register() + i];
    }
}
