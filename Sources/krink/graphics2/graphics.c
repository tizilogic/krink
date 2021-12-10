#include "graphics.h"
#include "coloredpainter.h"
#include "imagepainter.h"
#include "sdfpainter.h"
#include "textpainter.h"
#include <assert.h>
#include <stdbool.h>

#include <kinc/graphics4/graphics.h>
#include <krink/math/vector.h>

static bool begin = false;
static uint32_t g2_color = 0;
static float g2_opacity = 1.0f;
static kinc_matrix3x3_t g2_transformation;
static krink_ttf_font_t *g2_font = NULL;
static float g2_font_size;
static float g2_ppx, g2_unit_width, g2_unit_height;

void krink_g2_init(int window_width, int window_height) {
	krink_g2_isp_init();
	krink_g2_csp_init();
	krink_g2_tsp_init();
	krink_g2_sdf_init();
	g2_transformation = kinc_matrix3x3_identity();
	krink_g2_set_window_size(window_width, window_height);
}

void krink_g2_set_window_size(int window_width, int window_height) {
	g2_ppx = window_width >= window_height ? (float)window_height : (float)window_width;
	g2_unit_width = (float)window_width / g2_ppx;
	g2_unit_height = (float)window_height / g2_ppx;

	// TODO: compute projection matrix;
}

float krink_g2_get_ppx(void) {
	return g2_ppx;
}

float krink_g2_get_unit_width(void) {
	return g2_unit_width;
}

float krink_g2_get_unit_height(void) {
	return g2_unit_width;
}

void krink_g2_destroy(void) {
	// Maybe not needed?
}

void krink_g2_begin(void) {
	assert(!begin);
	kinc_g4_begin(0); // TODO: support more than one FB/window
	begin = true;
}

void krink_g2_end(void) {
	assert(begin);
	kinc_g4_end(0);
	begin = false;
}

void krink_g2_clear(uint32_t color) {
	assert(begin);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, color, 0.0f, 0);
}

void krink_g2_set_color(uint32_t color) {
	g2_color = color;
}

void krink_g2_set_opacity(float opacity) {
	g2_opacity = opacity;
}

void krink_g2_draw_rect(float x, float y, float width, float height, float strength) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	float hs = strength / 2.0f;
	krink_g2_csp_fill_rect(x - hs, y - hs, width + hs, strength, g2_color, g2_opacity,
	                       g2_transformation);
	krink_g2_csp_fill_rect(x - hs, y + hs, strength, height - hs, g2_color, g2_opacity,
	                       g2_transformation);
	krink_g2_csp_fill_rect(x - hs, y + height - hs, width + hs, strength, g2_color, g2_opacity,
	                       g2_transformation);
	krink_g2_csp_fill_rect(x + width - hs, y + hs, strength, height - hs, g2_color, g2_opacity,
	                       g2_transformation);
}

void krink_g2_fill_rect(float x, float y, float width, float height) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_csp_fill_rect(x, y, width, height, g2_color, g2_opacity, g2_transformation);
}

void krink_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_csp_fill_triangle(x1, y1, x2, y2, x3, y3, g2_color, g2_opacity, g2_transformation);
}

void krink_g2_draw_string(const char *text, float x, float y) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_csp_end();
	krink_g2_sdf_end();

	krink_g2_tsp_draw_string(text, g2_opacity, g2_color, x, y, g2_transformation);
}

void krink_g2_draw_characters(int *text, int start, int length, float x, float y) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_csp_end();
	krink_g2_sdf_end();

	krink_g2_tsp_draw_characters(text, start, length, g2_opacity, g2_color, x, y,
	                             g2_transformation);
}

void krink_g2_draw_line(float x1, float y1, float x2, float y2, float strength) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_vec2_t vec;

	if (y2 == y1) {
		vec = (krink_vec2_t){0.0f, -1.0f};
	}
	else {
		vec = (krink_vec2_t){1.0f, -(x2 - x1) / (y2 - y1)};
	}

	krink_vec2_set_length(&vec, strength);

	krink_vec2_t p1 = (krink_vec2_t){x1 + 0.5f * vec.x, y1 + 0.5f * vec.y};
	krink_vec2_t p2 = (krink_vec2_t){x2 + 0.5f * vec.x, y2 + 0.5f * vec.y};
	krink_vec2_t p3 = krink_vec2_subv(p1, vec);
	krink_vec2_t p4 = krink_vec2_subv(p2, vec);
	krink_g2_csp_fill_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, g2_color, g2_opacity,
	                           g2_transformation);
	krink_g2_csp_fill_triangle(p3.x, p3.y, p2.x, p2.y, p4.x, p4.y, g2_color, g2_opacity,
	                           g2_transformation);
}

void krink_g2_draw_scaled_sub_image(krink_image_t *img, float sx, float sy, float sw, float sh,
                                    float dx, float dy, float dw, float dh) {
	assert(begin);
	krink_g2_csp_end();
	krink_g2_tsp_end();
	krink_g2_sdf_end();

	krink_g2_isp_draw_scaled_sub_image(img, sx, sy, sw, sh, dx, dy, dw, dh, g2_opacity, g2_color,
	                                   g2_transformation);
}

void krink_g2_set_transform(kinc_matrix3x3_t m) {
	g2_transformation = m;
}

kinc_matrix3x3_t krink_g2_get_transform(void) {
	return g2_transformation;
}

krink_ttf_font_t *krink_g2_get_font(void) {
	return g2_font;
}

float krink_g2_get_font_size(void) {
	return g2_font_size;
}

void krink_g2_set_font(krink_ttf_font_t *font, float size) {
	g2_font = font;
	g2_font_size = size;
	krink_g2_tsp_set_font(font);
	krink_g2_tsp_set_font_size((int)(size * g2_ppx));
}

void krink_g2_draw_sdf_rect(float x, float y, float width, float height,
                            krink_sdf_corner_radius_t corner, float border, uint32_t border_color,
                            float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_rect(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                       g2_opacity, g2_transformation);
}

void krink_g2_draw_sdf_rect_symm(float x, float y, float width, float height, float corner,
                                 float border, uint32_t border_color, float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_rect_symm(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                            g2_opacity, g2_transformation);
}

void krink_g2_draw_sdf_circle(float x, float y, float r, float border, uint32_t border_color,
                              float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_circle(x, y, r, border, smooth, g2_color, border_color, g2_opacity,
	                         g2_transformation);
}

void krink_g2_draw_sdf_line(float x1, float y1, float x2, float y2, float strength, float smooth) {
	assert(begin);
	krink_g2_isp_end();
	krink_g2_tsp_end();
	krink_g2_csp_end();

	krink_g2_sdf_draw_line(x1, y1, x2, y2, strength, smooth, g2_color, g2_opacity,
	                       g2_transformation);
}

void krink_g2_scissor(float x, float y, float w, float h) {
	kinc_g4_scissor((int)(x * g2_ppx), (int)(y * g2_ppx), (int)(w * g2_ppx), (int)(h * g2_ppx));
}

void krink_g2_disable_scissor(void) {
	kinc_g4_disable_scissor();
}
