#include "include/scene.h"
#include "include/geometry.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <lapack.h>

Scene_t _scene = {
    .dim         = {.x=1000, .y=1000, .z=1000},
    .camera_pos  = {.x=0.0f, .y=0.0f, .z=1.0f},
    .camera_vert = {.x=0.0f, .y=1.0f, .z=0.0f},
    .center      = {.x=0.0f, .y=0.0f, .z=0.0f},
    .light       = {.x=0.0f, .y=0.0f, .z=-1.0f},
    .modelview   = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }},
    .viewport = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }},
    .proj = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }},
    .transform = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }},
    .transform_it = {{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }},
    .is_ready = 0,
};

// Apply the translation to the transform
void Transform3f_translate(Transform3f* transform, Vec3f const* translation){
    transform->t[3] += translation->x;
    transform->t[7] += translation->y;
    transform->t[11] += translation->z;
}

// Apply the scaling to the transform
void Transform3f_scale(Transform3f* transform, Vec3f const* scaling){
    transform->t[0] *= scaling->x;
    transform->t[5] *= scaling->y;
    transform->t[10] *= scaling->z;
}

void Transform3f_rotate(Transform3f* transform, Vec3f const* rotation){
    Transform3f const Rx = {.t={
        1.0f, 0.0f,               0.0f,              0.0f, 
        0.0f, cosf(rotation->x),  sinf(rotation->x), 0.0f, 
        0.0f, -sinf(rotation->x), cosf(rotation->y), 0.0f, 
        0.0f, 0.0f,               0.0f,              1.0f, 
    }};

    Transform3f const Ry = {.t={
        cosf(rotation->y),  0.0f, -sinf(rotation->y), 0.0f,
        0.0f,               1.0f, 0.0f,               0.0f,
        sinf(rotation->y),  0.0f, cosf(rotation->y),  0.0f,
        0.0f,               0.0f, 0.0f,               1.0f,
    }};

    Transform3f const Rz = {.t={
        cosf(rotation->z),  sinf(rotation->z), 0.0f, 0.0f,
        -sinf(rotation->z), cosf(rotation->z), 0.0f, 0.0f,
        0.0f,               0.0f,              1.0f, 0.0f, 
        0.0f,               0.0f,              0.0f, 1.0f, 
    }};

    Transform3f_compose(transform, (Transform3f[]){*transform, Rx, Ry, Rz}, 4);
}

// Compose n transforms into a single one
void Transform3f_compose(Transform3f* out, Transform3f const* t, int32_t const nt){
    *out = (Transform3f) {.t={
        1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f
    }};

    for (int32_t n = 0; n < nt; n++){
        float tmp[16] = {0};
        for (int32_t i = 0; i < 4; i++){
            for (int32_t j = 0; j < 4; j++){
                for (int32_t k = 0; k < 4; k++){
                    tmp[i*4 + j] += out->t[i*4 + k] * t[n].t[k*4+j];
                }
            }
        }
        memcpy(out->t, tmp, sizeof(float)*16);
    }
}

void Transform3f_compute_translation(Transform3f* out, Vec3f const* translation){
    *out = (Transform3f) {.t={
        1.0f, 0.0f, 0.0f, translation->x,
        0.0f, 1.0f, 0.0f, translation->y,
        0.0f, 0.0f, 1.0f, translation->z,
        0.0f, 0.0f, 0.0f, 1.0f,
    }};
}

void Transform3f_compute_viewport(Transform3f* out, int32_t x, int32_t y, Vec3i const * dim){
    *out = (Transform3f){.t={
        0.5f*dim->x, 0.0f,        0.0f,        x + 0.5f*dim->x,
        0.0f,        0.5f*dim->y, 0.0f,        y + 0.5f*dim->y,
        0.0f,        0.0f,        0.5f*dim->z, 0.5f*dim->z,
        0.0f,        0.0f,        0.0f,        1.0f
    }};
}

void Transform3f_compute_modelview(Transform3f* out, Scene_t const * scene){
    Vec3f x, y, z;

    Vec3f_axpby(&z, &scene->center, &scene->camera_pos, -1.0f, 1.0f);
    Vec3f_normalize(&z); 

    Vec3f_cross(&x, &scene->camera_vert, &z);
    Vec3f_normalize(&x);
    Vec3f_cross(&y, &z, &x);
    Vec3f_normalize(&y);

    *out = (Transform3f){.t={
        x.x,  x.y,  x.z,  -scene->center.x,
        y.x,  y.y,  y.z,  -scene->center.y,
        z.x,  z.y,  z.z,  -scene->center.z,
        0.0f, 0.0f, 0.0f, 1.0f
    }};
}

void Scene_prepare(void){
    _scene.is_ready = 0;
}

