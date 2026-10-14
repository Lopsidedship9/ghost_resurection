#include "movement.h"

namespace
{
    // Índices de las capas del mapa (empezando por 0).
    const int OBJECTS_LAYER = 2; // Capa 4 de Tiled: Objetos interactuables
    const int PLAYER_LAYER = 3; // Capa 3 de Tiled: personaje
    const int COLLISION_LAYER = 0; // Capa 1 de Tiled: edificios/colisiones
}

bool movePlayer(GameMap* map,
                int& player_x,
                int& player_y,
                MoveDirection direction,
                bool game_is_running)
{
    // No mover si el juego no está activo, no hay mapa o no hay capas necesarias.
    if (!game_is_running || map == nullptr || map->layers == nullptr ||
        map->numLayers <= PLAYER_LAYER || map->numLayers <= COLLISION_LAYER)
    {
        return false;
    }

    int new_x = player_x;
    int new_y = player_y;

    switch (direction)
    {
        case MoveDirection::UP:    --new_y; break;
        case MoveDirection::DOWN:  ++new_y; break;
        case MoveDirection::LEFT:  --new_x; break;
        case MoveDirection::RIGHT: ++new_x; break;
        case MoveDirection::NONE:
        default:
            return false;
    }

    // Comprobar que el destino está dentro del mapa.
    if (new_x < 0 || new_x >= map->width || new_y < 0 || new_y >= map->height)
        return false;

    // No atravesar edificios ni sobrescribir otra celda ocupada en la capa del jugador.
    sCell& collision_cell = map->getCell(new_x, new_y, COLLISION_LAYER);
    sCell& destination_cell = map->getCell(new_x, new_y, PLAYER_LAYER);
    sCell& current_cell = map->getCell(player_x, player_y, PLAYER_LAYER);

    if (collision_cell.type >= 0 || destination_cell.type >= 0 || current_cell.type < 0)
        return false;

    // Trasladar el tile del jugador a la nueva celda.
    destination_cell.type = current_cell.type;
    current_cell.type = -1;

    player_x = new_x;
    player_y = new_y;
    return true;
}
