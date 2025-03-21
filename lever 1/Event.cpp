    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type==SDL_QUIT)
                quit = 1; // Nếu close window >> thoát game
            if (e.type==SDL_KEYDOWN) { // Xử lý bàn phím
                switch (e.key.keysym.sym) { // Lấy thông tin phím được nhấn
                    case SDLK_LEFT:
                        moveLeft=1;
                        faceLeft=1;
                        current=&charRun;
                        break;
                    case SDLK_RIGHT:
                        moveRight=1;
                        faceLeft=0;
                        current=&charRun;
                        break;
                    case SDLK_SPACE:
                        if (!isJumping){
                            isJumping=1;
                            Vjump=-jump_height/10;
                        }
                        break;
                }
            }
            if (e.type==SDL_KEYUP) {
                switch(e.key.keysym.sym){ // Khi thả phím
                    case SDLK_LEFT:
                        moveLeft=0; // Không di chuyển
                        break;
                    case SDLK_RIGHT:
                        moveRight=0;
                        break;
                }
            }
            if (e.type==SDL_MOUSEBUTTONDOWN) {
                isAnimating=1;
                animationFrame=0;
                lastFrameTime=SDL_GetTicks();
                animationEndTime=lastFrameTime+(charAttack.size()*speednext);
                current=&charAttack;
                cout << "Switched to attack animation!" <<endl;

                int charCenterX=charRect.x+charRect.w/2;
                int enemyCenterX=enemyT.x+enemyT.w/2;
                int distanceToEnemy=abs(charCenterX-enemyCenterX);// Khoảng cách giữa hai tâm

                if (checkEnemy&&distanceToEnemy<distance_max_attack) {
                    checkEnemy=0;
                    showExplosion=1;
                    explosionStartTime=SDL_GetTicks(); // Lưu thời gian bắt đầu hiệu ứng nổ
                }
            }
        }

        if(moveLeft){
            charRect.x-=speedrun;
            if(charRect.x<0) charRect.x=0; // Giới hạn không cho đi ra khỏi màn hình trái
        }
        if(moveRight) {
            charRect.x+=speedrun;
            if(charRect.x+charRect.w>screen_width)charRect.x=screen_width-charRect.w; // Giới hạn không cho đi ra khỏi màn hình phải
        }
        if((moveLeft==0)&&(moveRight==0)&&(isJumping==0)&&(isAnimating==0)) {
            current=&charBack_afterAttack;
        }
        if(isJumping) {
            charRect.y+=Vjump;
            Vjump+=trongtruong;
            if(charRect.y>=groundLevel){
                charRect.y=groundLevel;
                isJumping=0;
            }
        }
        if(isAnimating&&SDL_GetTicks()>=animationEndTime){
            isAnimating=0;
            if(moveLeft==0&&moveRight==0&&isJumping==0){
                current=&charBack_afterAttack;
            }
        }
        if(showExplosion&&SDL_GetTicks()-explosionStartTime>=thoigianno){
            showExplosion=0;
        }
        if(SDL_GetTicks()-lastFrameTime>speednext){
            animationFrame=(animationFrame+1)%current->size();
            lastFrameTime=SDL_GetTicks();
        }

        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        for(int j = 0;j<screen_width;j+=groundWidth){
            SDL_Rect groundRect={j, screen_height-groundHeight, groundWidth, groundHeight };
            SDL_RenderCopy(renderer, ground, nullptr, &groundRect);
        }

        SDL_RenderCopy(renderer, ground, nullptr, &floatingPlatform);
        SDL_RenderCopyEx(renderer, (*current)[animationFrame], nullptr, &charRect, 0, nullptr, faceLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        if (checkEnemy) {
            SDL_RenderCopyEx(renderer, enemy, nullptr, &enemyT, 0, nullptr, SDL_FLIP_HORIZONTAL);
        }

        if (showExplosion) {
            SDL_Rect explosionRect = { enemyT.x - 50, enemyT.y - 50, 200, 200 };
            SDL_RenderCopy(renderer, explosion, nullptr, &explosionRect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
