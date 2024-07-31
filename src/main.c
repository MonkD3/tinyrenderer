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
const TGAColor_t green = {.r=0,   .g=255, .b=0,   .a=255};
const int32_t WIDTH  = 200;
const int32_t HEIGHT = 200;

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_flip_vertically(&img);
 
    int32_t t0[3][2] = {{10, 70}, {50, 160},  {70, 80}}; 
    int32_t t1[3][2] = {{180, 50},  {150, 1},   {70, 180}}; 
    int32_t t2[3][2] = {{180, 150}, {120, 160}, {130, 180}}; 

    bench_t* b = bench_init(1000, 10000, 0.01);
    BENCH_START(b)
    triangle2D(t0[0], t0[1], t0[2], &img, &red); 
    triangle2D(t1[0], t1[1], t1[2], &img, &white); 
    triangle2D(t2[0], t2[1], t2[2], &img, &green); 
    BENCH_STOP(b);
    BENCH_OUTPUT(b);

    bench_destroy(b);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    return EXIT_SUCCESS;
}

