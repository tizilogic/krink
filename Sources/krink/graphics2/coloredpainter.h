#pragma once

#include <kinc/math/matrix.h>

#ifndef KRINK_G2_CSP_BUFFER_SIZE
#define KRINK_G2_CSP_BUFFER_SIZE 1000
#endif

void krink_g2_csp_init(void);
void krink_g2_csp_set_projection_matrix(kinc_matrix4x4_t mat);
void krink_g2_csp_fill_rect(float x, float y, float width, float height, unsigned int color,
                            float opacity, kinc_matrix3x3_t transformation);
void krink_g2_csp_fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
                                unsigned int color, float opacity, kinc_matrix3x3_t transformation);
void krink_g2_csp_end(void);
