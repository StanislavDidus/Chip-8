#include "../../inc/Audio/audio.hpp"

#include <algorithm>

audio::audio()
{
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    spec.freq = 44100;

    audio_stream = SDL_CreateAudioStream(&spec, &spec);
    if (!audio_stream)
        std::cerr << "Could not open audio device stream: " << SDL_GetError() << std::endl;
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    SDL_BindAudioStream(device, audio_stream);
    SDL_ResumeAudioDevice(device);
}

audio::~audio()
{
    SDL_CloseAudioDevice(device);
}

void audio::set_volume(float value)
{
    float value_clamped = std::clamp(value, 0.0f, 1.0f);
    volume = value_clamped * 0.2f;
}
