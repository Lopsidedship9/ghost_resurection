#ifndef COLLECTABLES_H
#define COLLECTABLES_H

#include "map.h"

// Tipo lógico del objeto. Se puede ampliar con monedas, llaves, pociones, etc.
enum class CollectibleType
{
    NONE,
    HEART
};

// Describe qué representa un tile del tileset cuando aparece en la capa de objetos.
// tile_type debe ser el ID interno usado por GameMap (GID de Tiled menos 1).
struct CollectibleDefinition
{
    int tile_type;
    CollectibleType type;
    const char* name;
    int amount;
};

// Inventario básico. Por ahora contamos los corazones recogidos;
// no modifica la vida porque el proyecto aún no tiene un sistema de salud.
struct PlayerInventory
{
    int hearts_collected;

    PlayerInventory() : hearts_collected(0) {}
};

// Comprueba el tile que hay bajo el jugador. Si es un objeto recogible,
// lo registra en el inventario y elimina el tile del mapa.
// Devuelve true si se recogió algo.
bool collectObjectAt(GameMap* map, int player_x, int player_y,
                     PlayerInventory& inventory);

#endif
