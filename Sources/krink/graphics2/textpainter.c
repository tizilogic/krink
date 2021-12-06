#include "textpainter.h"

#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
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
static kinc_g4_constant_location_t proj_mat_loc;
static kinc_matrix4x4_t projection_matrix;
static float *rect_verts = NULL;

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
	kinc_g4_pipeline_compile(&pipeline);

	texunit = kinc_g4_pipeline_get_texture_unit(&pipeline, "tex");
	proj_mat_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "projectionMatrix");
}

void krink_g2_tsp_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry, float btrx, float btry) {}

void krink_g2_tsp_set_rect_tex_coords(float left, float top, float right, float bottom) {}

void krink_g2_tsp_set_rect_colors(float opacity, unsigned int color) {}

void krink_g2_tsp_draw_buffer(void) {}

void krink_g2_tsp_set_bilinear_filter(bool bilinear) {
	bilinear_filter = bilinear;
}

void krink_g2_tsp_set_font(krink_ttf_font_t *font) {
	active_font = font;
}

void krink_g2_tsp_set_font_size(int size) {
	font_size = size;
}

void krink_g2_tsp_draw_string(const char *text, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation) {}

void krink_g2_tsp_draw_characters(int *text, int start, int length, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation) {}

void krink_g2_tsp_end(void) {}
