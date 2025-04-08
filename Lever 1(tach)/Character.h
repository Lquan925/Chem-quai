#pragma once
#include <SDL.h>
#include <vector>

class Character {
public:
    Character(SDL_Renderer* renderer);
    ~Character();

    void handleEvent(SDL_Event& e);
    void update();
    void render(SDL_Renderer* renderer);

    SDL_Rect getRect() const { return charRect; }
    bool isAttacking() const { return isAnimating; }
    bool isFacingLeft() const { return faceLeft; }

private:
    std::vector<SDL_Texture*> charRun;
    std::vector<SDL_Texture*> charAttack;
    std::vector<SDL_Texture*> charBack_afterAttack;
    std::vector<SDL_Texture*>* current;

    SDL_Rect charRect;
    int animationFrame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 animationEndTime = 0;

    bool isJumping = false;
    int Vjump = 0;
    bool moveLeft = false, moveRight = false;
    bool faceLeft = false;
    bool isAnimating = false;
    int groundLevel;
};
