#pragma once

#include "ttf.h"

#include <kinc/math/matrix.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef KRINK_G2_TSP_BUFFER_SIZE
#define KRINK_G2_TSP_BUFFER_SIZE 1000
#endif

void krink_g2_tsp_init(void);
void krink_g2_tsp_set_ppu(float ppu);
void krink_g2_tsp_draw_buffer(bool end);
void krink_g2_tsp_set_bilinear_filter(bool bilinear);
void krink_g2_tsp_set_projection_matrix(kinc_matrix4x4_t mat);
void krink_g2_tsp_set_font(krink_ttf_font_t *font);
void krink_g2_tsp_set_font_size(float size);
void krink_g2_tsp_draw_string(const char *text, float opacity, uint32_t color, float x, float y,
                              kinc_matrix3x3_t transformation);
void krink_g2_tsp_draw_characters(int *text, int start, int length, float opacity, uint32_t color,
                                  float x, float y, kinc_matrix3x3_t transformation);
void krink_g2_tsp_end(void);
