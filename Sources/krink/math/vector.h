#pragma once

/*! \file vector.h
    \brief Provides basic vector operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kr_vec2 {
	float x;
	float y;
} kr_vec2_t;

float kr_vec2_length(kr_vec2_t v);
float kr_vec2_set_length(kr_vec2_t *v, float length);
kr_vec2_t kr_vec2_addv(kr_vec2_t vlh, kr_vec2_t vrh);
kr_vec2_t kr_vec2_subv(kr_vec2_t vlh, kr_vec2_t vrh);
kr_vec2_t kr_vec2_addf(kr_vec2_t vlh, float vrh);
kr_vec2_t kr_vec2_subf(kr_vec2_t vlh, float vrh);
kr_vec2_t kr_vec2_mult(kr_vec2_t v, float s);
kr_vec2_t kr_vec2_div(kr_vec2_t v, float s);
float kr_vec2_dot(kr_vec2_t vlh, kr_vec2_t vrh);
kr_vec2_t kr_vec2_normalized(kr_vec2_t v);
float kr_vec2_angle(kr_vec2_t vlh, kr_vec2_t vrh);

typedef struct kr_vec3 {
	float x;
	float y;
	float z;
} kr_vec3_t;

float kr_vec3_length(kr_vec3_t v);
float kr_vec3_set_length(kr_vec3_t *v, float length);
kr_vec3_t kr_vec3_addv(kr_vec3_t vlh, kr_vec3_t vrh);
kr_vec3_t kr_vec3_subv(kr_vec3_t vlh, kr_vec3_t vrh);
kr_vec3_t kr_vec3_addf(kr_vec3_t vlh, float vrh);
kr_vec3_t kr_vec3_subf(kr_vec3_t vlh, float vrh);
kr_vec3_t kr_vec3_mult(kr_vec3_t v, float s);
kr_vec3_t kr_vec3_div(kr_vec3_t v, float s);
float kr_vec3_dot(kr_vec3_t vlh, kr_vec3_t vrh);
kr_vec3_t kr_vec3_cross(kr_vec3_t vlh, kr_vec3_t vrh);
kr_vec3_t kr_vec3_normalized(kr_vec3_t v);

typedef struct kr_vec4 {
	float x;
	float y;
	float z;
	float w;
} kr_vec4_t;

float kr_vec4_length(kr_vec4_t v);
float kr_vec4_set_length(kr_vec4_t *v, float length);
kr_vec4_t kr_vec4_addv(kr_vec4_t vlh, kr_vec4_t vrh);
kr_vec4_t kr_vec4_subv(kr_vec4_t vlh, kr_vec4_t vrh);
kr_vec4_t kr_vec4_addf(kr_vec4_t vlh, float vrh);
kr_vec4_t kr_vec4_subf(kr_vec4_t vlh, float vrh);
kr_vec4_t kr_vec4_mult(kr_vec4_t v, float s);
kr_vec4_t kr_vec4_div(kr_vec4_t v, float s);
kr_vec4_t kr_vec4_normalized(kr_vec4_t v);
// TODO: implement if needed
// float kr_vec4_dot(kr_vec4_t vlh, kr_vec4_t vrh);
// kr_vec4_t kr_vec4_cross(kr_vec4_t vlh, kr_vec4_t vrh);

#ifdef __cplusplus
}
#endif
