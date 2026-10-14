#include "collectables.h"
#include "heart_markers.h"


namespace
{
    // Índices de capas de mapilla.tmj, empezando en 0.
    const int OBJECTS_LAYER = 2;

    // ID interno del mapa (GID de Tiled).
    const CollectibleDefinition COLLECTIBLE_DEFINITIONS[] =
    {
        { 530, CollectibleType::HEART, "Corazon", 1 }
    };

    const int NUM_COLLECTIBLE_DEFINITIONS =
        sizeof(COLLECTIBLE_DEFINITIONS) / sizeof(COLLECTIBLE_DEFINITIONS[0]);

    const CollectibleDefinition* findCollectible(int tile_type)
    {
        for (int i = 0; i < NUM_COLLECTIBLE_DEFINITIONS; ++i)
        {
            if (COLLECTIBLE_DEFINITIONS[i].tile_type == tile_type)
                return &COLLECTIBLE_DEFINITIONS[i];
        }
        return nullptr;
    }
}

bool collectObjectAt(GameMap* map, int player_x, int player_y,
                     PlayerInventory& inventory)
{
    if (map == nullptr || map->layers == nullptr ||
        map->numLayers <= OBJECTS_LAYER)
        return false;

    if (player_x < 0 || player_x >= map->width ||
        player_y < 0 || player_y >= map->height)
        return false;

    sCell& object_cell = map->getCell(player_x, player_y, OBJECTS_LAYER);
    if (object_cell.type < 0)
        return false;

    const CollectibleDefinition* item = findCollectible(object_cell.type);
    if (item == nullptr)
        return false; // Es un objeto decorativo/no recogible.

    switch (item->type)
    {
        case CollectibleType::HEART:
            inventory.hearts_collected += item->amount;
            updateHeartMarkers(map);
            break;

        case CollectibleType::NONE:
        default:
            return false;
    }

    // El objeto desaparece del mapa después de recogerlo.
    object_cell.type = -1;
    return true;
}
