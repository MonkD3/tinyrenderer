#include "include/scene.h"
#include <string.h>

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

void Transform3f_get_translation(Transform3f* out, Vec3f const* translation){
    *out = (Transform3f) {.t={
        1.0f, 0.0f, 0.0f, translation->x,
        0.0f, 1.0f, 0.0f, translation->y,
        0.0f, 0.0f, 1.0f, translation->z,
        0.0f, 0.0f, 0.0f, 1.0f,
    }};
}
// Get the transformation corresponding to a scaling
void Transform3f_get_scaling(Transform3f* out, Vec3f const* scaling){
    *out = (Transform3f) {.t={
        scaling->x, 0.0f,       0.0f,       0.0f,
        0.0f,       scaling->y, 0.0f,       0.0f,
        0.0f,       0.0f,       scaling->z, 0.0f,
        0.0f,       0.0f,       0.0f,       1.0f,
    }};
}

void Transform3f_get_camera_projection(Transform3f* tr, Scene_t const * s){
    *tr = (Transform3f) {.t={
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -1.0f/s->camera_pos.z, 1.0f
    }};
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
