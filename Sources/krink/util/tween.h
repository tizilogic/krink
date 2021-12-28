#pragma once

/*
This implementation was ported and adapted from Lubos Lenkos' armory3d/iron,
licensed under zlib/libpng license
https://github.com/armory3d/iron/blob/a9749a252edd8b4763d4bde10c7f048cfa8479c6/Sources/iron/system/Tween.hx
*/

#define KR_TWEEN_DEFAULT_OVERSHOOT 1.70158f

typedef enum kr_tween_ease {
	KR_EASE_LINEAR = 0,
	KR_EASE_SINE_IN,
	KR_EASE_SINE_OUT,
	KR_EASE_SINE_IN_OUT,
	KR_EASE_QUAD_IN,
	KR_EASE_QUAD_OUT,
	KR_EASE_QUAD_IN_OUT,
	KR_EASE_CUBIC_IN,
	KR_EASE_CUBIC_OUT,
	KR_EASE_CUBIC_IN_OUT,
	KR_EASE_QUART_IN,
	KR_EASE_QUART_OUT,
	KR_EASE_QUART_IN_OUT,
	KR_EASE_QUINT_IN,
	KR_EASE_QUINT_OUT,
	KR_EASE_QUINT_IN_OUT,
	KR_EASE_EXPO_IN,
	KR_EASE_EXPO_OUT,
	KR_EASE_EXPO_IN_OUT,
	KR_EASE_CIRC_IN,
	KR_EASE_CIRC_OUT,
	KR_EASE_CIRC_IN_OUT,
	KR_EASE_BACK_IN,
	KR_EASE_BACK_OUT,
	KR_EASE_BACK_IN_OUT,
	KR_EASE_BOUNCE_IN,
	KR_EASE_BOUNCE_OUT,
	KR_EASE_BOUNCE_IN_OUT,
	KR_EASE_ELASTIC_IN,
	KR_EASE_ELASTIC_OUT,
	KR_EASE_ELASTIC_IN_OUT,
    KR_EASE_TOTAL
} kr_tween_ease_t;

float kr_tween(kr_tween_ease_t ease, float k);
