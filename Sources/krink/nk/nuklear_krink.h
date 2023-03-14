#pragma once
#define NK_INCLUDE_FIXED_TYPES
#define NK_BUTTON_TRIGGER_ON_RELEASE

#include "nuklear.h"
#include <krink/color.h>
#include <krink/eventhandler.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/ttf.h>

/*! \file nuklear_krink.h
    \brief Provides Nuklear IM-GUI operations.
*/

/*! \def KR_NK_MAX_CHARACTERS
    \brief Maximum number of characters of any element.

    Internally used to compute text width.
*/
#ifndef KR_NK_MAX_CHARACTERS
#define KR_NK_MAX_CHARACTERS 512
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Call before using Nuklear.
/// </summary>
/// <param name="font">The font to use in Nuklear</param>
/// <param name="font_height">Font size</param>
/// <param name="memory">A block of memory to be used by Nuklear</param>
/// <param name="size">The size of the allocated memory</param>
void kr_nk_init(kr_ttf_font_t *font, float font_height, void *memory, nk_size size);

/// <summary>
/// Retrieve the `nk_context` struct pointer.
/// </summary>
struct nk_context *kr_nk_get_ctx(void);

/// <summary>
/// Render the current `nk_context`.
/// </summary>
/// <param name="window">The framebuffer to render to</param>
/// <param name="clear">Color to clear</param>
NK_API void kr_nk_render(int window, struct nk_color clear);

#ifdef __cplusplus
}
#endif
