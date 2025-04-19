#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>

SDL_Texture* LoadTexture(const std::string& path, SDL_Renderer* renderer);
Mix_Chunk* LoadSound(const std::string& path);
