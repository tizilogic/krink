#include "coloredpainter.h"
#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <krink/color.h>
#include <krink/math/vector.h>
#include <krink/memory.h>
#include <stdbool.h>
#include <stddef.h>

static kinc_g4_vertex_buffer_t rect_vertex_buffer;
static kinc_g4_index_buffer_t rect_index_buffer;
static kinc_g4_vertex_buffer_t tris_vertex_buffer;
static kinc_g4_index_buffer_t tris_index_buffer;
static kinc_g4_shader_t vert_shader;
static kinc_g4_shader_t frag_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_constant_location_t proj_mat_loc;
static kinc_matrix4x4_t projection_matrix;
static float *rect_verts = NULL;
static float *tris_verts = NULL;
static int rect_buffer_index = 0;
static int rect_buffer_start = 0;
static int tris_buffer_index = 0;
static int tris_buffer_start = 0;

static void csp_rect_end(bool tris_done);
static void csp_tris_end(bool rects_done);

void krink_g2_csp_init(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-colored.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		krink_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-colored.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "vertexPosition", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "vertexColor", KINC_G4_VERTEX_DATA_FLOAT4);
	kinc_g4_pipeline_init(&pipeline);
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.vertex_shader = &vert_shader;
	pipeline.fragment_shader = &frag_shader;
	pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	pipeline.alpha_blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	pipeline.alpha_blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	kinc_g4_pipeline_compile(&pipeline);

	proj_mat_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&rect_vertex_buffer, KRINK_G2_CSP_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	rect_verts = kinc_g4_vertex_buffer_lock_all(&rect_vertex_buffer);

	kinc_g4_index_buffer_init(&rect_index_buffer, KRINK_G2_CSP_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&rect_index_buffer);
	for (int i = 0; i < KRINK_G2_CSP_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&rect_index_buffer);

	kinc_g4_vertex_buffer_init(&tris_vertex_buffer, KRINK_G2_CSP_BUFFER_SIZE * 3, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	tris_verts = kinc_g4_vertex_buffer_lock_all(&tris_vertex_buffer);

	kinc_g4_index_buffer_init(&tris_index_buffer, KRINK_G2_CSP_BUFFER_SIZE * 3,
	                          KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&tris_index_buffer);
	for (int i = 0; i < KRINK_G2_CSP_BUFFER_SIZE; ++i) {
		indices[i * 3 + 0] = i * 3 + 0;
		indices[i * 3 + 1] = i * 3 + 1;
		indices[i * 3 + 2] = i * 3 + 2;
	}
	kinc_g4_index_buffer_unlock(&tris_index_buffer);
}

// Rect Impl

static void csp_rect_set_verts(float btlx, float btly, float tplx, float tply, float tprx,
                               float tpry, float btrx, float btry) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 7 * 4;
	rect_verts[base_idx + 0] = btlx;
	rect_verts[base_idx + 1] = btly;
	rect_verts[base_idx + 2] = -5.0f;

	rect_verts[base_idx + 7] = tplx;
	rect_verts[base_idx + 8] = tply;
	rect_verts[base_idx + 9] = -5.0f;

	rect_verts[base_idx + 14] = tprx;
	rect_verts[base_idx + 15] = tpry;
	rect_verts[base_idx + 16] = -5.0f;

	rect_verts[base_idx + 21] = btrx;
	rect_verts[base_idx + 22] = btry;
	rect_verts[base_idx + 23] = -5.0f;
}

static void csp_rect_set_colors(float opacity, unsigned int color) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 7 * 4;
	float a = opacity * ((float)krink_color_get_channel(color, 'A') / 255.0f);
	float r = ((float)krink_color_get_channel(color, 'R') / 255.0f);
	float g = ((float)krink_color_get_channel(color, 'G') / 255.0f);
	float b = ((float)krink_color_get_channel(color, 'B') / 255.0f);

	rect_verts[base_idx + 3] = r;
	rect_verts[base_idx + 4] = g;
	rect_verts[base_idx + 5] = b;
	rect_verts[base_idx + 6] = a;

	rect_verts[base_idx + 10] = r;
	rect_verts[base_idx + 11] = g;
	rect_verts[base_idx + 12] = b;
	rect_verts[base_idx + 13] = a;

	rect_verts[base_idx + 17] = r;
	rect_verts[base_idx + 18] = g;
	rect_verts[base_idx + 19] = b;
	rect_verts[base_idx + 20] = a;

	rect_verts[base_idx + 24] = r;
	rect_verts[base_idx + 25] = g;
	rect_verts[base_idx + 26] = b;
	rect_verts[base_idx + 27] = a;
}

static void csp_rect_draw_buffer(bool tris_done) {
	if (rect_buffer_index - rect_buffer_start == 0) {
		return;
	}

	if (!tris_done) csp_tris_end(true);

	kinc_g4_vertex_buffer_unlock(&rect_vertex_buffer, (rect_buffer_index - rect_buffer_start) * 4);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&rect_vertex_buffer);
	kinc_g4_set_index_buffer(&rect_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(rect_buffer_start * 2 * 3,
	                                      (rect_buffer_index - rect_buffer_start) * 2 * 3);

	if (rect_buffer_index + 1 >= KRINK_G2_CSP_BUFFER_SIZE) {
		rect_buffer_start = 0;
		rect_buffer_index = 0;
		rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, 0, KRINK_G2_CSP_BUFFER_SIZE * 4);
	}
	else {
		rect_buffer_start = rect_buffer_index;
		rect_verts = kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, rect_buffer_start * 4,
		                                        (KRINK_G2_CSP_BUFFER_SIZE - rect_buffer_start) * 4);
	}
}

