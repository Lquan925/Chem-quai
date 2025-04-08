#pragma once
#include <SDL.h>
#include "Character.h"
#include "Enemy.h"

class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Texture* background = nullptr;
    SDL_Texture* explosionTexture = nullptr;

    Character* character = nullptr;
    Enemy* enemy = nullptr;

    void handleEvents();
    void update();
    void render();
    void clean();

    bool quit = false;
};
