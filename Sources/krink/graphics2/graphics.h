#pragma once
#include <kinc/global.h>
#include <kinc/math/vector.h>
#include <kinc/math/matrix.h>
#include <kinc/graphics4/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initialize krink_g2.
/// </summary>
void krink_g2_init(void);

/// <summary>
/// Destroy krink_g2.
/// </summary>
/// <param name="texture">The texture to set a mipmap-level for</param>
void krink_g2_destroy(void);

/// <summary>
/// Needs to be called before rendering to a window. Typically called at the start of each frame.
/// </summary>
void krink_g2_begin(void);

/// <summary>
/// Needs to be called after rendering to a window. Typically called at the end of each frame.
/// </summary>
void krink_g2_end(void);

/// <summary>
/// Clears the color, depth and/or stencil-components of the current framebuffer or render-target.
/// </summary>
/// <param name="color">The color-value to clear to</param>
void krink_g2_clear(uint32_t color);

/// <summary>
/// Set vertex color for following draw calls.
/// </summary>
/// <param name="color">The color-value to draw with</param>
void krink_g2_set_color(uint32_t color);

/// <summary>
/// Draw a rectangle.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="strength"></param>
void krink_g2_draw_rect(float x, float y, float width, float height, float strength);

/// <summary>
/// Fill a rectangle.
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void krink_g2_fill_rect(float x, float y, float width, float height);

/// <summary>
/// Fill a triangle.
/// </summary>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="x2"></param>
/// <param name="y2"></param>
/// <param name="x3"></param>
/// <param name="y3"></param>
void krink_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3);

/// <summary>
/// Draw a string.
/// </summary>
/// <param name="text"></param>
/// <param name="x"></param>
/// <param name="y"></param>
void krink_g2_draw_string(const char* text, float x, float y);

void krink_g2_draw_line(float x1, float y1, float x2, float y2, float strength);

void krink_g2_draw_scaled_sub_image(kinc_g4_texture_t* img, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
/*
void krink_g2_reset_transform();
void krink_g2_pushTransformation(kinc_matrix3x3_t* trans);
kinc_matrix3x3_t* krink_g2_popTransformation();
void krink_g2_translate(kinc_vector2_t pos);
void krink_g2_set_rotation(float angle, float centerx, float centery);
*/
const char* krink_g2_get_font();
int krink_g2_get_font_size();
void krink_g2_set_font(const char* font, size_t size);
void krink_g2_scissor(int x, int y, int w, int h);
void krink_g2_disable_scissor(void);

#ifdef __cplusplus
}
#endif
