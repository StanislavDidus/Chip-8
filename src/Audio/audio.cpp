#include "Audio/audio.hpp"

audio::audio()
{
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = 44100;

    audio_stream = SDL_CreateAudioStream(&spec, &spec);
    if (!audio_stream)
        std::cerr << "Could not open audio device stream: " << SDL_GetError() << std::endl;
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    SDL_BindAudioStream(device, audio_stream);
    SDL_ResumeAudioDevice(device);
}

audio::~audio()
{
    SDL_CloseAudioDevice(device);
}

void audio::play_sound()
{
    float buffer[1024];
    for (int i = 0; i < 1024; ++i)
    {
        if (phase < 0.5f)
        {
            buffer[i] = amplitude;
        }
        else
        {
            buffer[i] = -amplitude;
        }

        phase += frequency / sample_rate;

        if (phase >= 1.0f)
            phase = -1.0f;
    }

    SDL_PutAudioStreamData(audio_stream, buffer, sizeof(buffer));
}
