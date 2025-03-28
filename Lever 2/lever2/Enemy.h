#pragma once
#include <SDL.h>
#include <vector>
#include "Constants.h"

class Enemy {
public:
    Enemy(SDL_Renderer* renderer);
    ~Enemy();

    void Update(const SDL_Rect& playerRect, bool& playerHit);
    void Render(SDL_Renderer* renderer);
    void Reset();

    SDL_Rect rect;
    bool isActive;

private:
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> runTextures;
    std::vector<SDL_Texture*> attackTextures;

    int animFrame;
    Uint32 lastAnimTime;
    EnemyState state;
    int direction;
    bool faceLeft;
    Uint32 lastAttackTime;
    Uint32 attackStartTime;
    bool inContact;
    Uint32 contactTime;

    void LoadTextures();
    void UpdateMovement();
    void UpdateAttack(const SDL_Rect& playerRect, bool& playerHit);
    bool CheckPlayerDistance(const SDL_Rect& playerRect);
};
