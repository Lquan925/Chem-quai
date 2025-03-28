#include "TextureManager.h"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cout << "Error loading image: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool TextureManager::CheckTexturesLoaded(const std::vector<SDL_Texture*>& textures) {
    for (auto tex : textures) {
        if (!tex) return false;
    }
    return true;
}
