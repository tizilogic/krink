#pragma once

#include <kinc/math/matrix.h>
#include <krink/image.h>
#include <stdbool.h>

#ifndef KRINK_G2_ISP_BUFFER_SIZE
#define KRINK_G2_ISP_BUFFER_SIZE 1000
#endif

void krink_g2_isp_init(void);
void krink_g2_isp_set_bilinear_filter(bool bilinear);
void krink_g2_isp_set_projection_matrix(kinc_matrix4x4_t mat);
void krink_g2_isp_draw_scaled_sub_image(krink_image_t *img, float sx, float sy, float sw, float sh,
                                        float dx, float dy, float dw, float dh, float opacity,
                                        unsigned int color, kinc_matrix3x3_t transformation);
void krink_g2_isp_end(void);
