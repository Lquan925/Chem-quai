#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "constants.h"
#include "texture_manager.h"
#include "enemy.h"
#include "player.h"
#include "game.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! Error: " << SDL_GetError() << endl;
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image could not initialize! Error: " << IMG_GetError() << endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window could not be created! Error: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cout << "Renderer could not be created! Error: " << SDL_GetError() << endl;
        return -1;
    }

    // Load textures with your original paths
    SDL_Texture* background = LoadTexture("D:\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    SDL_Texture* ground = LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);

    // Player textures
    vector<SDL_Texture*> playerRun = {
        LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_2_resized_1.5x.png", renderer)
    };

    vector<SDL_Texture*> playerAttack = {
        LoadTexture("D:\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };

    vector<SDL_Texture*> playerIdle = {
        LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };

    // Enemy textures
    vector<SDL_Texture*> enemyRun = {
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134637-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134703-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134742-removebg-preview.png", renderer)
    };

    vector<SDL_Texture*> enemyAttack = {
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144841-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144928-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_145144-removebg-preview.png", renderer)
    };

    SDL_Texture* explosion = LoadTexture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    SDL_Texture* youWin = LoadTexture("D:\\game over\\you_win (1).png", renderer);
    SDL_Texture* youLose = LoadTexture("D:\\game over\\you_lose.png", renderer);

    // Check all textures
    bool texturesLoaded = background && ground && explosion && youWin && youLose;
    for (auto tex : playerRun) if (!tex) texturesLoaded = false;
    for (auto tex : playerAttack) if (!tex) texturesLoaded = false;
    for (auto tex : playerIdle) if (!tex) texturesLoaded = false;
    for (auto tex : enemyRun) if (!tex) texturesLoaded = false;
    for (auto tex : enemyAttack) if (!tex) texturesLoaded = false;

    if (!texturesLoaded) {
        cout << "Failed to load some textures!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Setup player
    int playerWidth, playerHeight;
    SDL_QueryTexture(playerIdle[0], NULL, NULL, &playerWidth, &playerHeight);
    SDL_Rect playerRect = {
        50,
        SCREEN_HEIGHT - GROUND_HEIGHT - (int)(playerHeight * PLAYER_SCALE),
        (int)(playerWidth * PLAYER_SCALE),
        (int)(playerHeight * PLAYER_SCALE)
    };
    int groundLevel = playerRect.y;
    int playerHealth = 3;

    // Setup enemy
    int enemyWidth, enemyHeight;
    SDL_QueryTexture(enemyRun[0], NULL, NULL, &enemyWidth, &enemyHeight);

    // Game variables
    vector<Enemy> enemies;
    Uint32 lastEnemySpawnTime = 0;
    vector<SDL_Texture*>* currentPlayerAnim = &playerIdle;
    int animFrame = 0;
    Uint32 lastAnimTime = 0;
    Uint32 animEndTime = 0;
    bool isJumping = false;
    int jumpVelocity = 0;
    bool moveLeft = false, moveRight = false;
    bool faceLeft = true;
    bool isAttacking = false;
    bool playerInvincible = false;
    Uint32 playerHitTime = 0;
    vector<pair<SDL_Rect, Uint32>> explosions;
    GameState gameState = PLAYING;
    srand(time(NULL));

    // Main game loop
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Event handling
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = true;
                        faceLeft = true;
                        if (!isAttacking) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        faceLeft = false;
                        if (!isAttacking) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_SPACE:
                        if (!isJumping) {
                            isJumping = true;
                            jumpVelocity = -JUMP_HEIGHT;
                        }
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_r:
                        if (gameState == GAME_OVER || gameState == PLAYER_WIN) {
                            // Reset game
                            gameState = PLAYING;
                            playerHealth = 3;
                            playerRect.x = 50;
                            playerRect.y = groundLevel;
                            enemies.clear();
                            explosions.clear();
                            lastEnemySpawnTime = SDL_GetTicks();
                        }
                        break;
                }
            }

            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = false;
                        if (!moveRight && !isAttacking) currentPlayerAnim = &playerIdle;
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        if (!moveLeft && !isAttacking) currentPlayerAnim = &playerIdle;
                        break;
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (!isAttacking) {
                    isAttacking = true;
                    animFrame = 0;
                    lastAnimTime = SDL_GetTicks();
                    animEndTime = lastAnimTime + (playerAttack.size() * ANIMATION_SPEED);
                    currentPlayerAnim = &playerAttack;

                    // Attack enemies
                    for (size_t i = 0; i < enemies.size(); i++) {
                        if (enemies[i].active) {
                            int playerCenterX = playerRect.x + playerRect.w / 2;
                            int enemyCenterX = enemies[i].rect.x + enemies[i].rect.w / 2;
                            int distance = abs(playerCenterX - enemyCenterX);

                            if (distance < ATTACK_DISTANCE) {
                                enemies[i].active = false;
                                explosions.push_back({enemies[i].rect, SDL_GetTicks()});
                                enemies.erase(enemies.begin() + i);
                                i--;

                                if (enemies.empty()) {
                                    gameState = PLAYER_WIN;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Game update
        if (gameState == PLAYING || gameState == PLAYER_WIN) {
            if (moveLeft) {
                playerRect.x -= PLAYER_SPEED;
                if (playerRect.x < 0) playerRect.x = 0;
                if (!isAttacking) currentPlayerAnim = &playerRun;
            }

            if (moveRight) {
                playerRect.x += PLAYER_SPEED;
                if (playerRect.x + playerRect.w > SCREEN_WIDTH) playerRect.x = SCREEN_WIDTH - playerRect.w;
                if (!isAttacking) currentPlayerAnim = &playerRun;
            }
        }

        if (gameState == PLAYING) {
            // Spawn enemies
            if (SDL_GetTicks() - lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL) {
                Enemy newEnemy;
                newEnemy.rect = {
                    0,
                    SCREEN_HEIGHT - GROUND_HEIGHT - (int)(enemyHeight * ENEMY_SCALE) * 0.75,
                    (int)(enemyWidth * ENEMY_SCALE),
                    (int)(enemyHeight * ENEMY_SCALE)
                };

                newEnemy.fromLeft = (rand() % 2) == 0;

                if (newEnemy.fromLeft) {
                    newEnemy.rect.x = -newEnemy.rect.w;
                    newEnemy.direction = 1;
                    newEnemy.faceLeft = false;
                } else {
                    newEnemy.rect.x = SCREEN_WIDTH;
                    newEnemy.direction = -1;
                    newEnemy.faceLeft = true;
                }

                newEnemy.active = true;
                newEnemy.state = ENEMY_MOVING;
                newEnemy.animFrame = 0;
                newEnemy.lastAnimTime = SDL_GetTicks();
                newEnemy.inContact = false;

                enemies.push_back(newEnemy);
                lastEnemySpawnTime = SDL_GetTicks();
            }

            // Update enemies
            for (size_t i = 0; i < enemies.size(); i++) {
                if (enemies[i].active) {
                    int playerCenterX = playerRect.x + playerRect.w / 2;
                    int enemyCenterX = enemies[i].rect.x + enemies[i].rect.w / 2;
                    int distance = abs(playerCenterX - enemyCenterX);

                    if (distance < ATTACK_DISTANCE) {
                        if (!enemies[i].inContact) {
                            enemies[i].inContact = true;
                            enemies[i].contactTime = SDL_GetTicks();
                        }

                        if (enemies[i].state == ENEMY_MOVING && SDL_GetTicks() - enemies[i].contactTime > ENEMY_ATTACK_COOLDOWN) {
                            enemies[i].state = ENEMY_ATTACKING;
                            enemies[i].attackStartTime = SDL_GetTicks();
                            enemies[i].animFrame = 0;
                        }
                    } else {
                        enemies[i].inContact = false;
                        enemies[i].state = ENEMY_MOVING;
                    }

                    switch (enemies[i].state) {
                        case ENEMY_MOVING:
                            enemies[i].rect.x += ENEMY_SPEED * enemies[i].direction;

                            if ((enemies[i].fromLeft && enemies[i].rect.x > SCREEN_WIDTH) ||
                                (!enemies[i].fromLeft && enemies[i].rect.x + enemies[i].rect.w < 0)) {
                                enemies.erase(enemies.begin() + i);
                                i--;
                                continue;
                            }

                            if (SDL_GetTicks() - enemies[i].lastAnimTime > ENEMY_ANIMATION_SPEED) {
                                enemies[i].animFrame = (enemies[i].animFrame + 1) % enemyRun.size();
                                enemies[i].lastAnimTime = SDL_GetTicks();
                            }
                            break;

                        case ENEMY_ATTACKING:
                            if (SDL_GetTicks() - enemies[i].lastAnimTime > ANIMATION_SPEED) {
                                enemies[i].animFrame = (enemies[i].animFrame + 1) % enemyAttack.size();
                                enemies[i].lastAnimTime = SDL_GetTicks();
                            }

                            if (SDL_GetTicks() - enemies[i].attackStartTime > ENEMY_ATTACK_DURATION) {
                                enemies[i].state = ENEMY_MOVING;
                                enemies[i].animFrame = 0;

                                if (!playerInvincible) {
                                    playerHealth--;
                                    playerInvincible = true;
                                    playerHitTime = SDL_GetTicks();

                                    if (playerHealth <= 0) {
                                        gameState = GAME_OVER;
                                    }
                                    else {
                                        gameState = PLAYER_HIT;
                                    }
                                }
                            }
                            break;
                    }
                }
            }

            // Update jump
            if (isJumping) {
                playerRect.y += jumpVelocity;
                jumpVelocity += GRAVITY;
                if (playerRect.y >= groundLevel) {
                    playerRect.y = groundLevel;
                    isJumping = false;
                }
            }

            // End attack animation
            if (isAttacking && SDL_GetTicks() >= animEndTime) {
                isAttacking = false;
                if (moveLeft || moveRight) {
                    currentPlayerAnim = &playerRun;
                } else {
                    currentPlayerAnim = &playerIdle;
                }
            }
        }
        else if (gameState == PLAYER_HIT) {
            if (SDL_GetTicks() - playerHitTime > PLAYER_HIT_DURATION) {
                gameState = PLAYING;
                playerInvincible = false;
                if (moveLeft || moveRight) {
                    currentPlayerAnim = &playerRun;
                } else {
                    currentPlayerAnim = &playerIdle;
                }
            }
        }

        // Remove old explosions
        for (size_t i = 0; i < explosions.size(); i++) {
            if (SDL_GetTicks() - explosions[i].second >= EXPLOSION_DURATION) {
                explosions.erase(explosions.begin() + i);
                i--;
            }
        }

        // Update animation
        if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
            animFrame = (animFrame + 1) % currentPlayerAnim->size();
            lastAnimTime = SDL_GetTicks();
        }

        // Rendering
        SDL_RenderClear(renderer);

        // Draw background
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // Draw ground
        for (int x = 0; x < SCREEN_WIDTH; x += 200) {
            SDL_Rect groundRect = {x, SCREEN_HEIGHT - GROUND_HEIGHT, 200, GROUND_HEIGHT};
            SDL_RenderCopy(renderer, ground, NULL, &groundRect);
        }

        // Draw player
        if (gameState != GAME_OVER) {
            bool shouldRenderPlayer = true;
            if (gameState == PLAYER_HIT) {
                shouldRenderPlayer = (SDL_GetTicks() / PLAYER_HIT_BLINK) % 2 == 0;
            }
            else if (playerInvincible) {
                shouldRenderPlayer = (SDL_GetTicks() / PLAYER_INVINCIBLE_BLINK) % 2 == 0;
            }

            if (shouldRenderPlayer) {
                SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                if (gameState == PLAYER_HIT) {
                    SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 150);
                    SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                    SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 255);
                } else {
                    SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                }
            }
        }

        // Draw enemies
        for (const auto& enemy : enemies) {
            if (enemy.active && gameState != GAME_OVER) {
                SDL_RendererFlip enemyFlip = enemy.faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                if (enemy.state == ENEMY_ATTACKING) {
                    SDL_RenderCopyEx(renderer, enemyAttack[enemy.animFrame], NULL, &enemy.rect, 0, NULL, enemyFlip);
                } else {
                    SDL_RenderCopyEx(renderer, enemyRun[enemy.animFrame], NULL, &enemy.rect, 0, NULL, enemyFlip);
                }
            }
        }

        // Draw explosions
        for (const auto& explosionData : explosions) {
            SDL_Rect explosionRect = {
                explosionData.first.x - explosionData.first.w/2,
                explosionData.first.y - explosionData.first.h/2,
                explosionData.first.w * 2,
                explosionData.first.h * 2
            };
            SDL_RenderCopy(renderer, explosion, NULL, &explosionRect);
        }

        // Draw health
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < playerHealth; i++) {
            SDL_Rect healthRect = {10 + i * 30, 10, 25, 25};
            SDL_RenderFillRect(renderer, &healthRect);
        }

        // Draw game over/win
        if (gameState == GAME_OVER || gameState == PLAYER_WIN) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_Rect gameOverRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &gameOverRect);

            SDL_Texture* resultTexture = (gameState == PLAYER_WIN) ? youWin : youLose;
            int texW, texH;
            SDL_QueryTexture(resultTexture, NULL, NULL, &texW, &texH);
            SDL_Rect resultRect = {
                SCREEN_WIDTH/2 - texW/2,
                SCREEN_HEIGHT/2 - texH/2,
                texW,
                texH
            };
            SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);
        }

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(explosion);
    SDL_DestroyTexture(youWin);
    SDL_DestroyTexture(youLose);
    for (auto tex : playerRun) SDL_DestroyTexture(tex);
    for (auto tex : playerAttack) SDL_DestroyTexture(tex);
    for (auto tex : playerIdle) SDL_DestroyTexture(tex);
    for (auto tex : enemyRun) SDL_DestroyTexture(tex);
    for (auto tex : enemyAttack) SDL_DestroyTexture(tex);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
