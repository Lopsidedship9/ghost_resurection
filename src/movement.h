#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "map.h"

// Direcciones posibles para el movimiento del jugador.
enum class MoveDirection
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// Intenta mover al jugador una casilla.
// Devuelve true si el jugador se ha movido y false si no era posible.
// game_is_running permite bloquear el movimiento cuando el juego no está activo.
bool movePlayer(GameMap* map,
                int& player_x,
                int& player_y,
                MoveDirection direction,
                bool game_is_running);

#endif
