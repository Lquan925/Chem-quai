#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"

class Game {
public:
    Game();
    ~Game();

    bool Initialize();
    void Run();
    void Cleanup();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* background;
    SDL_Texture* ground;
    SDL_Texture* explosion;
    SDL_Texture* youWin;
    SDL_Texture* youLose;

    Player* player;
    Enemy* enemy;

    GameState state;
    bool showExplosion;
    Uint32 explosionStartTime;

    bool LoadTextures();
    void ProcessEvents(bool& quit);
    void Update();
    void Render();
    void RenderGameOver();
    void ResetGame();
};
