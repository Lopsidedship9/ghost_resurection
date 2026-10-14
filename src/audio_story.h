#ifndef AUDIO_STORY_H
#define AUDIO_STORY_H

// Carga el WAV en memoria. Se llama una vez, por ejemplo al iniciar el juego.
bool loadStoryAudio(const char* path);

// Inicia o reinicia la reproduccion del audio ya cargado.
bool playStoryAudio();

// Indica si la narracion sigue reproduciendose.
bool isStoryAudioPlaying();

// Detiene la reproduccion, pero conserva el audio cargado en memoria.
void stopStoryAudio();

// Libera el audio cargado y cierra el dispositivo.
void unloadStoryAudio();

#endif // AUDIO_STORY_H
