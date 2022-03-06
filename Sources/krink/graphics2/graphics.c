#include "graphics.h"
#include "coloredpainter.h"
#include "imagepainter.h"
#include "sdfpainter.h"
#include "textpainter.h"
#include <assert.h>
#include <stdbool.h>

#include <kinc/graphics4/graphics.h>
#include <kinc/math/core.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>
#include <kinc/log.h>

static bool begin = false;
static uint32_t g2_color = 0;
static float g2_opacity = 1.0f;
static kr_matrix3x3_t g2_transformation;
static kr_ttf_font_t *g2_font = NULL;
static kinc_matrix4x4_t g2_projection_matrix;
static int g2_font_size;
static float g2_width[KR_G2_MAX_WIN], g2_height[KR_G2_MAX_WIN];
static bool g2_painters_initialized = false;
static int g2_active_window = -1;

void kr_g2_init(int window, int window_width, int window_height) {
	if (!g2_painters_initialized) {
		kr_isp_init();
		kr_csp_init();
		kr_tsp_init();
		kr_sdf_init();
		g2_transformation = kr_matrix3x3_identity();
	}
	kr_g2_set_window_size(window, window_width, window_height);
}

void kr_g2_set_window_size(int window, int window_width, int window_height) {
	assert(window >= 0 && window < KR_G2_MAX_WIN);
	g2_width[window] = (float)window_width;
	g2_height[window] = (float)window_height;

	// TODO: Verify on all platforms
	kr_matrix4x4_t proj;
	if (!kinc_g4_render_targets_inverted_y()) {
		kinc_log(KINC_LOG_LEVEL_INFO, "Non-Inverted Y render target");
		proj = kr_matrix4x4_orthogonal_projection(0.0f, g2_width[window], g2_height[window], 0.0f,
		                                          0.1f, 1000.0f);
	}
	else {
		kinc_log(KINC_LOG_LEVEL_INFO, "Inverted Y render target");
		proj = kr_matrix4x4_orthogonal_projection(0.0f, g2_width[window], g2_height[window], 0.0f,
		                                          0.1f, 1000.0f);
	}
	g2_projection_matrix = kr_matrix4x4_to_kinc(&proj);
	kr_isp_set_projection_matrix(g2_projection_matrix);
	kr_tsp_set_projection_matrix(g2_projection_matrix);
	kr_csp_set_projection_matrix(g2_projection_matrix);
	kr_sdf_set_projection_matrix(g2_projection_matrix);
}

float kr_g2_get_width(int window) {
	assert(window >= 0 && window < KR_G2_MAX_WIN);
	return g2_width[window];
}

float kr_g2_get_height(int window) {
	assert(window >= 0 && window < KR_G2_MAX_WIN);
	return g2_height[window];
}

void kr_g2_destroy(void) {
	// Maybe not needed?
}

void kr_g2_begin(int window) {
	assert(!begin && g2_active_window == -1);
	g2_active_window = window;
	begin = true;
}

void kr_g2_end(void) {
	assert(begin && g2_active_window != -1);
	kr_csp_end();
	kr_tsp_end();
	kr_isp_end();
	kr_sdf_end();
	begin = false;
	g2_active_window = -1;
}

void kr_g2_clear(uint32_t color) {
	assert(begin);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, color, 0.0f, 0);
}

void kr_g2_set_color(uint32_t color) {
	g2_color = color;
}

void kr_g2_set_opacity(float opacity) {
	g2_opacity = opacity;
}

void kr_g2_draw_rect(float x, float y, float width, float height, float strength) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_sdf_end();

	float hs = strength / 2.0f;
	kr_csp_fill_rect(x - hs, y - hs, width + strength, strength, g2_color, g2_opacity,
	                 g2_transformation); // top
	kr_csp_fill_rect(x - hs, y + hs, strength, height - strength, g2_color, g2_opacity,
	                 g2_transformation); // left
	kr_csp_fill_rect(x - hs, y + height - hs, width + strength, strength, g2_color, g2_opacity,
	                 g2_transformation); // bottom
	kr_csp_fill_rect(x + width - hs, y + hs, strength, height - strength, g2_color, g2_opacity,
	                 g2_transformation); // right
}

void kr_g2_fill_rect(float x, float y, float width, float height) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_sdf_end();

	kr_csp_fill_rect(x, y, width, height, g2_color, g2_opacity, g2_transformation);
}

typedef struct center_radius {
	float x, y, r;
} center_radius_t;

static center_radius_t barycenter(float x1, float y1, float x2, float y2, float x3, float y3) {
	float la, lb, lc;
	la = kr_vec2_length(kr_vec2_subv((kr_vec2_t){x2, y2}, (kr_vec2_t){x3, y3}));
	lb = kr_vec2_length(kr_vec2_subv((kr_vec2_t){x1, y1}, (kr_vec2_t){x3, y3}));
	lc = kr_vec2_length(kr_vec2_subv((kr_vec2_t){x1, y1}, (kr_vec2_t){x2, y2}));
	center_radius_t c;
	c.x = (la * x1 + lb * x2 + lc * x3) / (la + lb + lc);
	c.y = (la * y1 + lb * y2 + lc * y3) / (la + lb + lc);

	float s = (la + lb + lc) / 2.0f;
	c.r = kinc_sqrt(((s - la) * (s - lb) * (s - lc)) / s);
	return c;
}

static kr_vec2_t comp_corner(float x, float y, center_radius_t c, float f) {
	kr_vec2_t cv = (kr_vec2_t){c.x, c.y};
	kr_vec2_t v = (kr_vec2_t){x, y};
	return kr_vec2_addv(cv, kr_vec2_mult(kr_vec2_subv(v, cv), f));
}

