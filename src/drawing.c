#include <math.h>
#include <stdint.h>
#include "include/drawing.h"
#include "include/tgaimage.h"
#include "include/geometry.h"

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, TGAImage_t* const img, TGAColor_t const * const c){

    // Enforce x0 < x1 
    if (x0 > x1) {
        int32_t tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int32_t const dx  = x1 - x0;  // dx is positive by definition
    int32_t const dy  = y1 - y0;
    int32_t const ady = abs(dy);
    int32_t const y_direction = dy > 0 ? 1 : - 1;
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

// Only draw the lines of the triangle
void triangleMesh2D(int32_t const * const v1, int32_t const * const v2, int32_t const * const v3, TGAImage_t * const img, TGAColor_t const * const c){
    line(v1[X_COORD], v1[Y_COORD], v2[X_COORD], v2[Y_COORD], img, c);
    line(v2[X_COORD], v2[Y_COORD], v3[X_COORD], v3[Y_COORD], img, c);
    line(v3[X_COORD], v3[Y_COORD], v1[X_COORD], v1[Y_COORD], img, c);
}

static int compareY(void const * v1, void const * v2){
    int32_t const * v1i = *((int32_t **)v1);
    int32_t const * v2i = *((int32_t **)v2);
    return v1i[Y_COORD] - v2i[Y_COORD];
}

void triangle2D(int32_t const * v1, int32_t const * v2, int32_t const * v3, TGAImage_t * const img, TGAColor_t const * const c){
    // First we sort the point by their height (y coordinates)
    int32_t const * p[3] = {v1, v2, v3};
    qsort(p, 3, sizeof(v1), compareY);
    int32_t const* low = p[0];
    int32_t const* mid = p[1];
    int32_t const* hi  = p[2];

    // Draw horizontally from lo to mid by taking into account the lines low--mid and low--hi 
    // then from mid to hi by taking into account the lines low--hi and mid--hi
    int32_t x_ml = low[X_COORD]; // Beginning of line
    int32_t x_hl = low[X_COORD]; // End of line
                                 
    int32_t adx_ml  = abs(mid[X_COORD] - low[X_COORD]); 
    int32_t ady_ml  = abs(mid[Y_COORD] - low[Y_COORD]);
    int32_t adx_hl  = abs(hi[X_COORD] - low[X_COORD]); 
    int32_t ady_hl  = abs(hi[Y_COORD] - low[Y_COORD]);

    int32_t xdir_ml = mid[X_COORD] > low[X_COORD] ? 1 : -1;
    int32_t xdir_hl = hi[X_COORD] > low[X_COORD] ? 1 : -1;

    int32_t e_ml = 0; // Error of left line
    int32_t e_hl = 0; // Error of right line
    for (int32_t y = low[Y_COORD]; y < mid[Y_COORD]; y++){
        int32_t max = x_ml > x_hl ? x_ml : x_hl;
        int32_t min = x_ml < x_hl ? x_ml : x_hl;
        for (int32_t xd = min; xd <= max; xd++) TGAImage_set(img, c, xd, y); // Draw the line
        e_ml += adx_ml;
        while (2*e_ml > ady_ml){
            x_ml += xdir_ml;
            e_ml -= ady_ml;
        }
        e_hl += adx_hl;
        while (2*e_hl > ady_hl){
            x_hl += xdir_hl;
            e_hl -= ady_hl;
        }
    }

    int32_t x_mh = mid[X_COORD]; // End of line
    int32_t adx_mh  = abs(mid[X_COORD] - hi[X_COORD]); 
    int32_t ady_mh  = abs(mid[Y_COORD] - hi[Y_COORD]);
    int32_t xdir_mh = hi[X_COORD] > mid[X_COORD] ? 1 : -1;
    int32_t e_mh = 0;
    for (int32_t y = mid[Y_COORD]; y < hi[Y_COORD]; y++){
        int32_t max = x_mh > x_hl ? x_mh : x_hl;
        int32_t min = x_mh < x_hl ? x_mh : x_hl;
        for (int32_t xd = min; xd <= max; xd++) TGAImage_set(img, c, xd, y); // Draw the line
        e_mh += adx_mh;
        while (2*e_mh > ady_mh){
            x_mh += xdir_mh;
            e_mh -= ady_mh;
        }
        e_hl += adx_hl;
        while (2*e_hl > ady_hl){
            x_hl += xdir_hl;
            e_hl -= ady_hl;
        }
    }
}
