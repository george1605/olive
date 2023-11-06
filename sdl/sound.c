#ifndef __SDL_SOUND__
#define __SDL_SOUND__

#include <SDL2/SDL.h>
#include <math.h>
#include <stdint.h>

SDL_AudioDeviceID ol_getaudio_sdl(int sample_rate, int channels, SDL_AudioCallback callback)
{
    SDL_AudioSpec audioSpec;
    audioSpec.freq = sample_rate;
    audioSpec.format = AUDIO_S16;
    audioSpec.channels = channels;
    audioSpec.samples = 4096;
    audioSpec.callback = callback;
    return SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
}

#endif