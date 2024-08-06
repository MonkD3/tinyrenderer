#include <math.h>
#include <stdint.h>
#include "include/tgaimage.h"
#include "include/drawing.h"
#include "include/geometry.h"

void Draw_line(Vec3i const v[2], TGAImage_t* const img, TGAColor_t const * const c){

    int32_t x0 = v[0].x;
    int32_t y0 = v[0].y;
    int32_t x1 = v[1].x;
    int32_t y1 = v[1].y;

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
void Draw_tri_outline(Vec3i const v[3], TGAImage_t * const img, TGAColor_t const * const c){
    Draw_line((Vec3i[2]){v[0], v[1]}, img, c);
    Draw_line((Vec3i[2]){v[1], v[2]}, img, c);
    Draw_line((Vec3i[2]){v[2], v[0]}, img, c);
}

void barycentric(Vec3f *bc, Vec3i const * A, Vec3i const* B, Vec3i const * C, Vec3i const* P) {
    Vec3f s[2], u;
    for (int32_t i=0; i < 2; i++) {
        s[i].x = C->raw[i]-A->raw[i];
        s[i].y = B->raw[i]-A->raw[i];
        s[i].z = A->raw[i]-P->raw[i];
    }
    Vec3f_cross(&u, &s[0], &s[1]);

    if (u.z) *bc = (Vec3f){.x=1.f-(u.x+u.y)/u.z, .y=u.y/u.z, .z=u.x/u.z};
    else     *bc = (Vec3f){.x=-1, .y=-1, .z=-1};
}

void Draw_tri_uniform(Vec3i const v[3], TGAImage_t * const img, TGAColor_t const * const c){
    Vec3i px = {0}, bbmin = {0}, bbmax = {0};
    Vec3f bc = {0};

    bounding_box(&bbmin, &bbmax, v, 3);

    for (px.y = bbmin.y; px.y < bbmax.y; px.y++){
        for (px.x = bbmin.x; px.x < bbmax.x; px.x++){
            barycentric(&bc, v, v+1, v+2, &px);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            TGAImage_set_unchecked(img, c, px.x, px.y);
        }
    }
}

void Draw_tri_uniform_z(Vec3i const v[3], float* zbuf, TGAImage_t * const img, TGAColor_t const * const c){
    Vec3i px = {0}, bbmin = {0}, bbmax = {0};
    Vec3f bc = {0};

    bounding_box(&bbmin, &bbmax, v, 3);

    for (px.y = bbmin.y; px.y < bbmax.y; px.y++){
        for (px.x = bbmin.x; px.x < bbmax.x; px.x++){
            barycentric(&bc, v, v+1, v+2, &px);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            px.z = bc.x * v[0].z + bc.y * v[1].z + bc.z * v[2].z;
            if (px.z > zbuf[px.x + px.y*img->width]) {
                zbuf[px.x + px.y*img->width] = px.z;
                TGAImage_set_unchecked(img, c, px.x, px.y);
            }
        }
    }
}

void Draw_tri_texture(Vec3i const v[3], Vec3i const t[3], TGAImage_t * const img, TGAImage_t const * const texture){
    Vec3i px = {0}, pxt = {0}, bbmin = {0}, bbmax = {0};
    Vec3f bc = {0};
    TGAColor_t *c;

    bounding_box(&bbmin, &bbmax, v, 3);
    for (px.y = bbmin.y; px.y < bbmax.y; px.y++){
        for (px.x = bbmin.x; px.x < bbmax.x; px.x++){
            barycentric(&bc, &v[0], &v[1], &v[2], &px);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue; // px is out of triangle

            // Compute texture position
            pxt.x = bc.x * t[0].x + bc.y * t[1].x + bc.z * t[2].x;
            pxt.y = bc.x * t[0].y + bc.y * t[1].y + bc.z * t[2].y;
            c = TGAImage_get(texture, pxt.x, pxt.y); 
            TGAImage_set(img, c, px.x, px.y);
        }
    }
}

void Draw_tri_texture_z(Vec3i const v[3], Vec3i const t[3], float* zbuf, TGAImage_t * const img, TGAImage_t const * const texture){
    Vec3i px = {0}, pxt = {0}, bbmin = {0}, bbmax = {0};
    Vec3f bc = {0};
    TGAColor_t *c;

    bounding_box(&bbmin, &bbmax, v, 3);
    for (px.y = bbmin.y; px.y < bbmax.y; px.y++){
        for (px.x = bbmin.x; px.x < bbmax.x; px.x++){
            barycentric(&bc, &v[0], &v[1], &v[2], &px);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue; // px is out of triangle

            px.z = bc.x * v[0].z + bc.y * v[1].z + bc.z * v[2].z; // Interpolate z
            if (px.z > zbuf[px.x + px.y*img->width]) {
                zbuf[px.x + px.y*img->width] = px.z;
                // Compute texture position
                pxt.x = bc.x * t[0].x + bc.y * t[1].x + bc.z * t[2].x;
                pxt.y = bc.x * t[0].y + bc.y * t[1].y + bc.z * t[2].y;
                c = TGAImage_get(texture, pxt.x, pxt.y); 
                TGAImage_set(img, c, px.x, px.y);
            }
        }
    }
}
