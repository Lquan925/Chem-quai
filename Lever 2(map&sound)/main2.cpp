#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
using namespace std;

// Các hằng số cấu hình
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 5;
const int JUMP_HEIGHT = 12;
const int GRAVITY = 1;
const int ANIMATION_SPEED = 100;
const int ATTACK_DISTANCE = 80;
const int EXPLOSION_DURATION = 500;
const int ENEMY_SPEED = 2;
const int ENEMY_ANIMATION_SPEED = 150;
const int ENEMY_ATTACK_ANIMATION_SPEED = 80;
const int ENEMY_ATTACK_DURATION = 500;
const float PLAYER_SCALE = 1.0f;
const float ENEMY_SCALE = 0.3f;
const int GROUND_HEIGHT = 50;
const int ENEMY_ATTACK_COOLDOWN = 3000;
const int PLAYER_HIT_DURATION = 1000;
const int PLAYER_INVINCIBLE_BLINK = 100;

// Cấu hình minimap
const float MINIMAP_SCALE = 0.25f;
const int MINIMAP_WIDTH = SCREEN_WIDTH * MINIMAP_SCALE;
const int MINIMAP_HEIGHT = SCREEN_HEIGHT * MINIMAP_SCALE;
const int MINIMAP_X = SCREEN_WIDTH - MINIMAP_WIDTH - 10;
const int MINIMAP_Y = 10;

// Kênh âm thanh
const int CHANNEL_PLAYER = 1;
const int CHANNEL_ENEMY = 2;
const int CHANNEL_EFFECTS = 3;

// Trạng thái game
enum GameState {
    PLAYING,
    PLAYER_HIT,
    GAME_OVER,
    PLAYER_WIN
};

// Trạng thái enemy
enum EnemyState {
    ENEMY_RUNNING,
    ENEMY_ATTACKING,
    ENEMY_DEAD
};

