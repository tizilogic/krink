#pragma once

#include <kinc/math/matrix.h>
#include <stdint.h>

#ifndef KR_G2_CSP_BUFFER_SIZE
#define KR_G2_CSP_BUFFER_SIZE 1000
#endif

void kr_csp_init(void);
void kr_csp_set_projection_matrix(kinc_matrix4x4_t mat);
void kr_csp_fill_rect(float x, float y, float width, float height, uint32_t color, float opacity,
                      kinc_matrix3x3_t transformation);
void kr_csp_fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
                          uint32_t color, float opacity, kinc_matrix3x3_t transformation);
void kr_csp_end(void);
