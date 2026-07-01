#pragma once
#include "audio.hpp"

class xochip_audio : public audio
{
public:
    xochip_audio() = default;
    ~xochip_audio() override = default;

    void play_sound() override;

    // Setters
    void set_pitch(uint8_t value);

    // Getters
    uint8_t& get_audio_pattern_buffer() { return *audio_pattern_buffer; }
private:
    uint8_t audio_pattern_buffer[16] =
        {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
         0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};

    float pitch = 44100.0f;
};
