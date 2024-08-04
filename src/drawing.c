#include <math.h>
#include <stdint.h>
#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/geometry.h"

void line(Vec3i const * v0, Vec3i const * v1, TGAImage_t* const img, TGAColor_t const * const c){

    int32_t x0 = v0->x;
    int32_t y0 = v0->y;
    int32_t x1 = v1->x;
    int32_t y1 = v1->y;

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
void triangleMesh2D(Vec3i const * const v1, Vec3i const * const v2, Vec3i const * const v3, TGAImage_t * const img, TGAColor_t const * const c){
    line(v1, v2, img, c);
    line(v2, v3, img, c);
    line(v3, v1, img, c);
}

void triangle2D(Vec3i const * v1, Vec3i const * v2, Vec3i const * v3, TGAImage_t * const img, TGAColor_t const * const c){
    Vec3i const * p[3] = {v1, v2, v3};
    if (p[0]->y > p[1]->y){
        Vec3i const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    if (p[1]->y > p[2]->y){
        Vec3i const * tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }
    if (p[0]->y > p[1]->y){
        Vec3i const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    Vec3i const* low = p[0];
    Vec3i const* mid = p[1];
    Vec3i const* hi  = p[2];

    int32_t x_ml = low->x; // Beginning of line
    int32_t x_hl = low->x; // End of line
                                 
    int32_t const adx_ml  = abs(mid->x - low->x); 
    int32_t const ady_ml  = abs(mid->y - low->y);
    int32_t const adx_hl  = abs(hi->x - low->x); 
    int32_t const ady_hl  = abs(hi->y - low->y);

    int32_t const xdir_ml = mid->x > low->x ? 1 : -1;
    int32_t const xdir_hl = hi->x > low->x ? 1 : -1;
    // As we draw from bottom to top : The edge with the lowest angular coefficient is at the right
    int32_t draw_dir = atan2(mid->y - low->y, mid->x - low->x) 
                     < atan2(hi->y - low->y, hi->x - low->x) ? -1 : 1;

    int32_t e_ml = 0; // Error of left line
    int32_t e_hl = 0; // Error of right line
    for (int32_t y = low->y; y < mid->y; y++){
        switch (draw_dir) {
            case -1:
                for (int32_t xd = x_hl; x_ml >= xd; xd++) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
                break;
            case 1:
                for (int32_t xd = x_ml; x_hl >= xd; xd++) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
                break;
        }
        e_ml += adx_ml;
        x_ml += (e_ml/ady_ml)*xdir_ml;
        e_ml -= (e_ml/ady_ml)*ady_ml;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }

    int32_t x_mh = mid->x; // End of line
    int32_t const adx_mh  = abs(mid->x - hi->x); 
    int32_t const ady_mh  = abs(mid->y - hi->y);
    int32_t const xdir_mh = hi->x > mid->x ? 1 : -1;
    draw_dir = x_mh > x_hl ? -1 : 1;
    int32_t e_mh = 0;
    for (int32_t y = mid->y; y < hi->y; y++){
        switch (draw_dir) {
            case -1:
                for (int32_t xd = x_hl; x_mh >= xd; xd++) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
                break;
            case 1:
                for (int32_t xd = x_mh; x_hl >= xd; xd++) TGAImage_set_unchecked(img, c, xd, y); // Draw the line
                break;
        }
        e_mh += adx_mh;
        x_mh += (e_mh/ady_mh)*xdir_mh;
        e_mh -= (e_mh/ady_mh)*ady_mh;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }
}

void triangleWithZbuf(Vec3i const * v1, Vec3i const * v2, Vec3i const * v3, int32_t * zbuf, TGAImage_t * const img, TGAColor_t const * const c){
    Vec3i const * p[3] = {v1, v2, v3};
    if (p[0]->y > p[1]->y){
        Vec3i const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    if (p[1]->y > p[2]->y){
        Vec3i const * tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }
    if (p[0]->y > p[1]->y){
        Vec3i const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    Vec3i const* low = p[0];
    Vec3i const* mid = p[1];
    Vec3i const* hi  = p[2];

    int32_t x_ml = low->x; // Beginning of line
    int32_t x_hl = low->x; // End of line
    int32_t z_ml = low->z; // Beginning of line
    int32_t z_hl = low->z; // End of line
                                 
    int32_t const adx_ml  = abs(mid->x - low->x); 
    int32_t const ady_ml  = abs(mid->y - low->y);
    int32_t const adz_ml  = abs(mid->z - low->z);
    int32_t const adx_hl  = abs(hi->x - low->x); 
    int32_t const ady_hl  = abs(hi->y - low->y);
    int32_t const adz_hl  = abs(hi->z - low->z);

    int32_t const xdir_ml = mid->x > low->x ? 1 : -1;
    int32_t const zdir_ml = mid->z > low->z ? 1 : -1;
    int32_t const xdir_hl = hi->x > low->x ? 1 : -1;
    int32_t const zdir_hl = hi->z > low->z ? 1 : -1;
    // As we draw from bottom to top : The edge with the lowest angular coefficient is at the right
    int32_t draw_dir = atan2(mid->y - low->y, mid->x - low->x) 
                     < atan2(hi->y - low->y, hi->x - low->x) ? -1 : 1;

    int32_t e_ml = 0; // Error of left line
    int32_t ez_ml = 0; 
    int32_t e_hl = 0; // Error of right line
    int32_t ez_hl = 0; 
    for (int32_t y = low->y; y < mid->y; y++){
        int32_t zd = z_ml;
        int32_t ez = 0;
        int32_t const adz = abs(z_hl - z_ml);
        int32_t const adx = abs(x_hl - x_ml);
        int32_t const z_dir = z_ml > z_hl ? -1 : 1;
        for (int32_t xd = x_ml; draw_dir*(x_hl-xd) >= 0; xd+=draw_dir) {
            if (zd > zbuf[y*img->width + xd]) {
                TGAImage_set_unchecked(img, c, xd, y); // Draw the line
                zbuf[y*img->width + xd] = zd;
            }
            ez += adz;
            if (adx > 0){
                zd += (ez/adx)*z_dir;
                ez -= (ez/adx)*adx;
            }
        }
        e_ml += adx_ml;
        x_ml += (e_ml/ady_ml)*xdir_ml;
        e_ml -= (e_ml/ady_ml)*ady_ml;
        ez_ml += adz_ml;
        z_ml += (ez_ml/ady_ml)*zdir_ml;
        ez_ml -= (ez_ml/ady_ml)*ady_ml;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
        ez_hl += adz_hl;
        z_hl += (ez_hl/ady_hl)*zdir_hl;
        ez_hl -= (ez_hl/ady_hl)*ady_hl;
    }

    int32_t x_mh = mid->x; // End of line
    int32_t z_mh = mid->z; // End of line
    int32_t const adx_mh  = abs(mid->x - hi->x); 
    int32_t const ady_mh  = abs(mid->y - hi->y);
    int32_t const adz_mh  = abs(mid->z - hi->z);
    int32_t const xdir_mh = hi->x > mid->x ? 1 : -1;
    int32_t const zdir_mh = hi->z > mid->z ? 1 : -1;
    draw_dir = x_mh > x_hl ? -1 : 1;
    int32_t e_mh = 0;
    int32_t ez_mh = 0;
    for (int32_t y = mid->y; y < hi->y; y++){
        int32_t zd = z_mh;
        int32_t ez = 0;
        int32_t const adz = abs(z_hl - z_mh);
        int32_t const adx = abs(x_hl - x_mh);
        int32_t const z_dir = z_mh > z_hl ? -1 : 1;
        for (int32_t xd = x_mh; draw_dir*(x_hl-xd) >= 0; xd+=draw_dir) {
            if (zd > zbuf[y*img->width + xd]) {
                zbuf[y*img->width + xd] = zd;
                TGAImage_set_unchecked(img, c, xd, y); // Draw the line
            }
            ez += adz;
            if (adx > 0){
                zd += (ez/adx)*z_dir;
                ez -= (ez/adx)*adx;
            }
        }
        e_mh += adx_mh;
        x_mh += (e_mh/ady_mh)*xdir_mh;
        e_mh -= (e_mh/ady_mh)*ady_mh;

        ez_mh += adz_mh;
        z_mh += (ez_mh/ady_mh)*zdir_mh;
        ez_mh -= (ez_mh/ady_mh)*ady_mh;


        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;

        ez_hl += adz_hl;
        z_hl += (ez_hl/ady_hl)*zdir_hl;
        ez_hl -= (ez_hl/ady_hl)*ady_hl;
    }
}
