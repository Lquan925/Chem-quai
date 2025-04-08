#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
using namespace std;

const int screen_width = 800;
const int screen_height = 600;
const int speedrun = 5;
const int jump_height = 200;
const int trongtruong = 5;
const int speednext = 100;
const int distance_max_attack = 50;
const int thoigianno = 500;

SDL_Texture* loadpicture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == nullptr) {
        cout << "Không thể tải ảnh! Lỗi: " << IMG_GetError() << endl;
        return nullptr;
    }
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Tải các texture
    SDL_Texture* background = loadpicture("D:\\Đồ họa(UPDATE)\\813c07108797191.5fc5eab3009d7.jpg", renderer);
    vector<SDL_Texture*> charRun = {
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer),
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_1_resized_1.5x.png", renderer),
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_2_resized_1.5x.png", renderer)
    };

    vector<SDL_Texture*> charAttack = {
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_3_resized_1.5x.png", renderer),
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_4_resized_1.5x.png", renderer),
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_5_resized_1.5x.png", renderer)
    };
    vector<SDL_Texture*> charBack_afterAttack = {
        loadpicture("D:\\Đồ họa(UPDATE)\\Nhân vật\\sprite_0_resized_1.5x.png", renderer)
    };
    SDL_Texture* enemy = loadpicture("D:\\Đồ họa(UPDATE)\\enemy\\Screenshot_2025-03-25_134534-removebg-preview.png", renderer);
    SDL_Texture* explosion = loadpicture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    vector<SDL_Texture*>* current = &charBack_afterAttack;

    int animationFrame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 animationEndTime = 0;
    bool checkEnemy = true;
    Uint32 explosionStartTime = 0;
    bool showExplosion = false;

    if (background == nullptr || charRun.empty() || charAttack.empty() || charBack_afterAttack.empty() || enemy == nullptr || explosion == nullptr) {
        cout << "Error loading textures!" << endl;
        return -1;
    }

    int charWidth = 40, charHeight = 40;
    SDL_Rect charRect = { 90, screen_height - charHeight - 50, charWidth, charHeight };
    int groundLevel = charRect.y;
    SDL_Rect enemyT = { screen_width - 200, screen_height - 115, 75, 75 };

    bool isJumping = false;
    int Vjump = 0;
    bool moveLeft = false, moveRight = false;
    bool faceLeft = false;
    bool isAnimating = false;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;

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
                cout << "Attack animation started!" << endl;

                int charCenterX = charRect.x + charRect.w / 2;
                int enemyCenterX = enemyT.x + enemyT.w / 2;
                int distanceToEnemy = abs(charCenterX - enemyCenterX);

                if (checkEnemy && distanceToEnemy < distance_max_attack) {
                    checkEnemy = false;
                    showExplosion = true;
                    explosionStartTime = SDL_GetTicks();
                }
            }
        }

        // Di chuyển nhân vật
        if (moveLeft) {
            charRect.x -= speedrun;
            if (charRect.x < 0) charRect.x = 0;
        }
        if (moveRight) {
            charRect.x += speedrun;
            if (charRect.x + charRect.w > screen_width) charRect.x = screen_width - charRect.w;
        }

        // Nhảy
        if (isJumping) {
            charRect.y += Vjump;
            Vjump += trongtruong;
            if (charRect.y >= groundLevel) {
                charRect.y = groundLevel;
                isJumping = false;
            }
        }

        // Kiểm tra kết thúc animation tấn công
        if (isAnimating && SDL_GetTicks() >= animationEndTime) {
            isAnimating = false;
            if (!moveLeft && !moveRight && !isJumping) {
                current = &charBack_afterAttack;
            } else {
                current = &charRun;
            }
            cout << "Attack animation finished!" << endl;
        }

        // Hiệu ứng nổ
        if (showExplosion && SDL_GetTicks() - explosionStartTime >= thoigianno) {
            showExplosion = false;
        }

        // Cập nhật frame animation
        if (SDL_GetTicks() - lastFrameTime > speednext) {
            animationFrame = (animationFrame + 1) % current->size();
            lastFrameTime = SDL_GetTicks();
        }

        // Vẽ mọi thứ
        SDL_RenderCopy(renderer, background, nullptr, nullptr);

        // Vẽ nhân vật
        SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &charRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        // Vẽ kẻ địch nếu còn sống
        if (checkEnemy) {
            SDL_RenderCopyEx(renderer, enemy, nullptr, &enemyT, 0, nullptr, SDL_FLIP_HORIZONTAL);
        }

        // Vẽ hiệu ứng nổ
        if (showExplosion) {
            SDL_Rect explosionRect = { enemyT.x - 50, enemyT.y - 50, 200, 200 };
            SDL_RenderCopy(renderer, explosion, nullptr, &explosionRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Giải phóng bộ nhớ
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(enemy);
    SDL_DestroyTexture(explosion);
    for (auto tex : charRun) SDL_DestroyTexture(tex);
    for (auto tex : charAttack) SDL_DestroyTexture(tex);
    for (auto tex : charBack_afterAttack) SDL_DestroyTexture(tex);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
