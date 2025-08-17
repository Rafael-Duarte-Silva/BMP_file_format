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

    sdl_t sdl = {0};
    if(init_sdl(&sdl, &bmp) == false) return 1;
    clear_screen(sdl);

    render_image(file, &bmp, &sdl);

    bmp.state = RUNNING;
    SDL_Event event;
    while(bmp.state != QUIT){
        SDL_WaitEvent(&event);
        handle_input(event, file, &bmp, &sdl);
    }
    
    fclose(file);
    final_cleanup(sdl);
}