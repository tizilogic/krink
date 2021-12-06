#include "vector.h"

#include <math.h>

// Vector2

float krink_vec2_length(krink_vec2_t v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

float krink_vec2_set_length(krink_vec2_t *v, float length) {
	float current_length = krink_vec2_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	return length;
}

krink_vec2_t krink_vec2_addv(krink_vec2_t vlh, krink_vec2_t vrh) {
	return (krink_vec2_t){vlh.x + vrh.x, vlh.y + vrh.y};
}

krink_vec2_t krink_vec2_subv(krink_vec2_t vlh, krink_vec2_t vrh) {
	return (krink_vec2_t){vlh.x - vrh.x, vlh.y - vrh.y};
}

krink_vec2_t krink_vec2_addf(krink_vec2_t vlh, float vrh) {
	return (krink_vec2_t){vlh.x + vrh, vlh.y + vrh};
}

krink_vec2_t krink_vec2_subf(krink_vec2_t vlh, float vrh) {
	return (krink_vec2_t){vlh.x - vrh, vlh.y - vrh};
}

krink_vec2_t krink_vec2_mult(krink_vec2_t v, float s) {
	return (krink_vec2_t){v.x * s, v.y * s};
}

krink_vec2_t krink_vec2_div(krink_vec2_t v, float s) {
	return krink_vec2_mult(v, 1.0f / s);
}

float krink_vec2_dot(krink_vec2_t vlh, krink_vec2_t vrh) {
	return vlh.x * vrh.x + vlh.y * vrh.y;
}

krink_vec2_t krink_vec2_normalized(krink_vec2_t v) {
	krink_vec2_t ret = (krink_vec2_t){v.x, v.y};
	krink_vec2_set_length(&ret, 1.0f);
	return ret;
}

float krink_vec2_angle(krink_vec2_t vlh, krink_vec2_t vrh) {
	return atan2f(vlh.x * vrh.y - vlh.y * vrh.x, vlh.x * vrh.x + vlh.y * vrh.y);
}

// Vector3

float krink_vec3_length(krink_vec3_t v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float krink_vec3_set_length(krink_vec3_t *v, float length) {
	float current_length = krink_vec3_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	v->z *= mul;
	return length;
}

krink_vec3_t krink_vec3_addv(krink_vec3_t vlh, krink_vec3_t vrh) {
	return (krink_vec3_t){vlh.x + vrh.x, vlh.y + vrh.y, vlh.z + vrh.z};
}

krink_vec3_t krink_vec3_subv(krink_vec3_t vlh, krink_vec3_t vrh) {
	return (krink_vec3_t){vlh.x - vrh.x, vlh.y - vrh.y, vlh.z - vrh.z};
}

krink_vec3_t krink_vec3_addf(krink_vec3_t vlh, float vrh) {
	return (krink_vec3_t){vlh.x + vrh, vlh.y + vrh, vlh.z + vrh};
}

krink_vec3_t krink_vec3_subf(krink_vec3_t vlh, float vrh) {
	return (krink_vec3_t){vlh.x - vrh, vlh.y - vrh, vlh.z - vrh};
}

krink_vec3_t krink_vec3_mult(krink_vec3_t v, float s) {
	return (krink_vec3_t){v.x * s, v.y * s, v.z * s};
}

krink_vec3_t krink_vec3_div(krink_vec3_t v, float s) {
	return krink_vec3_mult(v, 1.0f / s);
}

float krink_vec3_dot(krink_vec3_t vlh, krink_vec3_t vrh) {
	return vlh.x * vrh.x + vlh.y * vrh.y + vlh.z * vrh.z;
}

krink_vec3_t krink_vec3_cross(krink_vec3_t vlh, krink_vec3_t vrh) {
	float x = vlh.y * vrh.z - vlh.z * vrh.y;
	float y = vlh.z * vrh.x - vlh.x * vrh.z;
	float z = vlh.x * vrh.y - vlh.y * vrh.x;
	return (krink_vec3_t){x, y, z};
}

krink_vec3_t krink_vec3_normalized(krink_vec3_t v) {
	krink_vec3_t ret = (krink_vec3_t){v.x, v.y, v.z};
	krink_vec3_set_length(&ret, 1.0f);
	return ret;
}

// Vector4

float krink_vec4_length(krink_vec4_t v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float krink_vec4_set_length(krink_vec4_t *v, float length) {
	float current_length = krink_vec4_length(*v);
	if (current_length == 0) return 0.0f;
	float mul = length / current_length;
	v->x *= mul;
	v->y *= mul;
	v->z *= mul;
	v->w *= mul;
	return length;
}

krink_vec4_t krink_vec4_addv(krink_vec4_t vlh, krink_vec4_t vrh) {
	return (krink_vec4_t){vlh.x + vrh.x, vlh.y + vrh.y, vlh.z + vrh.z, vlh.w + vrh.w};
}

krink_vec4_t krink_vec4_subv(krink_vec4_t vlh, krink_vec4_t vrh) {
	return (krink_vec4_t){vlh.x - vrh.x, vlh.y - vrh.y, vlh.z - vrh.z, vlh.w - vrh.w};
}

krink_vec4_t krink_vec4_addf(krink_vec4_t vlh, float vrh) {
	return (krink_vec4_t){vlh.x + vrh, vlh.y + vrh, vlh.z + vrh, vlh.w + vrh};
}

krink_vec4_t krink_vec4_subf(krink_vec4_t vlh, float vrh) {
	return (krink_vec4_t){vlh.x - vrh, vlh.y - vrh, vlh.z - vrh, vlh.w - vrh};
}

krink_vec4_t krink_vec4_mult(krink_vec4_t v, float s) {
	return (krink_vec4_t){v.x * s, v.y * s, v.z * s, v.w * s};
}

krink_vec4_t krink_vec4_div(krink_vec4_t v, float s) {
	return krink_vec4_mult(v, 1.0f / s);
}

krink_vec4_t krink_vec4_normalized(krink_vec4_t v) {
	krink_vec4_t ret = (krink_vec4_t){v.x, v.y, v.z};
	krink_vec4_set_length(&ret, 1.0f);
	return ret;
}
