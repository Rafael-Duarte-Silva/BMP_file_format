#ifndef SCREEN_H
#define SCREEN_H

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "types.h"
#include "bmp.h"

bool init_sdl(sdl_t *sdl, bmp_t *bmp);
void final_cleanup(const sdl_t sdl);
void clear_screen(const sdl_t sdl);
void handle_input(FILE *file, bmp_t *bmp, sdl_t *sdl);

#endif