#pragma once

#include <kinc/math/matrix.h>
#include <stdbool.h>

#ifndef KRINK_G2_SDF_BUFFER_SIZE
#define KRINK_G2_SDF_BUFFER_SIZE 1000
#endif

typedef struct krink_sdf_corner_radius {
	float top_left, bottom_left, bottom_right, top_right;
} krink_sdf_corner_radius_t;

void krink_g2_sdf_init(void);
void krink_g2_sdf_set_projection_matrix(kinc_matrix4x4_t mat);
void krink_g2_sdf_draw_rect(float x, float y, float width, float height,
                            krink_sdf_corner_radius_t corner, float border, float smooth,
                            unsigned int color, unsigned int border_color, float opacity,
                            kinc_matrix3x3_t transformation);
void krink_g2_sdf_draw_rect_symm(float x, float y, float width, float height, float corner,
                                 float border, float smooth, unsigned int color,
                                 unsigned int border_color, float opacity,
                                 kinc_matrix3x3_t transformation);
void krink_g2_sdf_draw_circle(float x, float y, float radius, float border, float smooth,
                              unsigned int color, unsigned int border_color, float opacity,
                              kinc_matrix3x3_t transformation);
void krink_g2_sdf_draw_line(float x0, float y0, float x1, float y1, float corner_radius,
                            float smooth, unsigned int color, float opacity,
                            kinc_matrix3x3_t transformation);
void krink_g2_sdf_end(void);
