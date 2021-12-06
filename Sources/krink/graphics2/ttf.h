#pragma once
#include <kinc/graphics4/texture.h>
#include <stdbool.h>

typedef struct krink_ttf_image krink_ttf_image_t;

typedef struct krink_ttf_aligned_quad {
	float x0, y0, s0, t0; // top-left
	float x1, y1, s1, t1; // bottom-right
	float xadvance;
} krink_ttf_aligned_quad_t;

typedef struct krink_ttf_font {
	unsigned char *blob;
	krink_ttf_image_t *images;
	size_t m_capacity;
	size_t m_images_len;
	int offset;
} krink_ttf_font_t;

/// <summary>
/// Initialize krink_ttf with optional glyphs array. The array passed in, if any, is copied and no
/// longer used by krink_ttf after calling this function.
/// </summary>
/// <param name="glyphs">Either an array of glyphs or `NULL` to use the default glyph array</param>
/// <param name="num_glyphs">The length of `glyphs` if not `NULL` otherwise this is ignored</param>
void krink_ttf_init(int *glyphs, int num_glyphs);

/// <summary>
/// Initializes a font object. You need one font object per font and font index.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="fontpath">Path to the TTF file</param>
/// <param name="font_index">Font index to determine offset. Silently falls back to 0 if unable to load the specified index</param>
void krink_ttf_font_init(krink_ttf_font_t *font, const char *fontpath, int font_index);

/// <summary>
/// Load a font with a given size.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
void krink_ttf_load(krink_ttf_font_t *font, int size);

/// <summary>
/// Returns the normalized font height in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float krink_ttf_height(krink_ttf_font_t *font, int size);

/// <summary>
/// Return the width of a string in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="str">Null terminated C string</param>
float krink_ttf_width(krink_ttf_font_t *font, int size, const char *str);

/// <summary>
/// Return the width of a subset of an array of characters in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="characters">Array of codepoints</param>
/// <param name="start">Start index</param>
/// <param name="length">Number of characters</param>
float krink_ttf_width_of_characters(krink_ttf_font_t *font, int size, int *characters, int start, int length);

/// <summary>
/// Returns the baseline for a font in pixel. (=Ascent)
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float krink_ttf_baseline(krink_ttf_font_t *font, int size);

/// <summary>
/// Returns the descent for a font in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float krink_ttf_descent(krink_ttf_font_t *font, int size);

/// <summary>
/// Returns the line gap for a font in pixel.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
float krink_ttf_line_gap(krink_ttf_font_t *font, int size);

/// <summary>
/// Get an aligned quad for a character. Returns false if an error occurred.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
/// <param name="q">Pointer to the quad</param>
/// <param name="char_code">Character codepoint</param>
/// <param name="xpos">x position</param>
/// <param name="ypos">y position</param>
bool krink_ttf_get_baked_quad(krink_ttf_font_t *font, int size, krink_ttf_aligned_quad_t *q, int char_code, float xpos, float ypos);

/// <summary>
/// Get the baked texture of the font.
/// </summary>
/// <param name="font">Pointer to your font object</param>
/// <param name="size">Font height in pixel</param>
kinc_g4_texture_t *krink_ttf_get_texture(krink_ttf_font_t *font, int size);

/// <summary>
/// Destroys a font with all loaded font sizes.
/// </summary>
/// <param name="font">Pointer to your font object</param>
void krink_ttf_font_destroy(krink_ttf_font_t *font);
