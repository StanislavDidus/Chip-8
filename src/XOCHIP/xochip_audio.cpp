#include "XOCHIP/xochip_audio.hpp"

void xochip_audio::play_sound()
{
    float buffer[128];
    int sample_index = 0;

    for (int i = 0; i < 16; ++i)
    {
        for (int bit = 7; bit >= 0; --bit)
        {
            bool is_bit_on = (audio_pattern_buffer[i] >> bit) & 1;

            buffer[sample_index++] = is_bit_on ? amplitude : -amplitude;
        }
    }

    SDL_PutAudioStreamData(audio_stream, buffer, sizeof(buffer));
}

void xochip_audio::set_pitch(uint8_t value)
{
    pitch = std::pow(4000.0f * 2.0f, (static_cast<float>(value) - 64.0f) / 48.0f);

    SDL_AudioSpec src_spec = {SDL_AUDIO_F32, 1, static_cast<int>(pitch)};
    SDL_SetAudioStreamFormat(audio_stream, &src_spec, nullptr);

}
