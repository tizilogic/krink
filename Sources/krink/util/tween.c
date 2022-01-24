#include "tween.h"
#include <kinc/math/core.h>

static inline float kr_tween_linear(float k) {
	return k;
}

static inline float kr_tween_sine_in(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else {
		return 1.0f - kinc_cos(k * KINC_PI / 2.0f);
	}
}

static inline float kr_tween_sine_out(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else {
		return kinc_sin(k * (KINC_PI * 0.5f));
	}
}

static inline float kr_tween_sine_in_out(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else {
		return -0.5f * (kinc_cos(KINC_PI * k) - 1.0f);
	}
}

static inline float kr_tween_quad_in(float k) {
	return k * k;
}

static inline float kr_tween_quad_out(float k) {
	return -k * (k - 2.0f);
}

static inline float kr_tween_quad_in_out(float k) {
	return (k < 0.5f) ? 2.0f * k * k : -2.0f * ((k - 1.0f) * (k - 1.0f)) + 1.0f;
}

static inline float kr_tween_cubic_in(float k) {
	return k * k * k;
}

static inline float kr_tween_cubic_out(float k) {
	k = k - 1.0f;
	return k * k * k + 1.0f;
}

static inline float kr_tween_cubic_in_out(float k) {
	k *= 2.0f;
	return (k < 1.0f) ? 0.5f * k * k * k : 0.5f * ((k - 2.0f) * (k - 2.0f) * (k - 2.0f) + 2.0f);
}

static inline float kr_tween_quart_in(float k) {
	k *= k;
	return k * k;
}

static inline float kr_tween_quart_out(float k) {
	k = k - 1.0f;
	k = k * k;
	return 1.0f - k * k;
}

static inline float kr_tween_quart_in_out(float k) {
	k *= 2.0f;
	if (k < 1.0f) return 0.5f * (k * k) * (k * k);
	k -= 2.0f;
	k = (k)*k;
	return -0.5f * (k * k - 2.0f);
}

static inline float kr_tween_quint_in(float k) {
	return k * (k * k) * (k * k);
}

static inline float kr_tween_quint_out(float k) {
	k = k - 1.0f;
	return k * (k * k) * (k * k) + 1.0f;
}

static inline float kr_tween_quint_in_out(float k) {
	k *= 2.0f;
	if (k < 1.0f) return 0.5f * k * (k * k) * (k * k);
	k -= 2.0f;
	return 0.5f * (k) * (k * k) * (k * k) + 1.0f;
}

static inline float kr_tween_expo_in(float k) {
	return k == 0.0f ? 0.0f : kinc_pow(2.0f, 10.0f * (k - 1.0f));
}

static inline float kr_tween_expo_out(float k) {
	return k == 1.0f ? 1.0f : (1.0f - kinc_pow(2.0f, -10.0f * k));
}

static inline float kr_tween_expo_in_out(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	if (k == 1.0f) {
		return 1.0f;
	}
	if ((k /= 1.0f / 2.0f) < 1.0f) {
		return 0.5f * kinc_pow(2.0f, 10.0f * (k - 1.0f));
	}
	return 0.5f * (2.0f - kinc_pow(2.0f, -10.0f * --k));
}

static inline float kr_tween_circ_in(float k) {
	return -(kinc_sqrt(1.0f - k * k) - 1.0f);
}

static inline float kr_tween_circ_out(float k) {
	return kinc_sqrt(1.0f - (k - 1.0f) * (k - 1.0f));
}

static inline float kr_tween_circ_in_out(float k) {
	return k <= 0.5f ? (kinc_sqrt(1.0f - k * k * 4.0f) - 1.0f) / -2.0f
	                 : (kinc_sqrt(1.0f - (k * 2.0f - 2.0f) * (k * 2.0f - 2.0f)) + 1.0f) / 2.0f;
}

static inline float kr_tween_back_in(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else {
		return k * k * ((KR_TWEEN_DEFAULT_OVERSHOOT + 1.0f) * k - KR_TWEEN_DEFAULT_OVERSHOOT);
	}
}

static inline float kr_tween_back_out(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else {
		k = k - 1.0f;
		return (k * k *
		            ((KR_TWEEN_DEFAULT_OVERSHOOT + 1.0f) * k + KR_TWEEN_DEFAULT_OVERSHOOT) +
		        1.0f);
	}
}

