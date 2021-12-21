#pragma once

#include "nuklear.h"
#include <krink/color.h>
#include <krink/eventhandler.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/ttf.h>

void nk_kr_init(kr_ttf_font_t *font, float font_height, void *memory, nk_size size);
NK_API void nk_kr_render(int window, struct nk_color clear);
