#pragma once
#include <kinc/graphics4/texture.h>
#include <stdbool.h>

typedef struct kr_ttf_image kr_ttf_image_t;

typedef struct kr_ttf_aligned_quad {
	float x0, y0, s0, t0; // top-left
	float x1, y1, s1, t1; // bottom-right
	float xadvance;
} kr_ttf_aligned_quad_t;

typedef struct kr_ttf_font {
	unsigned char *blob;
	kr_ttf_image_t *images;
	size_t m_capacity;
	size_t m_images_len;
	int offset;
} kr_ttf_font_t;

/// <summary>
/// Initialize kr_ttf with optional glyphs array. The array passed in, if any, is copied and no
/// longer used by kr_ttf after calling this function.
/// </summary>
/// <param name="glyphs">Either an array of glyphs or `NULL` to use the default glyph array</param>
/// <param name="num_glyphs">The length of `glyphs` if not `NULL` otherwise this is ignored</param>
void kr_ttf_init(int *glyphs, int num_glyphs);

/// <summary>
/// Initializes a font object. You need one font object per font and font index.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="fontpath">Path to the TTF file</param>
/// <param name="font_index">Font index to determine offset. Silently falls back to 0 if unable to
/// load the specified index</param>
void kr_ttf_font_init(kr_ttf_font_t *font, const char *fontpath, int font_index);

/// <summary>
/// Load a font with a given size.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
void kr_ttf_load(kr_ttf_font_t *font, int size);

/// <summary>
/// Returns the normalized font height in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float kr_ttf_height(kr_ttf_font_t *font, int size);

/// <summary>
/// Return the width of a string in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="str">Null terminated C string</param>
float kr_ttf_width(kr_ttf_font_t *font, int size, const char *str);

/// <summary>
/// Return the width of a subset of an array of characters in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="characters">Array of codepoints</param>
/// <param name="start">Start index</param>
/// <param name="length">Number of characters</param>
float kr_ttf_width_of_characters(kr_ttf_font_t *font, int size, int *characters, int start,
                                 int length);

/// <summary>
/// Returns the baseline for a font in pixel. (=Ascent)
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float kr_ttf_baseline(kr_ttf_font_t *font, int size);

/// <summary>
/// Returns the descent for a font in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float kr_ttf_descent(kr_ttf_font_t *font, int size);

/// <summary>
/// Returns the line gap for a font in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float kr_ttf_line_gap(kr_ttf_font_t *font, int size);

/// <summary>
/// Get an aligned quad for a character. Returns false if an error occurred.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="q">Pointer to the quad</param>
/// <param name="char_code">Character codepoint</param>
/// <param name="xpos">x position</param>
/// <param name="ypos">y position</param>
bool kr_ttf_get_baked_quad(kr_ttf_font_t *font, int size, kr_ttf_aligned_quad_t *q, int char_code,
                           float xpos, float ypos);

/// <summary>
/// Get the baked texture of the font.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
kinc_g4_texture_t *kr_ttf_get_texture(kr_ttf_font_t *font, int size);

/// <summary>
/// Destroys a font with all loaded font sizes.
/// </summary>
/// <param name="font">Pointer to your font object</param>
void kr_ttf_font_destroy(kr_ttf_font_t *font);
