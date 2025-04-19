#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "constants.h"
#include "texture_manager.h"
#include "enemy.h"
#include "game.h"

void RenderSceneToTexture(SDL_Renderer* renderer, SDL_Texture* targetTexture, float scale,
                         SDL_Texture* background, SDL_Texture* ground, SDL_Rect& playerRect,
                         std::vector<SDL_Texture*>* currentPlayerAnim, int animFrame, bool faceLeft,
                         std::vector<Enemy>& enemies, std::vector<SDL_Texture*>& enemyRun,
                         std::vector<SDL_Texture*>& enemyAttack, SDL_Texture* explosion,
                         std::vector<std::pair<SDL_Rect, Uint32>>& explosions, GameState gameState) {
    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_RenderClear(renderer);

    SDL_Rect bgRect = {0, 0, (int)(SCREEN_WIDTH * scale), (int)(SCREEN_HEIGHT * scale)};
    SDL_RenderCopy(renderer, background, NULL, &bgRect);

    for (int x = 0; x < SCREEN_WIDTH; x += 200) {
        SDL_Rect groundRect = {
            (int)(x * scale),
            (int)((SCREEN_HEIGHT - GROUND_HEIGHT) * scale),
            (int)(200 * scale),
            (int)(GROUND_HEIGHT * scale)
        };
        SDL_RenderCopy(renderer, ground, NULL, &groundRect);
    }

    if (gameState != GAME_OVER) {
        SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_Rect playerMiniRect = {
            (int)(playerRect.x * scale),
            (int)(playerRect.y * scale),
            (int)(playerRect.w * scale),
            (int)(playerRect.h * scale)
        };
        SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerMiniRect, 0, NULL, flip);
    }

    for (const auto& enemy : enemies) {
        if (enemy.active && gameState != GAME_OVER) {
            SDL_RendererFlip enemyFlip = enemy.faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_Rect enemyMiniRect = {
                (int)(enemy.rect.x * scale),
                (int)(enemy.rect.y * scale),
                (int)(enemy.rect.w * scale),
                (int)(enemy.rect.h * scale)
            };
            if (enemy.state == ENEMY_ATTACKING) {
                SDL_RenderCopyEx(renderer, enemyAttack[enemy.animFrame], NULL, &enemyMiniRect, 0, NULL, enemyFlip);
            } else {
                SDL_RenderCopyEx(renderer, enemyRun[enemy.animFrame], NULL, &enemyMiniRect, 0, NULL, enemyFlip);
            }
        }
    }

    for (const auto& explosionData : explosions) {
        SDL_Rect explosionMiniRect = {
            (int)((explosionData.first.x - explosionData.first.w/2) * scale),
            (int)((explosionData.first.y - explosionData.first.h/2) * scale),
            (int)(explosionData.first.w * 2 * scale),
            (int)(explosionData.first.h * 2 * scale)
        };
        SDL_RenderCopy(renderer, explosion, NULL, &explosionMiniRect);
    }

    SDL_SetRenderTarget(renderer, NULL);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Load textures
    SDL_Texture* background = LoadTexture("D:\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    SDL_Texture* ground = LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);

    // Player textures
    std::vector<SDL_Texture*> playerRun = {
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_2_resized_1.5x.png", renderer)
    };

    std::vector<SDL_Texture*> playerAttack = {
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };

    std::vector<SDL_Texture*> playerIdle = {
        LoadTexture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };

    // Enemy textures
    std::vector<SDL_Texture*> enemyRun = {
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134637-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134703-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134742-removebg-preview.png", renderer)
    };


    std::vector<SDL_Texture*> enemyAttack = {
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144841-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144928-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_145144-removebg-preview.png", renderer)
    };

    SDL_Texture* explosion = LoadTexture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    SDL_Texture* youWin = LoadTexture("D:\\game over\\you_win (1).png", renderer);
    SDL_Texture* youLose = LoadTexture("D:\\game over\\you_lose.png", renderer);

    // Load sounds
    Mix_Chunk* playerRunSound = LoadSound("D:\\Âm thanh\\run.wav");
    Mix_Chunk* playerAttackSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* enemyDeathSound = LoadSound("D:\\Âm thanh\\fight.wav");

    // Set sound volumes
    Mix_Volume(CHANNEL_PLAYER, MIX_MAX_VOLUME / 2);
    Mix_Volume(CHANNEL_EFFECTS, MIX_MAX_VOLUME / 2);

    // Create minimap texture
    SDL_Texture* minimapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    if (!minimapTexture) {
        std::cout << "Failed to create minimap texture! Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Check all assets loaded successfully
    bool assetsLoaded = background && ground && explosion && youWin && youLose && minimapTexture &&
                       playerRunSound && playerAttackSound && enemyDeathSound;

    for (auto tex : playerRun) if (!tex) assetsLoaded = false;
    for (auto tex : playerAttack) if (!tex) assetsLoaded = false;
    for (auto tex : playerIdle) if (!tex) assetsLoaded = false;
    for (auto tex : enemyRun) if (!tex) assetsLoaded = false;
    for (auto tex : enemyAttack) if (!tex) assetsLoaded = false;

    if (!assetsLoaded) {
        std::cout << "Failed to load some assets!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
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
    Game game;
    game.state = PLAYING;
    game.lastEnemySpawnTime = 0;
    game.playerRunSound = playerRunSound;
    game.playerAttackSound = playerAttackSound;
    game.enemyDeathSound = enemyDeathSound;

    std::vector<Enemy> enemies;
    std::vector<SDL_Texture*>* currentPlayerAnim = &playerIdle;
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
    std::vector<std::pair<SDL_Rect, Uint32>> explosions;
    GameState gameState = PLAYING;
    bool wasMoving = false;
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
                            gameState = PLAYING;
                            playerHealth = 3;
                            playerRect.x = 50;
                            playerRect.y = groundLevel;
                            enemies.clear();
                            explosions.clear();
                            game.lastEnemySpawnTime = SDL_GetTicks();
                            Mix_HaltChannel(CHANNEL_PLAYER);
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

                    Mix_PlayChannel(CHANNEL_EFFECTS, playerAttackSound, 0);

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

                                Mix_PlayChannel(CHANNEL_EFFECTS, enemyDeathSound, 0);

                                if (enemies.empty()) {
                                    gameState = PLAYER_WIN;
                                    Mix_HaltChannel(CHANNEL_PLAYER);
                                }
                            }
                        }
                    }
                }
            }
        }

        // Play/stop run sound based on movement
        bool isMoving = moveLeft || moveRight;
        if (isMoving && !wasMoving) {
            Mix_PlayChannel(CHANNEL_PLAYER, playerRunSound, -1);
        }
        else if (!isMoving && wasMoving) {
            Mix_HaltChannel(CHANNEL_PLAYER);
        }
        wasMoving = isMoving;

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
            if (SDL_GetTicks() - game.lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL) {
                Enemy newEnemy;
                newEnemy.rect = {
                    0,
                    static_cast<int>(SCREEN_HEIGHT - GROUND_HEIGHT - (enemyHeight * ENEMY_SCALE) * 0.75),
                    static_cast<int>(enemyWidth * ENEMY_SCALE),
                    static_cast<int>(enemyHeight * ENEMY_SCALE)
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
                game.lastEnemySpawnTime = SDL_GetTicks();
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
                                        Mix_HaltChannel(CHANNEL_PLAYER);
                                    }
                                    else {
                                        gameState = PLAYER_HIT;
                                    }
                                }
                            }
                            break;

                        case ENEMY_COOLDOWN:
                            // Handle cooldown state if needed
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

        // Render scene to mini map
        float scale = (float)MINIMAP_WIDTH / SCREEN_WIDTH;
        RenderSceneToTexture(renderer, minimapTexture, scale, background, ground, playerRect,
                            currentPlayerAnim, animFrame, faceLeft, enemies, enemyRun,
                            enemyAttack, explosion, explosions, gameState);

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

        // Draw mini map
        if (gameState != GAME_OVER && gameState != PLAYER_WIN) {
            SDL_Rect minimapDest = {MINIMAP_X, MINIMAP_Y, MINIMAP_WIDTH, MINIMAP_HEIGHT};
            SDL_RenderCopy(renderer, minimapTexture, NULL, &minimapDest);
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
    Mix_FreeChunk(playerRunSound);
    Mix_FreeChunk(playerAttackSound);
    Mix_FreeChunk(enemyDeathSound);
    Mix_CloseAudio();

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(explosion);
    SDL_DestroyTexture(youWin);
    SDL_DestroyTexture(youLose);
    SDL_DestroyTexture(minimapTexture);

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
