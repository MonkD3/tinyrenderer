#include "tgaimage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void TGAColor_init_default(TGAColor_t* c){
    c->val = 0;
}

void TGAColor_init_rgba(TGAColor_t* c, uint8_t const R, uint8_t const G, uint8_t const B, uint8_t const A){
    c->r = R;
    c->g = G;
    c->b = B;
    c->a = A;
}

void TGAColor_init_val(TGAColor_t* c, uint32_t const v){
    c->val = v;
}

void TGAColor_init_raw(TGAColor_t* c, uint8_t* p, int32_t const bpp){
    for (int32_t i = 0; i < bpp; i++) c->raw[i] = p[i];
}

void TGAColor_copy(TGAColor_t const * src, TGAColor_t* dest, int32_t bpp){
    for (int32_t i = 0; i < bpp; i++) dest->raw[i] = src->raw[i];
}

TGAColor_t* TGAImage_get(TGAImage_t const * const img, int32_t const x, int32_t const y){
    if (!img->data || x < 0 || y < 0 || x >= img->width || y >= img->height) {
        return NULL;
    } 
    return (TGAColor_t*) (img->data + (x+y*img->width)*img->bytespp);
}

void TGAImage_init_default(TGAImage_t* img){
    img->data = NULL;
    img->width = 0;
    img->height = 0;
    img->bytespp = 0;
}

void TGAImage_init(TGAImage_t* img, int32_t const w, int32_t const h, int32_t const bpp){
    img->width = w;
    img->height = h;
    img->bytespp = bpp;
    img->data = calloc(w*h*bpp, sizeof(uint8_t));
}

void TGAImage_copy(TGAImage_t const * src, TGAImage_t* dest){
    if (dest->data) free(dest->data);

    dest->width = src->width;
    dest->height = src->height;
    dest->bytespp = src->bytespp;

    size_t const s = dest->width * dest->height * dest->bytespp;
    dest->data = malloc(sizeof(uint8_t) * s);
    memcpy(dest->data, src->data, s);
}

void TGAImage_destroy(TGAImage_t* img){
    if (img->data) free(img->data);
}

bool TGAImage_load_rle_data(TGAImage_t const * const img, FILE* in){
	size_t pixelcount = img->width*img->height;
	size_t currentpixel = 0;
	size_t currentbyte  = 0;
	TGAColor_t colorbuffer;
	do {
		uint8_t chunkheader = 0;
        size_t nread = fread(&chunkheader, 1, 1, in);
		if (nread < 1 || ferror(in)) {
			fputs("An error occured while reading the data\n", stderr);
			return false;
		}
		if (chunkheader<128) {
			chunkheader++;
			for (uint8_t i = 0; i < chunkheader; i++) {
                nread = fread(colorbuffer.raw, 1, img->bytespp, in);
				if (nread < (size_t) img->bytespp || ferror(in)) {
                    fputs("An error occured while reading the header\n", stderr);
					return false;
				}

				for (int32_t t = 0; t < img->bytespp; t++) {
					img->data[currentbyte++] = colorbuffer.raw[t];
                }
				currentpixel++;

				if (currentpixel > pixelcount) {
                    fputs("Too many pixels read\n", stderr);
					return false;
				}
			}
		} else {
			chunkheader -= 127;
            nread = fread(colorbuffer.raw, 1, img->bytespp, in);
            if (nread < (size_t) img->bytespp || ferror(in)) {
                fputs("An error occured while reading the header\n", stderr);
                return false;
            }
			for (uint8_t i = 0; i < chunkheader; i++) {

				for (int32_t t = 0; t < img->bytespp; t++){
					img->data[currentbyte++] = colorbuffer.raw[t];
                }
				currentpixel++;

				if (currentpixel > pixelcount) {
                    fputs("Too many pixels read\n", stderr);
					return false;
				}
			}
		}
	} while (currentpixel < pixelcount);
	return true;
}
bool TGAImage_unload_rle_data(TGAImage_t const * const img, FILE* out){
	const uint8_t max_chunk_length = 128;
    const int32_t bytespp = img->bytespp;
    uint8_t* const data = img->data;
	size_t npixels = img->width*img->height;
	size_t curpix = 0;

	while (curpix < npixels) {
		size_t chunkstart = curpix*bytespp;
		size_t curbyte = curpix*bytespp;
		uint8_t run_length = 1;
		bool raw = true;
		while (curpix + run_length < npixels && run_length < max_chunk_length) {
			bool succ_eq = true;
			for (int32_t t = 0; succ_eq && t < bytespp; t++) {
				succ_eq = (data[curbyte+t] == data[curbyte+t+bytespp]);
			}
			curbyte += bytespp;
			if (1==run_length) {
				raw = !succ_eq;
			}
			if (raw && succ_eq) {
				run_length--;
				break;
			}
			if (!raw && !succ_eq) {
				break;
			}
			run_length++;
		}
		curpix += run_length;
		int ch = fputc(raw ? run_length-1 : run_length+127, out);
		if (ch == EOF || ferror(out)) {
			fputs("Can't dump the tga file\n", stderr);
			return false;
		}

        size_t bytes_to_write = raw?run_length*bytespp:bytespp;
		size_t nwrite = fwrite((char *)(data+chunkstart), 1, bytes_to_write, out);
		if (nwrite < bytes_to_write || ferror(out)) {
			fputs("Can't dump the tga file\n", stderr);
			return false;
		}
	}
	return true;
}

