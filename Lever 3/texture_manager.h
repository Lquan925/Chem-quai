#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
using namespace std;
SDL_Texture* LoadTexture(const string& path, SDL_Renderer* renderer);
