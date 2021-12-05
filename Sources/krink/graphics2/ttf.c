#include "ttf.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <assert.h>
#include <kinc/io/filereader.h>
#include <krink/memory.h>

static int* krink_ttf_glyph_blocks = NULL;
static int* krink_ttf_glyphs = NULL;
static int krink_ttf_num_glyph_blocks = -1;
static int krink_ttf_num_glyphs = -1;


typedef struct krink_ttf_image {
	float m_size;
	stbtt_bakedchar* chars;
	kinc_g4_texture_t* image;
	int width, height;
	float baseline;
} krink_ttf_image_t;

void krink_ttf_internal_load_font_blob(krink_ttf_font_t* font, const char* fontpath) {
	kinc_file_reader_t fh;
	assert(kinc_file_reader_open(&fh, fontpath, KINC_FILE_TYPE_ASSET));
	size_t sz = kinc_file_reader_size(&fh);
	font->blob = krink_malloc(sz);
	assert(font->blob != NULL);
	assert(kinc_file_reader_read(&fh, font->blob, sz) == sz);
	kinc_file_reader_close(&fh);
}

krink_ttf_image_t* krink_ttf_internal_get_image(krink_ttf_font_t* font, int size) {
	for (int i=0; i < font->m_images_len; ++i) {
		if ((int) font->images[i].m_size == size) {
			return &(font->images[i]);
		}
	}
	return NULL;
}

void krink_ttf_init(int* glyphs, int num_glyphs) {
	if (glyphs == NULL) {
		krink_ttf_glyph_blocks = (int*) krink_malloc(2 * sizeof(int));
		krink_ttf_glyphs = (int*) krink_malloc(224 * sizeof(int));
		krink_ttf_num_glyph_blocks = 2;
		krink_ttf_num_glyphs = 224;
		krink_ttf_glyph_blocks[0] = 32;
		krink_ttf_glyph_blocks[1] = 255;
		for (int i = 32; i < 256; ++i) krink_ttf_glyphs[i - 32] = i;
	}
	else {
		krink_ttf_glyphs = (int*) krink_malloc(num_glyphs * sizeof(int));
		krink_ttf_num_glyphs = num_glyphs;
		int blocks = 1;
		krink_ttf_glyphs[0] = glyphs[0];
		int next_char = glyphs[0] + 1;
		int pos = 1;
		while (pos < num_glyphs) {
			krink_ttf_glyphs[pos] = glyphs[pos];
			if (glyphs[pos] != next_char) {
				++blocks;
				next_char = glyphs[pos] + 1;
			}
			else {
				++next_char;
			}
			++pos;
		}

		krink_ttf_glyph_blocks = (int*) krink_malloc(blocks * 2 * sizeof(int));
		krink_ttf_num_glyph_blocks = 2 * blocks;
		krink_ttf_glyph_blocks[0] = glyphs[0];
		next_char = glyphs[0] + 1;
		pos = 1;
		for (int i = 1; i < num_glyphs; ++i) {
			if (glyphs[i] != next_char) {
				krink_ttf_glyph_blocks[pos * 2 - 1] = glyphs[i - 1];
				krink_ttf_glyph_blocks[pos * 2] = glyphs[i];
				++pos;
				next_char = glyphs[i] + 1;
			}
			else {
				++next_char;
			}
		}
		krink_ttf_glyph_blocks[blocks * 2 - 1] = glyphs[num_glyphs - 1];
	}
}

void krink_ttf_font_init(krink_ttf_font_t* font) {
	font->blob = NULL;
	font->images = NULL;
	font->m_images_len = 0;
	font->m_capacity = 0;
	font->font_index = 0;
}

void krink_ttf_load(krink_ttf_font_t* font, const char* fontpath, int size) {
	// only load blob on first load call
	if (font->m_images_len == 0) {
		krink_ttf_internal_load_font_blob(font, fontpath);
	}
	if (krink_ttf_internal_get_image(font, size) != NULL) return; // nothing to do

	// resize images array if necessary
	if (font->m_capacity <= font->m_images_len) {
		size_t new_capacity = 2;
		if (font->m_capacity == 0) {
			font->images = (krink_ttf_image_t*) krink_malloc(new_capacity * sizeof(krink_ttf_image_t));
		}
		else {
			while (font->m_capacity > new_capacity) new_capacity *= 2;
			font->images = (krink_ttf_image_t*) krink_realloc(font->images, new_capacity * sizeof(krink_ttf_image_t));
		}
		assert(font->images != NULL);
		font->m_capacity = new_capacity;
	}

	// create image
}

float krink_ttf_height(krink_ttf_font_t* font, int size) {
	krink_ttf_image_t* img = krink_ttf_internal_get_image(font, size);
	assert(img != NULL);
	return img->m_size;
}

float krink_ttf_width(krink_ttf_font_t* font, int size, const char* str) {

}

float krink_ttf_width_of_characters(krink_ttf_font_t* font, int size, int* characters, int start, int length) {

}

float krink_ttf_baseline(krink_ttf_font_t* font, int size) {

}

void krink_ttf_get_baked_quad(krink_ttf_font_t* font, int size, krink_ttf_aligned_quad_t* quad, int char_index, float xpos, float ypos) {

}

void krink_ttf_get_texture(krink_ttf_font_t* font, int size, kinc_g4_texture_t* tex) {

}

void krink_ttf_font_destroy(krink_ttf_font_t* font) {
	for (int i=0; i < font->m_images_len; ++i) {
		kinc_g4_texture_destroy(font->images[i].image);
		krink_free(font->images[i].image);
		krink_free(font->images[i].chars);
		krink_free(&(font->images[i]));
	}
	krink_free(font->blob);
	krink_free(font->images);
}
