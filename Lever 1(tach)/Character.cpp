#include "Character.h"
#include "Constants.h"
#include "TextureManager.h"
#include <iostream>

Character::Character(SDL_Renderer* renderer) {
    charRun = {
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_2_resized_1.5x.png", renderer)
    };

    charAttack = {
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };

    charBack_afterAttack = {
        TextureManager::loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };

    current = &charBack_afterAttack;

    charRect = { 90, screen_height - 40 - 50, 40, 40 };
    groundLevel = charRect.y;
}

Character::~Character() {
    for (auto tex : charRun) SDL_DestroyTexture(tex);
    for (auto tex : charAttack) SDL_DestroyTexture(tex);
    for (auto tex : charBack_afterAttack) SDL_DestroyTexture(tex);
}

void Character::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                moveLeft = true;
                faceLeft = true;
                if (!isAnimating) current = &charRun;
                break;
            case SDLK_RIGHT:
                moveRight = true;
                faceLeft = false;
                if (!isAnimating) current = &charRun;
                break;
            case SDLK_SPACE:
                if (!isJumping) {
                    isJumping = true;
                    Vjump = -jump_height / 10;
                }
                break;
        }
    }

    if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
            case SDLK_LEFT:
                moveLeft = false;
                if (!isAnimating && !moveRight && !isJumping) current = &charBack_afterAttack;
                break;
            case SDLK_RIGHT:
                moveRight = false;
                if (!isAnimating && !moveLeft && !isJumping) current = &charBack_afterAttack;
                break;
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && !isAnimating) {
        isAnimating = true;
        animationFrame = 0;
        lastFrameTime = SDL_GetTicks();
        animationEndTime = lastFrameTime + (charAttack.size() * speednext);
        current = &charAttack;
        std::cout << "Attack animation started!" << std::endl;
    }
}

void Character::update() {
    // Movement
    if (moveLeft) {
        charRect.x -= speedrun;
        if (charRect.x < 0) charRect.x = 0;
    }
    if (moveRight) {
        charRect.x += speedrun;
        if (charRect.x + charRect.w > screen_width) charRect.x = screen_width - charRect.w;
    }

    // Jumping
    if (isJumping) {
        charRect.y += Vjump;
        Vjump += trongtruong;
        if (charRect.y >= groundLevel) {
            charRect.y = groundLevel;
            isJumping = false;
        }
    }

    // Check attack animation end
    if (isAnimating && SDL_GetTicks() >= animationEndTime) {
        isAnimating = false;
        if (!moveLeft && !moveRight && !isJumping) {
            current = &charBack_afterAttack;
        } else {
            current = &charRun;
        }
        std::cout << "Attack animation finished!" << std::endl;
    }

    // Update animation frame
    if (SDL_GetTicks() - lastFrameTime > speednext) {
        animationFrame = (animationFrame + 1) % current->size();
        lastFrameTime = SDL_GetTicks();
    }
}

void Character::render(SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &charRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}
