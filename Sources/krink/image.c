#include "image.h"
#include "memory.h"

#include <assert.h>

void krink_image_init(krink_image_t *img) {
	img->real_width = 0.0f;
	img->real_height = 0.0f;
	img->path = NULL;
	img->image = NULL;
	img->in_memory = false;
	img->loaded = false;
}

void krink_image_load(krink_image_t *img, char *path, bool keep_in_memory) {
	krink_image_init(img);
	img->path = path;
	img->image = (kinc_image_t *)krink_malloc(sizeof(kinc_image_t));
	assert(img->image != NULL);

	void *image_mem = krink_malloc(kinc_image_size_from_file(img->path));
	assert(image_mem != NULL);
	kinc_image_init_from_file(img->image, image_mem, path);
	kinc_g4_texture_init_from_image(&(img->tex), img->image);
	img->loaded = true;
	img->in_memory = keep_in_memory;
	img->real_width = (float)img->image->width;
	img->real_height = (float)img->image->height;

	if (!keep_in_memory) {
		kinc_image_destroy(img->image);
		krink_free(img->image);
		img->image = NULL;
		krink_free(image_mem);
	}
}

void krink_image_destroy(krink_image_t *img) {
	if (!img->loaded) return;
	kinc_g4_texture_destroy(&(img->tex));
	if (img->loaded) {
		krink_free(img->image->data);
		kinc_image_destroy(img->image);
	}
	krink_image_init(img);
}
