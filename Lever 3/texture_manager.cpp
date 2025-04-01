#include "texture_manager.h"
#include <iostream>
using namespace std;
SDL_Texture* LoadTexture(const string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        cout << "Error loading image: " << IMG_GetError() << endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        cout << "Error creating texture: " << SDL_GetError() << endl;
    }

    return texture;
}
