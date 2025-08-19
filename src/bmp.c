#include "bmp.h"

void render_image(FILE *file, bmp_t *bmp, sdl_t *sdl, Effect effect){
    void *pixels;
    int pitch;
    SDL_LockTexture(sdl->texture, NULL, &pixels, &pitch);
    uint8_t *dest = (uint8_t *)pixels; 

    int y = 0;
    for(int img_column = 0; img_column < bmp->height; img_column++){
        y = bmp->height - img_column - 1;

        for(int img_row = 0; img_row < bmp->width; img_row++){
            uint8_t *ptr_data = calculate_ptr_data(bmp, img_column, img_row);
            uint8_t *ptr_dest = dest + y * pitch + img_row * 3;
            
            effect(ptr_dest, ptr_data);
        }
    }
    SDL_UnlockTexture(sdl->texture);

    SDL_RenderClear(sdl->renderer);
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
}

void flip_image_x(FILE *file, bmp_t *bmp, sdl_t *sdl){
    for(int img_column = 0; img_column < bmp->height; img_column++){
        int img_row_right = bmp->width - 1;
        int img_row_left = 0;
        while(img_row_left < img_row_right){
            uint8_t *ptr_data_right = calculate_ptr_data(bmp, img_column, img_row_left);
            uint8_t *ptr_data_left = calculate_ptr_data(bmp, img_column, img_row_right);

            swap_ptr(ptr_data_right, ptr_data_left, 0);
            swap_ptr(ptr_data_right, ptr_data_left, 1);
            swap_ptr(ptr_data_right, ptr_data_left, 2);

            img_row_left++;
            img_row_right--;
        }
    }

    render_image(file, bmp, sdl, render_image_normal);
}

void flip_image_y(FILE *file, bmp_t *bmp, sdl_t *sdl){
    int img_column_bottom = bmp->height - 1;
    int img_column_top = 0;
    while(img_column_top < img_column_bottom){
       for(int img_row = 0; img_row < bmp->width; img_row++){
            uint8_t *ptr_data_bottom = calculate_ptr_data(bmp, img_column_bottom, img_row);
            uint8_t *ptr_data_top = calculate_ptr_data(bmp, img_column_top, img_row);

            swap_ptr(ptr_data_top, ptr_data_bottom, 0);
            swap_ptr(ptr_data_top, ptr_data_bottom, 1);
            swap_ptr(ptr_data_top, ptr_data_bottom, 2);
        }

        img_column_top++;
        img_column_bottom--;
    }

    render_image(file, bmp, sdl, render_image_normal);
}

void swap_ptr(uint8_t *ptr_data_source,  uint8_t *ptr_data_dest, int index){
    uint8_t swap = ptr_data_source[index];
    ptr_data_source[index] = ptr_data_dest[index];
    ptr_data_dest[index] = swap;
}

uint8_t *calculate_ptr_data(bmp_t *bmp, int y, int x){
    return bmp->img_data + y * bmp->img_row_size + x * 3;
}

void render_image_normal(uint8_t *ptr_dest, uint8_t *ptr_data){
    ptr_dest[0] = ptr_data[0]; // blue
    ptr_dest[1] = ptr_data[1]; // green
    ptr_dest[2] = ptr_data[2]; // red
}

void render_image_negative(uint8_t *ptr_dest, uint8_t *ptr_data){
    ptr_data[0] = 255 - ptr_data[0]; // blue
    ptr_data[1] = 255 - ptr_data[1]; // green
    ptr_data[2] = 255 - ptr_data[2]; // red

    ptr_dest[0] = ptr_data[0]; // blue
    ptr_dest[1] = ptr_data[1]; // green
    ptr_dest[2] = ptr_data[2]; // red
}

bool get_header(FILE *file, bmp_t *bmp){
    printf("--- HEADER ---\n\n");

    uint8_t *header_block = malloc(0X0E);
    if(!header_block) return false;

    fseek(file, 0X00, SEEK_SET);
    fread(header_block, 0x01, 0x0E, file);

    memcpy(&bmp->signature, &header_block[0x00], 0x02);
    printf("signature: %s\n", bmp->signature);

    memcpy(&bmp->file_size, &header_block[0X02], 0x04);
    printf("file size: %d\n", bmp->file_size);

    memcpy(&bmp->reserved, &header_block[0X06], 0x04);
    printf("reserved: %d\n", bmp->reserved);

    memcpy(&bmp->data_offset, &header_block[0X0A], 0x04);
    printf("data offset: %d\n", bmp->data_offset);

    free(header_block);

    printf("\n--- HEADER ---\n\n");

    return true;
}

bool get_info_header(FILE *file, bmp_t *bmp){
    printf("--- INFO HEADER ---\n\n");

    fseek(file, 0X0E, SEEK_SET);
    fread(&bmp->size, 0x04, 1, file);
    printf("size: %d\n", bmp->size);

    uint8_t *info_header_block = malloc(bmp->size - 0x04);
    if(!info_header_block) return false;

    fseek(file, 0X0E + 0x04, SEEK_SET);
    fread(info_header_block, 0x01, bmp->size - 0x04, file);

    memcpy(&bmp->width, &info_header_block[0X00], 0x04);
    printf("width: %d\n", bmp->width);

    memcpy(&bmp->height, &info_header_block[0X04], 0x04);
    printf("height: %d\n", bmp->height);

    memcpy(&bmp->planes, &info_header_block[0X08], 0x02);
    printf("planes: %d\n", bmp->planes);

    memcpy(&bmp->bits_per_pixel, &info_header_block[0X0A], 0x02);
    printf("bits per pixel: %d\n", bmp->bits_per_pixel);

    memcpy(&bmp->compression, &info_header_block[0X0C], 0x04);
    printf("compression: %d\n", bmp->compression);

    memcpy(&bmp->image_size, &info_header_block[0X10], 0x04);
    printf("image size: %d\n", bmp->image_size);

    memcpy(&bmp->x_pixels_per_m, &info_header_block[0X14], 0x04);
    printf("x pixels per m: %d\n", bmp->x_pixels_per_m);

    memcpy(&bmp->y_pixels_per_m, &info_header_block[0X18], 0x04);
    printf("y pixels per m: %d\n", bmp->y_pixels_per_m);

    memcpy(&bmp->colors_used, &info_header_block[0X1C], 0x04);
    printf("colors used: %d\n", bmp->colors_used);

    memcpy(&bmp->important_colors, &info_header_block[0X20], 0x04);
    printf("important colors: %d\n", bmp->important_colors);

    free(info_header_block);

    printf("\n--- INFO HEADER ---\n\n");

    return true;
}

bool get_color_table(FILE *file, bmp_t *bmp){
    return true;
}

bool get_image_data(FILE *file, bmp_t *bmp){
    bmp->img_data = malloc(bmp->image_size);
    if(!bmp->img_data) return false;
    
    fseek(file, bmp->data_offset, SEEK_SET);
    fread(bmp->img_data, 0x01, bmp->image_size, file);

    bmp->img_row_size = (bmp->bits_per_pixel * bmp->width / 32) * 4;

    return true;
}