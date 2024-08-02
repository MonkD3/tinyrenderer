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
