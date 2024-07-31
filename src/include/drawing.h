#ifndef __DRAWING_H
#define __DRAWING_H

#include "./tgaimage.h"
#include <stdint.h>

// Draw a line with color c from (x0, y0) to (x1, y1) on the image img.
__attribute__((nonnull))
void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, TGAImage_t* const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangleMesh2D(int32_t const * const v1, int32_t const * const v2, int32_t const * const v3, TGAImage_t * const img, TGAColor_t const * const c);

__attribute__((nonnull))
void triangle2D(int32_t const * v1, int32_t const * v2, int32_t const * v3, TGAImage_t * const img, TGAColor_t const * const c);
#endif // __DRAWING_H
