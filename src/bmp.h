#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "types.h"

typedef struct
{
    char signature[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;

    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels_per_m;
    uint32_t y_pixels_per_m;
    uint32_t colors_used;
    uint32_t important_colors;

    uint8_t *img_data;
    bool img_y_is_flip;
    bool img_x_is_flip;

    state_t state;
} bmp_t;

bool get_header(FILE *file, bmp_t *bmp);
bool get_info_header(FILE *file, bmp_t *bmp);
bool get_color_table(FILE *file, bmp_t *bmp);
bool get_image_data(FILE *file, bmp_t *bmp);

void render_image(FILE *file, bmp_t *bmp, sdl_t *sdl);

#endif