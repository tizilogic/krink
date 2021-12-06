#pragma once

#include "ttf.h"

#include <stdbool.h>
#include <kinc/math/matrix.h>

#ifndef KRINK_G2_BUFFER_SIZE
#define KRINK_G2_BUFFER_SIZE 1000
#endif


void krink_g2_tsp_init(void);
void krink_g2_tsp_set_rect_verts(float btlx, float btly, float tplx, float tply, float tprx, float tpry, float btrx, float btry);
void krink_g2_tsp_set_rect_tex_coords(float left, float top, float right, float bottom);
void krink_g2_tsp_set_rect_colors(float opacity, unsigned int color);
void krink_g2_tsp_draw_buffer(void);
void krink_g2_tsp_set_bilinear_filter(bool bilinear);
void krink_g2_tsp_set_font(krink_ttf_font_t* font);
void krink_g2_tsp_draw_string(const char* text, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation);
void krink_g2_tsp_draw_characters(int* text, int start, int length, float opacity, unsigned int color, float x, float y, kinc_matrix3x3_t transformation);
void krink_g2_tsp_end(void);


/*
class TextShaderPainter {
	var projectionMatrix: FastMatrix4;

	static var standardTextPipeline: PipelineCache = null;
	static var structure: VertexStructure = null;
	static inline var bufferSize: Int = 1000;
	static var bufferIndex: Int;
	static var rectVertexBuffer: VertexBuffer;
	static var rectVertices: Float32Array;
	static var indexBuffer: IndexBuffer;

	var font: Kravur;

	static var lastTexture: Image;

	var g: Graphics;
	var myPipeline: PipelineCache = null;

	public var pipeline(get, set): PipelineCache;
	public var fontSize: Int;

	var bilinear: Bool = false;

	public function new(g4: Graphics) {
		this.g = g4;
		bufferIndex = 0;
		initShaders();
		myPipeline = standardTextPipeline;
		initBuffers();
	}

	function get_pipeline(): PipelineCache {
		return myPipeline;
	}

	function set_pipeline(pipe: PipelineCache): PipelineCache {
		myPipeline = pipe != null ? pipe : standardTextPipeline;
		return myPipeline;
	}

	public function setProjection(projectionMatrix: FastMatrix4): Void {
		this.projectionMatrix = projectionMatrix;
	}

	static function initShaders(): Void {
		if (structure == null) {
			structure = Graphics2.createTexgg..tVertexStructure();
		}
		if (standardTextPipeline == null) {
			var pipeline = Graphics2.createTextPipeline(structure);
			standardTextPipeline = new PerFramebufferPipelineCache(pipeline, true);
		}
	}

	function initBuffers(): Void {
		if (rectVertexBuffer == null) {
			rectVertexBuffer = new VertexBuffer(bufferSize * 4, structure, Usage.DynamicUsage);
			rectVertices = rectVertexBuffer.lock();

			indexBuffer = new IndexBuffer(bufferSize * 3 * 2, Usage.StaticUsage);
			var indices = indexBuffer.lock();
			for (i in 0...bufferSize) {
				indices[i * 3 * 2 + 0] = i * 4 + 0;
				indices[i * 3 * 2 + 1] = i * 4 + 1;
				indices[i * 3 * 2 + 2] = i * 4 + 2;
				indices[i * 3 * 2 + 3] = i * 4 + 0;
				indices[i * 3 * 2 + 4] = i * 4 + 2;
				indices[i * 3 * 2 + 5] = i * 4 + 3;
			}
			indexBuffer.unlock();
		}
	}

	function setRectVertices(bottomleftx: Float, bottomlefty: Float, topleftx: Float, toplefty: Float, toprightx: Float, toprighty: Float,
			bottomrightx: Float, bottomrighty: Float): Void {
		var baseIndex: Int = bufferIndex * 9 * 4;
		rectVertices.set(baseIndex + 0, bottomleftx);
		rectVertices.set(baseIndex + 1, bottomlefty);
		rectVertices.set(baseIndex + 2, -5.0);

		rectVertices.set(baseIndex + 9, topleftx);
		rectVertices.set(baseIndex + 10, toplefty);
		rectVertices.set(baseIndex + 11, -5.0);

		rectVertices.set(baseIndex + 18, toprightx);
		rectVertices.set(baseIndex + 19, toprighty);
		rectVertices.set(baseIndex + 20, -5.0);

		rectVertices.set(baseIndex + 27, bottomrightx);
		rectVertices.set(baseIndex + 28, bottomrighty);
		rectVertices.set(baseIndex + 29, -5.0);
	}

	function setRectTexCoords(left: Float, top: Float, right: Float, bottom: Float): Void {
		var baseIndex: Int = bufferIndex * 9 * 4;
		rectVertices.set(baseIndex + 3, left);
		rectVertices.set(baseIndex + 4, bottom);

		rectVertices.set(baseIndex + 12, left);
		rectVertices.set(baseIndex + 13, top);

		rectVertices.set(baseIndex + 21, right);
		rectVertices.set(baseIndex + 22, top);

		rectVertices.set(baseIndex + 30, right);
		rectVertices.set(baseIndex + 31, bottom);
	}

	function setRectColors(opacity: FastFloat, color: Color): Void {
		var baseIndex: Int = bufferIndex * 9 * 4;
		var a: FastFloat = opacity * color.A;
		rectVertices.set(baseIndex + 5, color.R);
		rectVertices.set(baseIndex + 6, color.G);
		rectVertices.set(baseIndex + 7, color.B);
		rectVertices.set(baseIndex + 8, a);

		rectVertices.set(baseIndex + 14, color.R);
		rectVertices.set(baseIndex + 15, color.G);
		rectVertices.set(baseIndex + 16, color.B);
		rectVertices.set(baseIndex + 17, a);

		rectVertices.set(baseIndex + 23, color.R);
		rectVertices.set(baseIndex + 24, color.G);
		rectVertices.set(baseIndex + 25, color.B);
		rectVertices.set(baseIndex + 26, a);

		rectVertices.set(baseIndex + 32, color.R);
		rectVertices.set(baseIndex + 33, color.G);
		rectVertices.set(baseIndex + 34, color.B);
		rectVertices.set(baseIndex + 35, a);
	}

	function drawBuffer(): Void {
		if (bufferIndex == 0) {
			return;
		}

		rectVertexBuffer.unlock(bufferIndex * 4);
		var pipeline = myPipeline.get(null, Depth24Stencil8);
		g.setPipeline(pipeline.pipeline);
		g.setVertexBuffer(rectVertexBuffer);
		g.setIndexBuffer(indexBuffer);
		g.setMatrix(pipeline.projectionLocation, projectionMatrix);
		g.setTexture(pipeline.textureLocation, lastTexture);
		g.setTextureParameters(pipeline.textureLocation, TextureAddressing.Clamp, TextureAddressing.Clamp,
			bilinear ? TextureFilter.LinearFilter : TextureFilter.PointFilter, bilinear ? TextureFilter.LinearFilter : TextureFilter.PointFilter,
			MipMapFilter.NoMipFilter);

		g.drawIndexedVertices(0, bufferIndex * 2 * 3);

		g.setTexture(pipeline.textureLocation, null);
		bufferIndex = 0;
		rectVertices = rectVertexBuffer.lock();
	}

	public function setBilinearFilter(bilinear: Bool): Void {
		end();
		this.bilinear = bilinear;
	}

	public function setFont(font: Font): Void {
		this.font = cast(font, Kravur);
	}

	var bakedQuadCache = new kha.Kravur.AlignedQuad();

	public function drawString(text: String, opacity: FastFloat, color: Color, x: Float, y: Float, transformation: FastMatrix3): Void {
		var font = this.font._get(fontSize);
		var tex = font.getTexture();
		if (lastTexture != null && tex != lastTexture)
			drawBuffer();
		lastTexture = tex;

		var xpos = x;
		var ypos = y;
		for (i in 0...text.length) {
			var charCode = StringTools.fastCodeAt(text, i);
			var q = font.getBakedQuad(bakedQuadCache, findIndex(charCode), xpos, ypos);
			if (q != null) {
				if (bufferIndex + 1 >= bufferSize)
					drawBuffer();
				setRectColors(opacity, color);
				setRectTexCoords(q.s0 * tex.width / tex.realWidth, q.t0 * tex.height / tex.realHeight, q.s1 * tex.width / tex.realWidth,
					q.t1 * tex.height / tex.realHeight);
				var p0 = transformation.multvec(new FastVector2(q.x0, q.y1)); // bottom-left
				var p1 = transformation.multvec(new FastVector2(q.x0, q.y0)); // top-left
				var p2 = transformation.multvec(new FastVector2(q.x1, q.y0)); // top-right
				var p3 = transformation.multvec(new FastVector2(q.x1, q.y1)); // bottom-right
				setRectVertices(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
				xpos += q.xadvance;
				++bufferIndex;
			}
		}
	}

	public function drawCharacters(text: Array<Int>, start: Int, length: Int, opacity: FastFloat, color: Color, x: Float, y: Float,
			transformation: FastMatrix3): Void {
		var font = this.font._get(fontSize);
		var tex = font.getTexture();
		if (lastTexture != null && tex != lastTexture)
			drawBuffer();
		lastTexture = tex;

		var xpos = x;
		var ypos = y;
		for (i in start...start + length) {
			var q = font.getBakedQuad(bakedQuadCache, findIndex(text[i]), xpos, ypos);
			if (q != null) {
				if (bufferIndex + 1 >= bufferSize)
					drawBuffer();
				setRectColors(opacity, color);
				setRectTexCoords(q.s0 * tex.width / tex.realWidth, q.t0 * tex.height / tex.realHeight, q.s1 * tex.width / tex.realWidth,
					q.t1 * tex.height / tex.realHeight);
				var p0 = transformation.multvec(new FastVector2(q.x0, q.y1)); // bottom-left
				var p1 = transformation.multvec(new FastVector2(q.x0, q.y0)); // top-left
				var p2 = transformation.multvec(new FastVector2(q.x1, q.y0)); // top-right
				var p3 = transformation.multvec(new FastVector2(q.x1, q.y1)); // bottom-right
				setRectVertices(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
				xpos += q.xadvance;
				++bufferIndex;
			}
		}
	}

	public function end(): Void {
		if (bufferIndex > 0)
			drawBuffer();
		lastTexture = null;
	}
}

*/


