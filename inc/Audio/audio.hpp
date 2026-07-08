#pragma once
#include <iostream>
#include <cmath>

#include "SDL3/SDL_audio.h"

static float square_wave(float input)
{
    return input  < 0.5f ? 1.0f : 0.0f;
}

static float sin_wave(float input)
{
    return (std::sin(input * 2.0f * SDL_PI_F) + 1) / 2.0f;
}

class audio
{
public:
    audio();
    virtual ~audio() = 0;

    virtual void play_sound() = 0;

    // Setters
    /// Accepts values ranging from 0 to 1.
    void set_volume(float value);
protected:
    SDL_AudioStream* audio_stream = nullptr;
    SDL_AudioDeviceID device = 0;

    float volume = 1.0f;
};
