#ifndef __SCENE_H
#define __SCENE_H

#include "geometry.h"

typedef struct {
    Vec3f camera_pos;        // Position of the camera in the world
    Vec3f camera_direction;  // Direction of the camera in the world
} Scene_t;

typedef struct {
    float t[16];
} Transform3f;
// Apply the translation to the transform. Equivalent to Transform3f_get_translation 
// followed by Transform3f_compose
void Transform3f_translate(Transform3f* transform, Vec3f const* translation);
// Apply the scaling to the transform. Equivalent to Transform3f_get_scaling 
// followed by Transform3f_compose
void Transform3f_scale(Transform3f* transform, Vec3f const* scaling);

// Compose nt transforms into a single one
void Transform3f_compose(Transform3f* out, Transform3f const* t, int32_t const nt);

// Get the transformation corresponding to a translation
void Transform3f_get_translation(Transform3f* out, Vec3f const* translation);
// Get the transformation corresponding to a scaling
void Transform3f_get_scaling(Transform3f* out, Vec3f const* scaling);
// Get the transformation corresponding to the camera projection
void Transform3f_get_camera_projection(Transform3f* out, Scene_t const * s);

// Apply the transformation t to the vertices v_in and store them in v_out
void Vec3f_transform(Vec3f * v_out, Vec3f const * v_in, Transform3f const * t, int32_t const n);

#endif // __SCENE_H
