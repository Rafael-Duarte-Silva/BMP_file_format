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