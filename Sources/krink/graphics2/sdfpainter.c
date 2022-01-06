#include "sdfpainter.h"

#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <krink/color.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>
#include <krink/memory.h>

static kinc_g4_vertex_buffer_t rect_vertex_buffer;
static kinc_g4_index_buffer_t rect_index_buffer;
static kinc_g4_vertex_buffer_t circle_vertex_buffer;
static kinc_g4_index_buffer_t circle_index_buffer;
static kinc_g4_vertex_buffer_t line_vertex_buffer;
static kinc_g4_index_buffer_t line_index_buffer;
static kinc_g4_shader_t vert_shader;
static kinc_g4_shader_t frag_shader;
static kinc_g4_pipeline_t rect_pipeline;
static kinc_g4_pipeline_t circle_pipeline;
static kinc_g4_pipeline_t line_pipeline;
static kinc_g4_constant_location_t rect_proj_mat_loc;
static kinc_g4_constant_location_t circle_proj_mat_loc;
static kinc_g4_constant_location_t line_proj_mat_loc;
static kinc_matrix4x4_t projection_matrix;
static float *rect_rect_verts = NULL;
static float *circle_rect_verts = NULL;
static float *line_rect_verts = NULL;
static int rect_buffer_index = 0;
static int rect_buffer_start = 0;
static int circle_buffer_index = 0;
static int circle_buffer_start = 0;
static int line_buffer_index = 0;
static int line_buffer_start = 0;

static void sdf_rect_end(void);
static void sdf_circle_end(void);
static void sdf_line_end(void);
static void sdf_rect_draw_buffer(bool end);
static void sdf_circle_draw_buffer(bool end);
static void sdf_line_draw_buffer(bool end);

static float max(float a, float b) {
	return (a > b ? a : b);
}

