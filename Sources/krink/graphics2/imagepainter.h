#pragma once

#include <krink/math/matrix.h>
#include <krink/image.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef KR_G2_ISP_BUFFER_SIZE
#define KR_G2_ISP_BUFFER_SIZE 1000
#endif

void kr_isp_init(void);
void kr_isp_set_bilinear_filter(bool bilinear);
void kr_isp_set_projection_matrix(kinc_matrix4x4_t mat);
void kr_isp_draw_scaled_sub_image(kr_image_t *img, float sx, float sy, float sw, float sh, float dx,
                                  float dy, float dw, float dh, float opacity, uint32_t color,
                                  kr_matrix3x3_t transformation);

#ifdef KR_FULL_RGBA_FONTS
#include "ttf.h"

void kr_tsp_set_font(kr_ttf_font_t *font);
void kr_tsp_set_font_size(int size);
void kr_tsp_draw_string(const char *text, float opacity, uint32_t color, float x, float y,
                        kr_matrix3x3_t transformation);
void kr_tsp_draw_characters(int *text, int start, int length, float opacity, uint32_t color,
                            float x, float y, kr_matrix3x3_t transformation);
#endif

void kr_isp_end(void);
