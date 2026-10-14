#include "ghost_ai.h"

#include <queue>
#include <vector>
#include <cmath>

namespace
{
    // Los índices de capa empiezan en cero: capa 5 de Tiled = índice 4.
    const int COLLISION_LAYER = 0;
    const int ENEMY_LAYER = 4;
    const int GHOST_TILE_ID = 367;

    const double NORMAL_STEP_SECONDS = 1.0;
    const double CHASE_STEP_SECONDS = 0.12;
    const double CHASE_DISTANCE = 2.0;

    int ghost_x = 2;
    int ghost_y = 1;

    double movement_timer = 0.0;
    bool game_over = false;


    bool insideMap(const GameMap* map, int x, int y)
    {
        return map && x >= 0 && x < map->width && y >= 0 && y < map->height;
    }

    bool canEnter(GameMap* map, int x, int y,
                  int ghost_x, int ghost_y, int player_x, int player_y)
    {
        if (!insideMap(map, x, y))
            return false;

        if (map->numLayers <= COLLISION_LAYER || map->numLayers <= ENEMY_LAYER)
            return false;

        // No atravesar paredes/obstáculos de la capa de colisiones.
        if (map->getCell(x, y, COLLISION_LAYER).type >= 0)
            return false;

        // La casilla del jugador sí es un destino válido para detectar la muerte.
        if (x == player_x && y == player_y)
            return true;

        // No atravesar otros elementos de la capa de enemigos.
        if ((x != ghost_x || y != ghost_y) &&
            map->getCell(x, y, ENEMY_LAYER).type >= 0)
            return false;

        return true;
    }

    bool findNextStep(GameMap* map, int ghost_x, int ghost_y,
                      int player_x, int player_y, int& next_x, int& next_y)
    {
        if (!insideMap(map, player_x, player_y))
            return false;

        const int width = map->width;
        const int total = map->width * map->height;
        const int start = ghost_x + ghost_y * width;
        const int goal = player_x + player_y * width;

        if (start == goal)
        {
            next_x = player_x;
            next_y = player_y;
            return true;
        }

        // BFS encuentra el camino más corto por casillas y evita las paredes.
        std::vector<int> previous(total, -1);
        std::queue<int> pending;
        previous[start] = start;
        pending.push(start);

        const int dx[4] = { 0, 1, 0, -1 };
        const int dy[4] = { -1, 0, 1, 0 };

        while (!pending.empty())
        {
            const int current = pending.front();
            pending.pop();
            if (current == goal)
                break;

            const int x = current % width;
            const int y = current / width;

            for (int dir = 0; dir < 4; ++dir)
            {
                const int nx = x + dx[dir];
                const int ny = y + dy[dir];
                if (!canEnter(map, nx, ny, ghost_x, ghost_y, player_x, player_y))
                    continue;

                const int neighbour = nx + ny * width;
                if (previous[neighbour] != -1)
                    continue;

                previous[neighbour] = current;
                pending.push(neighbour);
            }
        }

        if (previous[goal] == -1)
            return false; // No existe un camino transitable.

        // Retroceder desde el jugador hasta encontrar la primera casilla tras el fantasma.
        int step = goal;
        while (previous[step] != start)
        {
            step = previous[step];
            if (step == previous[step])
                return false;
        }

        next_x = step % width;
        next_y = step / width;
        return true;
    }
}

void initGhostAI(int x, int y)
{
    ghost_x = x;
    ghost_y = y;
    movement_timer = 0.0;
    game_over = false;
}

void updateGhostAI(GameMap* map, int player_x, int player_y, double seconds_elapsed)
{
    if (game_over || !map || !map->layers || map->numLayers <= ENEMY_LAYER)
        return;

    // Si ya comparten casilla, activar la pantalla de muerte.
    if (ghost_x == player_x && ghost_y == player_y)
    {
        game_over = true;
        return;
    }

    const double dx = static_cast<double>(ghost_x - player_x);
    const double dy = static_cast<double>(ghost_y - player_y);
    const bool chasing = (dx * dx + dy * dy) < (CHASE_DISTANCE * CHASE_DISTANCE);
    const double step_seconds = chasing ? CHASE_STEP_SECONDS : NORMAL_STEP_SECONDS;

    movement_timer += seconds_elapsed;
    if (movement_timer < step_seconds)
        return;

    // Un solo paso por actualización: evita que un cambio a modo persecución
    // acumule tiempo y provoque saltos de muchas casillas en un único segundo.
    movement_timer = 0.0;

    int next_x = ghost_x;
    int next_y = ghost_y;
    if (!findNextStep(map, ghost_x, ghost_y, player_x, player_y, next_x, next_y))
        return;

    // Trasladar el tile del fantasma dentro de la capa 5.
    map->getCell(ghost_x, ghost_y, ENEMY_LAYER).type = -1;
    map->getCell(next_x, next_y, ENEMY_LAYER).type = GHOST_TILE_ID;

    ghost_x = next_x;
    ghost_y = next_y;

    if (next_x == player_x && next_y == player_y)
        game_over = true;
}

bool isGhostGameOver()
{
    return game_over;
}

void resetGhostAI()
{
    ghost_x = 2;
    ghost_y = 1;
    movement_timer = 0.0;
    game_over = false;
}