static void build_rect_pipeline(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-rect.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		kr_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-rect.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "box", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "rCol", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
	kinc_g4_vertex_structure_add(&structure, "bCol", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
	kinc_g4_vertex_structure_add(&structure, "corner", KINC_G4_VERTEX_DATA_FLOAT4);
	kinc_g4_vertex_structure_add(&structure, "border", KINC_G4_VERTEX_DATA_FLOAT1);
	kinc_g4_vertex_structure_add(&structure, "smth", KINC_G4_VERTEX_DATA_FLOAT1);
	kinc_g4_pipeline_init(&rect_pipeline);
	rect_pipeline.input_layout[0] = &structure;
	rect_pipeline.input_layout[1] = NULL;
	rect_pipeline.vertex_shader = &vert_shader;
	rect_pipeline.fragment_shader = &frag_shader;
	rect_pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	rect_pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	rect_pipeline.alpha_blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	rect_pipeline.alpha_blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	kinc_g4_pipeline_compile(&rect_pipeline);

	rect_proj_mat_loc = kinc_g4_pipeline_get_constant_location(&rect_pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&rect_vertex_buffer, KR_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	rect_rect_verts = kinc_g4_vertex_buffer_lock_all(&rect_vertex_buffer);

	kinc_g4_index_buffer_init(&rect_index_buffer, KR_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&rect_index_buffer);
	for (int i = 0; i < KR_G2_SDF_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&rect_index_buffer);
}

static void build_circle_pipeline(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-circle.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		kr_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-circle.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "rCol", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
	kinc_g4_vertex_structure_add(&structure, "bCol", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
	kinc_g4_vertex_structure_add(&structure, "border", KINC_G4_VERTEX_DATA_FLOAT1);
	kinc_g4_vertex_structure_add(&structure, "smth", KINC_G4_VERTEX_DATA_FLOAT1);
	kinc_g4_pipeline_init(&circle_pipeline);
	circle_pipeline.input_layout[0] = &structure;
	circle_pipeline.input_layout[1] = NULL;
	circle_pipeline.vertex_shader = &vert_shader;
	circle_pipeline.fragment_shader = &frag_shader;
	circle_pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	circle_pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	circle_pipeline.alpha_blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	circle_pipeline.alpha_blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	kinc_g4_pipeline_compile(&circle_pipeline);

	circle_proj_mat_loc =
	    kinc_g4_pipeline_get_constant_location(&circle_pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&circle_vertex_buffer, KR_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	circle_rect_verts = kinc_g4_vertex_buffer_lock_all(&circle_vertex_buffer);

	kinc_g4_index_buffer_init(&circle_index_buffer, KR_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&circle_index_buffer);
	for (int i = 0; i < KR_G2_SDF_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&circle_index_buffer);
}

static void build_line_pipeline(void) {
	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-line.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		kr_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-line.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = kr_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "color", KINC_G4_VERTEX_DATA_U8_4X_NORMALIZED);
	kinc_g4_vertex_structure_add(&structure, "dim", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "smth", KINC_G4_VERTEX_DATA_FLOAT1);
	kinc_g4_pipeline_init(&line_pipeline);
	line_pipeline.input_layout[0] = &structure;
	line_pipeline.input_layout[1] = NULL;
	line_pipeline.vertex_shader = &vert_shader;
	line_pipeline.fragment_shader = &frag_shader;
	line_pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	line_pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	line_pipeline.alpha_blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
	line_pipeline.alpha_blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
	kinc_g4_pipeline_compile(&line_pipeline);

	line_proj_mat_loc = kinc_g4_pipeline_get_constant_location(&line_pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&line_vertex_buffer, KR_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	line_rect_verts = kinc_g4_vertex_buffer_lock_all(&line_vertex_buffer);

	kinc_g4_index_buffer_init(&line_index_buffer, KR_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&line_index_buffer);
	for (int i = 0; i < KR_G2_SDF_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&line_index_buffer);
}

void kr_sdf_init(void) {
	build_rect_pipeline();
	build_circle_pipeline();
	build_line_pipeline();
}

// SDF Rect Impl

static void sdf_rect_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx,
                                    float tpry, float btrx, float btry) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	rect_rect_verts[base_idx + 0] = btlx;
	rect_rect_verts[base_idx + 1] = btly;
	rect_rect_verts[base_idx + 2] = -5.0f;

	rect_rect_verts[base_idx + 15] = tplx;
	rect_rect_verts[base_idx + 16] = tply;
	rect_rect_verts[base_idx + 17] = -5.0f;

	rect_rect_verts[base_idx + 30] = tprx;
	rect_rect_verts[base_idx + 31] = tpry;
	rect_rect_verts[base_idx + 32] = -5.0f;

	rect_rect_verts[base_idx + 45] = btrx;
	rect_rect_verts[base_idx + 46] = btry;
	rect_rect_verts[base_idx + 47] = -5.0f;
}

static void sdf_rect_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	rect_rect_verts[base_idx + 3] = left;
	rect_rect_verts[base_idx + 4] = bottom;

	rect_rect_verts[base_idx + 18] = left;
	rect_rect_verts[base_idx + 19] = top;

	rect_rect_verts[base_idx + 33] = right;
	rect_rect_verts[base_idx + 34] = top;

	rect_rect_verts[base_idx + 48] = right;
	rect_rect_verts[base_idx + 49] = bottom;
}

static void sdf_rect_set_rect_box(float u, float v) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	rect_rect_verts[base_idx + 5] = u;
	rect_rect_verts[base_idx + 6] = v;

	rect_rect_verts[base_idx + 20] = u;
	rect_rect_verts[base_idx + 21] = v;

	rect_rect_verts[base_idx + 35] = u;
	rect_rect_verts[base_idx + 36] = v;

	rect_rect_verts[base_idx + 50] = u;
	rect_rect_verts[base_idx + 51] = v;
}

static void sdf_rect_set_rect_colors(float opacity, uint32_t color, uint32_t border_color) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	uint32_t a = kr_color_get_channel(color, 'A');
	a = (uint32_t)((float)a * opacity);
	color = kr_color_set_channel(color, 'A', a);

	a = kr_color_get_channel(border_color, 'A');
	a = (uint32_t)((float)a * opacity);
	border_color = kr_color_set_channel(border_color, 'A', a);

	rect_rect_verts[base_idx + 7] = *(float *)&color;
	;
	rect_rect_verts[base_idx + 8] = *(float *)&border_color;
	;

	rect_rect_verts[base_idx + 22] = *(float *)&color;
	;
	rect_rect_verts[base_idx + 23] = *(float *)&border_color;
	;

	rect_rect_verts[base_idx + 37] = *(float *)&color;
	;
	rect_rect_verts[base_idx + 38] = *(float *)&border_color;
	;

	rect_rect_verts[base_idx + 52] = *(float *)&color;
	;
	rect_rect_verts[base_idx + 53] = *(float *)&border_color;
	;
}

