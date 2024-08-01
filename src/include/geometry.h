#ifndef __GEOMETRY_H
#define __GEOMETRY_H
#include <stdint.h>

#define X_COORD 0
#define Y_COORD 1
#define Z_COORD 2

void vecaxpby(double* r, double const* x, double const* y, double a, double b, int32_t const n);
void vecax(double *x, double const a, int32_t const n);
void vecnormalize(double * x, int32_t const n);

double vecnorm(double const* x, int32_t const n);
double vecscal(double const* x, double const* y, int32_t const n);

#endif // __GEOMETRY_H
