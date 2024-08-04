#ifndef __DRAWING_H
#define __DRAWING_H

#include "tgaimage.h"
#include "geometry.h"
#include <stdint.h>

// Draw a line with color c from (x0, y0) to (x1, y1) on the image img.
__attribute__((nonnull))
void line(Vec3i const * v0, Vec3i const * v1, TGAImage_t* const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangleMesh2D(Vec3i const * const v1, Vec3i const * const v2, Vec3i const * const v3, TGAImage_t * const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangle2D(Vec3i const v[3], TGAImage_t * const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangleWithZbuf(Vec3i const v[3], int32_t * zbuf, TGAImage_t * const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangle_barycentric(Vec3i const * v1, Vec3i const * v2, Vec3i const * v3, float* zbuf, TGAImage_t * const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangle_texture(Vec3i const v[3], Vec3i const n[3], float* zbuf, TGAImage_t * const img, TGAImage_t const * const t);
#endif // __DRAWING_H
