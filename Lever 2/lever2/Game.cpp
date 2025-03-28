#include "Game.h"
#include "TextureManager.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr),
               background(nullptr), ground(nullptr),
               explosion(nullptr), youWin(nullptr), youLose(nullptr),
               player(nullptr), enemy(nullptr),
               state(PLAYING), showExplosion(false) {}

Game::~Game() {
    Cleanup();
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "Error SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Level 2",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Error window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cout << "Error renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!LoadTextures()) {
        return false;
    }

    player = new Player(renderer);
    enemy = new Enemy(renderer);

    return true;
}

bool Game::LoadTextures() {
    // Replace with your actual paths
    background = TextureManager::LoadTexture("D:\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    ground = TextureManager::LoadTexture("C:\\Layer_0001_8-removebg-preview.png", renderer);
    explosion = TextureManager::LoadTexture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    youWin = TextureManager::LoadTexture("D:\\game over\\you_win (1).png", renderer);
    youLose = TextureManager::LoadTexture("D:\\game over\\you_lose.png", renderer);

    if (!background || !ground || !explosion || !youWin || !youLose) {
        std::cout << "Error loading textures" << std::endl;
        return false;
    }

    return true;
}

void Game::Run() {
    bool quit = false;
    while (!quit) {
        ProcessEvents(quit);
        Update();
        Render();
    }
}

void Game::ProcessEvents(bool& quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }

        player->HandleEvent(e, state);

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_r:
                    if (state == GAME_OVER || state == PLAYER_WIN) {
                        ResetGame();
                    }
                    break;
            }
        }
    }
}

void Game::Update() {
    if (state == PLAYING || state == PLAYER_WIN) {
        player->Update();
    }

    if (state == PLAYING) {
        bool playerHit = false;
        enemy->Update(player->rect, playerHit);

        if (playerHit && !player->isInvincible) {
            player->health--;
            player->isInvincible = true;
            player->hitTime = SDL_GetTicks();

            if (player->health <= 0) {
                state = GAME_OVER;
            } else {
                state = PLAYER_HIT;
            }
        }

        // Check if player attacked enemy
        if (player->isAttacking && enemy->isActive) {
            int playerCenterX = player->rect.x + player->rect.w / 2;
            int enemyCenterX = enemy->rect.x + enemy->rect.w / 2;
            int distance = abs(playerCenterX - enemyCenterX);

            if (distance < ATTACK_DISTANCE) {
                enemy->isActive = false;
                showExplosion = true;
                explosionStartTime = SDL_GetTicks();
                state = PLAYER_WIN;
            }
        }
    } else if (state == PLAYER_HIT) {
        player->Update();
        if (!player->isInvincible) {
            state = PLAYING;
        }
    }

    if (showExplosion && SDL_GetTicks() - explosionStartTime >= EXPLOSION_DURATION) {
        showExplosion = false;
    }
}

void Game::Render() {
    SDL_RenderClear(renderer);

    // Render background
    SDL_RenderCopy(renderer, background, NULL, NULL);

    // Render ground
    for (int x = 0; x < SCREEN_WIDTH; x += 200) {
        SDL_Rect groundRect = { x, SCREEN_HEIGHT - GROUND_HEIGHT, 200, GROUND_HEIGHT };
        SDL_RenderCopy(renderer, ground, NULL, &groundRect);
    }

    // Render player if not game over
    if (state != GAME_OVER) {
        player->Render(renderer);
    }

    // Render enemy if active and not game over
    if (enemy->isActive && state != GAME_OVER) {
        enemy->Render(renderer);
    }

    // Render explosion
    if (showExplosion) {
        SDL_Rect explosionRect = {
            enemy->rect.x - enemy->rect.w/2,
            enemy->rect.y - enemy->rect.h/2,
            enemy->rect.w * 2,
            enemy->rect.h * 2
        };
        SDL_RenderCopy(renderer, explosion, NULL, &explosionRect);
    }

    // Render health
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < player->health; i++) {
        SDL_Rect healthRect = {10 + i * 30, 10, 25, 25};
        SDL_RenderFillRect(renderer, &healthRect);
    }

    // Render game over/win screen
    if (state == GAME_OVER || state == PLAYER_WIN) {
        RenderGameOver();
    }

    SDL_RenderPresent(renderer);
}

void Game::RenderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_Texture* result = (state == PLAYER_WIN) ? youWin : youLose;
    int texW, texH;
    SDL_QueryTexture(result, NULL, NULL, &texW, &texH);
    SDL_Rect resultRect = {
        SCREEN_WIDTH/2 - texW/2,
        SCREEN_HEIGHT/2 - texH/2,
        texW,
        texH
    };
    SDL_RenderCopy(renderer, result, NULL, &resultRect);
}

void Game::ResetGame() {
    player->Reset();
    enemy->Reset();
    state = PLAYING;
    showExplosion = false;
}

void Game::Cleanup() {
    delete player;
    delete enemy;

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(explosion);
    SDL_DestroyTexture(youWin);
    SDL_DestroyTexture(youLose);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
