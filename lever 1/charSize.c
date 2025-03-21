    int grass_height=50;
    int charWidth=160, charHeight = 160;
    SDL_Rect charRect={ 50,screen_height-grass_height-charHeight+13,charWidth,charHeight};
    int groundLevel=charRect.y;
    SDL_Rect enemyT={ screen_width - 200,screen_height - grass_height - 105, 150,150};
