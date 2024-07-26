#include "tgaimage.h"
#include <stdlib.h>

const TGAColor_t white = {.r=255, .g=255, .b=255, .a=255};
const TGAColor_t red   = {.r=255, .g=0,   .b=0,   .a=255};

int main(void){
    TGAImage_t img;
    TGAImage_init(&img, 100, 100, RGB);
    TGAImage_set(&img, &red, 52, 41);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, "outputsmall.tga", true);

    TGAImage_scale(&img, 1920, 1080);
    TGAImage_write_tga_file(&img, "outputbig.tga", true);

    TGAImage_destroy(&img);
    return EXIT_SUCCESS;
}