static void sdf_rect_set_rect_corner(kr_sdf_corner_radius_t c) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	rect_rect_verts[base_idx + 9] = c.bottom_right;
	rect_rect_verts[base_idx + 10] = c.top_right;
	rect_rect_verts[base_idx + 11] = c.bottom_left;
	rect_rect_verts[base_idx + 12] = c.top_left;

	rect_rect_verts[base_idx + 24] = c.bottom_right;
	rect_rect_verts[base_idx + 25] = c.top_right;
	rect_rect_verts[base_idx + 26] = c.bottom_left;
	rect_rect_verts[base_idx + 27] = c.top_left;

	rect_rect_verts[base_idx + 39] = c.bottom_right;
	rect_rect_verts[base_idx + 40] = c.top_right;
	rect_rect_verts[base_idx + 41] = c.bottom_left;
	rect_rect_verts[base_idx + 42] = c.top_left;

	rect_rect_verts[base_idx + 54] = c.bottom_right;
	rect_rect_verts[base_idx + 55] = c.top_right;
	rect_rect_verts[base_idx + 56] = c.bottom_left;
	rect_rect_verts[base_idx + 57] = c.top_left;
}

static void sdf_rect_set_border_smooth(float b, float s) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 15 * 4;
	rect_rect_verts[base_idx + 13] = b;
	rect_rect_verts[base_idx + 14] = s;

	rect_rect_verts[base_idx + 28] = b;
	rect_rect_verts[base_idx + 29] = s;

	rect_rect_verts[base_idx + 43] = b;
	rect_rect_verts[base_idx + 44] = s;

	rect_rect_verts[base_idx + 58] = b;
	rect_rect_verts[base_idx + 59] = s;
}

static void sdf_rect_draw_buffer(bool end) {
	if (rect_buffer_index - rect_buffer_start == 0) return;
	if (!end) {
		sdf_circle_end();
		sdf_line_end();
	}
	kinc_g4_vertex_buffer_unlock(&rect_vertex_buffer, (rect_buffer_index - rect_buffer_start) * 4);
	kinc_g4_set_pipeline(&rect_pipeline);
	kinc_g4_set_matrix4(rect_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&rect_vertex_buffer);
	kinc_g4_set_index_buffer(&rect_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(rect_buffer_start * 2 * 3,
	                                      (rect_buffer_index - rect_buffer_start) * 2 * 3);

	if (end || rect_buffer_index + 1 >= KR_G2_SDF_BUFFER_SIZE) {
		rect_buffer_start = 0;
		rect_buffer_index = 0;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, 0, KR_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		rect_buffer_start = rect_buffer_index;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, rect_buffer_start * 4,
		                               (KR_G2_SDF_BUFFER_SIZE - rect_buffer_start) * 4);
	}
}

void kr_sdf_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void kr_sdf_draw_rect(float x, float y, float width, float height, kr_sdf_corner_radius_t corner,
                      float border, float smooth, uint32_t color, uint32_t border_color,
                      float opacity, kr_matrix3x3_t transformation) {
	sdf_circle_draw_buffer(false);
	sdf_line_draw_buffer(false);
	kr_vec2_t p0 = kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x, y + height}); // bottom-left
	kr_vec2_t p1 = kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x, y});          // top-left
	kr_vec2_t p2 = kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x + width, y});  // top-right
	kr_vec2_t p3 =
	    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x + width, y + height}); // bottom-right
	sdf_rect_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
	float w = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p2.x, p2.y}, (kr_vec2_t){p1.x, p1.y}));
	float h = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p0.x, p0.y}, (kr_vec2_t){p1.x, p1.y}));
	float u = w / (w > h ? w : h);
	float v = h / (w > h ? w : h);
	float f = (u >= v ? u / w : v / h) * max(w / width, h / height);

	corner.top_right *= f;
	corner.bottom_right *= f;
	corner.top_left *= f;
	corner.bottom_left *= f;

	sdf_rect_set_rect_colors(opacity, color, border_color);
	sdf_rect_set_rect_tex_coords(0.0f, 0.0f, u, v);
	sdf_rect_set_rect_box(u / 2.0f, v / 2.0f);
	sdf_rect_set_rect_corner(corner);
	sdf_rect_set_border_smooth(border * f, smooth * f);

	++rect_buffer_index;
}

