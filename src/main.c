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

int main(int argc, char** argv){
    char* filename = "output.tga";
    char* objname = "assets/african_head.obj";
    if (argc > 1) objname = argv[1];
    
    TGAImage_t img, tx={0};
    TGAImage_init(&img, _scene.dim.x, _scene.dim.y, RGB);
    TGAImage_read_tga_file(&tx, "assets/african_head_diffuse.tga");
    TGAImage_flip_vertically(&tx);

    OBJModel_t obj = {0};
    OBJModel_read_file(&obj, objname);

    bench_t* b = bench_init(100, 1000, 0.01);

    float * zbuff = malloc(img.width*img.height*sizeof(float));
    for (int32_t i = 0; i < img.width*img.height; i++) zbuff[i] = FLT_MIN;

    Scene_set_light(&(Vec3f){.x=-1.0f, .y=1.0f, .z=-1.0f});
    Scene_set_campos(&(Vec3f) {.x=1.0f, .y=1.0f, .z=3.0f});
    Scene_set_proj();
    Scene_set_viewport(0, 0);
    Scene_set_modelview();

    Transform3f_compose(&_scene.transform, (Transform3f[]){_scene.viewport, _scene.proj, _scene.modelview}, 3);

    float * v_tr = malloc(sizeof(float) * obj.nv * obj.dv);
    BENCH_START(b);
    Vec3f_transform((Vec3f*)v_tr, (Vec3f*)obj.v, &_scene.transform, obj.nv);
    const int32_t dv = obj.dv;
    const int32_t dt = obj.dt;
    Vec3i v[3], t[3];
    Vec3f e1, e2, normal;
    Vec3f light = _scene.light;
    float light_intensity[3];
    for (uint64_t i = 0; i < obj.nf; i++){
        uint64_t j = obj.fx[i];
        Vec3f * wv0 = (Vec3f*)&(v_tr[obj.fvx[j]*dv]);
        Vec3f * wv1 = (Vec3f*)&(v_tr[obj.fvx[j+1]*dv]);
        Vec3f * wv2 = (Vec3f*)&(v_tr[obj.fvx[j+2]*dv]);

        Vec3f_axpby(&e1, wv1, wv0, 1.0, -1.0);
        Vec3f_axpby(&e2, wv2, wv0, 1.0, -1.0);

        Vec3f_cross(&normal, &e2, &e1);
        Vec3f_normalize(&normal);

        Vec3f * wt0 = (Vec3f*)&(obj.t[obj.ftx[j]*dt]);
        Vec3f * wt1 = (Vec3f*)&(obj.t[obj.ftx[j+1]*dt]);
        Vec3f * wt2 = (Vec3f*)&(obj.t[obj.ftx[j+2]*dt]);

        v[0] = (Vec3i){.x=wv0->x, .y=wv0->y, .z=wv0->z};
        v[1] = (Vec3i){.x=wv1->x, .y=wv1->y, .z=wv1->z};
        v[2] = (Vec3i){.x=wv2->x, .y=wv2->y, .z=wv2->z};

        t[0] = (Vec3i) {.x=wt0->x*tx.width, .y=wt0->y*tx.height, .z=0};
        t[1] = (Vec3i) {.x=wt1->x*tx.width, .y=wt1->y*tx.height, .z=0};
        t[2] = (Vec3i) {.x=wt2->x*tx.width, .y=wt2->y*tx.height, .z=0};

        Vec3f * n0 = (Vec3f*)&(obj.n[obj.fnx[j]*3]);
        Vec3f * n1 = (Vec3f*)&(obj.n[obj.fnx[j+1]*3]);
        Vec3f * n2 = (Vec3f*)&(obj.n[obj.fnx[j+2]*3]);

        light_intensity[0] = -Vec3f_scal(n0, &light);
        light_intensity[1] = -Vec3f_scal(n1, &light);
        light_intensity[2] = -Vec3f_scal(n2, &light);

        Draw_tri_texture_z(v, t, light_intensity, zbuff, &img, &tx);
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

