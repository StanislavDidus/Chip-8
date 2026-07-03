#pragma once
#include <functional>
#include <unordered_map>
#include <cstdint>

class chip8;

using instruction = std::function<void()>;
using instruction_wrapper = std::function<instruction()>;

using instruction_table = std::unordered_map<uint8_t, instruction>;
using instruction_table_wrapper = std::unordered_map<uint8_t, instruction_wrapper>;
class instructions
{
public:
    instructions(chip8& chip8);
    virtual ~instructions() = 0;

    std::function<void()> decode(uint16_t opcode);

protected:
    instruction_table_wrapper table;
    instruction_table table_0;
    instruction_table table_8;
    instruction_table table_E;
    instruction_table table_F;

    [[nodiscard]] uint16_t get_nnn_address() const { return opcode & 0x0FFF; }
    [[nodiscard]] uint8_t get_nn_value() const { return opcode & 0x00FF; }
    [[nodiscard]] uint8_t get_n_value() const { return opcode & 0x000F; }
    [[nodiscard]] uint8_t get_registry_x_index() const { return (opcode & 0x0F00) >> 8; }
    [[nodiscard]] uint8_t get_registry_y_index() const { return (opcode & 0x00F0) >> 4; }
    [[nodiscard]] uint8_t get_registry_xy_index() const { return (opcode & 0xFF0) >> 4; }

    uint16_t opcode {};
    chip8& owner;
private:
    void init_table();

    // Instructions
    void OP_00E0(); // Clear screen
    void OP_00EE(); // Call
    void OP_1NNN(); // Jump
    void OP_2NNN(); // Return
    void OP_6XNN(); // Set
    void OP_7XNN(); // Add
    void OP_ANNN(); // Set index
    void OP_DXYN(); // Draw

    // Skip conditionally
    void OP_3XNN(); // Skip if equal
    void OP_4XNN(); // Skip if not equal
    void OP_5XY0(); // Skip if equal
    void OP_9XY0(); // Skip if not equal

    // Logical and Arithmetic instructions
    void OP_8XY0(); // Set
    void OP_8XY1(); // Binary OR
    void OP_8XY2(); // Binary AND
    void OP_8XY3(); // Logical XOR
    void OP_8XY4(); // Add
    void OP_8XY5(); // Subtract x - y
    void OP_8XY7(); // Subtract y - x
    void OP_8XY6(); // Shift right
    virtual void OP_8XYE(); // Shift left

    void OP_BNNN(); // Jump with offset
    void OP_CXNN(); // Random
    void OP_EX9E(); // Skip if key
    void OP_EXA1(); // Skip if key

    // Timers
    void OP_FX07(); // Read delay timer
    void OP_FX15(); // Set delay timer
    void OP_FX18(); // set sound timer

    void OP_FX1E(); // Add to index
    void OP_FX0A(); // Get key
    void OP_FX29(); // Font character
    void OP_FX33(); // Binary-coded decimal conversion
    void OP_FX55(); // Store memory
    void OP_FX65(); // Load memory

    instruction Table_0();
    instruction Table_8();
    instruction Table_E();
    instruction Table_F();
};
