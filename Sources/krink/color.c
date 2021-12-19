#include "color.h"
#include <kinc/math/core.h>

uint32_t kr_color_get_channel(uint32_t color, const char ch) {
	switch (ch) {
	case 'A':
		return color >> 24;
	case 'B':
		return (color & 0x00ff0000) >> 16;
	case 'G':
		return (color & 0x0000ff00) >> 8;
	case 'R':
		return color & 0x000000ff;
	}
	return 0;
}

uint32_t kr_color_set_channel(uint32_t color, const char ch, uint32_t value) {
	switch (ch) {
	case 'A':
		return (color & 0x00ffffff) + (value << 24);
	case 'B':
		return (color & 0xff00ffff) + (value << 16);
	case 'G':
		return (color & 0xffff00ff) + (value << 8);
	case 'R':
		return (color & 0xffffff00) + value;
	}
	return 0;
}

uint32_t kr_color_from_rgbai(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	uint32_t c = (uint32_t)a << 24;
	c += (uint32_t)b << 16;
	c += (uint32_t)g << 8;
	c += (uint32_t)r;
	return c;
}

uint32_t kr_color_from_rgbaf_normalized(float r, float g, float b, float a) {
	uint32_t c = (uint32_t)(kinc_min(1.0f, kinc_max(a, 0.0f)) * 255.0f) << 24;
	c += (uint32_t)(kinc_min(1.0f, kinc_max(b, 0.0f)) * 255.0f) << 16;
	c += (uint32_t)(kinc_min(1.0f, kinc_max(g, 0.0f)) * 255.0f) << 8;
	c += (uint32_t)(kinc_min(1.0f, kinc_max(r, 0.0f)) * 255.0f);
	return c;
}
