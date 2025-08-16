#include "bmp.h"

void image(FILE *file, bmp_t *bmp, sdl_t *sdl){
    SDL_Rect rect = {.w = 1, .h = 1};

    uint32_t row_size = (bmp->bits_per_pixel * bmp->width / 32) * 4;
    uint8_t *img_row = malloc(row_size);
    uint16_t img_column = 0;
    uint16_t img_row_ptr = 0;
    while(img_column < bmp->height){
        fseek(file, bmp->data_offset + img_column * row_size, SEEK_SET);
        fread(img_row, row_size, 1, file);

        rect.y = bmp->height - img_column - 1;
        if(bmp->img_y_is_flip) rect.y = img_column;

        for(int x = 0; x < bmp->width; x++){
            bmp->img_blue = img_row[img_row_ptr];
            //printf("img blue: %d\n", bmp->img_blue);

            bmp->img_green = img_row[img_row_ptr + 1];
            //printf("img green: %d\n", bmp->img_green);

            bmp->img_red = img_row[img_row_ptr + 2];
            //printf("img red: %d\n", bmp->img_red);

            rect.x = x;
            if(bmp->img_x_is_flip) rect.x = bmp->width - x - 1;
            
            SDL_SetRenderDrawColor(sdl->renderer, bmp->img_red, bmp->img_green, bmp->img_blue, 0xFF);
            SDL_RenderFillRect(sdl->renderer, &rect);

            img_row_ptr += 3;
        }

        img_row_ptr = 0;
        img_column++;
    }

    SDL_RenderPresent(sdl->renderer);
    free(img_row);
}

void header(FILE *file, bmp_t *bmp){
    printf("--- HEADER ---\n\n");

    uint8_t *header_block = malloc(0X0E);
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
}

void info_header(FILE *file, bmp_t *bmp){
    printf("--- INFO HEADER ---\n\n");

    fseek(file, 0X0E, SEEK_SET);
    fread(&bmp->size, 0x04, 1, file);
    printf("size: %d\n", bmp->size);

    uint8_t *info_header_block = malloc(bmp->size - 0x04);
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
}

void color_table(FILE *file, bmp_t *bmp){
}