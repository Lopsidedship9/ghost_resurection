#ifndef INTERACTABLES_H
#define INTERACTABLES_H

#include "map.h"

// Comprueba si se ha pulsado Z o X y activa un objeto interactuable adyacente.
// El tile 201 pasa a 200 y se elimina la colision de esa casilla.
bool interactWithNearbyObject(GameMap* map, int player_x, int player_y);

#endif
