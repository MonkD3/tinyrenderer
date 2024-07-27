#include <math.h>
#include "./include/drawing.h"
#include "include/tgaimage.h"

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, TGAImage_t* img, TGAColor_t const * c){

    // Enforce x0 < x1 
    if (x0 > x1) {
        int32_t tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int32_t const dx = x1 - x0;  // dx is positive by definition
    int32_t const dy = y1 - y0;
    int32_t const ady = abs(dy);
    int32_t const y_direction = dy / ady;
    int32_t e  = 0;

    if (dx > ady){
        for (int32_t i = 0; i < dx; i++){
            TGAImage_set(img, c, x0, y0);
            x0++;
            e += ady;
            if (2*e > dx){
                e -= dx;
                y0 += y_direction;
            }
        }
    } else {
        for (int32_t i = 0; i < ady; i++){
            TGAImage_set(img, c, x0, y0);
            y0 += y_direction;
            e += dx;
            if (2*e > ady){
                e -= ady;
                x0++;
            }
        }
    }
}
