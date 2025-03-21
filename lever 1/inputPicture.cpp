SDL_Texture* background = loadpicture("C:\\Users\\lq865\\Downloads\\Free Pixel Art Forest\\Free Pixel Art Forest\\Preview\\Background.png", renderer);
    SDL_Texture* ground = loadpicture("C:\\Layer_0001_8-removebg-preview.png", renderer);
    vector<SDL_Texture*> charRun = {
        loadpicture("C:\\Nhan Vat\\sprite_0.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_1.png", renderer)
    };

    vector<SDL_Texture*> charAttack = {
        loadpicture("C:\\Nhan Vat\\sprite_2.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_3.png", renderer),
        loadpicture("C:\\Nhan Vat\\sprite_4.png", renderer)
    };
    vector<SDL_Texture*> charBack_afterAttack = { loadpicture("C:\\Nhan Vat\\sprite_0.png", renderer) };
    SDL_Texture* enemy = loadpicture("D:\\Screenshot_2025-03-17_160427-removebg-preview.png", renderer);
    SDL_Texture* explosion = loadpicture("C:\\Users\\lq865\\Downloads\\sprite_5-removebg-preview.png", renderer);
    vector<SDL_Texture*>* current = &charBack_afterAttack;
