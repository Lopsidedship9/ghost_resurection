#include "game_ending.h"

namespace
{
    const int OBJECTS_LAYER = 2;
    const int FINAL_TILE = 735;
    const int REQUIRED_HEARTS = 5;

    bool ending_active = false;
}

bool tryActivateGameEnding(GameMap* map, int player_x, int player_y,
                           int hearts_collected)
{
    if (ending_active)
        return true;

    if (hearts_collected < REQUIRED_HEARTS || map == nullptr ||
        map->layers == nullptr || map->numLayers <= OBJECTS_LAYER)
        return false;

    // Se puede interactuar con la casilla final estando encima o junto a ella.
    const int offsets[5][2] = {
        { 0,  0 }, // Casilla actual
        { 0, -1 }, // Arriba
        { 1,  0 }, // Derecha
        { 0,  1 }, // Abajo
        { -1, 0 }  // Izquierda
    };

    for (int i = 0; i < 5; ++i)
    {
        const int x = player_x + offsets[i][0];
        const int y = player_y + offsets[i][1];

        if (x < 0 || x >= map->width || y < 0 || y >= map->height)
            continue;

        if (map->getCell(x, y, OBJECTS_LAYER).type == FINAL_TILE)
        {
            ending_active = true;
            return true;
        }
    }

    return false;
}

bool isGameEndingActive()
{
    return ending_active;
}

void resetGameEnding()
{
    ending_active = false;
}