void kr_sdf_draw_rect_symm(float x, float y, float width, float height, float corner, float border,
                           float smooth, uint32_t color, uint32_t border_color, float opacity,
                           kr_matrix3x3_t transformation) {
	kr_sdf_draw_rect(x, y, width, height, (kr_sdf_corner_radius_t){corner, corner, corner, corner},
	                 border, smooth, color, border_color, opacity, transformation);
}

// SDF Circle Impl

static void sdf_circle_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx,
                                      float tpry, float btrx, float btry) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 9 * 4;
	circle_rect_verts[base_idx + 0] = btlx;
	circle_rect_verts[base_idx + 1] = btly;
	circle_rect_verts[base_idx + 2] = -5.0f;

	circle_rect_verts[base_idx + 9] = tplx;
	circle_rect_verts[base_idx + 10] = tply;
	circle_rect_verts[base_idx + 11] = -5.0f;

	circle_rect_verts[base_idx + 18] = tprx;
	circle_rect_verts[base_idx + 19] = tpry;
	circle_rect_verts[base_idx + 20] = -5.0f;

	circle_rect_verts[base_idx + 27] = btrx;
	circle_rect_verts[base_idx + 28] = btry;
	circle_rect_verts[base_idx + 29] = -5.0f;
}

static void sdf_circle_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 9 * 4;
	circle_rect_verts[base_idx + 3] = left;
	circle_rect_verts[base_idx + 4] = bottom;

	circle_rect_verts[base_idx + 12] = left;
	circle_rect_verts[base_idx + 13] = top;

	circle_rect_verts[base_idx + 21] = right;
	circle_rect_verts[base_idx + 22] = top;

	circle_rect_verts[base_idx + 30] = right;
	circle_rect_verts[base_idx + 31] = bottom;
}

static void sdf_circle_set_rect_colors(float opacity, uint32_t color, uint32_t border_color) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 9 * 4;
	uint32_t a = kr_color_get_channel(color, 'A');
	a = (uint32_t)((float)a * opacity);
	color = kr_color_set_channel(color, 'A', a);

	a = kr_color_get_channel(border_color, 'A');
	a = (uint32_t)((float)a * opacity);
	border_color = kr_color_set_channel(border_color, 'A', a);

	circle_rect_verts[base_idx + 5] = *(float *)&color;
	;
	circle_rect_verts[base_idx + 6] = *(float *)&border_color;
	;

	circle_rect_verts[base_idx + 14] = *(float *)&color;
	;
	circle_rect_verts[base_idx + 15] = *(float *)&border_color;
	;

	circle_rect_verts[base_idx + 23] = *(float *)&color;
	;
	circle_rect_verts[base_idx + 24] = *(float *)&border_color;
	;

	circle_rect_verts[base_idx + 32] = *(float *)&color;
	;
	circle_rect_verts[base_idx + 33] = *(float *)&border_color;
	;
}

static void sdf_circle_set_border_smooth(float b, float s) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 9 * 4;
	circle_rect_verts[base_idx + 7] = b;
	circle_rect_verts[base_idx + 8] = s;

	circle_rect_verts[base_idx + 16] = b;
	circle_rect_verts[base_idx + 17] = s;

	circle_rect_verts[base_idx + 25] = b;
	circle_rect_verts[base_idx + 26] = s;

	circle_rect_verts[base_idx + 34] = b;
	circle_rect_verts[base_idx + 35] = s;
}

static void sdf_circle_draw_buffer(bool end) {
	if (circle_buffer_index - circle_buffer_start == 0) return;
	if (!end) {
		sdf_rect_end();
		sdf_line_end();
	}
	kinc_g4_vertex_buffer_unlock(&circle_vertex_buffer,
	                             (circle_buffer_index - circle_buffer_start) * 4);
	kinc_g4_set_pipeline(&circle_pipeline);
	kinc_g4_set_matrix4(circle_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&circle_vertex_buffer);
	kinc_g4_set_index_buffer(&circle_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(circle_buffer_start * 2 * 3,
	                                      (circle_buffer_index - circle_buffer_start) * 2 * 3);

	if (end || circle_buffer_index + 1 >= KR_G2_SDF_BUFFER_SIZE) {
		circle_buffer_start = 0;
		circle_buffer_index = 0;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, 0, KR_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		circle_buffer_start = circle_buffer_index;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, circle_buffer_start * 4,
		                               (KR_G2_SDF_BUFFER_SIZE - circle_buffer_start) * 4);
	}
}

