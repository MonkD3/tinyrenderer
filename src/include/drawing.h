#ifndef __DRAWING_H
#define __DRAWING_H

#include "tgaimage.h"
#include "geometry.h"
#include <stdint.h>

// Draw a line with color c from (x0, y0) to (x1, y1) on the image img.
__attribute__((nonnull))
void Draw_line(Vec3i const v[2], TGAImage_t* const img, TGAColor_t const * const c);

// Draw the outline of a triangle with vertices v
__attribute__((nonnull))
void Draw_tri_outline(Vec3i const v[3], TGAImage_t * const img, TGAColor_t const * const c);

// Draw a triangle with vertices v and uniform color c. Use the zbuffer information
// Always draw on top
__attribute__((nonnull))
void Draw_tri_uniform(Vec3i const v[3], TGAImage_t * const img, TGAColor_t const * const c);

// Draw a triangle with vertices v with texture tx interpolated on texture vertices t
// Always draw on top
__attribute__((nonnull))
void Draw_tri_texture(Vec3i const v[3], Vec3i const t[3], TGAImage_t * const img, TGAImage_t const * const tx);

// Draw a triangle with vertices v and uniform color c. Use the zbuffer information
__attribute__((nonnull))
void Draw_tri_uniform_z(Vec3i const v[3], float* zbuf, TGAImage_t * const img, TGAColor_t const * const c);

// Draw a triangle with vertices v with texture tx interpolated on texture vertices t
// Use the zbuffer information
__attribute__((nonnull))
void Draw_tri_texture_z(Vec3i const v[3], Vec3i const t[3], float const light_intensity[3], float* zbuf, TGAImage_t * const img, TGAImage_t const * const tx);
#endif // __DRAWING_H
