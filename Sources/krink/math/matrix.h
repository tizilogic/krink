#pragma once

#include "vector.h"

typedef struct krink_matrix3x3 {
	float m00, m10, m20, m01, m11, m21, m02, m12, m22;
} krink_matrix3x3_t;

krink_matrix3x3_t krink_matrix3x3_translation(float x, float y);
krink_matrix3x3_t krink_matrix3x3_empty(void);
krink_matrix3x3_t krink_matrix3x3_identity(void);
krink_matrix3x3_t krink_matrix3x3_scale(float x, float y);
krink_matrix3x3_t krink_matrix3x3_rotation(float alpha);
krink_matrix3x3_t krink_matrix3x3_add(krink_matrix3x3_t mlh, krink_matrix3x3_t mrh);
krink_matrix3x3_t krink_matrix3x3_sub(krink_matrix3x3_t mlh, krink_matrix3x3_t mrh);
krink_matrix3x3_t krink_matrix3x3_mult(krink_matrix3x3_t m, float value);
krink_matrix3x3_t krink_matrix3x3_transpose(krink_matrix3x3_t m);
float krink_matrix3x3_trace(krink_matrix3x3_t m);
krink_matrix3x3_t krink_matrix3x3_multmat(krink_matrix3x3_t mlh, krink_matrix3x3_t mrh);
krink_vec2_t krink_matrix3x3_multvec(krink_matrix3x3_t m, krink_vec2_t v);
float krink_matrix3x3_cofactor(float m0, float m1, float m2, float m3);
float krink_matrix3x3_determinant(krink_matrix3x3_t m);
krink_matrix3x3_t krink_matrix3x3_inverse(krink_matrix3x3_t m);

typedef struct krink_matrix4x4 {
	float m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33;
} krink_matrix4x4_t;

krink_matrix4x4_t krink_matrix4x4_translation(float x, float y, float z);
krink_matrix4x4_t krink_matrix4x4_empty(void);
krink_matrix4x4_t krink_matrix4x4_identity(void);
krink_matrix4x4_t krink_matrix4x4_scale(float x, float y, float z);
krink_matrix4x4_t krink_matrix4x4_rotation_x(float alpha);
krink_matrix4x4_t krink_matrix4x4_rotation_y(float alpha);
krink_matrix4x4_t krink_matrix4x4_rotation_z(float alpha);
krink_matrix4x4_t krink_matrix4x4_rotation(float yaw, float pitch, float roll);
krink_matrix4x4_t krink_matrix4x4_orthogonal_projection(float left, float right, float bottom, float top, float zn, float zf);
krink_matrix4x4_t krink_matrix4x4_perspective_projection(float fovY, float aspect, float zn, float zf);
krink_matrix4x4_t krink_matrix4x4_look_at(krink_vec3_t eye, krink_vec3_t at, krink_vec3_t up);
krink_matrix4x4_t krink_matrix4x4_add(krink_matrix4x4_t mlh, krink_matrix4x4_t mrh);
krink_matrix4x4_t krink_matrix4x4_sub(krink_matrix4x4_t mlh, krink_matrix4x4_t mrh);
krink_matrix4x4_t krink_matrix4x4_mult(krink_matrix4x4_t m, float value);
krink_matrix4x4_t krink_matrix4x4_transpose(krink_matrix4x4_t m);
krink_matrix4x4_t krink_matrix4x4_transpose3x3(krink_matrix4x4_t m);
float krink_matrix4x4_trace(krink_matrix4x4_t m);
krink_matrix4x4_t krink_matrix4x4_multmat(krink_matrix4x4_t mlh, krink_matrix4x4_t mrh);
krink_vec4_t krink_matrix4x4_multvec(krink_matrix4x4_t m, krink_vec4_t v);
float krink_matrix4x4_cofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8);
float krink_matrix4x4_determinant(krink_matrix4x4_t m);
krink_matrix4x4_t krink_matrix4x4_inverse(krink_matrix4x4_t m);
