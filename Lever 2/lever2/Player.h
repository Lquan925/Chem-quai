#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.h"

class Player {
public:
    Player(SDL_Renderer* renderer);
    ~Player();

    void Update();
    void Render(SDL_Renderer* renderer);
    void HandleEvent(SDL_Event& e, GameState gameState);
    void Reset();

    SDL_Rect rect;
    int health;
    bool isInvincible;
    bool isAttacking;
    Uint32 hitTime;

private:
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> playerRun;
    std::vector<SDL_Texture*> playerAttack;
    std::vector<SDL_Texture*> playerIdle;

    std::vector<SDL_Texture*>* currentAnim;
    int animFrame;
    Uint32 lastAnimTime;
    Uint32 animEndTime;

    bool isJumping;
    int jumpVelocity;
    bool moveLeft, moveRight;
    bool faceLeft;
    int groundLevel;

    void LoadTextures();
    void UpdatePosition();
    void UpdateAnimation();
};
