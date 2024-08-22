#include "include/geometry.h"
#include "include/shader.h"
#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/objparser.h"
#include "include/scene.h"

#include "benchmarks.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>

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
    
    TGAImage_t img;
    TGAImage_init(&img, _scene.dim.x, _scene.dim.y, RGB);

    OBJModel_t *obj = OBJModel_init();
    OBJModel_read_file(obj, objname);
    OBJModel_read_texture(obj, "assets/african_head_diffuse.tga");
    OBJModel_read_normalmap(obj, "assets/african_head_nm.tga");
    OBJModel_read_specularmap(obj, "assets/african_head_spec.tga");

    Scene_prepare();
    Scene_set_light(&(Vec3f){.x=1.0f, .y=-1.0f, .z=1.0f});
    Scene_set_campos(&(Vec3f) {.x=1.0f, .y=1.0f, .z=3.0f});
    Scene_set_proj();
    Scene_set_viewport(0, 0);
    Scene_set_modelview();
    Scene_finish();

    float * zbuff = malloc(img.width*img.height*sizeof(float));
    for (int32_t i = 0; i < img.width*img.height; i++) zbuff[i] = FLT_MIN;
    TextureNormalShaderData_t* shd = malloc(TextureNormalShader.datasize);
    Transform3f_compose(&shd->uniform_M, (Transform3f[]){_scene.proj, _scene.modelview}, 2);
    Transform3f_compose(&shd->uniform_MIT, (Transform3f[]){_scene.proj, _scene.modelview}, 2);
    Transform3f_compute_inverse_transpose(&shd->uniform_MIT);

    bench_t* b = bench_init(100, 1000, 0.01);
    BENCH_START(b);
    Vec3f screen[3];
    for (int32_t i = 0; i < obj->nf; i++){
        for (int32_t j = 0; j < 3; j++){
            TextureNormalShader.vsh(screen+j, obj, i, j, shd);
        }
        Draw_tri_shader(screen, &TextureNormalShader, obj, shd, zbuff, &img);
    }
    BENCH_STOP(b);
    BENCH_OUTPUT(b);


    bench_destroy(b);

    free(zbuff);
    free(shd);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, filename, true);
    TGAImage_destroy(&img);
    OBJModel_destroy(obj);
    free(obj);
    return EXIT_SUCCESS;
}