void Scene_finish(void){
    _scene.is_ready = 1;
    Transform3f_compose(&_scene.transform, (Transform3f[]){_scene.viewport, _scene.proj, _scene.modelview}, 3);
}

void Scene_set_proj(void){ 
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    Transform3f_compute_proj(&(_scene.proj), &_scene); 
}

void Scene_set_viewport(int32_t x, int32_t y){ 
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    Transform3f_compute_viewport(&(_scene.viewport), x, y, &(_scene.dim)); 
}

void Scene_set_modelview(void){ 
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    Transform3f_compute_modelview(&(_scene.modelview), &_scene); 
}

void Scene_set_dim(Vec3i const* v){
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    _scene.dim.x = v->x;
    _scene.dim.y = v->y;
    _scene.dim.z = v->z;
}

void Scene_set_campos(Vec3f const* v){
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    _scene.camera_pos.x = v->x;
    _scene.camera_pos.y = v->y;
    _scene.camera_pos.z = v->z;
}
void Scene_set_up(Vec3f const* v){
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    _scene.camera_vert.x = v->x;
    _scene.camera_vert.y = v->y;
    _scene.camera_vert.z = v->z;
}
void Scene_set_center(Vec3f const* v){
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    _scene.center.x = v->x;
    _scene.center.y = v->y;
    _scene.center.z = v->z;
}

void Scene_set_light(Vec3f const* v){
    if (_scene.is_ready){
        fprintf(stderr, "Scene cannot be modified, call Scene_prepare() to modify it.");
        return;
    }
    _scene.light.x = v->x;
    _scene.light.y = v->y;
    _scene.light.z = v->z;
    Vec3f_normalize(&(_scene.light));
}

// Get the transformation corresponding to a scaling
void Transform3f_compute_scaling(Transform3f* out, Vec3f const* scaling){
    *out = (Transform3f) {.t={
        scaling->x, 0.0f,       0.0f,       0.0f,
        0.0f,       scaling->y, 0.0f,       0.0f,
        0.0f,       0.0f,       scaling->z, 0.0f,
        0.0f,       0.0f,       0.0f,       1.0f,
    }};
}

void Transform3f_compute_proj(Transform3f* tr, Scene_t const * s){
    Vec3f look_dir = {
        .x=s->center.x - s->camera_pos.x,
        .y=s->center.y - s->camera_pos.y,
        .z=s->center.z - s->camera_pos.z,
    };
    float dist = Vec3f_norm(&look_dir);
    *tr = (Transform3f) {.t={
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f/dist, 1.0f
    }};
}

void Transform3f_compute_inverse_transpose(Transform3f* tr){
    int32_t ipiv[4] = {0, 1, 2, 3};
    int32_t info;
    int32_t dim = 4;
    LAPACK_sgetrf(&dim, &dim, tr->t, &dim, ipiv, &info);
    if (info){
        if (info > 0) {
            fprintf(stderr, "The factor U(%d, %d) is exactly singular, cannot compute inverse of the transformation\n", info, info);
        } else {
            fprintf(stderr, "The %d-th argument of SGETRF is illegal\n", -info);
        }
        return;
    }

    int32_t const lwork = 16;
    float work[lwork];
    LAPACK_sgetri(&dim, tr->t, &dim, ipiv, work, &lwork, &info);
    if (info){
        if (info > 0) {
            fprintf(stderr, "The factor U(%d, %d) is exactly singular, cannot compute inverse of the transformation\n", info, info);
        } else {
            fprintf(stderr, "The %d-th argument of SGETRI is illegal\n", -info);
        }
        return;
    }

    // Transpose the inverse
    for (int32_t i = 0; i < 4; i++){
        for (int32_t j = 0; j < i; j++){
            float tmp = tr->t[i*4+j];
            tr->t[i*4+j] = tr->t[i + 4*j];
            tr->t[i + 4*j] = tmp;
        }
    }
}

// Apply the transformation t to the vertices v_in and store them in v_out
void Vec3f_transform(Vec3f * v_out, Vec3f const * v_in, Transform3f const * t, int32_t const n){
    float const * mat = t->t;
    for (int32_t i = 0; i < n; i++){
        float const s = 1.f/(mat[12]*v_in[i].x + mat[13]*v_in[i].y + mat[14]*v_in[i].z + mat[15]);
        v_out[i].x = s*(mat[0]*v_in[i].x + mat[1]*v_in[i].y + mat[2]*v_in[i].z + mat[3]);
        v_out[i].y = s*(mat[4]*v_in[i].x + mat[5]*v_in[i].y + mat[6]*v_in[i].z + mat[7]);
        v_out[i].z = s*(mat[8]*v_in[i].x + mat[9]*v_in[i].y + mat[10]*v_in[i].z + mat[11]);
    }
}
