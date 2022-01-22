#pragma once

#include <kinc/graphics4/texture.h>
#include <kinc/image.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file image.h
    \brief Functionality for creating and loading images. See Kinc docs for underlying capabilities.
*/

typedef struct kr_image {
	kinc_g4_texture_t tex;
	kinc_image_t *image;
	float real_width, real_height;
	char *path;
	bool in_memory, loaded;
} kr_image_t;

/// <summary>
/// Initialize an image type. Call this before doing anything with a new image.
/// </summary>
/// <param name="img"></param>
void kr_image_init(kr_image_t *img);

/// <summary>
/// Load an image.
/// </summary>
/// <param name="img"></param>
/// <param name="path"></param>
/// <param name="keep_in_memory">Whether to keep the image data in memory.</param>
void kr_image_load(kr_image_t *img, const char *path, bool keep_in_memory);

/// <summary>
/// Generate mipmaps for a loaded image.
/// </summary>
/// <param name="img"></param>
/// <param name="levels"></param>
void kr_image_generate_mipmaps(kr_image_t *img, int levels);

/// <summary>
/// Destroy an image.
/// </summary>
/// <param name="img"></param>
void kr_image_destroy(kr_image_t *img);
