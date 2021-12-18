#include "color.h"

uint32_t kr_color_get_channel(uint32_t color, const char ch) {
	switch (ch) {
	case 'A':
		return color >> 24;
	case 'R':
		return (color & 0x00ff0000) >> 16;
	case 'G':
		return (color & 0x0000ff00) >> 8;
	case 'B':
		return color & 0x000000ff;
	}
	return 0;
}

uint32_t kr_color_set_channel(uint32_t color, const char ch, uint32_t value) {
	switch (ch) {
	case 'A':
		return (color & 0x00ffffff) + (value << 24);
	case 'R':
		return (color & 0xff00ffff) + (value << 16);
	case 'G':
		return (color & 0xffff00ff) + (value << 8);
	case 'B':
		return (color & 0xffffff00) + value;
	}
	return 0;
}
