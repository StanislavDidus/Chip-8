#include "CHIP8/chip8_instructions.hpp"

#include "chip8.hpp"

chip8_instructions::chip8_instructions(chip8& chip8)
    : instructions(chip8)
{
    init_table();
}

void chip8_instructions::init_table()
{
    table[0xB] = [this]{ return [this] { OP_BNNN();}; };

    table_8[0x6] = [this]{ OP_8XY6(); };
    table_8[0xE] = [this]{ OP_8XYE(); };

    table_F[0x55] = [this]{ OP_FX55(); };
    table_F[0x65] = [this]{ OP_FX65(); };
}

void chip8_instructions::OP_8XY6()
{
    uint8_t x = get_registry_x_index();
    uint8_t y = get_registry_y_index();
    core& core = owner.get_core();

    core.V(x) = core.V(y);
    uint8_t removed_bit = core.V(x) & 1;
    core.V(x) = core.V(x) >> 1;

    core.V(0xF) = removed_bit;
}

void chip8_instructions::OP_8XYE()
{
    core& core = owner.get_core();
    uint8_t x = get_registry_x_index();
    uint8_t y = get_registry_y_index();

    core.V(x) = core.V(y);
    uint8_t removed_bit = core.V(x) >> 7;
    core.V(x) = core.V(x) << 1;

    core.V(0xF) = removed_bit;
}

void chip8_instructions::OP_BNNN()
{
    core& core = owner.get_core();
    core.set_pc(get_nnn_address() + core.V(0x0));
}

void chip8_instructions::OP_FX55()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    for (int i = 0; i <= get_registry_x_index(); ++i)
    {
        memory.access_memory()[core.get_index_register() + i] = core.V(i);
    }

    core.set_index_register(core.get_index_register() + get_registry_x_index() + 1);
}

void chip8_instructions::OP_FX65()
{
    core& core = owner.get_core();
    memory& memory = owner.get_memory();
    for (int i = 0; i <= get_registry_x_index(); ++i)
    {
        core.V(i) = memory.access_memory()[core.get_index_register() + i];
    }

    core.set_index_register(core.get_index_register() + get_registry_x_index() + 1);
}