void kr_sdf_draw_circle(float x, float y, float radius, float border, float smooth, uint32_t color,
                        uint32_t border_color, float opacity, kr_matrix3x3_t transformation) {
	sdf_rect_draw_buffer(false);
	sdf_line_draw_buffer(false);
	kr_vec2_t p0 =
	    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x - radius, y + radius}); // bottom-left
	kr_vec2_t p1 =
	    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x - radius, y - radius}); // top-left
	kr_vec2_t p2 =
	    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x + radius, y - radius}); // top-right
	kr_vec2_t p3 =
	    kr_matrix3x3_multvec(&transformation, (kr_vec2_t){x + radius, y + radius}); // bottom-right
	sdf_circle_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
	float w = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p2.x, p2.y}, (kr_vec2_t){p1.x, p1.y}));
	float h = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p0.x, p0.y}, (kr_vec2_t){p1.x, p1.y}));
	float u = w / (w > h ? w : h);
	float v = h / (w > h ? w : h);
	float f = (u >= v ? u / w : v / h) * max(w / (2 * radius), h / (2 * radius));

	sdf_circle_set_rect_colors(opacity, color, border_color);
	sdf_circle_set_rect_tex_coords(0.0f, 0.0f, 1.0f, 1.0f);
	sdf_circle_set_border_smooth(border * f, smooth * f);

	++circle_buffer_index;
}

// SDF Line Impl

static void sdf_line_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx,
                                    float tpry, float btrx, float btry) {
	int base_idx = (line_buffer_index - line_buffer_start) * 9 * 4;
	line_rect_verts[base_idx + 0] = btlx;
	line_rect_verts[base_idx + 1] = btly;
	line_rect_verts[base_idx + 2] = -5.0f;

	line_rect_verts[base_idx + 9] = tplx;
	line_rect_verts[base_idx + 10] = tply;
	line_rect_verts[base_idx + 11] = -5.0f;

	line_rect_verts[base_idx + 18] = tprx;
	line_rect_verts[base_idx + 19] = tpry;
	line_rect_verts[base_idx + 20] = -5.0f;

	line_rect_verts[base_idx + 27] = btrx;
	line_rect_verts[base_idx + 28] = btry;
	line_rect_verts[base_idx + 29] = -5.0f;
}

static void sdf_line_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (line_buffer_index - line_buffer_start) * 9 * 4;
	line_rect_verts[base_idx + 3] = left;
	line_rect_verts[base_idx + 4] = bottom;

	line_rect_verts[base_idx + 12] = left;
	line_rect_verts[base_idx + 13] = top;

	line_rect_verts[base_idx + 21] = right;
	line_rect_verts[base_idx + 22] = top;

	line_rect_verts[base_idx + 30] = right;
	line_rect_verts[base_idx + 31] = bottom;
}

static void sdf_line_set_rect_colors(float opacity, uint32_t color) {
	int base_idx = (line_buffer_index - line_buffer_start) * 9 * 4;

	uint32_t a = kr_color_get_channel(color, 'A');
	a = (uint32_t)((float)a * opacity);
	color = kr_color_set_channel(color, 'A', a);

	line_rect_verts[base_idx + 5] = *(float *)&color;
	;
	line_rect_verts[base_idx + 14] = *(float *)&color;
	;
	line_rect_verts[base_idx + 23] = *(float *)&color;
	;
	line_rect_verts[base_idx + 32] = *(float *)&color;
	;
}

static void sdf_line_set_dim(float u, float v) {
	int base_idx = (line_buffer_index - line_buffer_start) * 9 * 4;
	line_rect_verts[base_idx + 6] = u;
	line_rect_verts[base_idx + 7] = v;

	line_rect_verts[base_idx + 15] = u;
	line_rect_verts[base_idx + 16] = v;

	line_rect_verts[base_idx + 24] = u;
	line_rect_verts[base_idx + 25] = v;

	line_rect_verts[base_idx + 33] = u;
	line_rect_verts[base_idx + 34] = v;
}

static void sdf_line_set_smooth(float s) {
	int base_idx = (line_buffer_index - line_buffer_start) * 9 * 4;
	line_rect_verts[base_idx + 8] = s;

	line_rect_verts[base_idx + 17] = s;

	line_rect_verts[base_idx + 26] = s;

	line_rect_verts[base_idx + 35] = s;
}

