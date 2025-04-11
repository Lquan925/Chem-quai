//Lever 1
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
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

const int mini_map_width = 200;
const int mini_map_height = 150;
const int mini_map_margin = 10;
const float mini_map_scale = 0.25f;

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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer không thể khởi tạo! Lỗi: " << Mix_GetError() << endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Animation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Tải âm thanh
    Mix_Chunk* soundAttack = Mix_LoadWAV("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* soundExplosion = Mix_LoadWAV("D:\\Âm thanh\\fight.wav");
    Mix_Chunk* soundRun = Mix_LoadWAV("D:\\Âm thanh\\run.wav");

    if (!soundAttack || !soundExplosion || !soundRun) {
        cout << "Không thể tải âm thanh! Lỗi: " << Mix_GetError() << endl;
        return -1;
    }

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

    SDL_Texture* miniMapTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mini_map_width, mini_map_height);
    SDL_SetTextureBlendMode(miniMapTexture, SDL_BLENDMODE_BLEND);

    // Biến quản lý âm thanh chạy
    Uint32 lastFootstepTime = 0;

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();

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
                lastFrameTime = currentTime;
                animationEndTime = currentTime + (charAttack.size() * speednext);
                current = &charAttack;

                Mix_PlayChannel(-1, soundAttack, 0);

                int charCenterX = charRect.x + charRect.w / 2;
                int enemyCenterX = enemyT.x + enemyT.w / 2;
                int distanceToEnemy = abs(charCenterX - enemyCenterX);

                if (checkEnemy && distanceToEnemy < distance_max_attack) {
                    checkEnemy = false;
                    showExplosion = true;
                    explosionStartTime = currentTime;
                    Mix_PlayChannel(-1, soundExplosion, 0);
                }
            }
        }

        // Phát âm thanh bước chân
        if ((moveLeft || moveRight) && !isJumping && !isAnimating) {
            if (currentTime - lastFootstepTime > 200) {
                Mix_PlayChannel(-1, soundRun, 0);
                lastFootstepTime = currentTime;
            }
        }

        if (moveLeft) {
            charRect.x -= speedrun;
            if (charRect.x < 0) charRect.x = 0;
        }
        if (moveRight) {
            charRect.x += speedrun;
            if (charRect.x + charRect.w > screen_width) charRect.x = screen_width - charRect.w;
        }

        if (isJumping) {
            charRect.y += Vjump;
            Vjump += trongtruong;
            if (charRect.y >= groundLevel) {
                charRect.y = groundLevel;
                isJumping = false;
            }
        }

        if (isAnimating && currentTime >= animationEndTime) {
            isAnimating = false;
            if (!moveLeft && !moveRight && !isJumping) current = &charBack_afterAttack;
            else current = &charRun;
        }

        if (showExplosion && currentTime - explosionStartTime >= thoigianno) {
            showExplosion = false;
        }

        if (currentTime - lastFrameTime > speednext) {
            animationFrame = (animationFrame + 1) % current->size();
            lastFrameTime = currentTime;
        }

        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &charRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        if (checkEnemy) SDL_RenderCopyEx(renderer, enemy, nullptr, &enemyT, 0, nullptr, SDL_FLIP_HORIZONTAL);
        if (showExplosion) {
            SDL_Rect explosionRect = { enemyT.x - 50, enemyT.y - 50, 200, 200 };
            SDL_RenderCopy(renderer, explosion, nullptr, &explosionRect);
        }

        // Mini-map
        SDL_SetRenderTarget(renderer, miniMapTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_Rect miniViewport = {0, 0, mini_map_width, mini_map_height};
        SDL_RenderSetViewport(renderer, &miniViewport);
        SDL_Rect bgRect = {0, 0, static_cast<int>(screen_width * mini_map_scale), static_cast<int>(screen_height * mini_map_scale)};
        SDL_RenderCopy(renderer, background, nullptr, &bgRect);
        SDL_Rect miniCharRect = {
            static_cast<int>(charRect.x * mini_map_scale),
            static_cast<int>(charRect.y * mini_map_scale),
            static_cast<int>(charRect.w * mini_map_scale),
            static_cast<int>(charRect.h * mini_map_scale)
        };
        SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &miniCharRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        if (checkEnemy) {
            SDL_Rect miniEnemyRect = {
                static_cast<int>(enemyT.x * mini_map_scale),
                static_cast<int>(enemyT.y * mini_map_scale),
                static_cast<int>(enemyT.w * mini_map_scale),
                static_cast<int>(enemyT.h * mini_map_scale)
            };
            SDL_RenderCopyEx(renderer, enemy, nullptr, &miniEnemyRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
        }
        if (showExplosion) {
            SDL_Rect miniExplosionRect = {
                static_cast<int>((enemyT.x - 50) * mini_map_scale),
                static_cast<int>((enemyT.y - 50) * mini_map_scale),
                static_cast<int>(200 * mini_map_scale),
                static_cast<int>(200 * mini_map_scale)
            };
            SDL_RenderCopy(renderer, explosion, nullptr, &miniExplosionRect);
        }

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderSetViewport(renderer, nullptr);
        SDL_Rect miniMapRect = {
            screen_width - mini_map_width - mini_map_margin,
            mini_map_margin,
            mini_map_width,
            mini_map_height
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &miniMapRect);
        SDL_RenderCopy(renderer, miniMapTexture, nullptr, &miniMapRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Dọn dẹp
    Mix_FreeChunk(soundAttack);
    Mix_FreeChunk(soundExplosion);
    Mix_FreeChunk(soundRun);
    Mix_CloseAudio();

    SDL_DestroyTexture(miniMapTexture);
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

