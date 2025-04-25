#ifndef GAME_H
#define GAME_H

#include <vector>
#include <SDL.h>
#include "Enemy.h"
#include "Character.h"

enum GameState { START_SCREEN, PLAYING, PLAYER_HIT, GAME_OVER, PLAYER_WIN };

struct Game {
    GameState state;
    std::vector<Enemy> enemies;
    std::vector<std::pair<SDL_Rect, Uint32>> explosions;
    Uint32 lastEnemySpawnTime;
    Uint32 gameStartTime;
    int level;
    int enemiesToDefeat;
    int enemiesDefeated;

    void InitializeLevel(int level, int enemyWidth, int enemyHeight);
    void ResetGame(int level, int enemyWidth, int enemyHeight, Character& player,
                   std::vector<SDL_Texture*>& playerIdle);
};

#endif
