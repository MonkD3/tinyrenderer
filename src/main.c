#include "include/geometry.h"
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
const int32_t WIDTH  = 1000;
const int32_t HEIGHT = 1000;

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_flip_vertically(&img);

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);

    const int32_t dv = obj.dv;
    float light[3] = {0, 0, -1.0f};
    vecnormalize(light, 3);

    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        float * v0 = &(obj.v[obj.fvx[j]*dv]);
        float * v1 = &(obj.v[obj.fvx[j+1]*dv]);
        float * v2 = &(obj.v[obj.fvx[j+2]*dv]);

        float e1[3], e2[3];
        vecaxpby(e1, v1, v0, 1.0, -1.0, 3);
        vecaxpby(e2, v2, v0, 1.0, -1.0, 3);

        float normal[3] = {e2[1]*e1[2] - e2[2]*e1[1], e2[2]*e1[0] - e2[0]*e1[2], e2[0]*e1[1] - e2[1]*e1[0]};
        vecnormalize(normal, 3);

        float scalar = vecscal(light, normal, 3);
        if (scalar > 0){
            int32_t v0i[2] = {(v0[0]+1.)*WIDTH/2, (v0[1]+1.)*HEIGHT/2};
            int32_t v1i[2] = {(v1[0]+1.)*WIDTH/2, (v1[1]+1.)*HEIGHT/2};
            int32_t v2i[2] = {(v2[0]+1.)*WIDTH/2, (v2[1]+1.)*HEIGHT/2};

            TGAColor_t color = {.r=scalar*255, .g=scalar*255, .b=scalar*255, .a=255};
            triangle2D(v0i, v1i, v2i, &img, &color);
        }
    }

    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    OBJModel_destroy(&obj);
    return EXIT_SUCCESS;
}

