#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef enum {
    QUIT,
    RUNNING,
} state_t;

#endif