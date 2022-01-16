#include "textpainter.h"

#include <assert.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/textureunit.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <krink/color.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>
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
static int buffer_start = 0;

static kr_ttf_font_t *active_font = NULL;
static bool bilinear_filter = false;
static int font_size = 0;

void kr_tsp_init(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-text.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		kr_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-text.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "vertexPosition", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "texPosition", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "vertexColor", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
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

	kinc_g4_vertex_buffer_init(&vertex_buffer, KR_G2_TSP_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	rect_verts = kinc_g4_vertex_buffer_lock_all(&vertex_buffer);

	kinc_g4_index_buffer_init(&index_buffer, KR_G2_TSP_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&index_buffer);
	for (int i = 0; i < KR_G2_TSP_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&index_buffer);
}

void kr_tsp_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry,
                           float btrx, float btry) {
	int base_idx = (buffer_index - buffer_start) * 6 * 4;
	rect_verts[base_idx + 0] = btlx;
	rect_verts[base_idx + 1] = btly;
	rect_verts[base_idx + 2] = -5.0f;

	rect_verts[base_idx + 6] = tplx;
	rect_verts[base_idx + 7] = tply;
	rect_verts[base_idx + 8] = -5.0f;

	rect_verts[base_idx + 12] = tprx;
	rect_verts[base_idx + 13] = tpry;
	rect_verts[base_idx + 14] = -5.0f;

	rect_verts[base_idx + 18] = btrx;
	rect_verts[base_idx + 19] = btry;
	rect_verts[base_idx + 20] = -5.0f;
}

void kr_tsp_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (buffer_index - buffer_start) * 6 * 4;
	rect_verts[base_idx + 3] = left;
	rect_verts[base_idx + 4] = bottom;

	rect_verts[base_idx + 9] = left;
	rect_verts[base_idx + 10] = top;

	rect_verts[base_idx + 15] = right;
	rect_verts[base_idx + 16] = top;

	rect_verts[base_idx + 21] = right;
	rect_verts[base_idx + 22] = bottom;
}

void kr_tsp_set_rect_colors(float opacity, uint32_t color) {
	int base_idx = (buffer_index - buffer_start) * 6 * 4;

	uint32_t a = kr_color_get_channel(color, 'A');
	a = (uint32_t)((float)a * opacity);
	color = kr_color_set_channel(color, 'A', a);

	rect_verts[base_idx + 5] = *(float *)&color;
	;
	rect_verts[base_idx + 11] = *(float *)&color;
	;
	rect_verts[base_idx + 17] = *(float *)&color;
	;
	rect_verts[base_idx + 23] = *(float *)&color;
	;
}

void kr_tsp_draw_buffer(bool end) {
	if (buffer_index - buffer_start == 0) return;
	kinc_g4_vertex_buffer_unlock(&vertex_buffer, buffer_index * 4);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&vertex_buffer);
	kinc_g4_set_index_buffer(&index_buffer);
	kinc_g4_set_texture(texunit, last_texture);
	kinc_g4_set_texture_addressing(texunit, KINC_G4_TEXTURE_ADDRESSING_CLAMP,
	                               KINC_G4_TEXTURE_ADDRESSING_CLAMP);
	kinc_g4_set_texture_mipmap_filter(texunit, KINC_G4_MIPMAP_FILTER_NONE);
	kinc_g4_set_texture_minification_filter(
	    texunit, bilinear_filter ? KINC_G4_TEXTURE_FILTER_LINEAR : KINC_G4_TEXTURE_FILTER_POINT);
	kinc_g4_set_texture_magnification_filter(
	    texunit, bilinear_filter ? KINC_G4_TEXTURE_FILTER_LINEAR : KINC_G4_TEXTURE_FILTER_POINT);
	kinc_g4_draw_indexed_vertices_from_to(buffer_start * 2 * 3,
	                                      (buffer_index - buffer_start) * 2 * 3);

	// kinc_g4_set_texture(texunit, NULL);
	if (end || buffer_index + 1 >= KR_G2_TSP_BUFFER_SIZE) {
		buffer_start = 0;
		buffer_index = 0;
		rect_verts = kinc_g4_vertex_buffer_lock(&vertex_buffer, 0, KR_G2_TSP_BUFFER_SIZE * 4);
	}
	else {
		buffer_start = buffer_index;
		rect_verts = kinc_g4_vertex_buffer_lock(&vertex_buffer, buffer_start * 4,
		                                        (KR_G2_TSP_BUFFER_SIZE - buffer_start) * 4);
	}
}

void kr_tsp_set_bilinear_filter(bool bilinear) {
	kr_tsp_end();
	bilinear_filter = bilinear;
}

void kr_tsp_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void kr_tsp_set_font(kr_ttf_font_t *font) {
	active_font = font;
}

void kr_tsp_set_font_size(int size) {
	assert(active_font != NULL);
	kr_ttf_load(active_font, size);
	font_size = size;
}

void kr_tsp_draw_string(const char *text, float opacity, uint32_t color, float x, float y,
                        kr_matrix3x3_t transformation) {
	kinc_g4_texture_t *tex = kr_ttf_get_texture(active_font, font_size);

	if (last_texture != NULL && tex != last_texture) kr_tsp_draw_buffer(false);
	last_texture = tex;

	float xpos = x;
	float ypos = y;
	kr_ttf_aligned_quad_t q;
	for (int i = 0; text[i] != 0; ++i) {
		int char_code = (unsigned int)text[i];
		if (kr_ttf_get_baked_quad(active_font, font_size, &q, char_code, xpos, ypos)) {
			if (buffer_index + 1 >= KR_G2_TSP_BUFFER_SIZE) kr_tsp_draw_buffer(false);
			kr_tsp_set_rect_colors(opacity, color);
			kr_tsp_set_rect_tex_coords(q.s0, q.t0, q.s1, q.t1);

			kr_vec2_t p[4];
			kr_matrix3x3_multquad(&transformation,
			                      (kr_quad_t){q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0}, p);
			kr_tsp_set_rect_verts(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
			xpos += q.xadvance;
			++buffer_index;
		}
	}
}

void kr_tsp_draw_characters(int *text, int start, int length, float opacity, uint32_t color,
                            float x, float y, kr_matrix3x3_t transformation) {
	kinc_g4_texture_t *tex = kr_ttf_get_texture(active_font, font_size);

	if (last_texture != NULL && tex != last_texture) kr_tsp_draw_buffer(false);
	last_texture = tex;

	float xpos = x;
	float ypos = y;
	kr_ttf_aligned_quad_t q;
	for (int i = start; i < start + length; ++i) {
		if (kr_ttf_get_baked_quad(active_font, font_size, &q, text[i], xpos, ypos)) {
			if (buffer_index + 1 >= KR_G2_TSP_BUFFER_SIZE) kr_tsp_draw_buffer(false);
			kr_tsp_set_rect_colors(opacity, color);
			kr_tsp_set_rect_tex_coords(q.s0, q.t0, q.s1, q.t1);
			kr_vec2_t p0 =
			    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){q.x0, q.y1}); // bottom-left
			kr_vec2_t p1 =
			    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){q.x0, q.y0}); // top-left
			kr_vec2_t p2 =
			    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){q.x1, q.y0}); // top-right
			kr_vec2_t p3 =
			    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){q.x1, q.y1}); // bottom-right
			kr_tsp_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
			xpos += q.xadvance;
			++buffer_index;
		}
	}
}

void kr_tsp_end(void) {
	if (buffer_index > 0) kr_tsp_draw_buffer(true);
	last_texture = NULL;
}
