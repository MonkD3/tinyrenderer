#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/objparser.h"
#include <math.h>
#include <stdlib.h>
#include "benchmarks.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const TGAColor_t white = {.r=255, .g=255, .b=255, .a=255};
const TGAColor_t red   = {.r=255, .g=0,   .b=0,   .a=255};
const int32_t WIDTH  = 1000;
const int32_t HEIGHT = 1000;

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);
    
    TGAImage_t img;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_flip_vertically(&img);

    const int32_t dv = obj.dv;
    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        uint64_t nvf = obj.fx[i+1] - j;
        for (uint64_t k = 0; k < nvf; k++){
            int32_t x0 = (obj.v[obj.fvx[j+k]*dv] + 1.) * WIDTH / 2;
            int32_t y0 = (obj.v[obj.fvx[j+k]*dv+1] + 1.) * HEIGHT / 2;
            int32_t x1 = (obj.v[obj.fvx[j+(1+k)%nvf]*dv] + 1.) * WIDTH / 2;
            int32_t y1 = (obj.v[obj.fvx[j+(1+k)%nvf]*dv + 1] + 1.) * HEIGHT / 2;
            line(x0, y0, x1, y1, &img, &white);
        }
    }

    TGAImage_flip_vertically(&img);

    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    OBJModel_destroy(&obj);
    return EXIT_SUCCESS;
}

