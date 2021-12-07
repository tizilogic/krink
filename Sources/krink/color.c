#include "color.h"

unsigned int krink_color_get_channel(unsigned int color, const char ch) {
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
