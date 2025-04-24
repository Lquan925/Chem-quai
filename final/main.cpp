#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "constants.h"
#include "enemy.h"
#include "game.h"
#include "texture_manager.h"
using namespace std;

void InitializeLevel(Game& game, int level, int enemyWidth, int enemyHeight) {
    game.level = level;
    game.enemies.clear();
    game.explosions.clear();
    game.lastEnemySpawnTime = SDL_GetTicks();
    game.gameStartTime = SDL_GetTicks();
    game.enemiesToDefeat = 1000;
    game.enemiesDefeated = 0;
    game.state = PLAYING;

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
    game.enemies.push_back(initialEnemy);
}

void ResetGame(Game& game, int level, int enemyWidth, int enemyHeight, SDL_Rect& playerRect, int groundLevel,
               int& playerHealth, bool& isJumping, int& jumpVelocity, bool& moveLeft, bool& moveRight,
               bool& faceLeft, bool& isAttacking, bool& playerInvincible, Uint32& playerHitTime, bool& wasMoving,
               vector<SDL_Texture*>*& currentPlayerAnim, vector<SDL_Texture*>& playerIdle, int& animFrame) {
    playerHealth = 3;
    playerRect.x = 50;
    playerRect.y = groundLevel;
    isJumping = false;
    jumpVelocity = 0;
    moveLeft = false;
    moveRight = false;
    faceLeft = true;
    isAttacking = false;
    playerInvincible = false;
    playerHitTime = 0;
    wasMoving = false;
    currentPlayerAnim = &playerIdle;
    animFrame = 0;
    Mix_HaltChannel(CHANNEL_PLAYER);
    InitializeLevel(game, level, enemyWidth, enemyHeight);
}

