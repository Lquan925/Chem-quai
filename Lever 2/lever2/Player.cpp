#include "Player.h"
#include "TextureManager.h"
#include <iostream>

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {
    LoadTextures();

    int width, height;
    SDL_QueryTexture(playerIdle[0], NULL, NULL, &width, &height);

    rect = {
        50,
        SCREEN_HEIGHT - GROUND_HEIGHT - static_cast<int>(height * PLAYER_SCALE),
        static_cast<int>(width * PLAYER_SCALE),
        static_cast<int>(height * PLAYER_SCALE)
    };

    groundLevel = rect.y;
    health = 3;
    currentAnim = &playerIdle;
    Reset();
}

Player::~Player() {
    for (auto tex : playerRun) SDL_DestroyTexture(tex);
    for (auto tex : playerAttack) SDL_DestroyTexture(tex);
    for (auto tex : playerIdle) SDL_DestroyTexture(tex);
}

void Player::LoadTextures() {
    // Replace with your actual paths
    playerRun = {
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_2_resized_1.5x.png", renderer),
    };

    playerAttack = {
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };

    playerIdle = {
        TextureManager::LoadTexture("D:\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };
}

void Player::Reset() {
    rect.x = 50;
    rect.y = groundLevel;
    health = 3;
    currentAnim = &playerIdle;
    animFrame = 0;
    isJumping = false;
    moveLeft = moveRight = false;
    faceLeft = true;
    isAttacking = false;
    isInvincible = false;
}

void Player::Update() {
    UpdatePosition();
    UpdateAnimation();
}

void Player::UpdatePosition() {
    if (moveLeft) {
        rect.x -= PLAYER_SPEED;
        if (rect.x < 0) rect.x = 0;
        if (!isAttacking) currentAnim = &playerRun;
    }

    if (moveRight) {
        rect.x += PLAYER_SPEED;
        if (rect.x + rect.w > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.w;
        if (!isAttacking) currentAnim = &playerRun;
    }

    if (isJumping) {
        rect.y += jumpVelocity;
        jumpVelocity += GRAVITY;
        if (rect.y >= groundLevel) {
            rect.y = groundLevel;
            isJumping = false;
        }
    }

    if (isAttacking && SDL_GetTicks() >= animEndTime) {
        isAttacking = false;
        if (moveLeft || moveRight) {
            currentAnim = &playerRun;
        } else {
            currentAnim = &playerIdle;
        }
    }

    if (isInvincible && SDL_GetTicks() - hitTime > PLAYER_HIT_DURATION) {
        isInvincible = false;
    }
}

void Player::UpdateAnimation() {
    if (SDL_GetTicks() - lastAnimTime > ANIMATION_SPEED) {
        animFrame = (animFrame + 1) % currentAnim->size();
        lastAnimTime = SDL_GetTicks();
    }
}

void Player::HandleEvent(SDL_Event& e, GameState gameState) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                moveLeft = true;
                faceLeft = true;
                if (!isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentAnim = &playerRun;
                break;
            case SDLK_RIGHT:
                moveRight = true;
                faceLeft = false;
                if (!isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentAnim = &playerRun;
                break;
            case SDLK_SPACE:
                if (!isJumping && (gameState == PLAYING || gameState == PLAYER_WIN)) {
                    isJumping = true;
                    jumpVelocity = -JUMP_HEIGHT;
                }
                break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                moveLeft = false;
                if (!moveRight && !isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentAnim = &playerIdle;
                break;
            case SDLK_RIGHT:
                moveRight = false;
                if (!moveLeft && !isAttacking && (gameState == PLAYING || gameState == PLAYER_WIN)) currentAnim = &playerIdle;
                break;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && (gameState == PLAYING || gameState == PLAYER_WIN)) {
        if (!isAttacking) {
            isAttacking = true;
            animFrame = 0;
            lastAnimTime = SDL_GetTicks();
            animEndTime = lastAnimTime + (playerAttack.size() * ANIMATION_SPEED);
            currentAnim = &playerAttack;
        }
    }
}

void Player::Render(SDL_Renderer* renderer) {
    bool shouldRender = true;
    if (isInvincible) {
        shouldRender = (SDL_GetTicks() / PLAYER_INVINCIBLE_BLINK) % 2 == 0;
    }

    if (shouldRender) {
        SDL_RendererFlip flip = faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderCopyEx(renderer, (*currentAnim)[animFrame], NULL, &rect, 0, NULL, flip);
    }
}
