#ifndef GAME_ENDING_H
#define GAME_ENDING_H

#include "map.h"

// Comprueba si el jugador interactua con el tile final (735) tras recoger
// los cinco corazones. Devuelve true cuando el final queda activado.
bool tryActivateGameEnding(GameMap* map, int player_x, int player_y,
                           int hearts_collected);

// Indica si el final ya se ha activado.
bool isGameEndingActive();
//Para volver a jugar sin tener que salir.
void resetGameEnding();

#endif
