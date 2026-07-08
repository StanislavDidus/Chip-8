#include "CHIP8/chip8_audio.hpp"

void chip8_audio::play_sound()
{
    float buffer[1024];
    for (int i = 0; i < 1024; ++i)
    {
        buffer[i] = sin_wave(phase) * volume;

        phase += frequency / sample_rate;

        if (phase >= 1.0f)
            phase = 0.0f;
    }

    SDL_PutAudioStreamData(audio_stream, buffer, sizeof(buffer));
}
