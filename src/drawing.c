#include <math.h>
#include "./include/drawing.h"
#include "include/tgaimage.h"

void line(int32_t const x0, int32_t const y0, int32_t const x1, int32_t const y1, TGAImage_t* img, TGAColor_t const * c){
    int32_t const dx = x1 - x0;
    int32_t const dy = y1 - y0;

    // Round up distance
    int32_t const d = (int32_t) ceilf(sqrtf(dx*dx + dy*dy));
    float const fdx = dx * 1.0f/d;
    float const fdy = dy * 1.0f/d;

    for (int32_t i = 0; i < d+1; i++){
        int32_t x = x0 + fdx*i;
        int32_t y = y0 + fdy*i;
        TGAImage_set(img, c, x, y);
    }
}
