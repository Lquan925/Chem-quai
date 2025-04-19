#include "texture_manager.h"
#include <iostream>

SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cout << "Error loading image: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
    }

    return texture;
}

Mix_Chunk* LoadSound(const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        std::cout << "Error loading sound: " << Mix_GetError() << std::endl;
    }
    return sound;
}
