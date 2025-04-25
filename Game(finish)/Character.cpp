#include "Character.h"

Character::Character(int x, int y, int w, int h, int ground) {
    rect = { x, y, w, h };
    health = 3;
    groundLevel = ground;
    isJumping = false;
    jumpVelocity = 0;
    moveLeft = false;
    moveRight = false;
    faceLeft = true;
    isAttacking = false;
    invincible = false;
    hitTime = 0;
    wasMoving = false;
    animFrame = 0;
    currentAnim = nullptr;
}

void Character::Move() {
    if (moveLeft) {
        rect.x -= PLAYER_SPEED;
        if (rect.x < 0) rect.x = 0;
    }
    if (moveRight) {
        rect.x += PLAYER_SPEED;
        if (rect.x + rect.w > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.w;
    }
}

void Character::Jump() {
    if (isJumping) {
        rect.y += jumpVelocity;
        jumpVelocity += GRAVITY;
        if (rect.y >= groundLevel) {
            rect.y = groundLevel;
            isJumping = false;
        }
    }
}
