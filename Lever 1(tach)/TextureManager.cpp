#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::loadpicture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == nullptr) {
        std::cout << "Không thể tải ảnh! Lỗi: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}
