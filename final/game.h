#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include "enemy.h"

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
};

// Function declarations
void InitializeLevel(Game& game, int level, int enemyWidth, int enemyHeight);
void ResetGame(Game& game, int level, int enemyWidth, int enemyHeight, SDL_Rect& playerRect, int groundLevel,
               int& playerHealth, bool& isJumping, int& jumpVelocity, bool& moveLeft, bool& moveRight,
               bool& faceLeft, bool& isAttacking, bool& playerInvincible, Uint32& playerHitTime, bool& wasMoving,
               std::vector<SDL_Texture*>*& currentPlayerAnim, std::vector<SDL_Texture*>& playerIdle, int& animFrame);
void RenderSceneToTexture(SDL_Renderer* renderer, SDL_Texture* targetTexture, float scale,
                          SDL_Texture* background, SDL_Texture* ground, SDL_Rect& playerRect,
                          std::vector<SDL_Texture*>* currentPlayerAnim, int animFrame, bool faceLeft,
                          std::vector<Enemy>& enemies, std::vector<SDL_Texture*>& enemyRun,
                          std::vector<SDL_Texture*>& enemyAttack, SDL_Texture* explosion,
                          std::vector<std::pair<SDL_Rect, Uint32>>& explosions, GameState gameState);

#endif