/*
#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/image.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <assert.h>
#include <stdlib.h>

static kinc_g4_shader_t vertexShader;
static kinc_g4_shader_t fragmentShader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_index_buffer_t indices;
static kinc_g4_texture_t texture;
static kinc_g4_texture_unit_t texunit;
static kinc_g4_constant_location_t offset;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update(void) {
	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);

	kinc_g4_set_pipeline(&pipeline);
	kinc_matrix3x3_t matrix = kinc_matrix3x3_rotation_z((float)kinc_time());
	kinc_g4_set_matrix3(offset, &matrix);
	kinc_g4_set_vertex_buffer(&vertices);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_set_texture(texunit, &texture);
	kinc_g4_draw_indexed_vertices();

	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	kinc_init("TextureTest", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	{
		kinc_image_t image;
		void *image_mem = allocate(250 * 250 * 4);
		kinc_image_init_from_file(&image, image_mem, "parrot.png");
		kinc_g4_texture_init_from_image(&texture, &image);
		kinc_image_destroy(&image);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "texture.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = allocate(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vertexShader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "texture.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = allocate(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&fragmentShader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}

	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_pipeline_init(&pipeline);
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.vertex_shader = &vertexShader;
	pipeline.fragment_shader = &fragmentShader;
	kinc_g4_pipeline_compile(&pipeline);

	texunit = kinc_g4_pipeline_get_texture_unit(&pipeline, "texsampler");
	offset = kinc_g4_pipeline_get_constant_location(&pipeline, "mvp");

	kinc_g4_vertex_buffer_init(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	float *v = kinc_g4_vertex_buffer_lock_all(&vertices);
	v[0] = -1.0f;
	v[1] = -1.0f;
	v[2] = 0.5f;
	v[3] = 0.0f;
	v[4] = 1.0f;
	v[5] = 1.0f;
	v[6] = -1.0f;
	v[7] = 0.5f;
	v[8] = 1.0f;
	v[9] = 1.0f;
	v[10] = -1.0f;
	v[11] = 1.0f;
	v[12] = 0.5f;
	v[13] = 0.0f;
	v[14] = 0.0f;
	kinc_g4_vertex_buffer_unlock_all(&vertices);

	kinc_g4_index_buffer_init(&indices, 3, KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *i = kinc_g4_index_buffer_lock(&indices);
	i[0] = 0;
	i[1] = 1;
	i[2] = 2;
	kinc_g4_index_buffer_unlock(&indices);

	kinc_start();

	return 0;
}
*/