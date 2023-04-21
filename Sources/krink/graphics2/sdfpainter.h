#pragma once

#include <krink/math/matrix.h>
#include <kinc/math/matrix.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef KR_G2_SDF_BUFFER_SIZE
#define KR_G2_SDF_BUFFER_SIZE 1000
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kr_sdf_corner_radius {
	float top_left, bottom_left, bottom_right, top_right;
} kr_sdf_corner_radius_t;

void kr_sdf_init(void);
void kr_sdf_destroy(void);
void kr_sdf_set_projection_matrix(kinc_matrix4x4_t mat);
void kr_sdf_draw_rect(float x, float y, float width, float height, kr_sdf_corner_radius_t corner,
                      float border, float smooth, uint32_t color, uint32_t border_color,
                      float opacity, kr_matrix3x3_t transformation);
void kr_sdf_draw_rect_symm(float x, float y, float width, float height, float corner, float border,
                           float smooth, uint32_t color, uint32_t border_color, float opacity,
                           kr_matrix3x3_t transformation);
void kr_sdf_draw_circle(float x, float y, float radius, float border, float smooth, uint32_t color,
                        uint32_t border_color, float opacity, kr_matrix3x3_t transformation);
void kr_sdf_draw_line(float x0, float y0, float x1, float y1, float strength, float smooth,
                      uint32_t color, float opacity, kr_matrix3x3_t transformation);
void kr_sdf_end(void);

#ifdef __cplusplus
}
#endif
