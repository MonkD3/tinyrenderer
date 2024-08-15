#include "include/shader.h"
#include "include/geometry.h"
#include "include/scene.h"
#include "include/tgaimage.h"
#include <math.h>

Shader_t const GouraudShader = {
    .vsh = gouraud_vertex_sh,
    .fsh = gouraud_fragment_sh,
    .datasize = sizeof(GouraudShaderData_t)
};


void gouraud_vertex_sh(Vec3f *v_out, const OBJModel_t *model, int32_t fx, int32_t vx, void *shd){
    GouraudShaderData_t * data = (GouraudShaderData_t*) shd;
    
    int32_t face = model->fx[fx];
    Vec3f * gl_Vertex = (Vec3f*) &model->v[model->fvx[face+vx]*model->dv];
    Vec3f * normal = (Vec3f*) &model->n[model->fnx[face+vx]*3];

    float vintensity = Vec3f_scal(&_scene.light, normal);
    data->intensity.raw[vx] = fmaxf(vintensity, 0.0f);
    
    Vec3f_transform(v_out, gl_Vertex, &_scene.transform, 1);
}

bool gouraud_fragment_sh(TGAColor_t *c, const Vec3f *bc, const void *shd){
    GouraudShaderData_t const * data = (GouraudShaderData_t*) shd;
    float intensity = bc->x*data->intensity.x + bc->y*data->intensity.y + bc->z*data->intensity.z;
    *c = (TGAColor_t) {.r=255*intensity, .g=255*intensity, .b=255*intensity, .a=255};
    return false;
}
