#pragma once

typedef struct krink_vec2 {
	float x;
	float y;
} krink_vec2_t;

float krink_vec2_length(krink_vec2_t v);
float krink_vec2_set_length(krink_vec2_t *v, float length);
krink_vec2_t krink_vec2_addv(krink_vec2_t vlh, krink_vec2_t vrh);
krink_vec2_t krink_vec2_subv(krink_vec2_t vlh, krink_vec2_t vrh);
krink_vec2_t krink_vec2_addf(krink_vec2_t vlh, float vrh);
krink_vec2_t krink_vec2_subf(krink_vec2_t vlh, float vrh);
krink_vec2_t krink_vec2_mult(krink_vec2_t v, float s);
krink_vec2_t krink_vec2_div(krink_vec2_t v, float s);
float krink_vec2_dot(krink_vec2_t vlh, krink_vec2_t vrh);
krink_vec2_t krink_vec2_normalized(krink_vec2_t v);
float krink_vec2_angle(krink_vec2_t vlh, krink_vec2_t vrh);

typedef struct krink_vec3 {
	float x;
	float y;
	float z;
} krink_vec3_t;

float krink_vec3_length(krink_vec3_t v);
float krink_vec3_set_length(krink_vec3_t *v, float length);
krink_vec3_t krink_vec3_addv(krink_vec3_t vlh, krink_vec3_t vrh);
krink_vec3_t krink_vec3_subv(krink_vec3_t vlh, krink_vec3_t vrh);
krink_vec3_t krink_vec3_addf(krink_vec3_t vlh, float vrh);
krink_vec3_t krink_vec3_subf(krink_vec3_t vlh, float vrh);
krink_vec3_t krink_vec3_mult(krink_vec3_t v, float s);
krink_vec3_t krink_vec3_div(krink_vec3_t v, float s);
float krink_vec3_dot(krink_vec3_t vlh, krink_vec3_t vrh);
krink_vec3_t krink_vec3_cross(krink_vec3_t vlh, krink_vec3_t vrh);
krink_vec3_t krink_vec3_normalized(krink_vec3_t v);

typedef struct krink_vec4 {
	float x;
	float y;
	float z;
	float w;
} krink_vec4_t;

float krink_vec4_length(krink_vec4_t v);
float krink_vec4_set_length(krink_vec4_t *v, float length);
krink_vec4_t krink_vec4_addv(krink_vec4_t vlh, krink_vec4_t vrh);
krink_vec4_t krink_vec4_subv(krink_vec4_t vlh, krink_vec4_t vrh);
krink_vec4_t krink_vec4_addf(krink_vec4_t vlh, float vrh);
krink_vec4_t krink_vec4_subf(krink_vec4_t vlh, float vrh);
krink_vec4_t krink_vec4_mult(krink_vec4_t v, float s);
krink_vec4_t krink_vec4_div(krink_vec4_t v, float s);
krink_vec4_t krink_vec4_normalized(krink_vec4_t v);
// TODO: implement if needed
// float krink_vec4_dot(krink_vec4_t vlh, krink_vec4_t vrh);
// krink_vec4_t krink_vec4_cross(krink_vec4_t vlh, krink_vec4_t vrh);
