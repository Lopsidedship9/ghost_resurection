#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "image.h"

// Una celda del mapa. type = id del tile en el tileset (empezando en 0), o -1 si esta vacia
struct sCell {
	int type;
};

// Una capa de Tiled (Suelo, Edificios, personajes...)
struct sLayer {
	sCell* data = NULL;
};

class GameMap
{
public:
	int width = 0;
	int height = 0;
	int tile_width = 16;
	int tile_height = 16;
	int numLayers = 0;
	sLayer* layers = NULL;

	GameMap(int w, int h);
	~GameMap();

	sCell& getCell(int x, int y, int layer);

	// tileset_columns: columnas del tileset (tilemap_packed.png = 12)
	// cam_x, cam_y: esquina superior izquierda de la camara, en pixeles del mundo
	void render(Image& fb, Image& tileset, int tileset_columns, int cam_x, int cam_y);
};

// Carga un mapa de Tiled (.tmj) usando nlohmann/json. Devuelve nullptr si falla.
GameMap* loadGameMap(const char* filename);

#endif
