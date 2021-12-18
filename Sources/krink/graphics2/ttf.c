#include "ttf.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <assert.h>
#include <kinc/image.h>
#include <kinc/io/filereader.h>
#include <krink/memory.h>

static int *kr_ttf_glyph_blocks = NULL;
static int *kr_ttf_glyphs = NULL;
static int kr_ttf_num_glyph_blocks = -1;
static int kr_ttf_num_glyphs = -1;

static float myround(float v) {
	return (float)((int)(v + 0.5f));
}

typedef struct kr_ttf_image {
	float m_size;
	stbtt_bakedchar *chars;
	kinc_g4_texture_t tex;
	int width, height;
	float baseline, descent, line_gap;
} kr_ttf_image_t;

void kr_ttf_load_font_blob_internal(kr_ttf_font_t *font, const char *fontpath) {
	kinc_file_reader_t fh;
	assert(kinc_file_reader_open(&fh, fontpath, KINC_FILE_TYPE_ASSET));
	size_t sz = kinc_file_reader_size(&fh);
	font->blob = kr_malloc(sz);
	assert(font->blob != NULL);
	assert(kinc_file_reader_read(&fh, font->blob, sz) == sz);
	kinc_file_reader_close(&fh);
}

kr_ttf_image_t *kr_ttf_get_image_internal(kr_ttf_font_t *font, int size) {
	for (int i = 0; i < font->m_images_len; ++i) {
		if ((int)font->images[i].m_size == size) {
			return &(font->images[i]);
		}
	}
	return NULL;
}

int kr_ttf_get_char_index_internal(kr_ttf_image_t *img, int char_index) {
	if (kr_ttf_num_glyphs <= 0) {
		return 0;
	}
	int offset = kr_ttf_glyph_blocks[0];
	if (char_index < offset) return 0;

	for (int i = 1; i < kr_ttf_num_glyph_blocks / 2; ++i) {
		int prev_end = kr_ttf_glyph_blocks[i * 2 - 1];
		int start = kr_ttf_glyph_blocks[i * 2];
		if (char_index > start - 1) offset += start - 1 - prev_end;
	}

	if (char_index - offset >= kr_ttf_num_glyphs) return 0;
	return char_index - offset;
}

float kr_ttf_get_char_width_internal(kr_ttf_image_t *img, int char_index) {
	return img->chars[kr_ttf_get_char_index_internal(img, char_index)].xadvance;
}

float kr_ttf_get_string_width_internal(kr_ttf_image_t *img, const char *str) {
	float width = 0.0f;
	for (int i = 0; str[i] != 0; ++i) {
		width += kr_ttf_get_char_width_internal(img, str[i]);
	}
	return width;
}

void kr_ttf_init(int *glyphs, int num_glyphs) {
	if (glyphs == NULL) {
		kr_ttf_glyph_blocks = (int *)kr_malloc(2 * sizeof(int));
		assert(kr_ttf_glyph_blocks != NULL);
		kr_ttf_glyphs = (int *)kr_malloc(224 * sizeof(int));
		assert(kr_ttf_glyphs != NULL);
		kr_ttf_num_glyph_blocks = 2;
		kr_ttf_num_glyphs = 224;
		kr_ttf_glyph_blocks[0] = 32;
		kr_ttf_glyph_blocks[1] = 255;
		for (int i = 32; i < 256; ++i) kr_ttf_glyphs[i - 32] = i;
	}
	else {
		kr_ttf_glyphs = (int *)kr_malloc(num_glyphs * sizeof(int));
		assert(kr_ttf_glyphs != NULL);
		kr_ttf_num_glyphs = num_glyphs;
		int blocks = 1;
		kr_ttf_glyphs[0] = glyphs[0];
		int next_char = glyphs[0] + 1;
		int pos = 1;
		while (pos < num_glyphs) {
			kr_ttf_glyphs[pos] = glyphs[pos];
			if (glyphs[pos] != next_char) {
				++blocks;
				next_char = glyphs[pos] + 1;
			}
			else {
				++next_char;
			}
			++pos;
		}

		kr_ttf_glyph_blocks = (int *)kr_malloc(blocks * 2 * sizeof(int));
		assert(kr_ttf_glyph_blocks != NULL);
		kr_ttf_num_glyph_blocks = 2 * blocks;
		kr_ttf_glyph_blocks[0] = glyphs[0];
		next_char = glyphs[0] + 1;
		pos = 1;
		for (int i = 1; i < num_glyphs; ++i) {
			if (glyphs[i] != next_char) {
				kr_ttf_glyph_blocks[pos * 2 - 1] = glyphs[i - 1];
				kr_ttf_glyph_blocks[pos * 2] = glyphs[i];
				++pos;
				next_char = glyphs[i] + 1;
			}
			else {
				++next_char;
			}
		}
		kr_ttf_glyph_blocks[blocks * 2 - 1] = glyphs[num_glyphs - 1];
	}
}

void kr_ttf_font_init(kr_ttf_font_t *font, const char *fontpath, int font_index) {
	font->blob = NULL;
	font->images = NULL;
	font->m_images_len = 0;
	font->m_capacity = 0;
	font_index = font_index;
	kr_ttf_load_font_blob_internal(font, fontpath);
	font->offset = stbtt_GetFontOffsetForIndex(font->blob, font_index);
	if (font->offset == -1) {
		font->offset = stbtt_GetFontOffsetForIndex(font->blob, 0);
	}
}

