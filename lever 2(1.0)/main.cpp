#include <SDL.h>
#include <SDL_image.h>
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
const float PLAYER_SCALE = 1.0f;
const float ENEMY_SCALE = 0.3f;
const int GROUND_HEIGHT = 50;
const int ENEMY_ATTACK_COOLDOWN = 400;
const int ENEMY_ATTACK_DURATION = 1000;
const int PLAYER_HIT_DURATION = 1000;
const int PLAYER_HIT_BLINK = 100;
const int PLAYER_INVINCIBLE_BLINK = 100;

// Trạng thái game
enum GameState {PLAYING,PLAYER_HIT,GAME_OVER,PLAYER_WIN};

// Trạng thái enemy
enum EnemyState {ENEMY_MOVING,ENEMY_ATTACKING,ENEMY_COOLDOWN};

// Hàm tải texture
SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        cout << "Error Picture: " << IMG_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Error SDL: " << SDL_GetError() << endl;
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Error SDL_image: " << IMG_GetError() << endl;
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("Level 2",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Error window: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cout << "Error renderer: " << SDL_GetError() << endl;
        return -1;
    }

    // Tải texture
    SDL_Texture* background = LoadTexture("D:\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    SDL_Texture* ground = LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);

    // Texture nhân vật
    vector<SDL_Texture*> playerRun = {
        LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_2_resized_1.5x.png", renderer),
    };

    vector<SDL_Texture*> playerAttack = {
        LoadTexture("D:\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        LoadTexture("D:\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };

    vector<SDL_Texture*> playerIdle = {
        LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };

    // Texture quái vật
    vector<SDL_Texture*> enemyRun = {
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134637-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134703-removebg-preview.png", renderer),
        LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134742-removebg-preview.png", renderer)
    };

    // Texture quái vật tấn công
    vector<SDL_Texture*> enemyAttack = {
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144841-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144928-removebg-preview.png", renderer),
        LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_145144-removebg-preview.png", renderer)
    };

    SDL_Texture* explosion = LoadTexture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);

    // Texture kết thúc game
    SDL_Texture* youWin = LoadTexture("D:\\game over\\you_win (1).png", renderer);
    SDL_Texture* youLose = LoadTexture("D:\\game over\\you_lose.png", renderer);

    // Kiểm tra tất cả texture
    bool texturesLoaded = background && ground && explosion && youWin && youLose;
    for (auto tex : playerRun)
        if (!tex)
            texturesLoaded = false;
    for (auto tex : playerAttack)
        if (!tex)
            texturesLoaded = false;
    for (auto tex : playerIdle)
        if (!tex)
            texturesLoaded = false;
    for (auto tex : enemyRun)
        if (!tex)
        texturesLoaded = false;
    for (auto tex : enemyAttack)
        if (!tex)
            texturesLoaded = false;

    if (!texturesLoaded) {
        cout << "Error texture." << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    // Thiết lập player
    int playerWidth, playerHeight;
    SDL_QueryTexture(playerIdle[0], NULL, NULL, &playerWidth, &playerHeight);
    SDL_Rect playerRect = {50,SCREEN_HEIGHT - GROUND_HEIGHT - (int)(playerHeight * PLAYER_SCALE),(int)(playerWidth * PLAYER_SCALE),(int)(playerHeight * PLAYER_SCALE)
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

    int enemyDirection = -1;// di chuyển hướng trái
    bool enemyFaceLeft = true;
    bool enemyActive = true;
    Uint32 lastEnemyAttackTime = 0;
    Uint32 enemyAttackStartTime = 0;
    int enemyAnimFrame = 0;
    Uint32 lastEnemyAnimTime = 0;
    EnemyState enemyState = ENEMY_MOVING;
    Uint32 enemyContactTime = 0;
    bool enemyInContact = false;

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

    // Hiệu ứng
    bool showExplosion = false;
    Uint32 explosionStartTime = 0;
    GameState gameState = PLAYING;

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
                        if (!isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        faceLeft = false;
                        if (!isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentPlayerAnim = &playerRun;
                        break;
                    case SDLK_SPACE:
                        if (!isJumping && (gameState == PLAYING || gameState == PLAYER_WIN)) {
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
                            enemyState = ENEMY_MOVING;
                            showExplosion = false;
                        }
                        break;
                }
            }

            if (e.type == SDL_KEYUP) {// nhả phím
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        moveLeft = false;
                        if (!moveRight && !isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentPlayerAnim = &playerIdle;
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        if (!moveLeft && !isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentPlayerAnim = &playerIdle;
                        break;
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && (gameState == PLAYING || gameState == PLAYER_WIN)) {
                if (!isAttacking) {
                    isAttacking = true;
                    animFrame = 0;
                    lastAnimTime = SDL_GetTicks();
                    animEndTime = lastAnimTime + (playerAttack.size() * ANIMATION_SPEED);
                    currentPlayerAnim = &playerAttack;

                    // Kiểm tra tấn công enemy
                    if (enemyActive) {
                        int playerCenterX = playerRect.x + playerRect.w / 2;
                        int enemyCenterX = enemyRect.x + enemyRect.w / 2;
                        int distance = abs(playerCenterX - enemyCenterX);

                        if (distance < ATTACK_DISTANCE) {
                            enemyActive = false;
                            showExplosion = true;
                            explosionStartTime = SDL_GetTicks();
                            gameState = PLAYER_WIN;
                        }
                    }
                }
            }
        }

        // Cập nhật game
        if (gameState == PLAYING || gameState == PLAYER_WIN) {
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
        }

        if (gameState == PLAYING) {
            // Cập nhật enemy
            if (enemyActive) {
                // Kiểm tra khoảng cách với player
                int playerCenterX = playerRect.x + playerRect.w / 2;
                int enemyCenterX = enemyRect.x + enemyRect.w / 2;
                int distance = abs(playerCenterX - enemyCenterX);

                if (distance < ATTACK_DISTANCE) {
                    if (!enemyInContact) {
                        enemyInContact = true;
                        enemyContactTime = SDL_GetTicks();
                    }

                    if (enemyState == ENEMY_MOVING && SDL_GetTicks() - enemyContactTime > ENEMY_ATTACK_COOLDOWN) {
                        enemyState = ENEMY_ATTACKING;
                        enemyAttackStartTime = SDL_GetTicks();
                        enemyAnimFrame = 0;
                    }
                } else {
                    enemyInContact = false;
                    enemyState = ENEMY_MOVING;
                }

                switch (enemyState) {
                    case ENEMY_MOVING:
                        enemyRect.x += ENEMY_SPEED * enemyDirection;

                        if (enemyRect.x + enemyRect.w > ENEMY_AREA_END) {
                            enemyDirection = -1;
                            enemyFaceLeft = true;
                        }
                        else if (enemyRect.x < ENEMY_AREA_START) {
                            enemyDirection = 1;
                            enemyFaceLeft = false;
                        }

                        if (SDL_GetTicks() - lastEnemyAnimTime > ENEMY_ANIMATION_SPEED) {
                            enemyAnimFrame = (enemyAnimFrame + 1) % enemyRun.size();
                            lastEnemyAnimTime = SDL_GetTicks();
                        }
                        break;

                    case ENEMY_ATTACKING:
                        if (SDL_GetTicks() - lastEnemyAnimTime > ANIMATION_SPEED) {
                            enemyAnimFrame = (enemyAnimFrame + 1) % enemyAttack.size();
                            lastEnemyAnimTime = SDL_GetTicks();
                        }

                        if (SDL_GetTicks() - enemyAttackStartTime > ENEMY_ATTACK_DURATION) {
                            enemyState = ENEMY_MOVING;
                            enemyAnimFrame = 0;

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

            // Cập nhật jump
            if (isJumping) {
                playerRect.y += jumpVelocity;
                jumpVelocity += GRAVITY;
                if (playerRect.y >= groundLevel) {
                    playerRect.y = groundLevel;
                    isJumping = false;
                }
            }

            // Kết thúc animation tấn công
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
            // Chờ hết thời gian hiệu ứng
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

        // Kiểm tra explosion hết thời gian
        if (showExplosion && SDL_GetTicks() - explosionStartTime >= EXPLOSION_DURATION) {
            showExplosion = false;
        }

        // Cập nhật animation player
        if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
            animFrame = (animFrame + 1) % currentPlayerAnim->size();
            lastAnimTime = SDL_GetTicks();
        }

        // Render
        SDL_RenderClear(renderer);

        // Vẽ background
        SDL_RenderCopy(renderer, background, NULL, NULL);

        // Vẽ ground
        for (int x = 0; x < SCREEN_WIDTH; x += 200) {
            SDL_Rect groundRect = { x, SCREEN_HEIGHT - GROUND_HEIGHT, 200, GROUND_HEIGHT };
            SDL_RenderCopy(renderer, ground, NULL, &groundRect);
        }

        // Vẽ player (chỉ khi không phải GAME_OVER)
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

                // Áp dụng hiệu ứng mờ nếu đang bị đánh
                if (gameState == PLAYER_HIT) {
                    SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 150);
                    SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                    SDL_SetTextureAlphaMod((*currentPlayerAnim)[animFrame], 255);
                } else {
                    SDL_RenderCopyEx(renderer, (*currentPlayerAnim)[animFrame], NULL, &playerRect, 0, NULL, flip);
                }
            }
        }

        // Vẽ enemy
        if (enemyActive && gameState != GAME_OVER) {
            SDL_RendererFlip enemyFlip = enemyFaceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

            if (enemyState == ENEMY_ATTACKING) {
                SDL_RenderCopyEx(renderer, enemyAttack[enemyAnimFrame], NULL, &enemyRect, 0, NULL, enemyFlip);
            } else {
                SDL_RenderCopyEx(renderer, enemyRun[enemyAnimFrame], NULL, &enemyRect, 0, NULL, enemyFlip);
            }
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

        // Hiển thị máu player
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < playerHealth; i++) {
            SDL_Rect healthRect = {10 + i * 30, 10, 25, 25};
            SDL_RenderFillRect(renderer, &healthRect);
        }

        // Hiển thị kết thúc game (ĐÃ SỬA - XÓA HÌNH CHỮ NHẬT)
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

    // Giải phóng bộ nhớ
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
