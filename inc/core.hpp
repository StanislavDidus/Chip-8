#pragma once
#include <cstdint>

#include "config.hpp"

class core
{
public:
    core() = default;
    ~core() = default; // Non virtual destructor!

    // Setters
    void set_pc(uint16_t value) { program_counter = value; }
    void increase_pc(uint16_t value) { program_counter += value; }
    void skip_next() { program_counter += 2; }
    void return_back() { program_counter -= 2; }

    void set_stack_pointer(uint16_t value) { stack_pointer = value; }
    void stack_pop() { program_counter = stack[--stack_pointer]; }
    void stack_push() { stack[stack_pointer++] = program_counter;  }

    void decrease_stack_pointer() { --stack_pointer; }
    void set_index_register(uint16_t value) { index_register = value; }
    void set_stack_value(uint8_t index, uint16_t value) { stack[index] = value; }

    void set_registry_value(uint8_t index, uint16_t value) { registry[index] = value; }
    void increase_registry_value(uint8_t index, uint16_t value) { registry[index] += value; }
    void set_registry_value_to_registry_value(uint8_t x, uint8_t y) { registry[x] = registry[y]; }
    void set_registry_value_with_OR(uint8_t x, uint8_t y) { registry[x] = registry[x] | registry[y]; };
    void set_registry_value_with_AND(uint8_t x, uint8_t y) { registry[x] = registry[x] & registry[y]; };

    void set_delay_timer(uint8_t value) { delay_timer = value; }
    void decrease_delay_timer() { --delay_timer; }
    void set_sound_timer(uint8_t value) { sound_timer = value; }
    void decrease_sound_timer() { --sound_timer; }

    // Getter
    [[nodiscard]] uint16_t get_pc() const { return program_counter; }
    [[nodiscard]] uint16_t get_stack_pointer() const { return stack_pointer; }
    [[nodiscard]] uint16_t get_index_register() const { return index_register; }
    [[nodiscard]] uint16_t get_stack_value(uint8_t index) const { return stack[index]; }
    [[nodiscard]] uint16_t get_registry_value(uint8_t index) const { return registry[index]; }
    [[nodiscard]] uint8_t get_delay_timer_value() const { return delay_timer; }
    [[nodiscard]] uint8_t get_sound_timer_value() const { return sound_timer; }

    // Return registry value Vx
    [[nodiscard]] uint8_t& V(uint8_t x) { return registry[x]; }
private:
    uint16_t program_counter { STARTING_POINT };
    uint16_t stack_pointer {};
    uint16_t index_register {};
    uint16_t stack[16] {};
    uint8_t registry[16] {};
    uint8_t delay_timer {};
    uint8_t sound_timer {};
};
