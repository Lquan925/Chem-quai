#ifndef CONSTANTS_H
#define CONSTANTS_H

// Hằng số của trò chơi
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
const int ENEMY_SPAWN_INTERVAL = 1000;
const int ENEMY_SPAWN_DURATION = 20000;

// Hằng số bản đồ nhỏ
const int MINIMAP_WIDTH = 200;
const int MINIMAP_HEIGHT = 150;
const int MINIMAP_X = SCREEN_WIDTH - MINIMAP_WIDTH - 10;
const int MINIMAP_Y = 10;

// Kênh âm thanh
const int CHANNEL_PLAYER = 1;
const int CHANNEL_ENEMY = 2;
const int CHANNEL_EFFECTS = 3;

// Hằng số nút bấm
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 100;
const int BUTTON_X = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
const int BUTTON_Y = SCREEN_HEIGHT / 2 + 100;

// Hằng số nút bắt đầu (cho màn hình bắt đầu)
const int START_BUTTON_WIDTH = 150;
const int START_BUTTON_HEIGHT = 75;
const int START_BUTTON_X = SCREEN_WIDTH / 2 - START_BUTTON_WIDTH / 2;
const int START_BUTTON_Y = SCREEN_HEIGHT / 2 - START_BUTTON_HEIGHT / 2;

#endif
