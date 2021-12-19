#pragma once
#include <stdint.h>

uint32_t kr_color_get_channel(uint32_t color, const char ch);
uint32_t kr_color_set_channel(uint32_t color, const char ch, uint32_t value);
uint32_t kr_color_from_rgbai(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
uint32_t kr_color_from_rgbaf_normalized(float r, float g, float b, float a);