static inline float kr_tween_back_in_out(float k) {
	if (k == 0.0f) {
		return 0.0f;
	}
	else if (k == 1.0f) {
		return 1.0f;
	}
	else if ((k *= 2.0f) < 1.0f) {
		return (0.5f * (k * k *
		                (((KR_TWEEN_DEFAULT_OVERSHOOT * 1.525f) + 1.0f) * k -
		                 KR_TWEEN_DEFAULT_OVERSHOOT * 1.525f)));
	}
	else {
		k -= 2.0f;
		return (0.5f * (k * k *
		                    (((KR_TWEEN_DEFAULT_OVERSHOOT * 1.525f) + 1.0f) * k +
		                     KR_TWEEN_DEFAULT_OVERSHOOT * 1.525f) +
		                2.0f));
	}
}

static inline float kr_tween_bounce_out(float k) {
	if (k < (1.0f / 2.75f)) {
		return 7.5625f * k * k;
	}
	else if (k < (2 / 2.75)) {
		k -= 1.5f / 2.75f;
		return 7.5625f * k * k + 0.75f;
	}
	else if (k < (2.5f / 2.75f)) {
		k -= 2.25f / 2.75f;
		return 7.5625f * k * k + 0.9375f;
	}
	k -= (2.625f / 2.75f);
	return 7.5625f * k * k + 0.984375f;
}

static inline float kr_tween_bounce_in(float k) {
	return 1.0f - kr_tween_bounce_out(1.0f - k);
}

static inline float kr_tween_bounce_in_out(float k) {
	return (k < 0.5f) ? kr_tween_bounce_in(k * 2.0f) * 0.5f
	                  : kr_tween_bounce_out(k * 2.0f - 1.0f) * 0.5f + 0.5f;
}

static inline float kr_tween_elastic_in(float k) {
	float s;
	float a = 0.1f, p = 0.4f;
	if (k == 0.0f) {
		return 0.0f;
	}
	if (k == 1.0f) {
		return 1.0f;
	}
	if (a < 1.0f) {
		a = 1.0f;
		s = p / 4.0f;
	}
	else {
		s = p * kinc_asin(1.0f / a) / (2.0f * KINC_PI);
	}
	k -= 1.0f;
	return -(a * kinc_pow(2.0f, 10.0f * k) * kinc_sin((k - s) * (2.0f * KINC_PI) / p));
}

static inline float kr_tween_elastic_out(float k) {
	float s;
	float a = 0.1f, p = 0.4f;
	if (k == 0.0f) {
		return 0.0f;
	}
	if (k == 1.0f) {
		return 1.0f;
	}
	if (a < 1.0f) {
		a = 1.0f;
		s = p / 4.0f;
	}
	else {
		s = p * kinc_asin(1 / a) / (2.0f * KINC_PI);
	}
	return (a * kinc_pow(2.0f, -10.0f * k) * kinc_sin((k - s) * (2.0f * KINC_PI) / p) + 1.0f);
}

static inline float kr_tween_elastic_in_out(float k) {
	float s;
	float a = 0.1f, p = 0.4f;
	if (k == 0.0f) {
		return 0.0f;
	}
	if (k == 1.0f) {
		return 1.0f;
	}
	if (a != 0 || a < 1.0f) {
		a = 1.0f;
		s = p / 4.0f;
	}
	else {
		s = p * kinc_asin(1 / a) / (2.0f * KINC_PI);
	}
	k *= 2.0f;
	if (k < 1.0f) {
		k -= 1.0f;
		return -0.5f * (a * kinc_pow(2.0f, 10.0f * k) * kinc_sin((k - s) * (2.0f * KINC_PI) / p));
	}
	k -= 1.0f;
	return a * kinc_pow(2.0f, -10.0f * k) * kinc_sin((k - s) * (2.0f * KINC_PI) / p) * 0.5f + 1.0f;
}

typedef float (*ease_func_t)(float);

static ease_func_t funcs[KR_EASE_TOTAL] = {
    kr_tween_linear,       kr_tween_sine_in,      kr_tween_sine_out,      kr_tween_sine_in_out,
    kr_tween_quad_in,      kr_tween_quad_out,     kr_tween_quad_in_out,   kr_tween_cubic_in,
    kr_tween_cubic_out,    kr_tween_cubic_in_out, kr_tween_quart_in,      kr_tween_quart_out,
    kr_tween_quart_in_out, kr_tween_quint_in,     kr_tween_quint_out,     kr_tween_quint_in_out,
    kr_tween_expo_in,      kr_tween_expo_out,     kr_tween_expo_in_out,   kr_tween_circ_in,
    kr_tween_circ_out,     kr_tween_circ_in_out,  kr_tween_back_in,       kr_tween_back_out,
    kr_tween_back_in_out,  kr_tween_bounce_in,    kr_tween_bounce_out,    kr_tween_bounce_in_out,
    kr_tween_elastic_in,   kr_tween_elastic_out,  kr_tween_elastic_in_out};

float kr_tween(kr_tween_ease_t ease, float k) {
	return funcs[ease](k);
}
