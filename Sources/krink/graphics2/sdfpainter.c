#include "sdfpainter.h"

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
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		krink_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-rect.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "box", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "rCol", KINC_G4_VERTEX_DATA_FLOAT4);
	kinc_g4_vertex_structure_add(&structure, "bCol", KINC_G4_VERTEX_DATA_FLOAT3);
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

	kinc_g4_vertex_buffer_init(&rect_vertex_buffer, KRINK_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	rect_rect_verts = kinc_g4_vertex_buffer_lock_all(&rect_vertex_buffer);

	kinc_g4_index_buffer_init(&rect_index_buffer, KRINK_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&rect_index_buffer);
	for (int i = 0; i < KRINK_G2_SDF_BUFFER_SIZE; ++i) {
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
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		krink_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-circle.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "rCol", KINC_G4_VERTEX_DATA_FLOAT4);
	kinc_g4_vertex_structure_add(&structure, "bCol", KINC_G4_VERTEX_DATA_FLOAT3);
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

	kinc_g4_vertex_buffer_init(&circle_vertex_buffer, KRINK_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	circle_rect_verts = kinc_g4_vertex_buffer_lock_all(&circle_vertex_buffer);

	kinc_g4_index_buffer_init(&circle_index_buffer, KRINK_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&circle_index_buffer);
	for (int i = 0; i < KRINK_G2_SDF_BUFFER_SIZE; ++i) {
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
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vert_shader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
		krink_free(data);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "painter-sdf-line.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = krink_malloc(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&frag_shader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}
	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(&structure, "color", KINC_G4_VERTEX_DATA_FLOAT4);
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

	kinc_g4_vertex_buffer_init(&line_vertex_buffer, KRINK_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	line_rect_verts = kinc_g4_vertex_buffer_lock_all(&line_vertex_buffer);

	kinc_g4_index_buffer_init(&line_index_buffer, KRINK_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *indices = kinc_g4_index_buffer_lock(&line_index_buffer);
	for (int i = 0; i < KRINK_G2_SDF_BUFFER_SIZE; ++i) {
		indices[i * 3 * 2 + 0] = i * 4 + 0;
		indices[i * 3 * 2 + 1] = i * 4 + 1;
		indices[i * 3 * 2 + 2] = i * 4 + 2;
		indices[i * 3 * 2 + 3] = i * 4 + 0;
		indices[i * 3 * 2 + 4] = i * 4 + 2;
		indices[i * 3 * 2 + 5] = i * 4 + 3;
	}
	kinc_g4_index_buffer_unlock(&line_index_buffer);
}

void krink_g2_sdf_init(void) {
	build_rect_pipeline();
	build_circle_pipeline();
	build_line_pipeline();
}

// SDF Rect Impl

static void sdf_rect_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx,
                                    float tpry, float btrx, float btry) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	rect_rect_verts[base_idx + 0] = btlx;
	rect_rect_verts[base_idx + 1] = btly;
	rect_rect_verts[base_idx + 2] = -5.0f;

	rect_rect_verts[base_idx + 20] = tplx;
	rect_rect_verts[base_idx + 21] = tply;
	rect_rect_verts[base_idx + 22] = -5.0f;

	rect_rect_verts[base_idx + 40] = tprx;
	rect_rect_verts[base_idx + 41] = tpry;
	rect_rect_verts[base_idx + 42] = -5.0f;

	rect_rect_verts[base_idx + 60] = btrx;
	rect_rect_verts[base_idx + 61] = btry;
	rect_rect_verts[base_idx + 62] = -5.0f;
}

static void sdf_rect_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	rect_rect_verts[base_idx + 3] = left;
	rect_rect_verts[base_idx + 4] = bottom;

	rect_rect_verts[base_idx + 23] = left;
	rect_rect_verts[base_idx + 24] = top;

	rect_rect_verts[base_idx + 43] = right;
	rect_rect_verts[base_idx + 44] = top;

	rect_rect_verts[base_idx + 63] = right;
	rect_rect_verts[base_idx + 64] = bottom;
}

static void sdf_rect_set_rect_box(float u, float v) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	rect_rect_verts[base_idx + 5] = u;
	rect_rect_verts[base_idx + 6] = v;

	rect_rect_verts[base_idx + 25] = u;
	rect_rect_verts[base_idx + 26] = v;

	rect_rect_verts[base_idx + 45] = u;
	rect_rect_verts[base_idx + 46] = v;

	rect_rect_verts[base_idx + 65] = u;
	rect_rect_verts[base_idx + 66] = v;
}

