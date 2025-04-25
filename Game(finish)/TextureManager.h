#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>

class TextureManager {
public:
    static SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
    static Mix_Chunk* LoadSound(const char* path);
};

#endif
