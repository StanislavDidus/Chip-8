#pragma once

class quirks
{
public:
    quirks() = default;
    virtual ~quirks() = 0;

protected:
    bool shift = true;
    bool jump_with_offset = true;
    bool add_to_index = true;
    bool load_store_memory = false;
};

inline quirks::~quirks()
{
}