bool TGAImage_read_tga_file(TGAImage_t* img, char const *filename){
    if (img->data) free(img->data);
    img->data = NULL;

    FILE* ifd = fopen(filename, "rb");
    if (!ifd) {
        fprintf(stderr, "Cannot open file '%s'\n", filename);
        return false;
    }
    TGA_Header_t header;
    size_t nread = fread(&header, sizeof(header), 1, ifd);
    if (!nread){
        fclose(ifd);
        fprintf(stderr, "An error occured while reading the header\n");
        return false;
    }
    img->width = header.width;
    img->height = header.height;
    img->bytespp = header.bitsperpixel >> 3; // 1 byte = 8 bits
	if (img->width <= 0 || img->height <= 0 || (img->bytespp != GRAYSCALE && img->bytespp != RGB && img->bytespp != RGBA)) {
		fclose(ifd);
		fprintf(stderr, "Bad bpp (or width/height) value\n");
		return false;
	}
	size_t nbytes = img->bytespp*img->width*img->height;
	img->data = (uint8_t*)  malloc(sizeof(uint8_t) * nbytes);

	if (3==header.datatypecode || 2==header.datatypecode) {
        nread = fread(img->data, sizeof(uint8_t), nbytes, ifd);
		if (nread < nbytes || ferror(ifd)) {
            fclose(ifd);
			fprintf(stderr, "An error occured while reading the data\n");
			return false;
		}
	} else if (10==header.datatypecode||11==header.datatypecode) {
		if (!TGAImage_load_rle_data(img, ifd)) {
            fclose(ifd);
			fprintf(stderr, "An error occured while reading the data\n");
			return false;
		}
	} else {
        fclose(ifd);
		fprintf(stderr, "unknown file format %d\n", (int)header.datatypecode);
		return false;
	}
	if (!(header.imagedescriptor & 0x20)) {
		TGAImage_flip_vertically(img);
	}
	if (header.imagedescriptor & 0x10) {
		TGAImage_flip_horizontally(img);
	}
    fclose(ifd);
    return true;
}

bool TGAImage_write_tga_file(TGAImage_t const * const img, char const *filename, bool const rle){
	uint8_t developer_area_ref[4] = {0, 0, 0, 0};
	uint8_t extension_area_ref[4] = {0, 0, 0, 0};
	uint8_t footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};

    FILE* ofd = fopen(filename, "wb");
    if (!ofd){
        fprintf(stderr, "Can't open file '%s'\n", filename);
		return false;
    }
	TGA_Header_t header = {0}; // Zero initialization from C11 !
	header.bitsperpixel = img->bytespp<<3;
	header.width  = img->width;
	header.height = img->height;
	header.datatypecode = (img->bytespp==GRAYSCALE?(rle?11:3):(rle?10:2));
	header.imagedescriptor = 0x20; // top-left origin
                                   //
    size_t nwrite = fwrite(&header, sizeof(header), 1, ofd);
	if (nwrite < 1 || ferror(ofd)) goto cleanup_on_write_error;

	if (!rle) {
        size_t nbytes = img->width*img->height*img->bytespp;
        nwrite = fwrite(img->data, sizeof(uint8_t), nbytes, ofd);
		if (nwrite < nbytes || ferror(ofd)) {
            fclose(ofd);
            fprintf(stderr, "Can't unload rle data\n");
            return false;
		}
	} else {
		if (!TGAImage_unload_rle_data(img, ofd)) {
            fclose(ofd);
            fprintf(stderr, "Can't unload rle data\n");
            return false;
		}
	}

    nwrite = fwrite(developer_area_ref, sizeof(uint8_t), sizeof(developer_area_ref), ofd);
	if (nwrite < sizeof(developer_area_ref) || ferror(ofd)) goto cleanup_on_write_error;

    nwrite = fwrite(extension_area_ref, sizeof(uint8_t), sizeof(extension_area_ref), ofd);
	if (nwrite < sizeof(extension_area_ref) || ferror(ofd)) goto cleanup_on_write_error;

    nwrite = fwrite(footer, sizeof(uint8_t), sizeof(footer), ofd);
	if (nwrite < sizeof(footer) || ferror(ofd)) goto cleanup_on_write_error;

    fclose(ofd);
    return true;

    cleanup_on_write_error:
        fclose(ofd);
        fprintf(stderr, "Can't dump the tga file\n");
        return false;
}

