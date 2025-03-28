#pragma once
#include <SDL.h>

// Game constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 5;
const int JUMP_HEIGHT = 12;
const int GRAVITY = 1;
const int ANIMATION_SPEED = 100;
const int ATTACK_DISTANCE = 80;
const int EXPLOSION_DURATION = 500;
const int ENEMY_SPEED = 2;
const int ENEMY_ANIMATION_SPEED = 150;
const float PLAYER_SCALE = 1.0f;
const float ENEMY_SCALE = 0.3f;
const int GROUND_HEIGHT = 50;
const int ENEMY_ATTACK_COOLDOWN = 400;
const int ENEMY_ATTACK_DURATION = 1000;
const int PLAYER_HIT_DURATION = 1000;
const int PLAYER_HIT_BLINK = 100;
const int PLAYER_INVINCIBLE_BLINK = 100;

// Game states
enum GameState {
    PLAYING,
    PLAYER_HIT,
    GAME_OVER,
    PLAYER_WIN
};

// Enemy states
enum EnemyState {
    ENEMY_MOVING,
    ENEMY_ATTACKING,
    ENEMY_COOLDOWN
};
