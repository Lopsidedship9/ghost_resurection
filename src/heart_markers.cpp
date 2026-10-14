#include "heart_markers.h"

#include <iostream>

void updateHeartMarkers(GameMap* map)
{
    const int COLLISION_LAYER = 0;
    const int OBJECTS_LAYER = 2;

    const int TILE_BEFORE = 738;
    const int TILE_AFTER = 739;

    const int FINAL_OBJECT_TILE = 735;
    const int FINAL_COLLISION_TILE = 587;

    const int FINAL_X = 18;
    const int FINAL_Y = 22;

    const int ANTORCHAS[5][2] = {
        {17, 22},
        {19, 22},
        {17, 23},
        {18, 23},
        {19, 23}
    };

    const int MAX_MARQUERS = 5;

    // Guarda el progreso entre llamadas.
    static int nextMarker = 0;

    if (map == nullptr || map->layers == nullptr ||
        map->numLayers <= OBJECTS_LAYER ||
        map->numLayers <= COLLISION_LAYER)
    {
        return;
    }

    // Ya se actualizaron todos los marcadores.
    if (nextMarker >= MAX_MARQUERS)
        return;

    // Obtener las coordenadas del siguiente marcador.
    int positionX = ANTORCHAS[nextMarker][0];
    int positionY = ANTORCHAS[nextMarker][1];

    // Comprobar que las coordenadas estan dentro del mapa.
    if (positionX < 0 || positionX >= map->width ||
        positionY < 0 || positionY >= map->height)
    {
        return;
    }

    // Obtener el tile del marcador.
    int& tile = map->getCell(
        positionX, positionY, OBJECTS_LAYER
    ).type;

    // Actualizar un marcador por cada corazon recogido.
    if (tile == TILE_BEFORE)
    {
        tile = TILE_AFTER;
        ++nextMarker;

        std::cout << "[HeartMarkers] Marcador "
            << nextMarker << "/" << MAX_MARQUERS
            << " actualizado en ("
            << positionX << ", " << positionY
            << "): 738 -> 739"
            << std::endl;

        // Si se han actualizado los cinco marcadores,
        // desbloquear el final inmediatamente.
        if (nextMarker >= MAX_MARQUERS &&
            FINAL_X >= 0 && FINAL_X < map->width &&
            FINAL_Y >= 0 && FINAL_Y < map->height)
        {
            sCell& finalObject = map->getCell(
                FINAL_X, FINAL_Y, OBJECTS_LAYER
            );

            sCell& finalCollision = map->getCell(
                FINAL_X, FINAL_Y, COLLISION_LAYER
            );

            finalObject.type = FINAL_OBJECT_TILE;
            finalCollision.type = FINAL_COLLISION_TILE;

            std::cout << "[HeartMarkers] Final desbloqueado en ("
                << FINAL_X << ", " << FINAL_Y
                << "): objeto 735 y colision 587."
                << std::endl;
        }
    }
    else
    {
        std::cout << "[HeartMarkers] Aviso: no se encontro "
            << "el tile 738 en ("
            << positionX << ", " << positionY
            << ")." << std::endl;
    }
}