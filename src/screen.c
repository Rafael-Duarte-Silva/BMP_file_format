#include "screen.h"
#include "bmp.h"

bool init_sdl(sdl_t *sdl, bmp_t *bmp){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0){
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

    sdl->texture = SDL_CreateTexture(
        sdl->renderer,
        SDL_PIXELFORMAT_BGR24,
        SDL_TEXTUREACCESS_STREAMING,
        bmp->width,
        bmp->height
    );
    if(!sdl->texture){
        SDL_Log("Could not create SDL texture! %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void clear_screen(const sdl_t sdl){
    SDL_SetRenderDrawColor(sdl.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(sdl.renderer);
}

void final_cleanup(const sdl_t sdl){
    SDL_DestroyTexture(sdl.texture);
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
                        flip_image_y(file, bmp, sdl);
                        break;

                    case SDLK_x:
                        flip_image_x(file, bmp, sdl);
                        break;

                    case SDLK_i:
                        render_image(file, bmp, sdl, render_image_negative);
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