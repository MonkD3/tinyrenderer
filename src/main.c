#include "include/geometry.h"
#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/objparser.h"
#include "include/scene.h"
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
const int32_t DEPTH  = 1000;
const Vec3i scene_dim = {.x = WIDTH, .y = HEIGHT, .z = DEPTH};
const Scene_t scene = {
    .camera_pos={.x=0.0f, .y=0.0f, .z=3.0f},
    .camera_direction={.x=0.0f, .y=0.0f, .z=-1.0f}
};

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img, tx={0};
    TGAImage_init(&img, WIDTH, HEIGHT, RGB);
    TGAImage_read_tga_file(&tx, "assets/african_head_diffuse.tga");
    TGAImage_flip_vertically(&tx);

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);

    bench_t* b = bench_init(100, 1000, 0.01);

    float * zbuff = malloc(img.width*img.height*sizeof(float));
    for (int32_t i = 0; i < img.width*img.height; i++) zbuff[i] = FLT_MIN;

    Vec3f light = {.x=0, .y=0, .z=-1.0f};
    Vec3f_normalize(&light);

    Transform3f tr; 
    Transform3f_get_camera_projection(&tr, &scene);

    float * v_tr = malloc(sizeof(float) * obj.nv * obj.dv);
    BENCH_START(b);
    Vec3f_transform((Vec3f*)v_tr, (Vec3f*)obj.v, &tr, obj.nv);
    const int32_t dv = obj.dv;
    const int32_t dt = obj.dt;
    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        Vec3f * wv0 = (Vec3f*)&(v_tr[obj.fvx[j]*dv]);
        Vec3f * wv1 = (Vec3f*)&(v_tr[obj.fvx[j+1]*dv]);
        Vec3f * wv2 = (Vec3f*)&(v_tr[obj.fvx[j+2]*dv]);

        Vec3f e1, e2, normal;
        Vec3f_axpby(&e1, wv1, wv0, 1.0, -1.0);
        Vec3f_axpby(&e2, wv2, wv0, 1.0, -1.0);

        Vec3f_cross(&normal, &e2, &e1);
        Vec3f_normalize(&normal);

        float scalar = Vec3f_scal(&light, &normal);
        if (scalar > 0){
            Vec3f * wt0 = (Vec3f*)&(obj.t[obj.ftx[j]*dt]);
            Vec3f * wt1 = (Vec3f*)&(obj.t[obj.ftx[j+1]*dt]);
            Vec3f * wt2 = (Vec3f*)&(obj.t[obj.ftx[j+2]*dt]);

            Vec3i v[3], t[3];
            world2scene(v, wv0, &scene_dim);
            world2scene(v+1, wv1, &scene_dim);
            world2scene(v+2, wv2, &scene_dim);

            t[0] = (Vec3i) {.x=wt0->x*tx.width, .y=wt0->y*tx.height, .z=0};
            t[1] = (Vec3i) {.x=wt1->x*tx.width, .y=wt1->y*tx.height, .z=0};
            t[2] = (Vec3i) {.x=wt2->x*tx.width, .y=wt2->y*tx.height, .z=0};
            
            Draw_tri_texture_z(v, t, zbuff, &img, &tx);
        }
    }
    BENCH_STOP(b);
    BENCH_OUTPUT(b);
    bench_destroy(b);

    free(zbuff);
    free(v_tr);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    TGAImage_destroy(&tx);
    OBJModel_destroy(&obj);
    return EXIT_SUCCESS;
}

