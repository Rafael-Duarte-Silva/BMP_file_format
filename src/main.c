#include <stdio.h>

#include "screen.h"
#include "bmp.h"

int main(int argc, char *argv[]) {
    FILE *file = fopen(argv[1], "rb");

    if(file == NULL){
        printf("File not found\n");
        return 1;
    }

    bmp_t bmp = {0};

    get_header(file, &bmp);
    get_info_header(file, &bmp);
    get_color_table(file, &bmp);
    if(get_image_data(file, &bmp) == false) return 1;

    sdl_t sdl = {0};
    if(init_sdl(&sdl, &bmp) == false) return 1;
    clear_screen(sdl);

    render_image(file, &bmp, &sdl);

    bmp.state = RUNNING;
    while(bmp.state != QUIT){
        handle_input(file, &bmp, &sdl);
        SDL_Delay(41.5f); // ≈ 24 fps 
    }
    
    free(bmp.img_data);
    fclose(file);
    final_cleanup(sdl);
}