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
const int32_t DEPTH  = 1000;
const Vec3i scene_dim = {.x = WIDTH, .y = HEIGHT, .z = DEPTH};

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img, texture;
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_flip_vertically(&img);

    TGAImage_read_tga_file(&texture, "assets/african_head_diffuse.tga");
    TGAImage_flip_vertically(&texture);

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);

    float * zbuf = malloc(img.width*img.height*sizeof(float));
    for (int32_t i = 0; i < img.width*img.height; i++) zbuf[i] = INT32_MIN;

    const int32_t dv = obj.dv;
    const int32_t dt = obj.dt;
    Vec3f light = {.x=0, .y=0, .z=-1.0f};
    Vec3f_normalize(&light);

    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        Vec3f * wv0 = (Vec3f*)&(obj.v[obj.fvx[j]*dv]);
        Vec3f * wv1 = (Vec3f*)&(obj.v[obj.fvx[j+1]*dv]);
        Vec3f * wv2 = (Vec3f*)&(obj.v[obj.fvx[j+2]*dv]);

        Vec3f * wt0 = (Vec3f*)&(obj.t[obj.ftx[j]*dt]);
        Vec3f * wt1 = (Vec3f*)&(obj.t[obj.ftx[j+1]*dt]);
        Vec3f * wt2 = (Vec3f*)&(obj.t[obj.ftx[j+2]*dt]);

        Vec3f e1, e2, normal;
        Vec3f_axpby(&e1, wv1, wv0, 1.0, -1.0);
        Vec3f_axpby(&e2, wv2, wv0, 1.0, -1.0);

        Vec3f_cross(&normal, &e2, &e1);
        Vec3f_normalize(&normal);

        float scalar = Vec3f_scal(&light, &normal);
        if (scalar > 0){
            // TGAColor_t color = {.r=scalar*255, .g=scalar*255, .b=scalar*255, .a=255};
            Vec3i sv0, sv1, sv2;
            world2scene(&sv0, wv0, &scene_dim);
            world2scene(&sv1, wv1, &scene_dim);
            world2scene(&sv2, wv2, &scene_dim);

            Vec3i st0 = {.x=wt0->x * texture.width, .y=wt0->y*texture.height, .z=0};
            Vec3i st1 = {.x=wt1->x * texture.width, .y=wt1->y*texture.height, .z=0};
            Vec3i st2 = {.x=wt2->x * texture.width, .y=wt2->y*texture.height, .z=0};

            // triangle_barycentric(&sv0, &sv1, &sv2, zbuf, &img, &color);
            // triangleWithZbuf(&sv0, &sv1, &sv2, zbuf, &img, &color);
            
            Vec3i v[3] = {sv0, sv1, sv2};
            Vec3i t[3] = {st0, st1, st2};
            triangle_texture(v, t, zbuf, &img, &texture);
            
        }
    }

    free(zbuf);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    TGAImage_destroy(&texture);
    OBJModel_destroy(&obj);
    return EXIT_SUCCESS;
}

