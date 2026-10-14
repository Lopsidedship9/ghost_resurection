#include "map.h"
#include "json.hpp"
#include <fstream>
#include <cmath>
#include <iostream>

GameMap::GameMap(int w, int h) {
	width = w;
	height = h;
}

GameMap::~GameMap() {
	for (int l = 0; l < numLayers; l++)
		delete[] layers[l].data;
	delete[] layers;
}

sCell& GameMap::getCell(int x, int y, int layer) {
	return layers[layer].data[x + y * width];
}

GameMap* loadGameMap(const char* filename)
{
	using json = nlohmann::json;
	std::ifstream f(filename);
	if (!f.good())
		return nullptr;
	json jData = json::parse(f);

	int w = jData["width"];
	int h = jData["height"];
	int numLayers = jData["layers"].size();

	GameMap* map = new GameMap(w, h);
	map->numLayers = numLayers;
	map->layers = new sLayer[numLayers];
	map->tile_width = jData["tilewidth"];
	map->tile_height = jData["tileheight"];

	for (int l = 0; l < numLayers; l++) {
		// Allocate memory for data inside each layer
		map->layers[l].data = new sCell[w * h];
		json layer = jData["layers"][l];
		for (int x = 0; x < map->width; x++) {
			for (int y = 0; y < map->height; y++) {
				int index = x + y * map->width;
				// los bits altos del numero indican si el tile esta girado/volteado;
				// los quitamos para quedarnos solo con el id del tile
				unsigned int gid = layer["data"][index].get<unsigned int>() & 0x1FFFFFFF;
				int type = (int)gid - 1; // -1 = celda vacia
				map->getCell(x, y, l).type = type;
			}
		}
	}

	return map;
}

void GameMap::render(Image& fb, Image& tileset, int tileset_columns, int cam_x, int cam_y)
{
	// solo recorremos las celdas visibles
	int first_i = (int)floor(cam_x / (float)tile_width);
	int first_j = (int)floor(cam_y / (float)tile_height);
	int last_i = (int)floor((cam_x + (int)fb.width - 1) / (float)tile_width);
	int last_j = (int)floor((cam_y + (int)fb.height - 1) / (float)tile_height);

	if (first_i < 0) first_i = 0;
	if (first_j < 0) first_j = 0;
	if (last_i >= width) last_i = width - 1;
	if (last_j >= height) last_j = height - 1;

	for (int l = 0; l < numLayers; l++)
		for (int i = first_i; i <= last_i; i++)
			for (int j = first_j; j <= last_j; j++) {
				int type = getCell(i, j, l).type;
				if (type < 0) // vacia
					continue;

				int tilex = (type % tileset_columns) * tile_width;
				int tiley = (type / tileset_columns) * tile_height;

				Area area((float)tilex, (float)tiley, (float)tile_width, (float)tile_height);
				fb.drawImage(tileset, i * tile_width - cam_x, j * tile_height - cam_y, area);
			}
}
