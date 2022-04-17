#include "image.h"
#include "memory.h"

#include <assert.h>

void kr_image_init(kr_image_t *img) {
	img->real_width = 0.0f;
	img->real_height = 0.0f;
	img->tex = NULL;
	img->path = NULL;
	img->image = NULL;
	img->in_memory = false;
	img->loaded = false;
	img->owns_tex = false;
}

void kr_image_load(kr_image_t *img, const char *path, bool keep_in_memory) {
	kr_image_init(img);
	img->path = path;
	img->tex = (kinc_g4_texture_t *)kr_malloc(sizeof(kinc_g4_texture_t));
	assert(img->tex != NULL);

	img->image = (kinc_image_t *)kr_malloc(sizeof(kinc_image_t));
	assert(img->image != NULL);

	void *image_mem = kr_malloc(kinc_image_size_from_file(img->path));
	assert(image_mem != NULL);
	kinc_image_init_from_file(img->image, image_mem, path);
	kinc_g4_texture_init_from_image(img->tex, img->image);
	img->loaded = true;
	img->in_memory = keep_in_memory;
	img->real_width = (float)img->image->width;
	img->real_height = (float)img->image->height;
	img->owns_tex = true;

	if (!keep_in_memory) {
		kinc_image_destroy(img->image);
		kr_free(img->image);
		img->image = NULL;
		kr_free(image_mem);
	}
}

void kr_image_from_texture(kr_image_t *img, kinc_g4_texture_t *tex, float real_width,
                           float real_height) {
	kr_image_init(img);
	img->tex = tex;
	img->path = "";
	img->image = NULL;
	img->loaded = true;
	img->in_memory = false;
	img->real_width = real_width == 0.0f ? (float)tex->tex_width : real_width;
	img->real_height = real_height == 0.0f ? (float)tex->tex_height : real_height;
	img->owns_tex = false;
}

void kr_image_generate_mipmaps(kr_image_t *img, int levels) {
	kinc_g4_texture_generate_mipmaps(img->tex, levels);
}

void kr_image_destroy(kr_image_t *img) {
	if (!img->loaded) return;
	if (img->owns_tex) {
		kinc_g4_texture_destroy(img->tex);
		kr_free(img->tex);
	}
	if (img->in_memory) {
		kr_free(img->image->data);
		kinc_image_destroy(img->image);
		kr_free(img->image);
	}
	kr_image_init(img);
}
