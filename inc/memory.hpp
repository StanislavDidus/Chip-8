#pragma once

class memory
{
public:
   memory() = default;
   virtual ~memory() = 0;

   [[nodiscard]] virtual uint8_t* access_memory() = 0;
private:

};

inline memory::~memory()
{
}
