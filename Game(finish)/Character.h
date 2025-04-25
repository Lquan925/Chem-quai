#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL.h>
#include <vector>
#include "Constants.h"

class Character {
public:
    SDL_Rect rect;
    int health;
    int groundLevel;
    bool isJumping;
    int jumpVelocity;
    bool moveLeft, moveRight;
    bool faceLeft;
    bool isAttacking;
    bool invincible;
    Uint32 hitTime;
    bool wasMoving;
    std::vector<SDL_Texture*>* currentAnim;
    int animFrame;

    Character(int x, int y, int w, int h, int ground);
    void Move();
    void Jump();
};

#endif
