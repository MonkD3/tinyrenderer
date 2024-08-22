#ifndef __SCENE_H
#define __SCENE_H

#include "geometry.h"
#include <stdbool.h>

typedef struct {
    float t[16];
} Transform3f;


typedef struct {
    Vec3i dim;            // Dimension of the scene
    Vec3f camera_pos;     // Position of the camera in the world
    Vec3f camera_vert;    // Which way points upward
    Vec3f center;         // Center of the scene
    Vec3f light;          // Lighting vector
    
    Transform3f modelview; 
    Transform3f viewport;  
    Transform3f proj;
    Transform3f transform; 
    Transform3f transform_it; // Inverse-transpose
    bool is_ready;
} Scene_t;

extern Scene_t _scene; 

void Scene_prepare(void);
void Scene_finish(void);

void Scene_set_proj(void);
void Scene_set_viewport(int32_t x, int32_t y);
void Scene_set_modelview(void);
void Scene_set_dim(Vec3i const* v);
void Scene_set_campos(Vec3f const* v);
void Scene_set_up(Vec3f const* v);
void Scene_set_center(Vec3f const* v);
void Scene_set_light(Vec3f const* v);

void Transform3f_translate(Transform3f* transform, Vec3f const* translation);
void Transform3f_scale(Transform3f* transform, Vec3f const* scaling);
void Transform3f_rotate(Transform3f* transform, Vec3f const* rotation);

void Transform3f_compute_translation(Transform3f* out, Vec3f const* translation);
void Transform3f_compute_scaling(Transform3f* out, Vec3f const* scaling);

void Transform3f_compute_proj(Transform3f* out, Scene_t const * s);
void Transform3f_compute_viewport(Transform3f* out, int32_t x, int32_t y, Vec3i const * dim);
void Transform3f_compute_modelview(Transform3f* out, Scene_t const * scene);

// Compose nt transforms into a single one
void Transform3f_compose(Transform3f* out, Transform3f const* t, int32_t const nt);
void Transform3f_compute_inverse_transpose(Transform3f* tr);

// Apply the transformation t to the vertices v_in and store them in v_out
void Vec3f_transform(Vec3f * v_out, Vec3f const * v_in, Transform3f const * t, int32_t const n);

#endif // __SCENE_H
