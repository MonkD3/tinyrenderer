#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Metadata of the file format, see :
// https://en.wikipedia.org/wiki/Truevision_TGA#Technical_details
typedef struct __attribute__((packed)) {
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
} TGA_Header_t ;


// Color representation
typedef union {
    struct {
        uint8_t b, g, r, a; 
    };
    uint8_t raw[4]; 
    uint32_t val;  
} TGAColor_t;

void TGAColor_init_default(TGAColor_t* c);
void TGAColor_init_rgba(TGAColor_t* c, uint8_t const R, uint8_t const G, uint8_t const B, uint8_t const A);
void TGAColor_init_val(TGAColor_t* c, uint32_t const v);
void TGAColor_init_raw(TGAColor_t* c, uint8_t* p, int32_t const bpp);
void TGAColor_copy(TGAColor_t const * src, TGAColor_t* dest, int32_t bpp);

typedef enum {
    GRAYSCALE=1, RGB=3, RGBA=4
} TGAImageFormat_et;

typedef struct {
    int32_t width;
    int32_t height;
    int32_t bytespp;
    uint8_t* data;
} TGAImage_t;

// Get the color of the pixel at position (x, y) in img.
inline TGAColor_t* TGAImage_get(TGAImage_t const * const img, int32_t const x, int32_t const y){
    if (!img->data || x < 0 || y < 0 || x >= img->width || y >= img->height) {
        return NULL;
    } 
    return (TGAColor_t*) (img->data + (x+y*img->width)*img->bytespp);
}
inline TGAColor_t* TGAImage_get_unchecked(TGAImage_t const * const img, int32_t const x, int32_t const y){
    return (TGAColor_t*) (img->data + (x+y*img->width)*img->bytespp);
}

void TGAImage_init_default(TGAImage_t* img);
void TGAImage_init(TGAImage_t* img, int32_t const w, int32_t const h, int32_t const bpp);
void TGAImage_copy(TGAImage_t const * src, TGAImage_t* dest);
void TGAImage_destroy(TGAImage_t* img);

bool TGAImage_load_rle_data(TGAImage_t const * const img, FILE* in);
bool TGAImage_unload_rle_data(TGAImage_t const * const img, FILE* out);

bool TGAImage_read_tga_file(TGAImage_t* img, char const *filename);
bool TGAImage_write_tga_file(TGAImage_t const * const img, char const *filename, bool const rle); 
bool TGAImage_flip_horizontally(TGAImage_t* img);
bool TGAImage_flip_vertically(TGAImage_t* img);
bool TGAImage_scale(TGAImage_t* img, int32_t const new_w, int32_t const new_h);

inline bool TGAImage_set(TGAImage_t* img, TGAColor_t const * c, int32_t const x, int32_t const y){
    TGAColor_t* pixel = TGAImage_get(img, x, y);
    if (!pixel) return false;

    TGAColor_copy(c, pixel, img->bytespp);
    return true;
}
inline void TGAImage_set_unchecked(TGAImage_t* img, TGAColor_t const * c, int32_t const x, int32_t const y){
    TGAColor_t* dest = (TGAColor_t*) (img->data + (x+y*img->width)*img->bytespp);
    for (int32_t i = 0; i < img->bytespp; i++) dest->raw[i] = c->raw[i];
}
void TGAImage_clear(TGAImage_t* img);

#endif //__IMAGE_H__
