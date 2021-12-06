#include "textpainter.h"

#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/textureunit.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <krink/memory.h>

static kinc_g4_vertex_buffer_t vertex_buffer;
static kinc_g4_index_buffer_t index_buffer;
static kinc_g4_shader_t vert_shader;
static kinc_g4_shader_t frag_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_texture_unit_t texunit;
static kinc_g4_texture_t *last_texture = NULL;
static kinc_g4_constant_location_t proj_mat_loc;
static kinc_matrix4x4_t projection_matrix;
static float *rect_verts = NULL;
static int buffer_index = 0;

static krink_ttf_font_t *active_font = NULL;
static bool bilinear_filter = false;
static int font_size;

void krink_g2_tsp_init(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-text.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		krink_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-text.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "vertexPosition", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "texPosition", KINC_G4_VERTEX_DATA_FLOAT2);
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

	texunit = kinc_g4_pipeline_get_texture_unit(&pipeline, "tex");
	proj_mat_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&vertex_buffer, KRINK_G2_BUFFER_SIZE * 4, &structure, KINC_G4_USAGE_DYNAMIC, 0);
	rect_verts = kinc_g4_vertex_buffer_lock_all(&vertex_buffer);

	kinc_g4_index_buffer_init(&index_buffer, KRINK_G2_BUFFER_SIZE * 3 * 2, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&index_buffer);
	for (int i = 0; i < KRINK_G2_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&index_buffer);
}

void krink_g2_tsp_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry, float btrx, float btry) {
	int base_idx = buffer_index * 9 * 4;
	rect_verts[base_idx + 0] = btlx;
	rect_verts[base_idx + 1] = btly;
	rect_verts[base_idx + 2] = -5.0f;

	rect_verts[base_idx + 9] = tplx;
	rect_verts[base_idx + 10] = tply;
	rect_verts[base_idx + 11] = -5.0f;

	rect_verts[base_idx + 18] = tprx;
	rect_verts[base_idx + 19] = tpry;
	rect_verts[base_idx + 20] = -5.0f;

	rect_verts[base_idx + 27] = btrx;
	rect_verts[base_idx + 28] = btry;
	rect_verts[base_idx + 29] = -5.0f;
}

void krink_g2_tsp_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = buffer_index * 9 * 4;
	rect_verts[base_idx + 3] = left;
	rect_verts[base_idx + 4] = bottom;

	rect_verts[base_idx + 12] = left;
	rect_verts[base_idx + 13] = top;

	rect_verts[base_idx + 21] = right;
	rect_verts[base_idx + 22] = top;

	rect_verts[base_idx + 30] = right;
	rect_verts[base_idx + 31] = bottom;
}

