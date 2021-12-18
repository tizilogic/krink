#include "vector.h"

#include <math.h>

// Vector2

float kr_vec2_length(kr_vec2_t v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

float kr_vec2_set_length(kr_vec2_t *v, float length) {
	float current_length = kr_vec2_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	return length;
}

kr_vec2_t kr_vec2_addv(kr_vec2_t vlh, kr_vec2_t vrh) {
	return (kr_vec2_t){vlh.x + vrh.x, vlh.y + vrh.y};
}

kr_vec2_t kr_vec2_subv(kr_vec2_t vlh, kr_vec2_t vrh) {
	return (kr_vec2_t){vlh.x - vrh.x, vlh.y - vrh.y};
}

kr_vec2_t kr_vec2_addf(kr_vec2_t vlh, float vrh) {
	return (kr_vec2_t){vlh.x + vrh, vlh.y + vrh};
}

kr_vec2_t kr_vec2_subf(kr_vec2_t vlh, float vrh) {
	return (kr_vec2_t){vlh.x - vrh, vlh.y - vrh};
}

kr_vec2_t kr_vec2_mult(kr_vec2_t v, float s) {
	return (kr_vec2_t){v.x * s, v.y * s};
}

kr_vec2_t kr_vec2_div(kr_vec2_t v, float s) {
	return kr_vec2_mult(v, 1.0f / s);
}

float kr_vec2_dot(kr_vec2_t vlh, kr_vec2_t vrh) {
	return vlh.x * vrh.x + vlh.y * vrh.y;
}

kr_vec2_t kr_vec2_normalized(kr_vec2_t v) {
	kr_vec2_t ret = (kr_vec2_t){v.x, v.y};
	kr_vec2_set_length(&ret, 1.0f);
	return ret;
}

float kr_vec2_angle(kr_vec2_t vlh, kr_vec2_t vrh) {
	return atan2f(vlh.x * vrh.y - vlh.y * vrh.x, vlh.x * vrh.x + vlh.y * vrh.y);
}

// Vector3

float kr_vec3_length(kr_vec3_t v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float kr_vec3_set_length(kr_vec3_t *v, float length) {
	float current_length = kr_vec3_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	v->z *= mul;
	return length;
}

kr_vec3_t kr_vec3_addv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){vlh.x + vrh.x, vlh.y + vrh.y, vlh.z + vrh.z};
}

kr_vec3_t kr_vec3_subv(kr_vec3_t vlh, kr_vec3_t vrh) {
	return (kr_vec3_t){vlh.x - vrh.x, vlh.y - vrh.y, vlh.z - vrh.z};
}

kr_vec3_t kr_vec3_addf(kr_vec3_t vlh, float vrh) {
	return (kr_vec3_t){vlh.x + vrh, vlh.y + vrh, vlh.z + vrh};
}

kr_vec3_t kr_vec3_subf(kr_vec3_t vlh, float vrh) {
	return (kr_vec3_t){vlh.x - vrh, vlh.y - vrh, vlh.z - vrh};
}

kr_vec3_t kr_vec3_mult(kr_vec3_t v, float s) {
	return (kr_vec3_t){v.x * s, v.y * s, v.z * s};
}

kr_vec3_t kr_vec3_div(kr_vec3_t v, float s) {
	return kr_vec3_mult(v, 1.0f / s);
}

float kr_vec3_dot(kr_vec3_t vlh, kr_vec3_t vrh) {
	return vlh.x * vrh.x + vlh.y * vrh.y + vlh.z * vrh.z;
}

kr_vec3_t kr_vec3_cross(kr_vec3_t vlh, kr_vec3_t vrh) {
	float x = vlh.y * vrh.z - vlh.z * vrh.y;
	float y = vlh.z * vrh.x - vlh.x * vrh.z;
	float z = vlh.x * vrh.y - vlh.y * vrh.x;
	return (kr_vec3_t){x, y, z};
}

kr_vec3_t kr_vec3_normalized(kr_vec3_t v) {
	kr_vec3_t ret = (kr_vec3_t){v.x, v.y, v.z};
	kr_vec3_set_length(&ret, 1.0f);
	return ret;
}

// Vector4

float kr_vec4_length(kr_vec4_t v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float kr_vec4_set_length(kr_vec4_t *v, float length) {
	float current_length = kr_vec4_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	v->z *= mul;
	v->w *= mul;
	return length;
}

kr_vec4_t kr_vec4_addv(kr_vec4_t vlh, kr_vec4_t vrh) {
	return (kr_vec4_t){vlh.x + vrh.x, vlh.y + vrh.y, vlh.z + vrh.z, vlh.w + vrh.w};
}

kr_vec4_t kr_vec4_subv(kr_vec4_t vlh, kr_vec4_t vrh) {
	return (kr_vec4_t){vlh.x - vrh.x, vlh.y - vrh.y, vlh.z - vrh.z, vlh.w - vrh.w};
}

kr_vec4_t kr_vec4_addf(kr_vec4_t vlh, float vrh) {
	return (kr_vec4_t){vlh.x + vrh, vlh.y + vrh, vlh.z + vrh, vlh.w + vrh};
}

kr_vec4_t kr_vec4_subf(kr_vec4_t vlh, float vrh) {
	return (kr_vec4_t){vlh.x - vrh, vlh.y - vrh, vlh.z - vrh, vlh.w - vrh};
}

kr_vec4_t kr_vec4_mult(kr_vec4_t v, float s) {
	return (kr_vec4_t){v.x * s, v.y * s, v.z * s, v.w * s};
}

kr_vec4_t kr_vec4_div(kr_vec4_t v, float s) {
	return kr_vec4_mult(v, 1.0f / s);
}

kr_vec4_t kr_vec4_normalized(kr_vec4_t v) {
	kr_vec4_t ret = (kr_vec4_t){v.x, v.y, v.z};
	kr_vec4_set_length(&ret, 1.0f);
	return ret;
}
