#pragma once

class memory
{
public:
   memory() = default;
   virtual ~memory() = 0;

   [[nodiscard]] virtual uint8_t* access_memory() = 0;
   [[nodiscard]] virtual uint16_t get_opcode(uint16_t pc) = 0;
   [[nodiscard]] virtual uint32_t get_size() = 0;
private:

};

inline memory::~memory()
{
}
