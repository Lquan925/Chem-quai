#pragma once
#include "constants.h"
#include "player.h"
#include "enemy.h"
#include <vector>
#include <utility>
using namespace std;
struct Game {
    GameState state;
    vector<Enemy> enemies;
    vector<pair<SDL_Rect, Uint32>> explosions;
    Uint32 lastEnemySpawnTime;
};
