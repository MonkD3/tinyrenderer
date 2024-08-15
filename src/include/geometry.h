#ifndef __GEOMETRY_H
#define __GEOMETRY_H
#include <stdint.h>

#define X_COORD 0
#define Y_COORD 1
#define Z_COORD 2

// =========== General linear algebra operations ==========
void vecaxpby(float* r, float const* x, float const* y, float a, float b, int32_t const n);
void vecax(float *x, float const a, int32_t const n);
void vecnormalize(float * x, int32_t const n);

float vecnorm(float const* x, int32_t const n);
float vecscal(float const* x, float const* y, int32_t const n);

// =========== 3D vectors ============
typedef union {
    struct {
        int32_t x,y,z;
    };
    int32_t raw[3];
} Vec3i;
void bounding_box(Vec3i* bbmin, Vec3i* bbmax, Vec3i const* v, int32_t nv);
void Vec3i_cross(Vec3i* out, Vec3i const* v1, Vec3i const* v2);

typedef union {
    struct {
        float x,y,z;
    };
    float raw[3];
} Vec3f;
void bounding_boxf(Vec3i* bbmin, Vec3i* bbmax, Vec3f const* v, int32_t nv);

void  world2scene(Vec3i* out, Vec3f const* in, Vec3i const* dim);
void  barycentric(Vec3f *bc, Vec3i const * v0, Vec3i const* v1, Vec3i const * v2, Vec3i const* px);
void barycentricf(Vec3f *bc, Vec3f const * v0, Vec3f const* v1, Vec3f const * v2, Vec3i const* px);
void  Vec3f_axpby(Vec3f* r, Vec3f const* x, Vec3f const* y, float a, float b);
void  Vec3f_ax(Vec3f* x, float a);
void  Vec3f_normalize(Vec3f * x);
float Vec3f_norm(Vec3f const* x);
float Vec3f_scal(Vec3f const* x, Vec3f const* y);
void  Vec3f_cross(Vec3f* out, Vec3f const* v1, Vec3f const* v2);





#endif // __GEOMETRY_H
