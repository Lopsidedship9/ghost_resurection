#include "audio_story.h"
#include "includes.h"

#include <iostream>

namespace
{
    SDL_AudioDeviceID story_audio_device = 0;
    SDL_AudioSpec story_audio_spec = {};
    Uint8* story_audio_buffer = nullptr;
    Uint32 story_audio_length = 0;
    bool story_audio_playing = false;
}

bool loadStoryAudio(const char* path)
{
    // Sustituir una carga anterior, si existe.
    unloadStoryAudio();

    if (SDL_LoadWAV(path, &story_audio_spec, &story_audio_buffer, &story_audio_length) == nullptr)
    {
        story_audio_buffer = nullptr;
        story_audio_length = 0;
        std::cerr << "No se pudo cargar " << path << ": " << SDL_GetError() << std::endl;
        return false;
    }

    // La reproduccion se hara mediante SDL_QueueAudio, sin callback.
    story_audio_spec.callback = nullptr;
    story_audio_spec.userdata = nullptr;
    return true;
}

bool playStoryAudio()
{
    if (story_audio_buffer == nullptr || story_audio_length == 0)
    {
        std::cerr << "No hay narracion cargada en memoria." << std::endl;
        return false;
    }

    if (story_audio_device == 0)
    {
        story_audio_device = SDL_OpenAudioDevice(nullptr, 0, &story_audio_spec, nullptr, 0);
        if (story_audio_device == 0)
        {
            std::cerr << "No se pudo abrir el dispositivo de audio: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    // Permite volver a reproducir desde el principio.
    SDL_ClearQueuedAudio(story_audio_device);

    if (SDL_QueueAudio(story_audio_device, story_audio_buffer, story_audio_length) < 0)
    {
        std::cerr << "No se pudo poner la narracion en la cola: " << SDL_GetError() << std::endl;
        story_audio_playing = false;
        return false;
    }

    story_audio_playing = true;
    SDL_PauseAudioDevice(story_audio_device, 0);
    return true;
}

bool isStoryAudioPlaying()
{
    if (!story_audio_playing || story_audio_device == 0)
        return false;

    if (SDL_GetQueuedAudioSize(story_audio_device) > 0)
        return true;

    // La cola esta vacia: la narracion ha terminado, pero se conserva en memoria.
    SDL_PauseAudioDevice(story_audio_device, 1);
    story_audio_playing = false;
    return false;
}

void stopStoryAudio()
{
    story_audio_playing = false;
    if (story_audio_device != 0)
    {
        SDL_PauseAudioDevice(story_audio_device, 1);
        SDL_ClearQueuedAudio(story_audio_device);
    }
}

void unloadStoryAudio()
{
    stopStoryAudio();

    if (story_audio_device != 0)
    {
        SDL_CloseAudioDevice(story_audio_device);
        story_audio_device = 0;
    }

    if (story_audio_buffer != nullptr)
    {
        SDL_FreeWAV(story_audio_buffer);
        story_audio_buffer = nullptr;
    }

    story_audio_length = 0;
    story_audio_spec = {};
}
