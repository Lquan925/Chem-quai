#pragma once
#include <SDL.h>

class Enemy {
public:
    Enemy(SDL_Renderer* renderer);
    ~Enemy();

    void update(bool isCharacterAttacking, const SDL_Rect& characterRect);
    void render(SDL_Renderer* renderer);

    bool isAlive() const { return checkEnemy; }
    bool shouldShowExplosion() const { return showExplosion; }
    Uint32 getExplosionStartTime() const { return explosionStartTime; }

private:
    SDL_Texture* enemyTexture;
    SDL_Rect enemyRect;
    bool checkEnemy = true;
    bool showExplosion = false;
    Uint32 explosionStartTime = 0;
};
