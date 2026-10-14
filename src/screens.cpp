
#include "screens.h"

void renderStartMenu(Image& framebuffer,
    const Image& playImage,
    const Image& exitImage,
    int selectedOption)
{
    const Image& selectedImage =
        (selectedOption == 1) ? exitImage : playImage;

    if (selectedImage.pixels == nullptr ||
        selectedImage.width != framebuffer.width ||
        selectedImage.height != framebuffer.height)
    {
        framebuffer.fill(Color(5, 13, 28));
        return;
    }

    framebuffer.drawImage(selectedImage, 0, 0);
}

StartMenuAction handleStartMenuKey(
    SDL_Keycode key, int& selectedOption)
{
    if (key == SDLK_UP || key == SDLK_w ||
        key == SDLK_DOWN || key == SDLK_s)
    {
        selectedOption = (selectedOption == 0) ? 1 : 0;
        return START_MENU_NONE;
    }

    if (key == SDLK_x)
        return selectedOption == 0
        ? START_MENU_PLAY : START_MENU_EXIT;

    if (key == SDLK_ESCAPE)
        return START_MENU_EXIT;

    return START_MENU_NONE;
}
