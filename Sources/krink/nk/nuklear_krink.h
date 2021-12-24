#pragma once
#define NK_INCLUDE_FIXED_TYPES
#define NK_BUTTON_TRIGGER_ON_RELEASE

#include "nuklear.h"
#include <krink/color.h>
#include <krink/eventhandler.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/ttf.h>

#ifndef KR_NK_MAX_CHARACTERS
#define KR_NK_MAX_CHARACTERS 512
#endif

void kr_nk_init(kr_ttf_font_t *font, float font_height, void *memory, nk_size size);
struct nk_context *kr_nk_get_ctx(void);
NK_API void kr_nk_render(int window, struct nk_color clear);