static void sdf_rect_set_rect_colors(float opacity, uint32_t color, uint32_t border_color) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	float a = opacity * ((float)krink_color_get_channel(color, 'A') / 255.0f);
	float r = ((float)krink_color_get_channel(color, 'R') / 255.0f);
	float g = ((float)krink_color_get_channel(color, 'G') / 255.0f);
	float b = ((float)krink_color_get_channel(color, 'B') / 255.0f);
	float br = ((float)krink_color_get_channel(border_color, 'R') / 255.0f);
	float bg = ((float)krink_color_get_channel(border_color, 'G') / 255.0f);
	float bb = ((float)krink_color_get_channel(border_color, 'B') / 255.0f);

	rect_rect_verts[base_idx + 7] = r;
	rect_rect_verts[base_idx + 8] = g;
	rect_rect_verts[base_idx + 9] = b;
	rect_rect_verts[base_idx + 10] = a;
	rect_rect_verts[base_idx + 11] = br;
	rect_rect_verts[base_idx + 12] = bg;
	rect_rect_verts[base_idx + 13] = bb;

	rect_rect_verts[base_idx + 27] = r;
	rect_rect_verts[base_idx + 28] = g;
	rect_rect_verts[base_idx + 29] = b;
	rect_rect_verts[base_idx + 30] = a;
	rect_rect_verts[base_idx + 31] = br;
	rect_rect_verts[base_idx + 32] = bg;
	rect_rect_verts[base_idx + 33] = bb;

	rect_rect_verts[base_idx + 47] = r;
	rect_rect_verts[base_idx + 48] = g;
	rect_rect_verts[base_idx + 49] = b;
	rect_rect_verts[base_idx + 50] = a;
	rect_rect_verts[base_idx + 51] = br;
	rect_rect_verts[base_idx + 52] = bg;
	rect_rect_verts[base_idx + 53] = bb;

	rect_rect_verts[base_idx + 67] = r;
	rect_rect_verts[base_idx + 68] = g;
	rect_rect_verts[base_idx + 69] = b;
	rect_rect_verts[base_idx + 70] = a;
	rect_rect_verts[base_idx + 71] = br;
	rect_rect_verts[base_idx + 72] = bg;
	rect_rect_verts[base_idx + 73] = bb;
}

static void sdf_rect_set_rect_corner(krink_sdf_corner_radius_t c) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	rect_rect_verts[base_idx + 14] = c.bottom_right;
	rect_rect_verts[base_idx + 15] = c.top_right;
	rect_rect_verts[base_idx + 16] = c.bottom_left;
	rect_rect_verts[base_idx + 17] = c.top_left;

	rect_rect_verts[base_idx + 34] = c.bottom_right;
	rect_rect_verts[base_idx + 35] = c.top_right;
	rect_rect_verts[base_idx + 36] = c.bottom_left;
	rect_rect_verts[base_idx + 37] = c.top_left;

	rect_rect_verts[base_idx + 54] = c.bottom_right;
	rect_rect_verts[base_idx + 55] = c.top_right;
	rect_rect_verts[base_idx + 56] = c.bottom_left;
	rect_rect_verts[base_idx + 57] = c.top_left;

	rect_rect_verts[base_idx + 74] = c.bottom_right;
	rect_rect_verts[base_idx + 75] = c.top_right;
	rect_rect_verts[base_idx + 76] = c.bottom_left;
	rect_rect_verts[base_idx + 77] = c.top_left;
}

static void sdf_rect_set_border_smooth(float b, float s) {
	int base_idx = (rect_buffer_index - rect_buffer_start) * 20 * 4;
	rect_rect_verts[base_idx + 18] = b;
	rect_rect_verts[base_idx + 19] = s;

	rect_rect_verts[base_idx + 38] = b;
	rect_rect_verts[base_idx + 39] = s;

	rect_rect_verts[base_idx + 58] = b;
	rect_rect_verts[base_idx + 59] = s;

	rect_rect_verts[base_idx + 78] = b;
	rect_rect_verts[base_idx + 79] = s;
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

	if (end || rect_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		rect_buffer_start = 0;
		rect_buffer_index = 0;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		rect_buffer_start = rect_buffer_index;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, rect_buffer_start * 4,
		                               (KRINK_G2_SDF_BUFFER_SIZE - rect_buffer_start) * 4);
	}
}