static unsigned int krink_get_color_channel_internal(unsigned int color, const char ch) {
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

void krink_g2_tsp_set_rect_colors(float opacity, unsigned int color) {
	int base_idx = buffer_index * 9 * 4;
	float a = opacity * ((float)krink_get_color_channel_internal(color, 'A') / 255.0f);
	float r = ((float)krink_get_color_channel_internal(color, 'R') / 255.0f);
	float g = ((float)krink_get_color_channel_internal(color, 'G') / 255.0f);
	float b = ((float)krink_get_color_channel_internal(color, 'B') / 255.0f);

	rect_verts[base_idx + 5] = r;
	rect_verts[base_idx + 6] = g;
	rect_verts[base_idx + 7] = b;
	rect_verts[base_idx + 8] = a;

	rect_verts[base_idx + 14] = r;
	rect_verts[base_idx + 15] = g;
	rect_verts[base_idx + 16] = b;
	rect_verts[base_idx + 17] = a;

	rect_verts[base_idx + 23] = r;
	rect_verts[base_idx + 24] = g;
	rect_verts[base_idx + 25] = b;
	rect_verts[base_idx + 26] = a;

	rect_verts[base_idx + 32] = r;
	rect_verts[base_idx + 33] = g;
	rect_verts[base_idx + 34] = b;
	rect_verts[base_idx + 35] = a;
}

void krink_g2_tsp_draw_buffer(void) {
	if (buffer_index == 0) return;
	kinc_g4_vertex_buffer_unlock(&vertex_buffer, buffer_index * 4);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&vertex_buffer);
	kinc_g4_set_index_buffer(&index_buffer);
	kinc_g4_set_texture(texunit, &last_texture);
	kinc_g4_set_texture_addressing(texunit, KINC_G4_TEXTURE_ADDRESSING_CLAMP, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
	kinc_g4_set_texture_mipmap_filter(texunit, KINC_G4_MIPMAP_FILTER_NONE);
	kinc_g4_set_texture_minification_filter(texunit, bilinear_filter ? KINC_G4_TEXTURE_FILTER_LINEAR : KINC_G4_TEXTURE_FILTER_POINT);
	kinc_g4_set_texture_magnification_filter(texunit, bilinear_filter ? KINC_G4_TEXTURE_FILTER_LINEAR : KINC_G4_TEXTURE_FILTER_POINT);
	kinc_g4_draw_indexed_vertices_from_to(0, buffer_index * 4);

	kinc_g4_set_texture(texunit, NULL);
	buffer_index = 0;
	rect_verts = kinc_g4_vertex_buffer_lock_all(&vertex_buffer);
}

void krink_g2_tsp_set_bilinear_filter(bool bilinear) {
	krink_g2_tsp_end();
	bilinear_filter = bilinear;
}

void krink_g2_tsp_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void krink_g2_tsp_set_font(krink_ttf_font_t *font) {
	active_font = font;
}

void krink_g2_tsp_set_font_size(int size) {
	font_size = size;
}

void krink_g2_tsp_draw_string(const char *text, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation) {
	kinc_g4_texture_t *tex = krink_ttf_get_texture(active_font, font_size);

	if (last_texture != NULL && tex != last_texture) krink_g2_tsp_draw_buffer();
	last_texture = tex;

	float xpos = x;
	float ypos = y;
	krink_ttf_aligned_quad_t q;
	for (int i = 0; text[i] != 0; ++i) {
		int char_code = (unsigned int)text[i];
		if (krink_ttf_get_baked_quad(active_font, font_size, &q, char_code, xpos, ypos)) {
			if (buffer_index + 1 >= KRINK_G2_BUFFER_SIZE) krink_g2_tsp_draw_buffer();
			krink_g2_tsp_set_rect_colors(opacity, color);
			krink_g2_tsp_set_rect_tex_coords(q.s0, q.t0, q.s1, q.t1);
			kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x0, q.y1, 0.0f}); // bottom-left
			kinc_vector3_t p1 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x0, q.y0, 0.0f}); // top-left
			kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x1, q.y0, 0.0f}); // top-right
			kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x1, q.y1, 0.0f}); // bottom-right
			krink_g2_tsp_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
			xpos += q.xadvance;
			++buffer_index;
		}
	}
}

void krink_g2_tsp_draw_characters(int *text, int start, int length, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation) {
	kinc_g4_texture_t *tex = krink_ttf_get_texture(active_font, font_size);

	if (last_texture != NULL && tex != last_texture) krink_g2_tsp_draw_buffer();
	last_texture = tex;

	float xpos = x;
	float ypos = y;
	krink_ttf_aligned_quad_t q;
	for (int i = start; i < start + length; ++i) {
		if (krink_ttf_get_baked_quad(active_font, font_size, &q, text[i], xpos, ypos)) {
			if (buffer_index + 1 >= KRINK_G2_BUFFER_SIZE) krink_g2_tsp_draw_buffer();
			krink_g2_tsp_set_rect_colors(opacity, color);
			krink_g2_tsp_set_rect_tex_coords(q.s0, q.t0, q.s1, q.t1);
			kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x0, q.y1, 0.0f}); // bottom-left
			kinc_vector3_t p1 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x0, q.y0, 0.0f}); // top-left
			kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x1, q.y0, 0.0f}); // top-right
			kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){q.x1, q.y1, 0.0f}); // bottom-right
			krink_g2_tsp_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
			xpos += q.xadvance;
			++buffer_index;
		}
	}
}

void krink_g2_tsp_end(void) {
	if (buffer_index > 0) krink_g2_tsp_draw_buffer();
	last_texture = NULL;
}
