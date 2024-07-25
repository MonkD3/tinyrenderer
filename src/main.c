#include "tgaimage.h"
#include <stdlib.h>

/*
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char** argv) {
	TGAImage image(5, 5, TGAImage::RGB);
	image.set(2, 2, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;

}
*/

const TGAColor_t white = {.r=255, .g=255, .b=255, .a=255};
const TGAColor_t red   = {.r=255, .g=0,   .b=0,   .a=255};

int main(void){
    TGAImage_t img;
    TGAImage_init(&img, 100, 100, RGB);
    TGAImage_set(&img, &red, 52, 41);
    TGAImage_flip_vertically(&img);
    TGAImage_write_tga_file(&img, "outputC.tga", true);
    return EXIT_SUCCESS;
}