void kr_ttf_load(kr_ttf_font_t *font, int size) {
	if (kr_ttf_get_image_internal(font, size) != NULL) return; // nothing to do

	// resize images array if necessary
	if (font->m_capacity <= font->m_images_len) {
		size_t new_capacity = 2;
		if (font->m_capacity == 0) {
			font->images = (kr_ttf_image_t *)kr_malloc(new_capacity * sizeof(kr_ttf_image_t));
		}
		else {
			while (font->m_capacity > new_capacity) new_capacity *= 2;
			font->images =
			    (kr_ttf_image_t *)kr_realloc(font->images, new_capacity * sizeof(kr_ttf_image_t));
		}
		assert(font->images != NULL);
		font->m_capacity = new_capacity;
	}

	// create image
	kr_ttf_image_t *img = &(font->images[font->m_images_len]);
	font->m_images_len += 1;
	int width = 64;
	int height = 32;
	stbtt_bakedchar *baked =
	    (stbtt_bakedchar *)kr_malloc(kr_ttf_num_glyphs * sizeof(stbtt_bakedchar));
	assert(baked != NULL);
	unsigned char *pixels = NULL;

	int status = -1;
	while (status <= 0) {
		if (height < width)
			height *= 2;
		else
			width *= 2;
		if (pixels == NULL)
			pixels = (unsigned char *)kr_malloc(width * height);
		else
			pixels = (unsigned char *)kr_realloc(pixels, width * height);
		assert(pixels != NULL);
		status = stbtt_BakeFontBitmapArr(font->blob, font->offset, (float)size, pixels, width,
		                                 height, kr_ttf_glyphs, kr_ttf_num_glyphs, baked);
	}

	// TODO: Scale pixels down if they exceed the supported texture size

	stbtt_fontinfo info;
	int ascent, descent, line_gap;
	stbtt_InitFont(&info, font->blob, font->offset);
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &line_gap);
	float scale = stbtt_ScaleForPixelHeight(&info, (float)size);
	img->m_size = (float)size;
	img->baseline = myround((float)ascent * scale);
	img->descent = myround((float)descent * scale);
	img->line_gap = myround((float)line_gap * scale);
	img->width = (float)width;
	img->height = (float)height;
	img->chars = baked;
	kinc_image_t fontimg;
	kinc_image_init_from_bytes(&fontimg, pixels, width, height, KINC_IMAGE_FORMAT_GREY8);
	kinc_g4_texture_init_from_image(&(img->tex), &fontimg);
	kinc_image_destroy(&fontimg);
	kr_free(pixels);
}

float kr_ttf_height(kr_ttf_font_t *font, int size) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return img->m_size;
}

float kr_ttf_width(kr_ttf_font_t *font, int size, const char *str) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return kr_ttf_get_string_width_internal(img, str);
}

float kr_ttf_width_of_characters(kr_ttf_font_t *font, int size, int *characters, int start,
                                 int length) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	float width = 0.0f;
	for (int i = start; i < start + length; ++i) {
		width += kr_ttf_get_char_width_internal(img, characters[i]);
	}
	return width;
}

float kr_ttf_baseline(kr_ttf_font_t *font, int size) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return img->baseline;
}

float kr_ttf_descent(kr_ttf_font_t *font, int size) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return img->descent;
}

float kr_ttf_line_gap(kr_ttf_font_t *font, int size) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return img->line_gap;
}

bool kr_ttf_get_baked_quad(kr_ttf_font_t *font, int size, kr_ttf_aligned_quad_t *q, int char_code,
                           float xpos, float ypos) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	int char_index = kr_ttf_get_char_index_internal(img, char_code);
	if (char_index >= kr_ttf_num_glyphs) return false;
	float ipw = 1.0f / (float)img->width;
	float iph = 1.0f / (float)img->height;
	stbtt_bakedchar b = img->chars[char_index];
	// if (b == NULL) return null;
	int round_x = (int)(xpos + b.xoff + 0.5);
	int round_y = (int)(ypos + b.yoff + 0.5);

	q->x0 = round_x;
	q->y0 = round_y;
	q->x1 = round_x + b.x1 - b.x0;
	q->y1 = round_y + b.y1 - b.y0;

	q->s0 = b.x0 * ipw;
	q->t0 = b.y0 * iph;
	q->s1 = b.x1 * ipw;
	q->t1 = b.y1 * iph;

	q->xadvance = b.xadvance;

	return true;
}

kinc_g4_texture_t *kr_ttf_get_texture(kr_ttf_font_t *font, int size) {
	kr_ttf_image_t *img = kr_ttf_get_image_internal(font, size);
	assert(img != NULL);
	return &(img->tex);
}

void kr_ttf_font_destroy(kr_ttf_font_t *font) {
	for (int i = 0; i < font->m_images_len; ++i) {
		kinc_g4_texture_destroy(&(font->images[i].tex));
		kr_free(font->images[i].chars);
	}
	kr_free(font->blob);
	kr_free(font->images);
}
