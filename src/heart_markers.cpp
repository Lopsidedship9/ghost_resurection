#include "heart_markers.h"

#include <iostream>

void updateHeartMarkers(GameMap* map)
{
    const int OBJECTS_LAYER = 2;
    const int TILE_BEFORE = 738;
    const int TILE_AFTER = 739;

	const int ANTORCHAS[5][2] = {
		{ 17, 22 },
		{ 19, 22 },
		{ 17, 23 },
		{ 18, 23 },
		{ 19, 23 }
	};

	const int MAX_MARQUERS = 5;

    // Mantiene el progreso entre llamadas: un marcador por corazon recogido.
    static int nextMarker = 0;

    if (map == nullptr || map->layers == nullptr ||
        map->numLayers <= OBJECTS_LAYER)
        return;

    // Ya se actualizaron todos los marcadores.
    if (nextMarker >= MAX_MARQUERS)
        return;

    // Obtener las coordenadas del siguiente marcador.
    int positionX = ANTORCHAS[nextMarker][0];
    int positionY = ANTORCHAS[nextMarker][1];

    // Comprobar que las coordenadas estan dentro del mapa.
    if (positionX < 0 || positionX >= map->width ||
        positionY < 0 || positionY >= map->height)
        return;

    // Obtener el tile situado en esas coordenadas.
    int& tile = map->getCell(
        positionX, positionY, OBJECTS_LAYER
    ).type;

    // Cambiar un solo marcador por cada corazon recogido.
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
    }
    else
    {
        std::cout << "[HeartMarkers] Aviso: no se encontro "
            << "el tile 738 en ("
            << positionX << ", " << positionY
            << ")." << std::endl;
    }
}