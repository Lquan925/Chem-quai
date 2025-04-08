#include "Game.h"
#include "Constants.h"
#include "TextureManager.h"
#include <iostream>

Game::Game() : window(nullptr), renderer(nullptr), character(nullptr), enemy(nullptr) {}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            screen_width, screen_height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load textures
    background = TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    explosionTexture = TextureManager::loadpicture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);

    if (!background || !explosionTexture) {
        std::cout << "Failed to load textures!" << std::endl;
        return false;
    }

    character = new Character(renderer);
    enemy = new Enemy(renderer);

    return true;
}

void Game::run() {
    while (!quit) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        character->handleEvent(e);
    }
}

void Game::update() {
    character->update();
    enemy->update(character->isAttacking(), character->getRect());
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Draw background
    SDL_RenderCopy(renderer, background, nullptr, nullptr);

    // Draw character
    character->render(renderer);

    // Draw enemy
    enemy->render(renderer);

    // Draw explosion if needed
    if (enemy->shouldShowExplosion() &&
        SDL_GetTicks() - enemy->getExplosionStartTime() < thoigianno) {
        SDL_Rect explosionRect = { screen_width - 250, screen_height - 165, 200, 200 };
        SDL_RenderCopy(renderer, explosionTexture, nullptr, &explosionRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    if (character) delete character;
    if (enemy) delete enemy;

    if (background) SDL_DestroyTexture(background);
    if (explosionTexture) SDL_DestroyTexture(explosionTexture);

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}
