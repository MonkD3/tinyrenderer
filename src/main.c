#include "include/tgaimage.h"
#include "include/drawing.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const TGAColor_t white = {.r=255, .g=255, .b=255, .a=255};
const TGAColor_t red   = {.r=255, .g=0,   .b=0,   .a=255};
const int32_t WIDTH  = 1000;
const int32_t HEIGHT = 1000;

int main(void){
    TGAImage_t img;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_flip_vertically(&img);

    int32_t const xc = WIDTH/2;
    int32_t const yc = HEIGHT/2;

    for (int32_t angle = 0; angle <= 90; angle += 10){
        float rad = angle / 180.0f * M_PI;
        int32_t x0 = xc - cosf(rad)*WIDTH/2;
        int32_t x1 = xc + cosf(rad)*WIDTH/2;
        int32_t y0 = yc - sinf(rad)*HEIGHT/2;
        int32_t y1 = yc + sinf(rad)*HEIGHT/2;

        line(x0, y0, x1, y1, &img, &red);
    }

    TGAImage_write_tga_file(&img, "lines.tga", true);
    TGAImage_destroy(&img);
    return EXIT_SUCCESS;
}

