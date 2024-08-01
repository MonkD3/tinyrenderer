#include <math.h>
#include <stdint.h>
#include <stdio.h>
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

void triangle2D(int32_t const * v1, int32_t const * v2, int32_t const * v3, TGAImage_t * const img, TGAColor_t const * const c){
    // First we sort the point by their height (y coordinates)
    int32_t const * p[3] = {v1, v2, v3};
    if (p[0][1] > p[1][1]){
        int32_t const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    if (p[1][1] > p[2][1]){
        int32_t const * tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }
    if (p[0][1] > p[1][1]){
        int32_t const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    int32_t const* low = p[0];
    int32_t const* mid = p[1];
    int32_t const* hi  = p[2];

    int32_t x_ml = low[X_COORD]; // Beginning of line
    int32_t x_hl = low[X_COORD]; // End of line
                                 
    int32_t const adx_ml  = abs(mid[X_COORD] - low[X_COORD]); 
    int32_t const ady_ml  = abs(mid[Y_COORD] - low[Y_COORD]);
    int32_t const adx_hl  = abs(hi[X_COORD] - low[X_COORD]); 
    int32_t const ady_hl  = abs(hi[Y_COORD] - low[Y_COORD]);

    int32_t const xdir_ml = mid[X_COORD] > low[X_COORD] ? 1 : -1;
    int32_t const xdir_hl = hi[X_COORD] > low[X_COORD] ? 1 : -1;
    // As we draw from bottom to top : The edge with the lowest angular coefficient is at the right
    int32_t draw_dir = atan2(mid[Y_COORD] - low[Y_COORD], mid[X_COORD] - low[X_COORD]) 
                     < atan2(hi[Y_COORD] - low[Y_COORD], hi[X_COORD] - low[X_COORD]) ? -1 : 1;

    int32_t e_ml = 0; // Error of left line
    int32_t e_hl = 0; // Error of right line
    for (int32_t y = low[Y_COORD]; y < mid[Y_COORD]; y++){
        for (int32_t xd = x_ml; draw_dir*(x_hl-xd) >= 0; xd += draw_dir) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
        e_ml += adx_ml;
        x_ml += (e_ml/ady_ml)*xdir_ml;
        e_ml -= (e_ml/ady_ml)*ady_ml;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }

    int32_t x_mh = mid[X_COORD]; // End of line
    int32_t const adx_mh  = abs(mid[X_COORD] - hi[X_COORD]); 
    int32_t const ady_mh  = abs(mid[Y_COORD] - hi[Y_COORD]);
    int32_t const xdir_mh = hi[X_COORD] > mid[X_COORD] ? 1 : -1;
    draw_dir = x_mh > x_hl ? -1 : 1;
    int32_t e_mh = 0;
    for (int32_t y = mid[Y_COORD]; y < hi[Y_COORD]; y++){
        for (int32_t xd = x_mh; draw_dir*(x_hl-xd) >= 0; xd += draw_dir) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
        e_mh += adx_mh;
        x_mh += (e_mh/ady_mh)*xdir_mh;
        e_mh -= (e_mh/ady_mh)*ady_mh;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }
}
