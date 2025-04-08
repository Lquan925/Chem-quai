#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

class TextureManager {
public:
    static SDL_Texture* loadpicture(const char* path, SDL_Renderer* renderer);
};
