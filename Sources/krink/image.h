#pragma once

#include <kinc/graphics4/texture.h>
#include <kinc/image.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct krink_image {
	kinc_g4_texture_t tex;
	kinc_image_t *image;
	float real_width, real_height;
	char *path;
	bool in_memory, loaded;
} krink_image_t;

void krink_image_init(krink_image_t *img);
void krink_image_load(krink_image_t *img, char *path, bool keep_in_memory);
void krink_image_destroy(krink_image_t *img);
