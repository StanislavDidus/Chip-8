#pragma once
#include <iostream>

#include "SDL3/SDL_audio.h"

class audio
{
public:
    audio();
    ~audio();

    void play_sound();
private:
    SDL_AudioStream* audio_stream = nullptr;
    SDL_AudioDeviceID device;

    // Sound data
    float phase = 0.0f;
    float frequency = 440.0f;
    float sample_rate = 44100.0f;
    float amplitude = 0.1f;
};
