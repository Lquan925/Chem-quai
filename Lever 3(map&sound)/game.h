#pragma once
#include "constants.h"
#include "enemy.h"
#include <vector>
#include <utility>

struct Game {
    GameState state;
    std::vector<Enemy> enemies;
    std::vector<std::pair<SDL_Rect, Uint32>> explosions;
    Uint32 lastEnemySpawnTime;

    // Sound effects
    Mix_Chunk* playerRunSound;
    Mix_Chunk* playerAttackSound;
    Mix_Chunk* enemyDeathSound;
};