void krink_g2_sdf_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void krink_g2_sdf_draw_rect(float x, float y, float width, float height,
                            krink_sdf_corner_radius_t corner, float border, float smooth,
                            uint32_t color, uint32_t border_color, float opacity,
                            kinc_matrix3x3_t transformation) {
	sdf_circle_draw_buffer(false);
	sdf_line_draw_buffer(false);
	kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x, y + height, 0.0f}); // bottom-left
	kinc_vector3_t p1 =
	    kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){x, y, 0.0f}); // top-left
	kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + width, y, 0.0f}); // top-right
	kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + width, y + height, 0.0f}); // bottom-right
	sdf_rect_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
	float w =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p2.x, p2.y}, (krink_vec2_t){p1.x, p1.y}));
	float h =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p0.x, p0.y}, (krink_vec2_t){p1.x, p1.y}));
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

void krink_g2_sdf_draw_rect_symm(float x, float y, float width, float height, float corner,
                                 float border, float smooth, uint32_t color, uint32_t border_color,
                                 float opacity, kinc_matrix3x3_t transformation) {
	krink_g2_sdf_draw_rect(x, y, width, height,
	                       (krink_sdf_corner_radius_t){corner, corner, corner, corner}, border,
	                       smooth, color, border_color, opacity, transformation);
}

// SDF Circle Impl

static void sdf_circle_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx,
                                      float tpry, float btrx, float btry) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 14 * 4;
	circle_rect_verts[base_idx + 0] = btlx;
	circle_rect_verts[base_idx + 1] = btly;
	circle_rect_verts[base_idx + 2] = -5.0f;

	circle_rect_verts[base_idx + 14] = tplx;
	circle_rect_verts[base_idx + 15] = tply;
	circle_rect_verts[base_idx + 16] = -5.0f;

	circle_rect_verts[base_idx + 28] = tprx;
	circle_rect_verts[base_idx + 29] = tpry;
	circle_rect_verts[base_idx + 30] = -5.0f;

	circle_rect_verts[base_idx + 42] = btrx;
	circle_rect_verts[base_idx + 43] = btry;
	circle_rect_verts[base_idx + 44] = -5.0f;
}

static void sdf_circle_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 14 * 4;
	circle_rect_verts[base_idx + 3] = left;
	circle_rect_verts[base_idx + 4] = bottom;

	circle_rect_verts[base_idx + 17] = left;
	circle_rect_verts[base_idx + 18] = top;

	circle_rect_verts[base_idx + 31] = right;
	circle_rect_verts[base_idx + 32] = top;

	circle_rect_verts[base_idx + 45] = right;
	circle_rect_verts[base_idx + 46] = bottom;
}

static void sdf_circle_set_rect_colors(float opacity, uint32_t color, uint32_t border_color) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 14 * 4;
	float a = opacity * ((float)krink_color_get_channel(color, 'A') / 255.0f);
	float r = ((float)krink_color_get_channel(color, 'R') / 255.0f);
	float g = ((float)krink_color_get_channel(color, 'G') / 255.0f);
	float b = ((float)krink_color_get_channel(color, 'B') / 255.0f);
	float br = ((float)krink_color_get_channel(border_color, 'R') / 255.0f);
	float bg = ((float)krink_color_get_channel(border_color, 'G') / 255.0f);
	float bb = ((float)krink_color_get_channel(border_color, 'B') / 255.0f);

	circle_rect_verts[base_idx + 5] = r;
	circle_rect_verts[base_idx + 6] = g;
	circle_rect_verts[base_idx + 7] = b;
	circle_rect_verts[base_idx + 8] = a;
	circle_rect_verts[base_idx + 9] = br;
	circle_rect_verts[base_idx + 10] = bg;
	circle_rect_verts[base_idx + 11] = bb;

	circle_rect_verts[base_idx + 19] = r;
	circle_rect_verts[base_idx + 20] = g;
	circle_rect_verts[base_idx + 21] = b;
	circle_rect_verts[base_idx + 22] = a;
	circle_rect_verts[base_idx + 23] = br;
	circle_rect_verts[base_idx + 24] = bg;
	circle_rect_verts[base_idx + 25] = bb;

	circle_rect_verts[base_idx + 33] = r;
	circle_rect_verts[base_idx + 34] = g;
	circle_rect_verts[base_idx + 35] = b;
	circle_rect_verts[base_idx + 36] = a;
	circle_rect_verts[base_idx + 37] = br;
	circle_rect_verts[base_idx + 38] = bg;
	circle_rect_verts[base_idx + 39] = bb;

	circle_rect_verts[base_idx + 47] = r;
	circle_rect_verts[base_idx + 48] = g;
	circle_rect_verts[base_idx + 49] = b;
	circle_rect_verts[base_idx + 50] = a;
	circle_rect_verts[base_idx + 51] = br;
	circle_rect_verts[base_idx + 52] = bg;
	circle_rect_verts[base_idx + 53] = bb;
}

