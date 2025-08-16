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

    header(file, &bmp);
    info_header(file, &bmp);
    color_table(file, &bmp);

    sdl_t sdl = {0};
    if(init_sdl(&sdl, &bmp) == false) return 1;
    clear_screen(sdl);

    image(file, &bmp, &sdl);

    bmp.state = RUNNING;
    while(bmp.state != QUIT){
        handle_input(file, &bmp, &sdl);
    }
    
    final_cleanup(sdl);
}