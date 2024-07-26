#ifndef __DRAWING_H
#define __DRAWING_H

#include "./tgaimage.h"
#include <stdint.h>

// Draw a line with color c from (x0, y0) to (x1, y1) on the image img.
__attribute__((nonnull))
void line(int32_t const x0, int32_t const y0, int32_t const x1, int32_t const y1, TGAImage_t* img, TGAColor_t const * c);

#endif // __DRAWING_H