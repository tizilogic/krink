#pragma once
#include <stdint.h>

uint32_t krink_color_get_channel(uint32_t color, const char ch);
uint32_t krink_color_set_channel(uint32_t color, const char ch, uint32_t value);
