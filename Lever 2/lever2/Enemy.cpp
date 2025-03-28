#include "Enemy.h"
#include "TextureManager.h"
#include <iostream>

Enemy::Enemy(SDL_Renderer* renderer) : renderer(renderer) {
    LoadTextures();

    int width, height;
    SDL_QueryTexture(runTextures[0], NULL, NULL, &width, &height);

    const int ENEMY_AREA_START = SCREEN_WIDTH * 2 / 3;
    const int ENEMY_AREA_END = SCREEN_WIDTH - 50;

    rect = {
        ENEMY_AREA_START,
        SCREEN_HEIGHT - GROUND_HEIGHT - (int)(height * ENEMY_SCALE)*0.75,
        (int)(width * ENEMY_SCALE),
        (int)(height * ENEMY_SCALE)
    };

    Reset();
}

Enemy::~Enemy() {
    for (auto tex : runTextures) SDL_DestroyTexture(tex);
    for (auto tex : attackTextures) SDL_DestroyTexture(tex);
}

void Enemy::LoadTextures() {
    // Replace with your actual paths
    runTextures = {
        TextureManager::LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer),
        TextureManager::LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134637-removebg-preview.png", renderer),
        TextureManager::LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134703-removebg-preview.png", renderer),
        TextureManager::LoadTexture("C:\\Users\\lq865\\Downloads\\Screenshot_2025-03-25_134742-removebg-preview.png", renderer)
    };

    attackTextures = {
        TextureManager::LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144841-removebg-preview.png", renderer),
        TextureManager::LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_144928-removebg-preview.png", renderer),
        TextureManager::LoadTexture("D:\\enemy attack\\Screenshot_2025-03-25_145144-removebg-preview.png", renderer)
    };
}

void Enemy::Reset() {
    const int ENEMY_AREA_START = SCREEN_WIDTH * 2 / 3;
    rect.x = ENEMY_AREA_START;
    isActive = true;
    state = ENEMY_MOVING;
    direction = -1;
    faceLeft = true;
    animFrame = 0;
    inContact = false;
}

void Enemy::Update(const SDL_Rect& playerRect, bool& playerHit) {
    if (!isActive) return;

    if (CheckPlayerDistance(playerRect)) {
        if (!inContact) {
            inContact = true;
            contactTime = SDL_GetTicks();
        }

        if (state == ENEMY_MOVING && SDL_GetTicks() - contactTime > ENEMY_ATTACK_COOLDOWN) {
            state = ENEMY_ATTACKING;
            attackStartTime = SDL_GetTicks();
            animFrame = 0;
        }
    } else {
        inContact = false;
        state = ENEMY_MOVING;
    }

    switch (state) {
        case ENEMY_MOVING:
            UpdateMovement();
            break;
        case ENEMY_ATTACKING:
            UpdateAttack(playerRect, playerHit);
            break;
    }
}

bool Enemy::CheckPlayerDistance(const SDL_Rect& playerRect) {
    int playerCenterX = playerRect.x + playerRect.w / 2;
    int enemyCenterX = rect.x + rect.w / 2;
    return abs(playerCenterX - enemyCenterX) < ATTACK_DISTANCE;
}

void Enemy::UpdateMovement() {
    const int ENEMY_AREA_START = SCREEN_WIDTH * 2 / 3;
    const int ENEMY_AREA_END = SCREEN_WIDTH - 50;

    rect.x += ENEMY_SPEED * direction;

    if (rect.x + rect.w > ENEMY_AREA_END) {
        direction = -1;
        faceLeft = true;
    }
    else if (rect.x < ENEMY_AREA_START) {
        direction = 1;
        faceLeft = false;
    }

    if (SDL_GetTicks() - lastAnimTime > ENEMY_ANIMATION_SPEED) {
        animFrame = (animFrame + 1) % runTextures.size();
        lastAnimTime = SDL_GetTicks();
    }
}

void Enemy::UpdateAttack(const SDL_Rect& playerRect, bool& playerHit) {
    if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
        animFrame = (animFrame + 1) % attackTextures.size();
        lastAnimTime = SDL_GetTicks();
    }

    if (SDL_GetTicks() - attackStartTime > ENEMY_ATTACK_DURATION) {
        state = ENEMY_MOVING;
        animFrame = 0;
        playerHit = true;
    }
}

void Enemy::Render(SDL_Renderer* renderer) {
    if (!isActive) return;

    SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (state == ENEMY_ATTACKING) {
        SDL_RenderCopyEx(renderer, attackTextures[animFrame], NULL, &rect, 0, NULL, flip);
    } else {
        SDL_RenderCopyEx(renderer, runTextures[animFrame], NULL, &rect, 0, NULL, flip);
    }
}