// Hàm tải texture
SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "Error loading image: " << IMG_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hàm tải âm thanh
Mix_Chunk* LoadSound(const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (!sound) {
        cout << "Error loading sound: " << Mix_GetError() << endl;
    }
    return sound;
}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Error initializing SDL_image: " << IMG_GetError() << endl;
        return -1;
    }

    // Khởi tạo SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "Error initializing SDL_mixer: " << Mix_GetError() << endl;
        return -1;
    }

    // Tạo cửa sổ
    SDL_Window* window = SDL_CreateWindow("Game with Enemy Attack",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        SCREEN_WIDTH,
                                        SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Error creating window: " << SDL_GetError() << endl;
        return -1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cout << "Error creating renderer: " << SDL_GetError() << endl;
        return -1;
    }

    // Tải texture
    SDL_Texture* background = LoadTexture("D:\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    SDL_Texture* ground = LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);

    // Texture nhân vật
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

    // Texture quái vật
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

    // Tải âm thanh
    Mix_Chunk* playerRunSound = LoadSound("D:\\Âm thanh\\run.wav");
    Mix_Chunk* playerAttackSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* enemyDeathSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* enemyAttackSound = LoadSound("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* playerHitSound = LoadSound("D:\\Âm thanh\\fight.wav");

    // Thiết lập volume
    Mix_Volume(CHANNEL_PLAYER, MIX_MAX_VOLUME / 2);
    Mix_Volume(CHANNEL_ENEMY, MIX_MAX_VOLUME / 2);
    Mix_Volume(CHANNEL_EFFECTS, MIX_MAX_VOLUME / 2);

    // Kiểm tra tất cả texture và âm thanh
    bool assetsLoaded = background && ground && explosion && youWin && youLose &&
                       playerRunSound && playerAttackSound && enemyDeathSound &&
                       enemyAttackSound && playerHitSound;

    for (auto tex : playerRun) if (!tex) assetsLoaded = false;
    for (auto tex : playerAttack) if (!tex) assetsLoaded = false;
    for (auto tex : playerIdle) if (!tex) assetsLoaded = false;
    for (auto tex : enemyRun) if (!tex) assetsLoaded = false;
    for (auto tex : enemyAttack) if (!tex) assetsLoaded = false;

    if (!assetsLoaded) {
        cout << "Failed to load some assets." << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // Thiết lập player
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

    // Thiết lập enemy
    int enemyWidth, enemyHeight;
    SDL_QueryTexture(enemyRun[0], NULL, NULL, &enemyWidth, &enemyHeight);

    const int ENEMY_AREA_START = SCREEN_WIDTH * 2 / 3;
    const int ENEMY_AREA_END = SCREEN_WIDTH - 50;

    SDL_Rect enemyRect = {
        ENEMY_AREA_START,
        SCREEN_HEIGHT - GROUND_HEIGHT - (int)(enemyHeight * ENEMY_SCALE)*0.75,
        (int)(enemyWidth * ENEMY_SCALE),
        (int)(enemyHeight * ENEMY_SCALE)
    };

    int enemyDirection = -1;
    bool enemyFaceLeft = true;
    bool enemyActive = true;
    Uint32 lastEnemyAttackTime = 0;
    int enemyAnimFrame = 0;
    Uint32 lastEnemyAnimTime = 0;
    EnemyState enemyState = ENEMY_RUNNING;
    bool enemyIsAttacking = false;
    Uint32 enemyAttackStartTime = 0;
    int enemyAttackAnimFrame = 0;
    vector<SDL_Texture*>* currentEnemyAnim = &enemyRun;

    // Biến trạng thái player
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

    // Hiệu ứng
    bool showExplosion = false;
    Uint32 explosionStartTime = 0;
    GameState gameState = PLAYING;

    // Tạo texture cho minimap
    SDL_Texture* minimapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                  SDL_TEXTUREACCESS_TARGET,
                                                  MINIMAP_WIDTH, MINIMAP_HEIGHT);
    if (!minimapTexture) {
        cout << "Error creating minimap texture: " << SDL_GetError() << endl;
    }

    // Vòng lặp chính
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = true;
                        faceLeft = true;
                        if (!isAttacking && gameState == PLAYING) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        faceLeft = false;
                        if (!isAttacking && gameState == PLAYING) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_SPACE:
                        if (!isJumping && gameState == PLAYING) {
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
                            enemyActive = true;
                            enemyRect.x = ENEMY_AREA_START;
                            enemyState = ENEMY_RUNNING;
                            enemyIsAttacking = false;
                            Mix_HaltChannel(CHANNEL_PLAYER);
                        }
                        break;
                }
            }

            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = false;
                        if (!moveRight && !isAttacking && gameState == PLAYING) currentPlayerAnim = &playerIdle;
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        if (!moveLeft && !isAttacking && gameState == PLAYING) currentPlayerAnim = &playerIdle;
                        break;
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && gameState == PLAYING) {
                if (!isAttacking) {
                    isAttacking = true;
                    animFrame = 0;
                    lastAnimTime = SDL_GetTicks();
                    animEndTime = lastAnimTime + (playerAttack.size() * ANIMATION_SPEED);
                    currentPlayerAnim = &playerAttack;
                    Mix_PlayChannel(CHANNEL_EFFECTS, playerAttackSound, 0);

                    // Kiểm tra tấn công enemy
                    if (enemyActive && !enemyIsAttacking) {
                        int playerCenterX = playerRect.x + playerRect.w / 2;
                        int enemyCenterX = enemyRect.x + enemyRect.w / 2;
                        int distance = abs(playerCenterX - enemyCenterX);

                        if (distance < ATTACK_DISTANCE) {
                            enemyActive = false;
                            enemyState = ENEMY_DEAD;
                            showExplosion = true;
                            explosionStartTime = SDL_GetTicks();
                            Mix_PlayChannel(CHANNEL_EFFECTS, enemyDeathSound, 0);
                            gameState = PLAYER_WIN;
                        }
                    }
                }
            }
        }

        // Phát âm thanh di chuyển
        bool isMoving = moveLeft || moveRight;
        if (isMoving && !wasMoving) {
            Mix_PlayChannel(CHANNEL_PLAYER, playerRunSound, -1); // Lặp âm thanh chạy
        }
        else if (!isMoving && wasMoving) {
            Mix_HaltChannel(CHANNEL_PLAYER); // Dừng âm thanh chạy
        }
        wasMoving = isMoving;

        // Cập nhật game
        if (gameState == PLAYING) {
            // Cập nhật enemy
            if (enemyActive) {
                // Kiểm tra khoảng cách với player
                int playerCenterX = playerRect.x + playerRect.w / 2;
                int enemyCenterX = enemyRect.x + enemyRect.w / 2;
                int distance = abs(playerCenterX - enemyCenterX);

                if (distance < ATTACK_DISTANCE) {
                    // Enemy trong phạm vi tấn công
                    if (!enemyIsAttacking && SDL_GetTicks() - lastEnemyAttackTime > ENEMY_ATTACK_COOLDOWN) {
                        enemyIsAttacking = true;
                        enemyState = ENEMY_ATTACKING;
                        enemyAttackStartTime = SDL_GetTicks();
                        enemyAttackAnimFrame = 0;
                        currentEnemyAnim = &enemyAttack;
                        Mix_PlayChannel(CHANNEL_ENEMY, enemyAttackSound, 0);
                    }
                } else {
                    // Enemy di chuyển
                    if (!enemyIsAttacking) {
                        enemyRect.x += ENEMY_SPEED * enemyDirection;

                        if (enemyRect.x + enemyRect.w > ENEMY_AREA_END) {
                            enemyDirection = -1;
                            enemyFaceLeft = true;
                        }
                        else if (enemyRect.x < ENEMY_AREA_START) {
                            enemyDirection = 1;
                            enemyFaceLeft = false;
                        }
                    }
                }

                // Cập nhật animation enemy
                if (enemyIsAttacking) {
                    if (SDL_GetTicks() - enemyAttackStartTime > ENEMY_ATTACK_DURATION) {
                        enemyIsAttacking = false;
                        enemyState = ENEMY_RUNNING;
                        currentEnemyAnim = &enemyRun;
                        lastEnemyAttackTime = SDL_GetTicks();
                    } else if (SDL_GetTicks() - lastEnemyAnimTime > ENEMY_ATTACK_ANIMATION_SPEED) {
                        enemyAttackAnimFrame = (enemyAttackAnimFrame + 1) % enemyAttack.size();
                        lastEnemyAnimTime = SDL_GetTicks();

                        // Kiểm tra va chạm khi enemy tấn công
                        if (enemyAttackAnimFrame == 2 && !playerInvincible) { // Frame tấn công chính
                            int playerCenterX = playerRect.x + playerRect.w / 2;
                            int enemyCenterX = enemyRect.x + enemyRect.w / 2;
                            int distance = abs(playerCenterX - enemyCenterX);

                            if (distance < ATTACK_DISTANCE) {
                                playerHealth--;
                                playerInvincible = true;
                                playerHitTime = SDL_GetTicks();
                                Mix_PlayChannel(CHANNEL_EFFECTS, playerHitSound, 0);

                                if (playerHealth <= 0) {
                                    gameState = GAME_OVER;
                                    Mix_HaltChannel(CHANNEL_PLAYER);
                                } else {
                                    gameState = PLAYER_HIT;
                                }
                            }
                        }
                    }
                } else if (SDL_GetTicks() - lastEnemyAnimTime > ENEMY_ANIMATION_SPEED) {
                    enemyAnimFrame = (enemyAnimFrame + 1) % enemyRun.size();
                    lastEnemyAnimTime = SDL_GetTicks();
                }
            }

            // Cập nhật player
            if (moveLeft) {
                playerRect.x -= PLAYER_SPEED;
                if (playerRect.x < 0) playerRect.x = 0;
                if (!isAttacking) currentPlayerAnim = &playerRun;
            }

            if (moveRight) {
                playerRect.x += PLAYER_SPEED;
                if (playerRect.x + playerRect.w > SCREEN_WIDTH) {
                    playerRect.x = SCREEN_WIDTH - playerRect.w;
                }
                if (!isAttacking) currentPlayerAnim = &playerRun;
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

        if (showExplosion && SDL_GetTicks() - explosionStartTime >= EXPLOSION_DURATION) {
            showExplosion = false;
        }

        // Cập nhật animation player
        if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
            animFrame = (animFrame + 1) % currentPlayerAnim->size();
            lastAnimTime = SDL_GetTicks();
        }

        // Render minimap
        SDL_SetRenderTarget(renderer, minimapTexture);
        SDL_RenderClear(renderer);

        // Vẽ background thu nhỏ
        SDL_Rect bgRect = {0, 0, MINIMAP_WIDTH, MINIMAP_HEIGHT};
        SDL_RenderCopy(renderer, background, NULL, &bgRect);

        // Vẽ player trên minimap
        SDL_Rect minimapPlayerRect = {
            (int)(playerRect.x * MINIMAP_SCALE),
            (int)(playerRect.y * MINIMAP_SCALE),
            (int)(playerRect.w * MINIMAP_SCALE),
            (int)(playerRect.h * MINIMAP_SCALE)
        };
        SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        if (!playerInvincible || (SDL_GetTicks() / PLAYER_INVINCIBLE_BLINK) % 2 == 0) {
            SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &minimapPlayerRect, 0, NULL, flip);
        }

        // Vẽ enemy trên minimap
        if (enemyActive && enemyState != ENEMY_DEAD) {
            SDL_Rect minimapEnemyRect = {
                (int)(enemyRect.x * MINIMAP_SCALE),
                (int)(enemyRect.y * MINIMAP_SCALE),
                (int)(enemyRect.w * MINIMAP_SCALE),
                (int)(enemyRect.h * MINIMAP_SCALE)
            };
            SDL_RendererFlip enemyFlip = enemyFaceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_Texture* currentEnemyFrame = (enemyState == ENEMY_ATTACKING) ?
                (*currentEnemyAnim)[enemyAttackAnimFrame] :
                (*currentEnemyAnim)[enemyAnimFrame];
            SDL_RenderCopyEx(renderer, currentEnemyFrame, NULL, &minimapEnemyRect, 0, NULL, enemyFlip);
        }

        // Vẽ explosion trên minimap
        if (showExplosion) {
            SDL_Rect explosionRect = {
                enemyRect.x - enemyRect.w/2,
                enemyRect.y - enemyRect.h/2,
                enemyRect.w * 2,
                enemyRect.h * 2
            };
            SDL_Rect minimapExplosionRect = {
                (int)(explosionRect.x * MINIMAP_SCALE),
                (int)(explosionRect.y * MINIMAP_SCALE),
                (int)(explosionRect.w * MINIMAP_SCALE),
                (int)(explosionRect.h * MINIMAP_SCALE)
            };
            SDL_RenderCopy(renderer, explosion, NULL, &minimapExplosionRect);
        }

        // Trở lại render bình thường
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderClear(renderer);

        // Vẽ màn hình chính
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // Vẽ ground
        SDL_Rect groundRect = {0, SCREEN_HEIGHT - GROUND_HEIGHT, SCREEN_WIDTH, GROUND_HEIGHT};
        SDL_RenderCopy(renderer, ground, NULL, &groundRect);

        // Vẽ player
        if (!playerInvincible || (SDL_GetTicks() / PLAYER_INVINCIBLE_BLINK) % 2 == 0) {
            SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
        }

        // Vẽ enemy
        if (enemyActive && enemyState != ENEMY_DEAD) {
            SDL_RendererFlip enemyFlip = enemyFaceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_Texture* currentEnemyFrame = (enemyState == ENEMY_ATTACKING) ?
                (*currentEnemyAnim)[enemyAttackAnimFrame] :
                (*currentEnemyAnim)[enemyAnimFrame];
            SDL_RenderCopyEx(renderer, currentEnemyFrame, NULL, &enemyRect, 0, NULL, enemyFlip);
        }

        // Vẽ explosion
        if (showExplosion) {
            SDL_Rect explosionRect = {
                enemyRect.x - enemyRect.w/2,
                enemyRect.y - enemyRect.h/2,
                enemyRect.w * 2,
                enemyRect.h * 2
            };
            SDL_RenderCopy(renderer, explosion, NULL, &explosionRect);
        }

        // Vẽ minimap lên màn hình
        SDL_Rect minimapDestRect = {MINIMAP_X, MINIMAP_Y, MINIMAP_WIDTH, MINIMAP_HEIGHT};
        SDL_RenderCopy(renderer, minimapTexture, NULL, &minimapDestRect);

        // Vẽ khung minimap
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &minimapDestRect);

        // Hiển thị máu player
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < playerHealth; i++) {
            SDL_Rect healthRect = {10 + i * 30, 10, 25, 25};
            SDL_RenderFillRect(renderer, &healthRect);
        }

        // Hiển thị game over hoặc you win
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

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_Rect restartRect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 40};
            SDL_RenderDrawRect(renderer, &restartRect);
        }

        SDL_RenderPresent(renderer);
    }

    // Giải phóng bộ nhớ
    Mix_FreeChunk(playerRunSound);
    Mix_FreeChunk(playerAttackSound);
    Mix_FreeChunk(enemyDeathSound);
    Mix_FreeChunk(enemyAttackSound);
    Mix_FreeChunk(playerHitSound);
    Mix_CloseAudio();

    SDL_DestroyTexture(minimapTexture);
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
