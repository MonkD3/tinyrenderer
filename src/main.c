#include "include/geometry.h"
#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/objparser.h"
#include <math.h>
#include <float.h>
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
const int32_t DEPTH  = 10000000;
const Vec3i scene_dim = {.x = WIDTH, .y = HEIGHT, .z = DEPTH};

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);

    bench_t* b = bench_init(100, 1000, 0.01);

    float * zbuff = malloc(img.width*img.height*sizeof(float));
    int32_t * zbufi = malloc(img.width*img.height*sizeof(int32_t));
    for (int32_t i = 0; i < img.width*img.height; i++) {
        zbufi[i] = INT32_MIN;
        zbuff[i] = FLT_MIN;
    }

    const int32_t dv = obj.dv;
    Vec3f light = {.x=0, .y=0, .z=-1.0f};
    Vec3f_normalize(&light);

    BENCH_START(b);
    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        Vec3f * wv0 = (Vec3f*)&(obj.v[obj.fvx[j]*dv]);
        Vec3f * wv1 = (Vec3f*)&(obj.v[obj.fvx[j+1]*dv]);
        Vec3f * wv2 = (Vec3f*)&(obj.v[obj.fvx[j+2]*dv]);

        Vec3f e1, e2, normal;
        Vec3f_axpby(&e1, wv1, wv0, 1.0, -1.0);
        Vec3f_axpby(&e2, wv2, wv0, 1.0, -1.0);

        Vec3f_cross(&normal, &e2, &e1);
        Vec3f_normalize(&normal);

        float scalar = Vec3f_scal(&light, &normal);
        if (scalar > 0){
            TGAColor_t color = {.r=scalar*255, .g=scalar*255, .b=scalar*255, .a=255};
            Vec3i v[3];
            world2scene(v, wv0, &scene_dim);
            world2scene(v+1, wv1, &scene_dim);
            world2scene(v+2, wv2, &scene_dim);
            
            Draw_tri_uniform_bcz(v, zbuff, &img, &color);
        }
    }
    BENCH_STOP(b);
    BENCH_OUTPUT(b);

    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, "barycentric_rendering.tga", true);
    TGAImage_flip_vertically(&img);

    bench_destroy(b);
    b = bench_init(100, 1000, 0.01);

    BENCH_START(b);
    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        Vec3f * wv0 = (Vec3f*)&(obj.v[obj.fvx[j]*dv]);
        Vec3f * wv1 = (Vec3f*)&(obj.v[obj.fvx[j+1]*dv]);
        Vec3f * wv2 = (Vec3f*)&(obj.v[obj.fvx[j+2]*dv]);

        Vec3f e1, e2, normal;
        Vec3f_axpby(&e1, wv1, wv0, 1.0, -1.0);
        Vec3f_axpby(&e2, wv2, wv0, 1.0, -1.0);

        Vec3f_cross(&normal, &e2, &e1);
        Vec3f_normalize(&normal);

        float scalar = Vec3f_scal(&light, &normal);
        if (scalar > 0){
            TGAColor_t color = {.r=scalar*255, .g=scalar*255, .b=scalar*255, .a=255};
            Vec3i v[3];
            world2scene(v, wv0, &scene_dim);
            world2scene(v+1, wv1, &scene_dim);
            world2scene(v+2, wv2, &scene_dim);
            
            Draw_tri_uniform_z(v, zbufi, &img, &color);
        }
    }
    BENCH_STOP(b);
    BENCH_OUTPUT(b);

    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, "pixel_rendering.tga", true);

    free(zbuff);
    free(zbufi);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    OBJModel_destroy(&obj);
    return EXIT_SUCCESS;
}

