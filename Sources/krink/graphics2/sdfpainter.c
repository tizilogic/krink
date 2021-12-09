#include "sdfpainter.h"

#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/vertexstructure.h>
#include <kinc/io/filereader.h>
#include <krink/color.h>
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

static unsigned int color = 0xffffffff;

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
	                          KINC_G4_USAGE_STATIC);
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

	circle_proj_mat_loc =>
	    kinc_g4_pipeline_get_constant_location(&circle_pipeline, "projectionMatrix");

	kinc_g4_vertex_buffer_init(&circle_vertex_buffer, KRINK_G2_SDF_BUFFER_SIZE * 4, &structure,
	                           KINC_G4_USAGE_DYNAMIC, 0);
	circle_rect_verts = kinc_g4_vertex_buffer_lock_all(&circle_vertex_buffer);

	kinc_g4_index_buffer_init(&circle_index_buffer, KRINK_G2_SDF_BUFFER_SIZE * 3 * 2,
	                          KINC_G4_USAGE_STATIC);
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
	                          KINC_G4_USAGE_STATIC);
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

void sdf_rect_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry,
                             float btrx, float btry) {
	int base_idx = rect_buffer_index * 20 * 4;
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

void sdf_rect_set_rect_tex_coords(float left, float top, float right, float bottom) {
	int base_idx = rect_buffer_index * 20 * 4;
	rect_rect_verts[base_idx + 3] = left;
	rect_rect_verts[base_idx + 4] = bottom;

	rect_rect_verts[base_idx + 23] = left;
	rect_rect_verts[base_idx + 24] = top;

	rect_rect_verts[base_idx + 43] = right;
	rect_rect_verts[base_idx + 44] = top;

	rect_rect_verts[base_idx + 63] = right;
	rect_rect_verts[base_idx + 64] = bottom;
}

void sdf_rect_set_rect_box(float u, float v) {
	int base_idx = rect_buffer_index * 20 * 4;
	rect_rect_verts[base_idx + 5] = u;
	rect_rect_verts[base_idx + 6] = v;

	rect_rect_verts[base_idx + 25] = u;
	rect_rect_verts[base_idx + 26] = v;

	rect_rect_verts[base_idx + 45] = u;
	rect_rect_verts[base_idx + 46] = v;

	rect_rect_verts[base_idx + 65] = u;
	rect_rect_verts[base_idx + 66] = v;
}

void sdf_rect_set_rect_colors(float opacity, unsigned int color, unsigned int border_color) {
	int base_idx = rect_buffer_index * 20 * 4;
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

void sdf_rect_set_rect_corner(krink_sdf_corner_radius_t c) {
	int base_idx = rect_buffer_index * 20 * 4;
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

void sdf_rect_set_border_smooth(float b, float s) {
	int base_idx = rect_buffer_index * 20 * 4;
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
	if (!end) {
		sdf_circle_end();
		sdf_line_end();
	}
	if (rect_buffer_index - rect_buffer_start == 0) return;
	kinc_g4_vertex_buffer_unlock(&rect_vertex_buffer, (rect_buffer_index - rect_buffer_start) * 4);
	kinc_g4_set_pipeline(&rect_pipeline);
	kinc_g4_set_matrix4(rect_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&rect_vertex_buffer);
	kinc_g4_set_index_buffer(&rect_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(rect_buffer_start, rect_buffer_index * 4);

	if (end || rect_buffer_start + rect_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		rect_buffer_start = 0;
		rect_buffer_index = 0;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		rect_buffer_start = rect_buffer_index;
		rect_rect_verts =
		    kinc_g4_vertex_buffer_lock(&rect_vertex_buffer, rect_buffer_start,
		                               (KRINK_G2_SDF_BUFFER_SIZE - rect_buffer_start) * 4);
	}
}

void krink_g2_sdf_set_projection_matrix(kinc_matrix4x4_t mat) {
	projection_matrix = mat;
}

void krink_g2_sdf_draw_rect(float x, float y, float width, float height,
                            krink_sdf_corner_radius_t corner, float border, float smooth,
                            unsigned int color, unsigned int border_color,
                            kinc_matrix3x3_t transformation) {
	/*
	var p1 = transformation.multvec(new FastVector2(x, y + height));
	        var p2 = transformation.multvec(new FastVector2(x, y));
	        var p3 = transformation.multvec(new FastVector2(x + width, y));
	        var p4 = transformation.multvec(new FastVector2(x + width, y + height));
	        var w = p3.sub(p2).length;
	        var h = p1.sub(p2).length;
	        var u = w / Math.max(w, h);
	        var v = h / Math.max(w, h);
	        var f = (u >= v ? u / w : v / h) * Math.max(w / width, h / height);
	        corner.br = corner.br != null ? corner.br : corner.tr;
	        corner.tl = corner.tl != null ? corner.tl : corner.tr;
	        corner.bl = corner.bl != null ? corner.bl : corner.tr;
	        corner.tr *= f;
	        corner.br *= f;
	        corner.tl *= f;
	        corner.bl *= f;
	        sdfRectPainter.drawSDFRect(opacity, color, borderColor, p1.x, p1.y, p2.x, p2.y, p3.x,
	p3.y, p4.x, p4.y, u, v, corner, border * f, smooth * f);

	setRectColor(rCol.R, rCol.G, rCol.B, rCol.A * opacity, bCol.R, bCol.G, bCol.B);
	        setRectTexCoords(0, 0, u, v);
	        setRectVertices(bottomleftx, bottomlefty, topleftx, toplefty, toprightx, toprighty,
	bottomrightx, bottomrighty); setRectBox(u / 2, v / 2); setCorner(c); setBorderSmooth(b, s);

	        ++bufferIndex;
	*/
}

static void sdf_circle_draw_buffer(bool end) {
	if (!end) {
		sdf_rect_end();
		sdf_line_end();
	}
	if (circle_buffer_index - circle_buffer_start == 0) return;
	kinc_g4_vertex_buffer_unlock(&circle_vertex_buffer,
	                             (circle_buffer_index - circle_buffer_start) * 4);
	kinc_g4_set_pipeline(&circle_pipeline);
	kinc_g4_set_matrix4(circle_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&circle_vertex_buffer);
	kinc_g4_set_index_buffer(&circle_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(circle_buffer_start, circle_buffer_index * 4);

	if (end || circle_buffer_start + circle_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		circle_buffer_start = 0;
		circle_buffer_index = 0;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		circle_buffer_start = circle_buffer_index;
		circle_rect_verts =
		    kinc_g4_vertex_buffer_lock(&circle_vertex_buffer, circle_buffer_start,
		                               (KRINK_G2_SDF_BUFFER_SIZE - circle_buffer_start) * 4);
	}
}

void krink_g2_sdf_draw_circle(float x, float y, float radius, float border, float smooth,
                              unsigned int color, unsigned int border_color,
                              kinc_matrix3x3_t transformation) {}

static void sdf_line_draw_buffer(bool end) {
	if (!end) {
		sdf_circle_end();
		sdf_rect_end();
	}
	if (line_buffer_index - line_buffer_start == 0) return;
	kinc_g4_vertex_buffer_unlock(&line_vertex_buffer, (line_buffer_index - line_buffer_start) * 4);
	kinc_g4_set_pipeline(&line_pipeline);
	kinc_g4_set_matrix4(line_proj_mat_loc, &projection_matrix);
	kinc_g4_set_vertex_buffer(&line_vertex_buffer);
	kinc_g4_set_index_buffer(&line_index_buffer);
	kinc_g4_draw_indexed_vertices_from_to(line_buffer_start, line_buffer_index * 4);

	if (end || line_buffer_start + line_buffer_index + 1 >= KRINK_G2_SDF_BUFFER_SIZE) {
		line_buffer_start = 0;
		line_buffer_index = 0;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, 0, KRINK_G2_SDF_BUFFER_SIZE * 4);
	}
	else {
		line_buffer_start = line_buffer_index;
		line_rect_verts =
		    kinc_g4_vertex_buffer_lock(&line_vertex_buffer, line_buffer_start,
		                               (KRINK_G2_SDF_BUFFER_SIZE - line_buffer_start) * 4);
	}
}

void krink_g2_sdf_draw_line(float x0, float y0, float x1, float y1, float corner_radius,
                            float smooth, unsigned int color, kinc_matrix3x3_t transformation) {}

/*void krink_g2_sdf_draw_scaled_sub_image(krink_image_t *img, float sx, float sy, float sw, float
sh, float dx, float dy, float dw, float dh, float opacity, unsigned int color,  kinc_matrix3x3_t
transformation) { kinc_g4_texture_t *tex = &(img->tex); if (buffer_index + 1 >=
KRINK_G2_sdf_BUFFER_SIZE || (last_texture != NULL && tex != last_texture))
krink_g2_sdf_draw_buffer(false);

    krink_g2_sdf_set_rect_tex_coords(sx / img->real_width, sy / img->real_height, (sx + sw) /
img->real_width, (sy + sh) / img->real_height); krink_g2_sdf_set_rect_colors(opacity, color);
    kinc_vector3_t p0 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){dx, dy +
dh, 0.0f}); // bottom-left kinc_vector3_t p1 = kinc_matrix3x3_multiply_vector(&transformation,
(kinc_vector3_t){dx, dy, 0.0f}); // top-left kinc_vector3_t p2 =
kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){dx + dw, dy, 0.0f}); // top-right
    kinc_vector3_t p3 = kinc_matrix3x3_multiply_vector(&transformation, (kinc_vector3_t){dx + dw, dy
+ dh, 0.0f}); // bottom-right krink_g2_sdf_set_rect_verts(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x,
p3.y);

    ++buffer_index;
    last_texture = tex;
}*/

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
