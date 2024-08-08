#include "include/geometry.h"
#include <math.h>

void vecaxpby(float* r, float const* x, float const* y, float a, float b, int32_t const n){
    for (int32_t i = 0; i < n; i++) r[i] = a*x[i] + b*y[i];
}

void vecax(float *x, float const a, int32_t const n){
    for (int32_t i = 0; i < n; i++) x[i] *= a;
}

void vecnormalize(float * x, int32_t const n){
    vecax(x, 1.0/vecnorm(x, n), n);
}

float vecnorm(float const* x, int32_t const n){
    float nrm = vecscal(x, x, n);
    return sqrt(nrm);
}

float vecscal(float const* x, float const* y, int32_t const n){
    float sum = 0.0;
    for (int32_t i = 0; i < n; i++) sum += x[i]*y[i];
    return sum;
}

// Convert the world coordinates into scene coordinates.
// World coordinates are in [-1, 1]^3 
// Scene coordinates are in [0, dim.x] x [0, dim.y] x [0, dim.z]
void world2scene(Vec3i* scene, Vec3f const* world, Vec3i const* dim){
    scene->x = (int32_t) (0.5f*(world->x+1.0f)*dim->x);
    scene->y = (int32_t) (0.5f*(world->y+1.0f)*dim->y);
    scene->z = (int32_t) (0.5f*(world->z+1.0f)*dim->z);
}

void barycentric(Vec3f *bc, Vec3i const * v0, Vec3i const* v1, Vec3i const * v2, Vec3i const* px) {
    Vec3i s0 = {
        .x = v2->x - v0->x,
        .y = v1->x - v0->x,
        .z = v0->x - px->x,
    };
    Vec3i s1 = {
        .x = v2->y - v0->y,
        .y = v1->y - v0->y,
        .z = v0->y - px->y,
    };
    Vec3i u;
    Vec3i_cross(&u, &s0, &s1);

    if (u.z) {
        float iuz = 1.f/u.z;
        *bc = (Vec3f){.x=1.f-(u.x+u.y)*iuz, .y=u.y*iuz, .z=u.x*iuz};
    }
    else     *bc = (Vec3f){.x=-1, .y=-1, .z=-1};
}


inline void Vec3f_axpby(Vec3f* r, Vec3f const* x, Vec3f const* y, float a, float b){
    r->raw[0] = a*x->raw[0] + b*y->raw[0];
    r->raw[1] = a*x->raw[1] + b*y->raw[1];
    r->raw[2] = a*x->raw[2] + b*y->raw[2];
}

inline void Vec3f_ax(Vec3f* x, float a){
    x->raw[0] *= a;
    x->raw[1] *= a;
    x->raw[2] *= a;
}

inline void Vec3f_normalize(Vec3f * x){
    Vec3f_ax(x, 1.0f/Vec3f_norm(x));
}

inline float Vec3f_norm(Vec3f const* x){
    return sqrtf(x->x*x->x + x->y*x->y + x->z*x->z);
}

inline float Vec3f_scal(Vec3f const* x, Vec3f const* y){
    return x->x*y->x + x->y*y->y + x->z*y->z;
}

inline void Vec3f_cross(Vec3f* out, Vec3f const* v0, Vec3f const* v1){
    out->x = v0->y*v1->z - v0->z*v1->y;
    out->y = v0->z*v1->x - v0->x*v1->z;
    out->z = v0->x*v1->y - v0->y*v1->x;
}

inline void Vec3i_cross(Vec3i* out, Vec3i const* v0, Vec3i const* v1){
    out->x = v0->y*v1->z - v0->z*v1->y;
    out->y = v0->z*v1->x - v0->x*v1->z;
    out->z = v0->x*v1->y - v0->y*v1->x;
}

void bounding_box(Vec3i* bbmin, Vec3i* bbmax, Vec3i const* v, int32_t nv){
    bbmin->x = bbmin->y = bbmin->z = INT32_MAX;
    bbmax->x = bbmax->y = bbmax->z = INT32_MIN;

    for (int32_t i = 0; i < nv; i++){
        bbmin->x = bbmin->x < v[i].x ? bbmin->x : v[i].x;
        bbmin->y = bbmin->y < v[i].y ? bbmin->y : v[i].y;
        bbmin->z = bbmin->z < v[i].z ? bbmin->z : v[i].z;

        bbmax->x = bbmax->x > v[i].x ? bbmax->x : v[i].x;
        bbmax->y = bbmax->y > v[i].y ? bbmax->y : v[i].y;
        bbmax->z = bbmax->z > v[i].z ? bbmax->z : v[i].z;
    }
}
