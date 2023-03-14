#pragma once

#include "vector.h"
#include <kinc/math/matrix.h>

/*! \file matrix.h
    \brief Provides basic matrix operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kr_matrix3x3 {
	float m00, m10, m20, m01, m11, m21, m02, m12, m22;
} kr_matrix3x3_t;

typedef struct kr_quad {
	float x, y, w, h;
} kr_quad_t;

kr_matrix3x3_t kr_matrix3x3_translation(float x, float y);
kr_matrix3x3_t kr_matrix3x3_empty(void);
kr_matrix3x3_t kr_matrix3x3_identity(void);
kr_matrix3x3_t kr_matrix3x3_scale(float x, float y);
kr_matrix3x3_t kr_matrix3x3_rotation(float alpha);
kr_matrix3x3_t kr_matrix3x3_add(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh);
kr_matrix3x3_t kr_matrix3x3_sub(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh);
kr_matrix3x3_t kr_matrix3x3_mult(kr_matrix3x3_t *m, float value);
kr_matrix3x3_t kr_matrix3x3_transpose(kr_matrix3x3_t *m);
float kr_matrix3x3_trace(kr_matrix3x3_t *m);
kr_matrix3x3_t kr_matrix3x3_multmat(kr_matrix3x3_t *mlh, kr_matrix3x3_t *mrh);
kr_vec2_t kr_matrix3x3_multvec(kr_matrix3x3_t *m, kr_vec2_t v);
void kr_matrix3x3_multquad(kr_matrix3x3_t *m, kr_quad_t q, kr_vec2_t *out);
float kr_matrix3x3_cofactor(float m0, float m1, float m2, float m3);
float kr_matrix3x3_determinant(kr_matrix3x3_t *m);
kr_matrix3x3_t kr_matrix3x3_inverse(kr_matrix3x3_t *m);
kinc_matrix3x3_t kr_matrix3x3_to_kinc(kr_matrix3x3_t *m);

typedef struct kr_matrix4x4 {
	float m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33;
} kr_matrix4x4_t;

kr_matrix4x4_t kr_matrix4x4_translation(float x, float y, float z);
kr_matrix4x4_t kr_matrix4x4_empty(void);
kr_matrix4x4_t kr_matrix4x4_identity(void);
kr_matrix4x4_t kr_matrix4x4_scale(float x, float y, float z);
kr_matrix4x4_t kr_matrix4x4_rotation_x(float alpha);
kr_matrix4x4_t kr_matrix4x4_rotation_y(float alpha);
kr_matrix4x4_t kr_matrix4x4_rotation_z(float alpha);
kr_matrix4x4_t kr_matrix4x4_rotation(float yaw, float pitch, float roll);
kr_matrix4x4_t kr_matrix4x4_orthogonal_projection(float left, float right, float bottom, float top,
                                                  float zn, float zf);
kr_matrix4x4_t kr_matrix4x4_perspective_projection(float fovY, float aspect, float zn, float zf);
kr_matrix4x4_t kr_matrix4x4_look_at(kr_vec3_t eye, kr_vec3_t at, kr_vec3_t up);
kr_matrix4x4_t kr_matrix4x4_add(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh);
kr_matrix4x4_t kr_matrix4x4_sub(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh);
kr_matrix4x4_t kr_matrix4x4_mult(kr_matrix4x4_t *m, float value);
kr_matrix4x4_t kr_matrix4x4_transpose(kr_matrix4x4_t *m);
kr_matrix4x4_t kr_matrix4x4_transpose3x3(kr_matrix4x4_t *m);
float kr_matrix4x4_trace(kr_matrix4x4_t *m);
kr_matrix4x4_t kr_matrix4x4_multmat(kr_matrix4x4_t *mlh, kr_matrix4x4_t *mrh);
kr_vec4_t kr_matrix4x4_multvec(kr_matrix4x4_t *m, kr_vec4_t v);
float kr_matrix4x4_cofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6,
                            float m7, float m8);
float kr_matrix4x4_determinant(kr_matrix4x4_t *m);
kr_matrix4x4_t kr_matrix4x4_inverse(kr_matrix4x4_t *m);
kinc_matrix4x4_t kr_matrix4x4_to_kinc(kr_matrix4x4_t *m);

#ifdef __cplusplus
}
#endif
