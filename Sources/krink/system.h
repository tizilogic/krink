#pragma once

#include <stddef.h>

/// <summary>
/// Needs to be called before using krink. Sets up memory management and font rendering. If `glyphs`
/// is not `NULL` the content of it will be copied to an internal buffer.
/// </summary>
/// <param name="memblk">A block of allocated memory for krink to use</param>
/// <param name="size">The size of `memblk`</param>
/// <param name="glyphs">For font rendering: Either an array of glyphs or `NULL` to use the default glyph array</param>
/// <param name="num_glyphs">The length of `glyphs` if not `NULL` otherwise this is ignored</param>
void krink_init(void *memblk, size_t size, int *glyphs, int num_glyphs);