static void sdf_circle_set_border_smooth(float b, float s) {
	int base_idx = (circle_buffer_index - circle_buffer_start) * 14 * 4;
	circle_rect_verts[base_idx + 12] = b;
	circle_rect_verts[base_idx + 13] = s;

	circle_rect_verts[base_idx + 26] = b;
	circle_rect_verts[base_idx + 27] = s;

	circle_rect_verts[base_idx + 40] = b;
	circle_rect_verts[base_idx + 41] = s;

	circle_rect_verts[base_idx + 54] = b;
	circle_rect_verts[base_idx + 55] = s;
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

	if (end || circle_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		circle_buffer_start = 0;
		circle_buffer_index = 0;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		circle_buffer_start = circle_buffer_index;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, circle_buffer_start * 4,
		                               (KRINK_G2_SDF_BUFFER_SIZE - circle_buffer_start) * 4);
	}
}

void krink_g2_sdf_draw_circle(float x, float y, float radius, float border, float smooth,
                              uint32_t color, uint32_t border_color, float opacity,
                              kinc_matrix3x3_t transformation) {
	sdf_rect_draw_buffer(false);
	sdf_line_draw_buffer(false);
	kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x - radius, y + radius, 0.0f}); // bottom-left
	kinc_vector3_t p1 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x - radius, y - radius, 0.0f}); // top-left
	kinc_vector3_t p2 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + radius, y - radius, 0.0f}); // top-right
	kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(
	    &transformation, (kinc_vector3_t){x + radius, y + radius, 0.0f}); // bottom-right
	sdf_circle_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
	float w =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p2.x, p2.y}, (krink_vec2_t){p1.x, p1.y}));
	float h =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p0.x, p0.y}, (krink_vec2_t){p1.x, p1.y}));
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
	int base_idx = (line_buffer_index - line_buffer_start) * 12 * 4;
	line_rect_verts[base_idx + 0] = btlx;
	line_rect_verts[base_idx + 1] = btly;
	line_rect_verts[base_idx + 2] = -5.0f;

	line_rect_verts[base_idx + 12] = tplx;
	line_rect_verts[base_idx + 13] = tply;
	line_rect_verts[base_idx + 14] = -5.0f;

	line_rect_verts[base_idx + 24] = tprx;
	line_rect_verts[base_idx + 25] = tpry;
	line_rect_verts[base_idx + 26] = -5.0f;

	line_rect_verts[base_idx + 36] = btrx;
	line_rect_verts[base_idx + 37] = btry;
	line_rect_verts[base_idx + 38] = -5.0f;
}

static void sdf_line_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = (line_buffer_index - line_buffer_start) * 12 * 4;
	line_rect_verts[base_idx + 3] = left;
	line_rect_verts[base_idx + 4] = bottom;

	line_rect_verts[base_idx + 15] = left;
	line_rect_verts[base_idx + 16] = top;

	line_rect_verts[base_idx + 27] = right;
	line_rect_verts[base_idx + 28] = top;

	line_rect_verts[base_idx + 39] = right;
	line_rect_verts[base_idx + 40] = bottom;
}

static void sdf_line_set_rect_colors(float opacity, uint32_t color) {
	int base_idx = (line_buffer_index - line_buffer_start) * 12 * 4;
	float a = opacity * ((float)krink_color_get_channel(color, 'A') / 255.0f);
	float r = ((float)krink_color_get_channel(color, 'R') / 255.0f);
	float g = ((float)krink_color_get_channel(color, 'G') / 255.0f);
	float b = ((float)krink_color_get_channel(color, 'B') / 255.0f);

	line_rect_verts[base_idx + 5] = r;
	line_rect_verts[base_idx + 6] = g;
	line_rect_verts[base_idx + 7] = b;
	line_rect_verts[base_idx + 8] = a;

	line_rect_verts[base_idx + 17] = r;
	line_rect_verts[base_idx + 18] = g;
	line_rect_verts[base_idx + 19] = b;
	line_rect_verts[base_idx + 20] = a;

	line_rect_verts[base_idx + 29] = r;
	line_rect_verts[base_idx + 30] = g;
	line_rect_verts[base_idx + 31] = b;
	line_rect_verts[base_idx + 32] = a;

	line_rect_verts[base_idx + 41] = r;
	line_rect_verts[base_idx + 42] = g;
	line_rect_verts[base_idx + 43] = b;
	line_rect_verts[base_idx + 44] = a;
}

