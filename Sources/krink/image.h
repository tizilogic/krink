#pragma once

#include <kinc/graphics4/texture.h>
#include <kinc/image.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct kr_image {
	kinc_g4_texture_t tex;
	kinc_image_t *image;
	float real_width, real_height;
	char *path;
	bool in_memory, loaded;
} kr_image_t;

void kr_image_init(kr_image_t *img);
void kr_image_load(kr_image_t *img, char *path, bool keep_in_memory);
void kr_image_destroy(kr_image_t *img);
