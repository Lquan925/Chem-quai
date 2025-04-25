#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "Lỗi khi tải hình ảnh: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cout << "Lỗi khi tạo texture: " << SDL_GetError() << std::endl;
    }

    return texture;
}

Mix_Chunk* TextureManager::LoadSound(const char* path) {
    Mix_Chunk* sound = Mix_LoadWAV(path);
    if (!sound) {
        std::cout << "Lỗi khi tải âm thanh: " << Mix_GetError() << std::endl;
    }
    return sound;
}
