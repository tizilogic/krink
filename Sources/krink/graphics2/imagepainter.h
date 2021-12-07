#pragma once

#include <kinc/graphics4/texture.h>
#include <kinc/math/matrix.h>
#include <krink/image.h>
#include <stdbool.h>

#ifndef KRINK_G2_ISP_BUFFER_SIZE
#define KRINK_G2_ISP_BUFFER_SIZE 1000
#endif

void krink_g2_isp_init(void);
void krink_g2_isp_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry, float btrx, float btry);
void krink_g2_isp_set_rect_tex_coords(float left, float top, float right, float bottom);
void krink_g2_isp_set_rect_colors(float opacity, unsigned int color);
void krink_g2_isp_draw_buffer(void);
void krink_g2_isp_set_bilinear_filter(bool bilinear);
void krink_g2_isp_set_projection_matrix(kinc_matrix4x4_t mat);
void krink_g2_isp_draw_scaled_sub_image(krink_image_t *img, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, float opacity,
                                        unsigned int color);
void krink_g2_isp_end(void);