static void sdf_line_draw_buffer(bool end) {
	if (line_buffer_index - line_buffer_start == 0) return;
	if (!end) {
		sdf_circle_end();
		sdf_rect_end();
	}
	kinc_g4_vertex_buffer_unlock(&line_vertex_buffer, (line_buffer_index - line_buffer_start) * 4);
	kinc_g4_set_pipeline(&line_pipeline);
	kinc_g4_set_matrix4(line_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&line_vertex_buffer);
	kinc_g4_set_index_buffer(&line_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(line_buffer_start * 2 * 3,
	                                      (line_buffer_index - line_buffer_start) * 2 * 3);

	if (end || line_buffer_index + 1 >= KR_G2_SDF_BUFFER_SIZE) {
		line_buffer_start = 0;
		line_buffer_index = 0;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, 0, KR_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		line_buffer_start = line_buffer_index;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, line_buffer_start * 4,
		                               (KR_G2_SDF_BUFFER_SIZE - line_buffer_start) * 4);
	}
}

static kr_vec2_t get_corner_vec(kr_vec2_t a, kr_vec2_t d0, kr_vec2_t d1) {
	a = kr_vec2_addv(kr_vec2_addv(a, d0), d1);
	return (kr_vec2_t){a.x, a.y};
}

void kr_sdf_draw_line(float x0, float y0, float x1, float y1, float strength, float smooth,
                      uint32_t color, float opacity, kr_matrix3x3_t transformation) {
	sdf_circle_draw_buffer(false);
	sdf_rect_draw_buffer(false);
	kr_vec2_t a = x0 <= x1 ? ((kr_vec2_t){x0, y0}) : ((kr_vec2_t){x1, y1});
	kr_vec2_t b = x0 <= x1 ? ((kr_vec2_t){x1, y1}) : ((kr_vec2_t){x0, y0});
	kr_vec2_t fw = kr_vec2_normalized(kr_vec2_subv(b, a));
	kr_vec2_t bw = kr_vec2_mult(fw, -1.0f);
	kr_vec2_t up = (kr_vec2_t){fw.y, -fw.x};
	kr_vec2_t down = kr_vec2_mult(up, -1.0f);
	float hs = strength / 2.0;
	fw = kr_vec2_mult(fw, hs);
	bw = kr_vec2_mult(bw, hs);
	up = kr_vec2_mult(up, hs);
	down = kr_vec2_mult(down, hs);

	kr_vec2_t p0 =
	    kr_matrix3x3_multvec(&transformation, get_corner_vec(a, down, bw));          // bottom-left
	kr_vec2_t p1 = kr_matrix3x3_multvec(&transformation, get_corner_vec(a, up, bw)); // top-left
	kr_vec2_t p2 = kr_matrix3x3_multvec(&transformation, get_corner_vec(b, up, fw)); // top-right
	kr_vec2_t p3 =
	    kr_matrix3x3_multvec(&transformation, get_corner_vec(b, down, fw)); // bottom-right

	float w = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p2.x, p2.y}, (kr_vec2_t){p1.x, p1.y}));
	float h = kr_vec2_length(kr_vec2_subv((kr_vec2_t){p0.x, p0.y}, (kr_vec2_t){p1.x, p1.y}));
	float u = w / (w > h ? w : h);
	float v = h / (w > h ? w : h);
	float wd = kr_vec2_length(kr_vec2_subv(a, b));
	float f = (u >= v ? u / w : v / h) * max(w / wd, h / strength);

	sdf_line_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
	sdf_line_set_rect_colors(opacity, color);
	sdf_line_set_rect_tex_coords(0.0f, 0.0f, u, v);
	sdf_line_set_dim(u / 2.0f, v / 2.0);
	sdf_line_set_smooth(smooth * f);

	++line_buffer_index;
}

static void sdf_rect_end(void) {
	if (rect_buffer_index - rect_buffer_start > 0) sdf_rect_draw_buffer(true);
}

static void sdf_circle_end(void) {
	if (circle_buffer_index - circle_buffer_start > 0) sdf_circle_draw_buffer(true);
}

static void sdf_line_end(void) {
	if (line_buffer_index - line_buffer_start > 0) sdf_line_draw_buffer(true);
}

void kr_sdf_end(void) {
	sdf_rect_end();
	sdf_circle_end();
	sdf_line_end();
}
