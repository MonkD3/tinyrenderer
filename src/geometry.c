#include "include/geometry.h"
#include <math.h>

void vecaxpby(double* r, double const* x, double const* y, double a, double b, int32_t const n){
    for (int32_t i = 0; i < n; i++) r[i] = a*x[i] + b*y[i];
}

void vecmul(double *x, double const a, int32_t const n){
    for (int32_t i = 0; i < n; i++) x[i] *= a;
}

void vecnormalize(double * x, int32_t const n){
    vecmul(x, 1.0/vecnorm(x, n), n);
}

double vecnorm(double const* x, int32_t const n){
    double nrm = vecscal(x, x, n);
    return sqrt(nrm);
}

double vecscal(double const* x, double const* y, int32_t const n){
    double sum = 0.0;
    for (int32_t i = 0; i < n; i++) sum += x[i]*y[i];
    return sum;
}
