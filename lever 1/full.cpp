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
    SDL_Texture* background = loadpicture("C:\\Users\\lq865\\Downloads\\Free Pixel Art Forest\\Free Pixel Art Forest\\Preview\\Background.png", renderer);
    SDL_Texture* ground = loadpicture("C:\\Layer_0001_8-removebg-preview.png", renderer);
    vector<SDL_Texture*> charRun = {
        loadpicture("C:\\Nhan Vat\\sprite_0.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_1.png", renderer)
    };

    vector<SDL_Texture*> charAttack = {
        loadpicture("C:\\Nhan Vat\\sprite_2.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_3.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_4.png", renderer)
    };
    vector<SDL_Texture*> charBack_afterAttack = { loadpicture("C:\\Nhan Vat\\sprite_0.png", renderer) };
    SDL_Texture* enemy = loadpicture("D:\\Screenshot_2025-03-17_160427-removebg-preview.png", renderer);
    SDL_Texture* explosion = loadpicture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    vector<SDL_Texture*>* current = &charBack_afterAttack;

    int animationFrame = 0;
    Uint32 lastFrameTime = 0;
    Uint32 animationEndTime = 0;
    bool checkEnemy = true;
    Uint32 explosionStartTime = 0;
    bool showExplosion = false;

    if (background == nullptr || ground == nullptr || charRun.empty() || charAttack.empty() || charBack_afterAttack.empty() || enemy == nullptr || explosion == nullptr) {
        cout << "Error!" << endl;
        return -1; // End the programme
    }

    int groundWidth, groundHeight;
    SDL_QueryTexture(ground, nullptr, nullptr, &groundWidth, &groundHeight);

    int grass_height = 50;
    int charWidth = 160, charHeight = 160;
    SDL_Rect charRect = { 50, screen_height - grass_height - charHeight + 13, charWidth, charHeight };
    int groundLevel = charRect.y;
    SDL_Rect enemyT = { screen_width - 200, screen_height - grass_height - 105, 150, 150 };

    bool isJumping = false;
    int Vjump = 0; // Vận tốc nhảy
    bool moveLeft = false, moveRight = false;
    bool faceLeft = false;
    bool isAnimating = false;

    SDL_Rect floatingPlatform = { 300, 300, groundWidth / 3, groundHeight };
    bool quit = false; // Không thoát game
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true; // Nếu close window >> thoát game
            if (e.type == SDL_KEYDOWN) { // Xử lý bàn phím
                switch (e.key.keysym.sym) { // Lấy thông tin phím được nhấn
                    case SDLK_LEFT:
                        moveLeft = true;
                        faceLeft = true;
                        current = &charRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight = true;
                        faceLeft = false;
                        current = &charRun;
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
                switch (e.key.keysym.sym) { // Khi thả phím
                    case SDLK_LEFT:
                        moveLeft = false; // Không di chuyển
                        break;
                    case SDLK_RIGHT:
                        moveRight = false;
                        break;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                isAnimating = true;
                animationFrame = 0;
                lastFrameTime = SDL_GetTicks();
                animationEndTime = lastFrameTime + (charAttack.size() * speednext);
                current = &charAttack;
                std::cout << "Switched to attack animation!" << std::endl;

                int charCenterX = charRect.x + charRect.w / 2;
                int enemyCenterX = enemyT.x + enemyT.w / 2;
                int distanceToEnemy = abs(charCenterX - enemyCenterX); // Khoảng cách giữa hai tâm

                if (checkEnemy && distanceToEnemy < distance_max_attack) {
                    checkEnemy = false;
                    showExplosion = true;
                    explosionStartTime = SDL_GetTicks(); // Lưu thời gian bắt đầu hiệu ứng nổ
                }
            }
        }

        if (moveLeft) {
            charRect.x -= speedrun;
            if (charRect.x < 0) charRect.x = 0; // Giới hạn không cho đi ra khỏi màn hình trái
        }
        if (moveRight) {
            charRect.x += speedrun;
            if (charRect.x + charRect.w > screen_width) charRect.x = screen_width - charRect.w; // Giới hạn không cho đi ra khỏi màn hình phải
        }

        if (!moveLeft && !moveRight && !isJumping && !isAnimating) {
            current = &charBack_afterAttack;
        }

        if (isJumping) {
            charRect.y += Vjump;
            Vjump += trongtruong;
            if (charRect.y >= groundLevel) {
                charRect.y = groundLevel;
                isJumping = false;
            }
        }

        if (isAnimating && SDL_GetTicks() >= animationEndTime) {
            isAnimating = false;
            if (!moveLeft && !moveRight && !isJumping) {
                current = &charBack_afterAttack;
            }
        }

        if (showExplosion && SDL_GetTicks() - explosionStartTime >= thoigianno) {
            showExplosion = false;
        }

        if (SDL_GetTicks() - lastFrameTime > speednext) {
            animationFrame = (animationFrame + 1) % current->size();
            lastFrameTime = SDL_GetTicks();
        }

        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        for (int j = 0; j < screen_width; j += groundWidth) {
            SDL_Rect groundRect = { j, screen_height - groundHeight, groundWidth, groundHeight };
            SDL_RenderCopy(renderer, ground, nullptr, &groundRect);
        }

        SDL_RenderCopy(renderer, ground, nullptr, &floatingPlatform);
        SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &charRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        if (checkEnemy) { // Chỉ vẽ quái vật nếu nó còn sống
            SDL_RenderCopyEx(renderer, enemy, nullptr, &enemyT, 0, nullptr, SDL_FLIP_HORIZONTAL);
        }

        if (showExplosion) { // Hiển thị hiệu ứng nổ
            SDL_Rect explosionRect = { enemyT.x - 50, enemyT.y - 50, 200, 200 }; // Điều chỉnh vị trí và kích thước
            SDL_RenderCopy(renderer, explosion, nullptr, &explosionRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(ground);
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