bool TGAImage_flip_horizontally(TGAImage_t* img){
	if (!img->data) return false;

    TGAColor_t tmp;
	int32_t half = img->width>>1;
    int32_t const bytespp = img->bytespp;
	for (int32_t i = 0; i < half; i++) {
		for (int32_t j = 0; j < img->height; j++) {
            TGAColor_t* const c1 = TGAImage_get(img, i, j);
            TGAColor_t* const c2 = TGAImage_get(img, img->width - 1 - i, j);

            TGAColor_copy(c1, &tmp, bytespp);
            TGAColor_copy(c2, c1, bytespp);
            TGAColor_copy(&tmp, c2, bytespp);
		}
	}
	return true;
}

bool TGAImage_flip_vertically(TGAImage_t* img){
    uint8_t* data = img->data;
	if (!img->data) return false;

	size_t const bytes_per_line = img->width*img->bytespp;
	uint8_t* restrict line = malloc(bytes_per_line);

	int half = img->height >> 1;
	for (int j = 0; j < half; j++) {
		size_t const l1 = j*bytes_per_line;
		size_t const l2 = (img->height-1-j)*bytes_per_line;

		memcpy(line,    data+l1, bytes_per_line);
		memcpy(data+l1, data+l2, bytes_per_line);
		memcpy(data+l2, line,    bytes_per_line);
	}
    free(line);
	return true;
}

bool TGAImage_scale(TGAImage_t* img, int32_t const new_w, int32_t const new_h){
	if (new_w<=0 || new_h<=0 || !img->data) return false;

	uint8_t *tdata = malloc(new_w*new_h*img->bytespp);

	int32_t nscanline = 0;
	int32_t oscanline = 0;
	int32_t erry = 0;
    int32_t const bytespp = img->bytespp;
    int32_t const height = img->height;
    int32_t const width = img->width;
	size_t const nlinebytes = new_w*bytespp;
	size_t const olinebytes = img->width*bytespp;

	for (int32_t j = 0; j < height; j++) {
		int32_t errx = width-new_w;
		int32_t nx   = -bytespp;
		int32_t ox   = -bytespp;
		for (int32_t i = 0; i < width; i++) {
			ox += bytespp;
			errx += new_w;
			while (errx >= width) {
				errx -= width;
				nx += bytespp;
				memcpy(tdata + nscanline + nx, img->data + oscanline + ox, bytespp);
			}
		}
		erry += new_h;
		oscanline += olinebytes;
		while (erry >= height) {
			if (erry >= height<<1) // it means we jump over a scanline
				memcpy(tdata+nscanline+nlinebytes, tdata+nscanline, nlinebytes);
			erry -= height;
			nscanline += nlinebytes;
		}
	}
	free(img->data);
	img->data = tdata;
	img->width = new_w;
	img->height = new_h;
	return true;
}

bool TGAImage_set(TGAImage_t* img, TGAColor_t const * c, int32_t const x, int32_t const y){
    TGAColor_t* pixel = TGAImage_get(img, x, y);
    if (!pixel) return false;

    TGAColor_copy(c, pixel, img->bytespp);
    return true;
}


void TGAImage_clear(TGAImage_t* img){
    if (img->data) memset(img->data, 0, img->width*img->height*img->bytespp);
}
