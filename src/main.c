#include <stdio.h>

#include "screen.h"
#include "bmp.h"

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");

    if(!file){
        printf("File not found\n");
        return 1;
    }

    bmp_t bmp = {0};

    if(!get_header(file, &bmp)) return 1;
    if(!get_info_header(file, &bmp)) return 1;
    if(!get_color_table(file, &bmp)) return 1;
    if(!get_image_data(file, &bmp)) return 1;

    sdl_t sdl = {0};
    if(!init_sdl(&sdl, &bmp)) return 1;
    clear_screen(sdl);

    render_image(file, &bmp, &sdl, render_image_normal);

    bmp.state = RUNNING;
    while(bmp.state != QUIT){
        handle_input(file, &bmp, &sdl);
        SDL_Delay(41.5f); // ≈ 24 fps 
    }
    
    free(bmp.img_data);
    fclose(file);
    final_cleanup(sdl);
}