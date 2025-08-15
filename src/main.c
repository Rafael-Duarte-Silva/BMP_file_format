#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef enum  {
    QUIT,
    RUNNING,
} state_t;

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

    uint8_t img_red;
    uint8_t img_blue;
    uint8_t img_green;

    state_t state;
} bmp_t;

void header(FILE *file, bmp_t *bmp);
void info_header(FILE *file, bmp_t *bmp);
void color_table(FILE *file, bmp_t *bmp);

void image(FILE *file, bmp_t *bmp, sdl_t *sdl);

bool init_sdl(sdl_t *sdl, bmp_t *bmp);
void final_cleanup(const sdl_t sdl);
void clear_screen(const sdl_t sdl);
void handle_input(bmp_t *bmp);

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");

    if(file == NULL){
        printf("File not found\n");
        return 1;
    }

    bmp_t bmp = {0};

    header(file, &bmp);
    info_header(file, &bmp);
    color_table(file, &bmp);

    sdl_t sdl = {0};
    if(init_sdl(&sdl, &bmp) == false) return 1;
    clear_screen(sdl);

    image(file, &bmp, &sdl);

    bmp.state = RUNNING;
    while(bmp.state != QUIT){
        handle_input(&bmp);
    }
    
    final_cleanup(sdl);
}

void image(FILE *file, bmp_t *bmp, sdl_t *sdl){
    SDL_Rect rect = {.x = 0, .y = bmp->height, .w = 1, .h = 1};

    uint32_t row_size = (bmp->bits_per_pixel * bmp->width / 32) * 4;
    uint32_t padding = 0;
    uint32_t rest = row_size - bmp->width * padding;
    if(rest % 4 != 0){
        padding = rest;
        printf("padding: %d\n\n", padding);
    }

    uint8_t *img_row = malloc(row_size);
    uint16_t img_column = 0;
    uint16_t img_row_ptr = 0;
    while(rect.y > 0){
        fseek(file, bmp->data_offset + img_column * row_size, SEEK_SET);
        fread(img_row, row_size, 1, file);
        while(rect.x < bmp->width){
            bmp->img_blue = img_row[img_row_ptr];
            //printf("img blue: %d\n", bmp->img_blue);

            bmp->img_green = img_row[img_row_ptr + 1];
            //printf("img green: %d\n", bmp->img_green);

            bmp->img_red = img_row[img_row_ptr + 2];
            //printf("img red: %d\n", bmp->img_red);

            SDL_SetRenderDrawColor(sdl->renderer, bmp->img_red, bmp->img_green, bmp->img_blue, 0xFF);
            SDL_RenderFillRect(sdl->renderer, &rect);

            img_row_ptr += 3;
            rect.x++;
        }

        img_row_ptr = 0;
        rect.x = 0;
        rect.y--;
        img_column++;
    }

    SDL_RenderPresent(sdl->renderer);
    free(img_row);
}

void header(FILE *file, bmp_t *bmp){
    fread(&bmp->signature, 0x02, 1, file);
    printf("signature: %s\n", bmp->signature);

    fseek(file, 0X02, SEEK_SET);
    fread(&bmp->file_size, 0x04, 1, file);
    printf("file size: %d\n", bmp->file_size);

    fseek(file, 0X06, SEEK_SET);
    fread(&bmp->reserved, 0x04, 1, file);
    printf("reserved: %d\n", bmp->reserved);

    fseek(file, 0X0A, SEEK_SET);
    fread(&bmp->data_offset, 0x04, 1, file);
    printf("data offset: %d\n", bmp->data_offset);
}

void info_header(FILE *file, bmp_t *bmp){
    fseek(file, 0X0E, SEEK_SET);
    fread(&bmp->size, 0x04, 1, file);
    printf("size: %d\n", bmp->size);

    fseek(file, 0X12, SEEK_SET);
    fread(&bmp->width, 0x04, 1, file);
    printf("width: %d\n", bmp->width);

    fseek(file, 0X16, SEEK_SET);
    fread(&bmp->height, 0x04, 1, file);
    printf("height: %d\n", bmp->height);

    fseek(file, 0X1A, SEEK_SET);
    fread(&bmp->planes, 0x02, 1, file);
    printf("planes: %d\n", bmp->planes);

    fseek(file, 0X1C, SEEK_SET);
    fread(&bmp->bits_per_pixel, 0x02, 1, file);
    printf("bits per pixel: %d\n", bmp->bits_per_pixel);

    fseek(file, 0X1E, SEEK_SET);
    fread(&bmp->compression, 0x04, 1, file);
    printf("compression: %d\n", bmp->compression);

    fseek(file, 0X22, SEEK_SET);
    fread(&bmp->image_size, 0x04, 1, file);
    printf("image size: %d\n", bmp->image_size);

    fseek(file, 0X26, SEEK_SET);
    fread(&bmp->x_pixels_per_m, 0x04, 1, file);
    printf("x pixels per m: %d\n", bmp->x_pixels_per_m);

    fseek(file, 0X2A, SEEK_SET);
    fread(&bmp->y_pixels_per_m, 0x04, 1, file);
    printf("y pixels per m: %d\n", bmp->y_pixels_per_m);

    fseek(file, 0X2E, SEEK_SET);
    fread(&bmp->colors_used, 0x04, 1, file);
    printf("colors used: %d\n", bmp->colors_used);

    fseek(file, 0X32, SEEK_SET);
    fread(&bmp->important_colors, 0x04, 1, file);
    printf("important colors: %d\n", bmp->important_colors);
}

void color_table(FILE *file, bmp_t *bmp){
}

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

void handle_input(bmp_t *bmp){
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
                        
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}