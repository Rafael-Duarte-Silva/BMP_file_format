#include "screen.h"
#include "bmp.h"

bool init_sdl(sdl_t *sdl, bmp_t *bmp){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow(
        "BMP",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        bmp->width,
        bmp->height,
        0
    );

    if(!sdl->window){
        SDL_Log("Could not create SDL window! %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(
        sdl->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if(!sdl->renderer){
        SDL_Log("Could not create SDL renderer! %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void clear_screen(const sdl_t sdl){
    SDL_SetRenderDrawColor(sdl.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(sdl.renderer);
}

void final_cleanup(const sdl_t sdl){
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    SDL_Quit();
}

void handle_input(FILE *file, bmp_t *bmp, sdl_t *sdl){
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch (event.type)
        {
            case SDL_QUIT:
                bmp->state = QUIT;
                return;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        bmp->state = QUIT;
                        return;

                    case SDLK_y:
                        bmp->img_y_is_flip = !bmp->img_y_is_flip;
                        image(file, bmp, sdl);
                        break;

                    case SDLK_x:
                        bmp->img_x_is_flip = !bmp->img_x_is_flip;
                        image(file, bmp, sdl);
                        break;
                        
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}