void RenderSceneToTexture(SDL_Renderer* renderer, SDL_Texture* targetTexture, float scale,
                          SDL_Texture* background, SDL_Texture* ground, SDL_Rect& playerRect,
                          vector<SDL_Texture*>* currentPlayerAnim, int animFrame, bool faceLeft,
                          vector<Enemy>& enemies, vector<SDL_Texture*>& enemyRun,
                          vector<SDL_Texture*>& enemyAttack, SDL_Texture* explosion,
                          vector<pair<SDL_Rect, Uint32>>& explosions, GameState gameState) {
    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_RenderClear(renderer);

    SDL_Rect bgRect = { 0, 0, (int)(SCREEN_WIDTH * scale), (int)(SCREEN_HEIGHT * scale) };
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
            }
            else {
                SDL_RenderCopyEx(renderer, enemyRun[enemy.animFrame], NULL, &enemyMiniRect, 0, NULL, enemyFlip);
            }
        }
    }

    for (const auto& explosionData : explosions) {
        SDL_Rect explosionMiniRect = {
            (int)((explosionData.first.x - explosionData.first.w / 2) * scale),
            (int)((explosionData.first.y - explosionData.first.h / 2) * scale),
            (int)(explosionData.first.w * 2 * scale),
            (int)(explosionData.first.h * 2 * scale)
        };
        SDL_RenderCopy(renderer, explosion, NULL, &explosionMiniRect);
    }

    SDL_SetRenderTarget(renderer, NULL);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "SDL could not initialize! Error: " << SDL_GetError() << endl;
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image could not initialize! Error: " << IMG_GetError() << endl;
        return -1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer could not initialize! Error: " << Mix_GetError() << endl;
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

    SDL_Texture* startScreenBackground = LoadTexture("D:\\Đồ họa(UPDATE)\\Start.png", renderer);
    SDL_Texture* gameplayBackground = LoadTexture("D:\\Đồ họa(UPDATE)\\Background.jpg", renderer);
    SDL_Texture* ground = LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);

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
    SDL_Texture* playAgainButton = LoadTexture("D:\\Đồ họa(UPDATE)\\back-removebg-preview.png", renderer);

    Mix_Chunk* playerRunSound = LoadSound("D:\\Âm thanh\\run.wav");
    Mix_Chunk* playerAttackSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* enemyDeathSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* enemyAttackSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* playerHitSound = LoadSound("D:\\Âm thanh\\fight.wav");

    Mix_Volume(CHANNEL_PLAYER, MIX_MAX_VOLUME / 2);
    Mix_Volume(CHANNEL_ENEMY, MIX_MAX_VOLUME / 2);
    Mix_Volume(CHANNEL_EFFECTS, MIX_MAX_VOLUME / 2);

    SDL_Texture* minimapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, MINIMAP_WIDTH, MINIMAP_HEIGHT);
    if (!minimapTexture) {
        cout << "Failed to create minimap texture! Error: " << SDL_GetError() << endl;
        return -1;
    }

    bool assetsLoaded = startScreenBackground && gameplayBackground && ground && explosion && youWin && youLose && minimapTexture && playAgainButton;
    assetsLoaded = assetsLoaded && playerRunSound && playerAttackSound && enemyDeathSound && enemyAttackSound && playerHitSound;
    for (auto tex : playerRun) if (!tex) assetsLoaded = false;
    for (auto tex : playerAttack) if (!tex) assetsLoaded = false;
    for (auto tex : playerIdle) if (!tex) assetsLoaded = false;
    for (auto tex : enemyRun) if (!tex) assetsLoaded = false;
    for (auto tex : enemyAttack) if (!tex) assetsLoaded = false;

    if (!assetsLoaded) {
        cout << "Failed to load some assets!" << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

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

    int enemyWidth, enemyHeight;
    SDL_QueryTexture(enemyRun[0], NULL, NULL, &enemyWidth, &enemyHeight);

    Game game;
    game.state = START_SCREEN;

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
    bool wasMoving = false;

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (game.state == START_SCREEN) {
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    SDL_Rect startButtonRect = { START_BUTTON_X, START_BUTTON_Y, START_BUTTON_WIDTH, START_BUTTON_HEIGHT };
                    if (mouseX >= startButtonRect.x && mouseX <= startButtonRect.x + startButtonRect.w &&
                        mouseY >= startButtonRect.y && mouseY <= startButtonRect.y + startButtonRect.h) {
                        ResetGame(game, 1, enemyWidth, enemyHeight, playerRect, groundLevel, playerHealth,
                                  isJumping, jumpVelocity, moveLeft, moveRight, faceLeft, isAttacking,
                                  playerInvincible, playerHitTime, wasMoving, currentPlayerAnim, playerIdle, animFrame);
                        game.state = PLAYING;
                    }
                }
            }
            else if (game.state == PLAYING || game.state == PLAYER_HIT) {
                if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = true;
                        faceLeft = true;
                        if (!isAttacking && game.state != GAME_OVER) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        faceLeft = false;
                        if (!isAttacking && game.state != GAME_OVER) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_SPACE:
                        if (!isJumping && game.state == PLAYING) {
                            isJumping = true;
                            jumpVelocity = -JUMP_HEIGHT;
                        }
                        break;
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    }
                }

                if (e.type == SDL_KEYUP) {
                    switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = false;
                        if (!moveRight && !isAttacking && game.state != GAME_OVER) currentPlayerAnim = &playerIdle;
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        if (!moveLeft && !isAttacking && game.state != GAME_OVER) currentPlayerAnim = &playerIdle;
                        break;
                    }
                }

                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    if (game.state == PLAYING) {
                        if (!isAttacking) {
                            isAttacking = true;
                            animFrame = 0;
                            lastAnimTime = SDL_GetTicks();
                            animEndTime = lastAnimTime + (playerAttack.size() * ANIMATION_SPEED);
                            currentPlayerAnim = &playerAttack;
                            Mix_PlayChannel(CHANNEL_EFFECTS, playerAttackSound, 0);

                            for (size_t i = 0; i < game.enemies.size(); i++) {
                                if (game.enemies[i].active) {
                                    int playerCenterX = playerRect.x + playerRect.w / 2;
                                    int enemyCenterX = game.enemies[i].rect.x + game.enemies[i].rect.w / 2;
                                    int distance = abs(playerCenterX - enemyCenterX);

                                    if (distance < ATTACK_DISTANCE) {
                                        game.enemies[i].active = false;
                                        game.explosions.push_back({ game.enemies[i].rect, SDL_GetTicks() });
                                        game.enemies.erase(game.enemies.begin() + i);
                                        i--;
                                        game.enemiesDefeated++;
                                        Mix_PlayChannel(CHANNEL_EFFECTS, enemyDeathSound, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (game.state == GAME_OVER || game.state == PLAYER_WIN) {
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = e.button.x;
                    int mouseY = e.button.y;
                    SDL_Rect buttonRect = { BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                    if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                        mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                        game.state = START_SCREEN;
                    }
                }
            }
        }

        if (game.state != START_SCREEN) {
            bool isMoving = moveLeft || moveRight;
            if (isMoving && !wasMoving) {
                Mix_PlayChannel(CHANNEL_PLAYER, playerRunSound, -1);
            }
            else if (!isMoving && wasMoving) {
                Mix_HaltChannel(CHANNEL_PLAYER);
            }
            wasMoving = isMoving;

            if (game.state == PLAYING) {
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

                if (game.enemiesDefeated >= 1 &&
                    SDL_GetTicks() - game.gameStartTime < ENEMY_SPAWN_DURATION &&
                    SDL_GetTicks() - game.lastEnemySpawnTime > ENEMY_SPAWN_INTERVAL &&
                    game.enemies.size() < 5) {
                    Enemy newEnemy;
                    newEnemy.rect = {
                        0,
                        (int)(SCREEN_HEIGHT - GROUND_HEIGHT - (enemyHeight * ENEMY_SCALE * 0.75)),
                        (int)(enemyWidth * ENEMY_SCALE),
                        (int)(enemyHeight * ENEMY_SCALE)
                    };
                    newEnemy.fromLeft = (rand() % 2) == 0;

                    if (newEnemy.fromLeft) {
                        newEnemy.rect.x = 0;
                        newEnemy.direction = 1;
                        newEnemy.faceLeft = false;
                    }
                    else {
                        newEnemy.rect.x = SCREEN_WIDTH - newEnemy.rect.w;
                        newEnemy.direction = -1;
                        newEnemy.faceLeft = true;
                    }

                    newEnemy.active = true;
                    newEnemy.state = ENEMY_MOVING;
                    newEnemy.animFrame = 0;
                    newEnemy.lastAnimTime = SDL_GetTicks();
                    newEnemy.inContact = false;
                    newEnemy.isInitialEnemy = false;
                    game.enemies.push_back(newEnemy);
                    game.lastEnemySpawnTime = SDL_GetTicks();
                }

                if (game.enemiesDefeated >= 1 &&
                    SDL_GetTicks() - game.gameStartTime >= ENEMY_SPAWN_DURATION &&
                    game.enemies.empty()) {
                    game.state = PLAYER_WIN;
                }

                for (size_t i = 0; i < game.enemies.size(); i++) {
                    if (game.enemies[i].active) {
                        int playerCenterX = playerRect.x + playerRect.w / 2;
                        int enemyCenterX = game.enemies[i].rect.x + game.enemies[i].rect.w / 2;
                        int distance = abs(playerCenterX - enemyCenterX);

                        if (distance < ATTACK_DISTANCE) {
                            if (!game.enemies[i].inContact) {
                                game.enemies[i].inContact = true;
                                game.enemies[i].contactTime = SDL_GetTicks();
                            }

                            if (game.enemies[i].state == ENEMY_MOVING &&
                                SDL_GetTicks() - game.enemies[i].contactTime > ENEMY_ATTACK_COOLDOWN) {
                                game.enemies[i].state = ENEMY_ATTACKING;
                                game.enemies[i].attackStartTime = SDL_GetTicks();
                                game.enemies[i].animFrame = 0;
                                Mix_PlayChannel(CHANNEL_ENEMY, enemyAttackSound, 0);
                            }
                        }
                        else {
                            game.enemies[i].inContact = false;
                            game.enemies[i].state = ENEMY_MOVING;
                        }

                        switch (game.enemies[i].state) {
                        case ENEMY_MOVING:
                            game.enemies[i].rect.x += ENEMY_SPEED * game.enemies[i].direction;

                            if (game.enemies[i].isInitialEnemy) {
                                if (game.enemies[i].rect.x >= SCREEN_WIDTH / 2) {
                                    game.enemies[i].rect.x = SCREEN_WIDTH / 2;
                                    game.enemies[i].direction = -1;
                                    game.enemies[i].faceLeft = true;
                                }
                                else if (game.enemies[i].rect.x <= 0) {
                                    game.enemies[i].rect.x = 0;
                                    game.enemies[i].direction = 1;
                                    game.enemies[i].faceLeft = false;
                                }
                            }
                            else {
                                if (game.enemies[i].rect.x >= SCREEN_WIDTH - game.enemies[i].rect.w) {
                                    game.enemies[i].rect.x = SCREEN_WIDTH - game.enemies[i].rect.w;
                                    game.enemies[i].direction = -1;
                                    game.enemies[i].faceLeft = true;
                                }
                                else if (game.enemies[i].rect.x <= 0) {
                                    game.enemies[i].rect.x = 0;
                                    game.enemies[i].direction = 1;
                                    game.enemies[i].faceLeft = false;
                                }
                            }

                            if (SDL_GetTicks() - game.enemies[i].lastAnimTime > ENEMY_ANIMATION_SPEED) {
                                game.enemies[i].animFrame = (game.enemies[i].animFrame + 1) % enemyRun.size();
                                game.enemies[i].lastAnimTime = SDL_GetTicks();
                            }
                            break;

                        case ENEMY_ATTACKING:
                            if (SDL_GetTicks() - game.enemies[i].lastAnimTime > ANIMATION_SPEED) {
                                game.enemies[i].animFrame = (game.enemies[i].animFrame + 1) % enemyAttack.size();
                                game.enemies[i].lastAnimTime = SDL_GetTicks();
                            }

                            if (SDL_GetTicks() - game.enemies[i].attackStartTime > ENEMY_ATTACK_DURATION) {
                                game.enemies[i].state = ENEMY_MOVING;
                                game.enemies[i].animFrame = 0;

                                if (!playerInvincible) {
                                    playerHealth--;
                                    playerInvincible = true;
                                    playerHitTime = SDL_GetTicks();
                                    Mix_PlayChannel(CHANNEL_EFFECTS, playerHitSound, 0);

                                    if (playerHealth <= 0) {
                                        game.state = GAME_OVER;
                                    }
                                    else {
                                        game.state = PLAYER_HIT;
                                    }
                                }
                            }
                            break;

                        case ENEMY_COOLDOWN:
                            break;
                        }
                    }
                }

                if (isJumping) {
                    playerRect.y += jumpVelocity;
                    jumpVelocity += GRAVITY;
                    if (playerRect.y >= groundLevel) {
                        playerRect.y = groundLevel;
                        isJumping = false;
                    }
                }

                if (isAttacking && SDL_GetTicks() >= animEndTime) {
                    isAttacking = false;
                    if (moveLeft || moveRight) {
                        currentPlayerAnim = &playerRun;
                    }
                    else {
                        currentPlayerAnim = &playerIdle;
                    }
                }
            }
            else if (game.state == PLAYER_HIT) {
                if (SDL_GetTicks() - playerHitTime > PLAYER_HIT_DURATION) {
                    game.state = PLAYING;
                    playerInvincible = false;
                    if (moveLeft || moveRight) {
                        currentPlayerAnim = &playerRun;
                    }
                    else {
                        currentPlayerAnim = &playerIdle;
                    }
                }
            }

            for (size_t i = 0; i < game.explosions.size(); i++) {
                if (SDL_GetTicks() - game.explosions[i].second >= EXPLOSION_DURATION) {
                    game.explosions.erase(game.explosions.begin() + i);
                    i--;
                }
            }

            if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
                animFrame = (animFrame + 1) % currentPlayerAnim->size();
                lastAnimTime = SDL_GetTicks();
            }
        }

        SDL_RenderClear(renderer);

        if (game.state == START_SCREEN) {
            SDL_Rect startScreenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            SDL_RenderCopy(renderer, startScreenBackground, NULL, &startScreenRect);
        }
        else {
            float scale = (float)MINIMAP_WIDTH / SCREEN_WIDTH;
            RenderSceneToTexture(renderer, minimapTexture, scale, gameplayBackground, ground, playerRect,
                currentPlayerAnim, animFrame, faceLeft, game.enemies, enemyRun,
                enemyAttack, explosion, game.explosions, game.state);

            SDL_RenderCopy(renderer, gameplayBackground, NULL, NULL);

            for (int x = 0; x < SCREEN_WIDTH; x += 200) {
                SDL_Rect groundRect = { x, SCREEN_HEIGHT - GROUND_HEIGHT, 200, GROUND_HEIGHT };
                SDL_RenderCopy(renderer, ground, NULL, &groundRect);
            }

            if (game.state != GAME_OVER) {
                bool shouldRenderPlayer = true;
                if (game.state == PLAYER_HIT) {
                    shouldRenderPlayer = (SDL_GetTicks() / PLAYER_HIT_BLINK) % 2 == 0;
                }
                else if (playerInvincible) {
                    shouldRenderPlayer = (SDL_GetTicks() / PLAYER_INVINCIBLE_BLINK) % 2 == 0;
                }

                if (shouldRenderPlayer) {
                    SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                    if (game.state == PLAYER_HIT) {
                        SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 150);
                        SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                        SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 255);
                    }
                    else {
                        SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                    }
                }
            }

            for (const auto& enemy : game.enemies) {
                if (enemy.active && game.state != GAME_OVER) {
                    SDL_RendererFlip enemyFlip = enemy.faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

                    if (enemy.state == ENEMY_ATTACKING) {
                        SDL_RenderCopyEx(renderer, enemyAttack[enemy.animFrame], NULL, &enemy.rect, 0, NULL, enemyFlip);
                    }
                    else {
                        SDL_RenderCopyEx(renderer, enemyRun[enemy.animFrame], NULL, &enemy.rect, 0, NULL, enemyFlip);
                    }
                }
            }

            for (const auto& explosionData : game.explosions) {
                SDL_Rect explosionRect = {
                    explosionData.first.x - explosionData.first.w / 2,
                    explosionData.first.y - explosionData.first.h / 2,
                    explosionData.first.w * 2,
                    explosionData.first.h * 2
                };
                SDL_RenderCopy(renderer, explosion, NULL, &explosionRect);
            }

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            for (int i = 0; i < playerHealth; i++) {
                SDL_Rect healthRect = { 10 + i * 30, 10, 25, 25 };
                SDL_RenderFillRect(renderer, &healthRect);
            }

            if (game.state != GAME_OVER && game.state != PLAYER_WIN) {
                SDL_Rect minimapDest = { MINIMAP_X, MINIMAP_Y, MINIMAP_WIDTH, MINIMAP_HEIGHT };
                SDL_RenderCopy(renderer, minimapTexture, NULL, &minimapDest);
            }

            if (game.state == GAME_OVER || game.state == PLAYER_WIN) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_Rect gameOverRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_RenderFillRect(renderer, &gameOverRect);

                SDL_Texture* resultTexture = (game.state == PLAYER_WIN) ? youWin : youLose;

                int texW, texH;
                SDL_QueryTexture(resultTexture, NULL, NULL, &texW, &texH);
                SDL_Rect resultRect = {
                    SCREEN_WIDTH / 2 - texW / 2,
                    SCREEN_HEIGHT / 2 - texH / 2,
                    texW,
                    texH
                };
                SDL_RenderCopy(renderer, resultTexture, NULL, &resultRect);

                SDL_Rect buttonRect = { BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT };
                SDL_RenderCopy(renderer, playAgainButton, NULL, &buttonRect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(startScreenBackground);
    SDL_DestroyTexture(gameplayBackground);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(explosion);
    SDL_DestroyTexture(youWin);
    SDL_DestroyTexture(youLose);
    SDL_DestroyTexture(minimapTexture);
    SDL_DestroyTexture(playAgainButton);

    Mix_FreeChunk(playerRunSound);
    Mix_FreeChunk(playerAttackSound);
    Mix_FreeChunk(enemyDeathSound);
    Mix_FreeChunk(enemyAttackSound);
    Mix_FreeChunk(playerHitSound);
    Mix_CloseAudio();

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
