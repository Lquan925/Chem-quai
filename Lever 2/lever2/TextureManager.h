#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>

class TextureManager {
public:
    static SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer);
    static bool CheckTexturesLoaded(const std::vector<SDL_Texture*>& textures);
};
