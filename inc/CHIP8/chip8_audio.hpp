#pragma once
#include "audio.hpp"

class chip8_audio : public audio
{
public:
    chip8_audio() = default;
    ~chip8_audio() override = default;

    void play_sound() override;
private:
    float phase = 0.0f;
    float frequency = 400.0f;
    float sample_rate = 44100.0f;
};