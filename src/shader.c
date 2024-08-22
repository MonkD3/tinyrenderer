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

Shader_t const TextureShader = {
    .vsh = texture_vertex_sh,
    .fsh = texture_fragment_sh,
    .datasize = sizeof(TextureShaderData_t)
};

Shader_t const TextureNormalShader = {
    .vsh = texture_normal_vertex_sh,
    .fsh = texture_normal_fragment_sh,
    .datasize = sizeof(TextureNormalShaderData_t)
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

bool gouraud_fragment_sh(TGAColor_t *c, OBJModel_t const* model, Vec3f const *bc, void const *shd){
    GouraudShaderData_t const * data = (GouraudShaderData_t*) shd;
    float intensity = bc->x*data->intensity.x + bc->y*data->intensity.y + bc->z*data->intensity.z;
    *c = (TGAColor_t) {.r=255*intensity, .g=255*intensity, .b=255*intensity, .a=255};
    return false;
}

void texture_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd){
    TextureShaderData_t * data = (TextureShaderData_t*) shd;
    int32_t const face = model->fx[fx];

    Vec3f * normal   = (Vec3f*) &model->n[model->fnx[face+vx]*3];
    float vintensity = Vec3f_scal(&_scene.light, normal);
    data->intensity.raw[vx] = fmaxf(vintensity, 0.0f);

    Vec3f * texture = (Vec3f*) &model->t[model->ftx[face+vx]*model->dt];
    data->tpos[vx] = (Vec3f) {.x=texture->x*model->dmap->width, .y=texture->y*model->dmap->height, .z=texture->z};

    Vec3f * gl_Vertex = (Vec3f*) &model->v[model->fvx[face+vx]*model->dv];
    Vec3f_transform(v_out, gl_Vertex, &_scene.transform, 1);
}

bool texture_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd){
    TextureShaderData_t const * data = (TextureShaderData_t*) shd;
    float intensity = bc->x*data->intensity.x + bc->y*data->intensity.y + bc->z*data->intensity.z;
    Vec3f tpos = {
        .x = data->tpos[0].x * bc->x + data->tpos[1].x * bc->y + data->tpos[2].x * bc->z,
        .y = data->tpos[0].y * bc->x + data->tpos[1].y * bc->y + data->tpos[2].y * bc->z,
        .z = data->tpos[0].z * bc->x + data->tpos[1].z * bc->y + data->tpos[2].z * bc->z,
    };

    TGAColor_t* tcol = TGAImage_get(model->dmap, tpos.x, tpos.y);
    *c = (TGAColor_t) {.r=tcol->r*intensity, .g=tcol->g*intensity, .b=tcol->b*intensity, .a=255};
    return false;
}


void texture_normal_vertex_sh(Vec3f* v_out, OBJModel_t const* model, int32_t fx, int32_t vx, void * shd){
    TextureNormalShaderData_t * data = (TextureNormalShaderData_t*) shd;

    int32_t const face = model->fx[fx];

    Vec3f * texture = (Vec3f*) &model->t[model->ftx[face+vx]*model->dt];
    data->tpos[vx] = (Vec3f) {.x=texture->x, .y=texture->y, .z=texture->z};

    Vec3f * gl_Vertex = (Vec3f*) &model->v[model->fvx[face+vx]*model->dv];
    Vec3f_transform(v_out, gl_Vertex, &_scene.transform, 1);

}
bool texture_normal_fragment_sh(TGAColor_t* c, OBJModel_t const* model, Vec3f const * bc, void const * shd){
    TextureNormalShaderData_t const * data = (TextureNormalShaderData_t*) shd;
    Vec3f pos = {
        .x = (data->tpos[0].x * bc->x + data->tpos[1].x * bc->y + data->tpos[2].x * bc->z),
        .y = (data->tpos[0].y * bc->x + data->tpos[1].y * bc->y + data->tpos[2].y * bc->z),
        .z = 0,
    };
    TGAColor_t* tcol = TGAImage_get(model->dmap, pos.x*model->dmap->width, pos.y*model->dmap->height);
    TGAColor_t* normal_c = TGAImage_get(model->nmap, pos.x*model->nmap->width, pos.y*model->nmap->height);
    Vec3f normal_tmp = {.x=normal_c->r/127.5f - 1.f, .y=normal_c->g/127.5f - 1.f, .z=normal_c->b/127.5f - 1.f};

    Vec3f normal, light_t;
    Vec3f_transform(&normal, &normal_tmp, &data->uniform_MIT, 1);
    Vec3f_normalize(&normal);
    Vec3f_transform(&light_t, &_scene.light, &data->uniform_M, 1);
    Vec3f_normalize(&light_t);

    float const intensity = fmax(0.0f, Vec3f_scal(&normal, &light_t));

    *c = (TGAColor_t) {.r=tcol->r*intensity, .g=tcol->g*intensity, .b=tcol->b*intensity, .a=255};
    return false;
}
