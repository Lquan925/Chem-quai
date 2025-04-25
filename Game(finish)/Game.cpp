#include "Game.h"
#include "Constants.h"
#include <SDL_mixer.h>

void Game::InitializeLevel(int level, int enemyWidth, int enemyHeight) {
    this->level = level;
    enemies.clear();
    explosions.clear();
    lastEnemySpawnTime = SDL_GetTicks();
    gameStartTime = SDL_GetTicks();
    enemiesToDefeat = 1000;
    enemiesDefeated = 0;
    state = PLAYING;

    Enemy initialEnemy;
    initialEnemy.rect = {
        0,
        (int)(SCREEN_HEIGHT - GROUND_HEIGHT - (enemyHeight * ENEMY_SCALE * 0.75)),
        (int)(enemyWidth * ENEMY_SCALE),
        (int)(enemyHeight * ENEMY_SCALE)
    };
    initialEnemy.direction = 1;
    initialEnemy.faceLeft = false;
    initialEnemy.active = true;
    initialEnemy.state = ENEMY_MOVING;
    initialEnemy.animFrame = 0;
    initialEnemy.lastAnimTime = SDL_GetTicks();
    initialEnemy.inContact = false;
    initialEnemy.fromLeft = true;
    initialEnemy.isInitialEnemy = true;
    enemies.push_back(initialEnemy);
}

void Game::ResetGame(int level, int enemyWidth, int enemyHeight, Character& player,
                     std::vector<SDL_Texture*>& playerIdle) {
    player.health = 3;
    player.rect.x = 50;
    player.rect.y = player.groundLevel;
    player.isJumping = false;
    player.jumpVelocity = 0;
    player.moveLeft = false;
    player.moveRight = false;
    player.faceLeft = true;
    player.isAttacking = false;
    player.invincible = false;
    player.hitTime = 0;
    player.wasMoving = false;
    player.currentAnim = &playerIdle;
    player.animFrame = 0;
    Mix_HaltChannel(CHANNEL_PLAYER);
    InitializeLevel(level, enemyWidth, enemyHeight);
}
