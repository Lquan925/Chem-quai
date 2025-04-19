#pragma once
#include "constants.h"
#include <SDL.h>

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
};