static void sdf_line_set_dim(float u, float v) {
	int base_idx = (line_buffer_index - line_buffer_start) * 12 * 4;
	line_rect_verts[base_idx + 9] = u;
	line_rect_verts[base_idx + 10] = v;

	line_rect_verts[base_idx + 21] = u;
	line_rect_verts[base_idx + 22] = v;

	line_rect_verts[base_idx + 33] = u;
	line_rect_verts[base_idx + 34] = v;

	line_rect_verts[base_idx + 45] = u;
	line_rect_verts[base_idx + 46] = v;
}

static void sdf_line_set_smooth(float s) {
	int base_idx = (line_buffer_index - line_buffer_start) * 12 * 4;
	line_rect_verts[base_idx + 11] = s;

	line_rect_verts[base_idx + 23] = s;

	line_rect_verts[base_idx + 35] = s;

	line_rect_verts[base_idx + 47] = s;
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

	if (end || line_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		line_buffer_start = 0;
		line_buffer_index = 0;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		line_buffer_start = line_buffer_index;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, line_buffer_start * 4,
		                               (KRINK_G2_SDF_BUFFER_SIZE - line_buffer_start) * 4);
	}
}

static kinc_vector3_t get_corner_vec(krink_vec2_t a, krink_vec2_t d0, krink_vec2_t d1) {
	a = krink_vec2_addv(krink_vec2_addv(a, d0), d1);
	return (kinc_vector3_t){a.x, a.y, 0.0f};
}

void krink_g2_sdf_draw_line(float x0, float y0, float x1, float y1, float strength, float smooth,
                            uint32_t color, float opacity, kinc_matrix3x3_t transformation) {
	sdf_circle_draw_buffer(false);
	sdf_rect_draw_buffer(false);
	krink_vec2_t a = x0 <= x1 ? ((krink_vec2_t){x0, y0}) : ((krink_vec2_t){x1, y1});
	krink_vec2_t b = x0 <= x1 ? ((krink_vec2_t){x1, y1}) : ((krink_vec2_t){x0, y0});
	krink_vec2_t fw = krink_vec2_normalized(krink_vec2_subv(b, a));
	krink_vec2_t bw = krink_vec2_mult(fw, -1.0f);
	krink_vec2_t up = (krink_vec2_t){fw.y, -fw.x};
	krink_vec2_t down = krink_vec2_mult(up, -1.0f);
	float hs = strength / 2.0;
	fw = krink_vec2_mult(fw, hs);
	bw = krink_vec2_mult(bw, hs);
	up = krink_vec2_mult(up, hs);
	down = krink_vec2_mult(down, hs);

	kinc_vector3_t p0 =
	    kinc_matrix3x3_multiply_vector(&transformation, get_corner_vec(a, down, bw)); // bottom-left
	kinc_vector3_t p1 =
	    kinc_matrix3x3_multiply_vector(&transformation, get_corner_vec(a, up, bw)); // top-left
	kinc_vector3_t p2 =
	    kinc_matrix3x3_multiply_vector(&transformation, get_corner_vec(b, up, fw)); // top-right
	kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(&transformation,
	                                                   get_corner_vec(b, down, fw)); // bottom-right

	float w =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p2.x, p2.y}, (krink_vec2_t){p1.x, p1.y}));
	float h =
	    krink_vec2_length(krink_vec2_subv((krink_vec2_t){p0.x, p0.y}, (krink_vec2_t){p1.x, p1.y}));
	float u = w / (w > h ? w : h);
	float v = h / (w > h ? w : h);
	float wd = krink_vec2_length(krink_vec2_subv(a, b));
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

void krink_g2_sdf_end(void) {
	sdf_rect_end();
	sdf_circle_end();
	sdf_line_end();
}
