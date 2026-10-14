#ifndef GHOST_AI_H
#define GHOST_AI_H

#include "map.h"

// Actualiza la IA del fantasma. Debe llamarse una vez por frame desde Game::update.
void updateGhostAI(GameMap* map, int player_x, int player_y, double seconds_elapsed);

// Indica si el fantasma ha alcanzado al jugador.
bool isGhostGameOver();

// Reinicia el estado interno de la IA (no recoloca el tile en el mapa).
void resetGhostAI();

void initGhostAI(int x, int y);

#endif