void kr_g2_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3,
                         float strength) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_sdf_end();

	center_radius_t c = barycenter(x1, y1, x2, y2, x3, y3);
	float s = (strength / 2.0f) / c.r;
	kr_vec2_t ia, ib, ic, oa, ob, oc;
	ia = comp_corner(x1, y1, c, 1.0f - s);
	ib = comp_corner(x2, y2, c, 1.0f - s);
	ic = comp_corner(x3, y3, c, 1.0f - s);
	oa = comp_corner(x1, y1, c, 1.0f + s);
	ob = comp_corner(x2, y2, c, 1.0f + s);
	oc = comp_corner(x3, y3, c, 1.0f + s);

	kr_g2_fill_triangle(oa.x, oa.y, ia.x, ia.y, oc.x, oc.y);
	kr_g2_fill_triangle(ia.x, ia.y, ic.x, ic.y, oc.x, oc.y);
	kr_g2_fill_triangle(oc.x, oc.y, ic.x, ic.y, ob.x, ob.y);
	kr_g2_fill_triangle(ic.x, ic.y, ib.x, ib.y, ob.x, ob.y);
	kr_g2_fill_triangle(ob.x, ob.y, ib.x, ib.y, ia.x, ia.y);
	kr_g2_fill_triangle(oa.x, oa.y, ob.x, ob.y, ia.x, ia.y);
}

void kr_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_sdf_end();

	kr_csp_fill_triangle(x1, y1, x2, y2, x3, y3, g2_color, g2_opacity, g2_transformation);
}

void kr_g2_draw_string(const char *text, float x, float y) {
	assert(begin);
	kr_isp_end();
	kr_csp_end();
	kr_sdf_end();

	kr_tsp_draw_string(text, g2_opacity, g2_color, x, y, g2_transformation);
}

void kr_g2_draw_characters(int *text, int start, int length, float x, float y) {
	assert(begin);
	kr_isp_end();
	kr_csp_end();
	kr_sdf_end();

	kr_tsp_draw_characters(text, start, length, g2_opacity, g2_color, x, y, g2_transformation);
}

void kr_g2_draw_line(float x1, float y1, float x2, float y2, float strength) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_sdf_end();

	kr_vec2_t vec;

	if (y2 == y1) {
		vec = (kr_vec2_t){0.0f, -1.0f};
	}
	else {
		vec = (kr_vec2_t){1.0f, -(x2 - x1) / (y2 - y1)};
	}

	kr_vec2_set_length(&vec, strength);

	kr_vec2_t p1 = (kr_vec2_t){x1 + 0.5f * vec.x, y1 + 0.5f * vec.y};
	kr_vec2_t p2 = (kr_vec2_t){x2 + 0.5f * vec.x, y2 + 0.5f * vec.y};
	kr_vec2_t p3 = kr_vec2_subv(p1, vec);
	kr_vec2_t p4 = kr_vec2_subv(p2, vec);
	kr_csp_fill_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, g2_color, g2_opacity,
	                     g2_transformation);
	kr_csp_fill_triangle(p3.x, p3.y, p2.x, p2.y, p4.x, p4.y, g2_color, g2_opacity,
	                     g2_transformation);
}

void kr_g2_draw_scaled_sub_image(kr_image_t *img, float sx, float sy, float sw, float sh, float dx,
                                 float dy, float dw, float dh) {
	assert(begin);
	kr_csp_end();
	kr_tsp_end();
	kr_sdf_end();

	kr_isp_draw_scaled_sub_image(img, sx, sy, sw, sh, dx, dy, dw, dh, g2_opacity, g2_color,
	                             g2_transformation);
}

void kr_g2_set_transform(kr_matrix3x3_t m) {
	g2_transformation = m;
}

kr_matrix3x3_t kr_g2_get_transform(void) {
	return g2_transformation;
}

kr_ttf_font_t *kr_g2_get_font(void) {
	return g2_font;
}

int kr_g2_get_font_size(void) {
	return g2_font_size;
}

void kr_g2_set_font(kr_ttf_font_t *font, int size) {
	g2_font = font;
	g2_font_size = size;
	kr_tsp_set_font(font);
	kr_tsp_set_font_size(size);
}

void kr_g2_draw_sdf_rect(float x, float y, float width, float height, kr_sdf_corner_radius_t corner,
                         float border, uint32_t border_color, float smooth) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();

	kr_sdf_draw_rect(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                 g2_opacity, g2_transformation);
}

void kr_g2_draw_sdf_rect_symm(float x, float y, float width, float height, float corner,
                              float border, uint32_t border_color, float smooth) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();

	kr_sdf_draw_rect_symm(x, y, width, height, corner, border, smooth, g2_color, border_color,
	                      g2_opacity, g2_transformation);
}

void kr_g2_draw_sdf_circle(float x, float y, float r, float border, uint32_t border_color,
                           float smooth) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();

	kr_sdf_draw_circle(x, y, r, border, smooth, g2_color, border_color, g2_opacity,
	                   g2_transformation);
}

void kr_g2_draw_sdf_line(float x1, float y1, float x2, float y2, float strength, float smooth) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();

	kr_sdf_draw_line(x1, y1, x2, y2, strength, smooth, g2_color, g2_opacity, g2_transformation);
}

void kr_g2_scissor(float x, float y, float w, float h) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();
	kr_sdf_end();
	kinc_g4_scissor((int)(x + 0.5f), (int)(y + 0.5f), (int)(w + 0.5f), (int)(h + 0.5f));
}

void kr_g2_disable_scissor(void) {
	assert(begin);
	kr_isp_end();
	kr_tsp_end();
	kr_csp_end();
	kr_sdf_end();
	kinc_g4_disable_scissor();
}

void kr_g2_set_bilinear_filter(bool bilinear) {
	kr_isp_set_bilinear_filter(bilinear);
}