// Tris Impl

static void csp_tris_set_verts(float x0, float y0, float x1, float y1, float x2, float y2) {
	int base_idx = (tris_buffer_index - tris_buffer_start) * 7 * 3;
	tris_verts[base_idx + 0] = x0;
	tris_verts[base_idx + 1] = y0;
	tris_verts[base_idx + 2] = -5.0f;

	tris_verts[base_idx + 7] = x1;
	tris_verts[base_idx + 8] = y1;
	tris_verts[base_idx + 9] = -5.0f;

	tris_verts[base_idx + 14] = x2;
	tris_verts[base_idx + 15] = y2;
	tris_verts[base_idx + 16] = -5.0f;
}

static void csp_tris_set_colors(float opacity, unsigned int color) {
	int base_idx = (tris_buffer_index - tris_buffer_start) * 7 * 3;
	float a = opacity * ((float)krink_color_get_channel(color, 'A') / 255.0f);
	float r = ((float)krink_color_get_channel(color, 'R') / 255.0f);
	float g = ((float)krink_color_get_channel(color, 'G') / 255.0f);
	float b = ((float)krink_color_get_channel(color, 'B') / 255.0f);

	tris_verts[base_idx + 3] = r;
	tris_verts[base_idx + 4] = g;
	tris_verts[base_idx + 5] = b;
	tris_verts[base_idx + 6] = a;

	tris_verts[base_idx + 10] = r;
	tris_verts[base_idx + 11] = g;
	tris_verts[base_idx + 12] = b;
	tris_verts[base_idx + 13] = a;

	tris_verts[base_idx + 17] = r;
	tris_verts[base_idx + 18] = g;
	tris_verts[base_idx + 19] = b;
	tris_verts[base_idx + 20] = a;
}

static void csp_tris_draw_buffer(bool rect_done) {
	if (tris_buffer_index - tris_buffer_start == 0) {
		return;
	}

	if (!rect_done) csp_rect_end(true);

	kinc_g4_vertex_buffer_unlock(&tris_vertex_buffer, (tris_buffer_index - tris_buffer_start) * 4);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&tris_vertex_buffer);
	kinc_g4_set_index_buffer(&tris_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(tris_buffer_start * 3,
	                                      (tris_buffer_index - tris_buffer_start) * 3);

	if (tris_buffer_index + 1 >= KRINK_G2_CSP_BUFFER_SIZE) {
		tris_buffer_start = 0;
		tris_buffer_index = 0;
		tris_verts =
		    kinc_g4_vertex_buffer_lock(&tris_vertex_buffer, 0, KRINK_G2_CSP_BUFFER_SIZE * 3);
	}
	else {
		tris_buffer_start = tris_buffer_index;
		tris_verts = kinc_g4_vertex_buffer_lock(&tris_vertex_buffer, tris_buffer_start * 3,
		                                        (KRINK_G2_CSP_BUFFER_SIZE - tris_buffer_start) * 3);
	}
}

void krink_g2_csp_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void krink_g2_csp_fill_rect(float x, float y, float width, float height, unsigned int color,
                            float opacity, kinc_matrix3x3_t transformation) {
	if (tris_buffer_index - tris_buffer_start > 0)
		csp_tris_draw_buffer(true); // Flush other buffer for right render order

	if (rect_buffer_index + 1 >= KRINK_G2_CSP_BUFFER_SIZE) csp_rect_draw_buffer(false);

	csp_rect_set_colors(opacity, color);

	kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x, y + height, 0.0f}); // bottom-left
	kinc_vector3_t p1 =
	    kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){x, y, 0.0f}); // top-left
	kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + width, y, 0.0f}); // top-right
	kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + width, y + height, 0.0f}); // bottom-right
	csp_rect_set_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);

	++rect_buffer_index;
}

void krink_g2_csp_fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
                                unsigned int color, float opacity,
                                kinc_matrix3x3_t transformation) {
	if (rect_buffer_index - rect_buffer_start > 0)
		csp_rect_draw_buffer(true); // Flush other buffer for right render order

	if (tris_buffer_index + 1 >= KRINK_G2_CSP_BUFFER_SIZE) csp_tris_draw_buffer(false);

	csp_tris_set_colors(opacity, color);

	kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x0, y0, 0.0f}); // bottom-left
	kinc_vector3_t p1 =
	    kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){x1, y1, 0.0f}); // top-left
	kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(&transformation,
	                                                   (kinc_vector3_t){x2, y2, 0.0f}); // top-right
	csp_tris_set_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);

	++tris_buffer_index;
}

static void csp_rect_end(bool tris_done) {
	if (rect_buffer_index - rect_buffer_start > 0) csp_rect_draw_buffer(tris_done);
}

static void csp_tris_end(bool rects_done) {
	if (tris_buffer_index - tris_buffer_start > 0) csp_tris_draw_buffer(rects_done);
}

void krink_g2_csp_end(void) {
	csp_rect_end(false);
	csp_tris_end(false);
}
