#include "Enemy.h"
#include "Constants.h"
#include "TextureManager.h"

Enemy::Enemy(SDL_Renderer* renderer) {
    enemyTexture = TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\enemy\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer);
    enemyRect = { screen_width - 200, screen_height - 115, 75, 75 };
}

Enemy::~Enemy() {
    SDL_DestroyTexture(enemyTexture);
}

void Enemy::update(bool isCharacterAttacking, const SDL_Rect& characterRect) {
    if (checkEnemy && isCharacterAttacking) {
        int charCenterX = characterRect.x + characterRect.w / 2;
        int enemyCenterX = enemyRect.x + enemyRect.w / 2;
        int distanceToEnemy = abs(charCenterX - enemyCenterX);

        if (distanceToEnemy < distance_max_attack) {
            checkEnemy = false;
            showExplosion = true;
            explosionStartTime = SDL_GetTicks();
        }
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    if (checkEnemy) {
        SDL_RenderCopyEx(renderer, enemyTexture, nullptr, &enemyRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
    }
}
