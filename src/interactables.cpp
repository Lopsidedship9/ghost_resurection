#include "interactables.h"

namespace
{
    // Índices de las capas de mapilla.tmj (empezando en 0).
    const int COLLISION_LAYER = 0;
    const int OBJECTS_LAYER = 2;

    // IDs internos de GameMap (GID de Tiled).
    const int INTERACTABLE_TILE = 199;
    const int USED_INTERACTABLE_TILE = 200;
}

bool interactWithNearbyObject(GameMap* map, int player_x, int player_y)
{
    if (map == nullptr || map->layers == nullptr ||
        map->numLayers <= COLLISION_LAYER ||
        map->numLayers <= OBJECTS_LAYER)
        return false;

    // Casillas adyacentes: arriba, derecha, abajo e izquierda.
    const int offsets[4][2] = {
        { 0, -1 },
        { 1,  0 },
        { 0,  1 },
        { -1, 0 }
    };

    for (int i = 0; i < 4; ++i)
    {
        const int x = player_x + offsets[i][0];
        const int y = player_y + offsets[i][1];

        if (x < 0 || x >= map->width || y < 0 || y >= map->height)
            continue;

        sCell& object_cell = map->getCell(x, y, OBJECTS_LAYER);

        /*
        std::cout << "Casilla (" << x << ", " << y
            << "), tipo: " << object_cell.type
            << ", tipo esperado: " << INTERACTABLE_TILE
            << std::endl;
        */
        if (object_cell.type != INTERACTABLE_TILE)
            continue;

        object_cell.type = USED_INTERACTABLE_TILE;
        map->getCell(x, y, COLLISION_LAYER).type = -1;

        /*
        std::cout << "Objeto interactuable activado en ("
            << x << ", " << y
            << "): 201 -> 200; colision eliminada."
            << std::endl;
        */
        return true;
    }

    return false;
}
