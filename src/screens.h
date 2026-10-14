#ifndef SCREENS_H
#define SCREENS_H

#include "includes.h"
#include "image.h"

enum StartMenuAction
{
    START_MENU_NONE,
    START_MENU_PLAY,
    START_MENU_EXIT
};

// selectedOption: 0 = JUGAR, 1 = SALIR.
void renderStartMenu(Image& framebuffer,
                    const Image& playImage,
                    const Image& exitImage,
                    int selectedOption);

// Flechas: cambian la seleccion. X: confirma la opcion.
StartMenuAction handleStartMenuKey(SDL_Keycode key, int& selectedOption);

#endif // SCREENS_H
