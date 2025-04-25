#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>

enum EnemyState { ENEMY_MOVING, ENEMY_ATTACKING, ENEMY_COOLDOWN };

struct Enemy {
    SDL_Rect rect;
    int direction;
    bool faceLeft;
    bool active;
    EnemyState state;
    int animFrame;
    Uint32 lastAnimTime;
    Uint32 attackStartTime;
    bool inContact;
    Uint32 contactTime;
    bool fromLeft;
    bool isInitialEnemy;
};

#endif
