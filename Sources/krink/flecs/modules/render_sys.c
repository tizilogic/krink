#define SYSTEMS_RENDER_IMPL
#include "render_sys.h"
#include "render_comp.h"
#include <kinc/math/core.h>
#include <kinc/math/matrix.h>
#include <kinc/system.h>
#include <krink/graphics2/graphics.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>

static bool scissor_active = false;

static void FrameTime(ecs_iter_t *it) {
	KrFrameTime *t = ecs_term(it, KrFrameTime, 1);
	t[0].time = kinc_time();
}

static void Clear(ecs_iter_t *it) {
	ecs_world_t *world = it->world;
	kr_g2_begin(0);
	const KrSingletonClearColor *clear_color =
	    ecs_get(world, ecs_id(KrSingletonClearColor), KrSingletonClearColor);
	kr_g2_clear(clear_color->color);
	scissor_active = false;
}

static void End(ecs_iter_t *it) {
	kr_g2_disable_scissor();
	kr_g2_set_opacity(1.0f);
	kr_g2_set_transform(kr_matrix3x3_identity());
	kr_g2_end();
}

static void Render(ecs_iter_t *it) {
	KrDrawable *drawable = ecs_term(it, KrDrawable, 1);
	KrPos2 *pos = ecs_term(it, KrPos2, 2);
	KrColor *color = ecs_term(it, KrColor, 3);
	KrImage *image = ecs_term(it, KrImage, 4);
	KrText *text = ecs_term(it, KrText, 5);
	KrStroke *stroke = ecs_term(it, KrStroke, 6);
	KrSmooth *smooth = ecs_term(it, KrSmooth, 7);
	KrTriangle *triangle = ecs_term(it, KrTriangle, 8);
	KrRect *rect = ecs_term(it, KrRect, 9);
	KrScissor *scissor = ecs_term(it, KrScissor, 10);
	KrCorner *corner = ecs_term(it, KrCorner, 11);
	KrCornerAsymm *corner_asymm = ecs_term(it, KrCornerAsymm, 12);
	KrCircle *circle = ecs_term(it, KrCircle, 13);
	KrBorder *border = ecs_term(it, KrBorder, 14);
	KrLine *line = ecs_term(it, KrLine, 15);
	KrAngle *angle = ecs_term(it, KrAngle, 16);
	KrRotationCenter *rotation_center = ecs_term(it, KrRotationCenter, 17);
	KrOpacity *opacity = ecs_term(it, KrOpacity, 18);
	KrScaleX *scale_x = ecs_term(it, KrScaleX, 19);
	KrScaleY *scale_y = ecs_term(it, KrScaleY, 20);
	KrTranslation *translation = ecs_term(it, KrTranslation, 21);

	for (int i = 0; i < it->count; ++i) {
		float border_strength = 0.0f;
		uint32_t border_color = 0;
		kr_matrix3x3_t transform = kr_matrix3x3_identity();

		float sx = 1.0f, sy = 1.0f;
		if (ecs_term_is_set(it, 21)) { // Translate
			kr_matrix3x3_t tmat = kr_matrix3x3_translation(translation[i].x, translation[i].y);
			transform = kr_matrix3x3_multmat(&transform, &tmat);
		}
		if (ecs_term_is_set(it, 19) && ecs_term_is_set(it, 20)) { // Scale asymmetric
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale_x[i].value, scale_y[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale_x[i].value;
			sy = scale_y[i].value;
		}
		else if (ecs_term_is_set(it, 19)) { // ScaleX
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale_x[i].value, 1.0f);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale_x[i].value;
			sy = 1.0f;
		}
		else if (ecs_term_is_set(it, 20)) { // ScaleY
			kr_matrix3x3_t smat = kr_matrix3x3_scale(1.0f, scale_y[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = 1.0f;
			sy = scale_y[i].value;
		}
		if (ecs_term_is_set(it, 16)) { // Rotate
			float cx, cy;
			if (ecs_term_is_set(it, 17)) {
				cx = rotation_center[i].x;
				cy = rotation_center[i].y;
			}
			else {
				float w, h;
				switch (drawable[i].pipeline) {
				case KR_COMP_PP_IMAGE: {
					w = image[i].dw;
					h = image[i].dh;
				} break;
				case KR_COMP_PP_TEXT: {
					w = kr_ttf_width(text[i].font, (int)(text[i].size + 0.5f), text[i].text);
					h = kr_ttf_height(text[i].font, (int)(text[i].size + 0.5f));
				} break;
				case KR_COMP_PP_COLOR_TRIS_STROKE:
				case KR_COMP_PP_COLOR_TRIS_FILL: {
					w = kinc_max(triangle[i].x1, kinc_max(triangle[i].x2, triangle[i].x3)) -
					    kinc_min(triangle[i].x1, kinc_min(triangle[i].x2, triangle[i].x3));
					h = kinc_max(triangle[i].y1, kinc_max(triangle[i].y2, triangle[i].y3)) -
					    kinc_min(triangle[i].y1, kinc_min(triangle[i].y2, triangle[i].y3));
					//w *= sx;
					//h *= sy;
				} break;
				case KR_COMP_PP_COLOR_QUAD_STROKE:
				case KR_COMP_PP_COLOR_QUAD_FILL:
				case KR_COMP_PP_SDF_RECT_WBORDER:
				case KR_COMP_PP_SDF_RECT:
				case KR_COMP_PP_SDF_RECT_ASYMM_WBORDER:
				case KR_COMP_PP_SDF_RECT_ASYMM: {
					w = rect[i].w;
					h = rect[i].h;
				} break;
				case KR_COMP_PP_SDF_CIRCLE_WBORDER:
				case KR_COMP_PP_SDF_CIRCLE: {
					w = circle[i].radius * 2.0f;
					h = circle[i].radius * 2.0f;
				} break;
				case KR_COMP_PP_LINE:
				case KR_COMP_PP_SDF_LINE: {
					w = kinc_max(pos[i].x, line[i].x_to) - kinc_min(pos[i].x, line[i].x_to);
					h = kinc_max(pos[i].y, line[i].y_to) - kinc_min(pos[i].y, line[i].y_to);
				} break;
				default: {
					w = h = 0.0f;
				} break;
				}
				cx = w / 2.0f;
				cy = h / 2.0f;
			}
			kr_matrix3x3_t rot = kr_matrix3x3_identity();
			kr_matrix3x3_t tmp = kr_matrix3x3_translation(cx, cy);
			rot = kr_matrix3x3_multmat(&rot, &tmp);
			tmp = kr_matrix3x3_rotation(angle[i].radians);
			rot = kr_matrix3x3_multmat(&rot, &tmp);
			tmp = kr_matrix3x3_translation(-cx, -cy);
			rot = kr_matrix3x3_multmat(&rot, &tmp);
			transform = kr_matrix3x3_multmat(&transform, &rot);
		}
		kr_g2_set_transform(transform);

		if (ecs_term_is_set(it, 18)) {
			kr_g2_set_opacity(opacity[i].alpha);
		}
		else {
			kr_g2_set_opacity(1.0f);
		}

		kr_g2_set_color(color[i].color);
		if (ecs_term_is_set(it, 10)) {
			kr_g2_scissor(scissor[i].x, scissor[i].y, scissor[i].w, scissor[i].h);
		}
		else if (scissor_active)
			kr_g2_disable_scissor();

		switch (drawable[i].pipeline) {
		case KR_COMP_PP_IMAGE: {
			kr_g2_set_bilinear_filter(image[i].bilinear);
			kr_g2_draw_scaled_sub_image(image[i].image, image[i].sx, image[i].sy, image[i].sw,
			                            image[i].sh, pos[i].x, pos[i].y, image[i].dw, image[i].dh);
		} break;
		case KR_COMP_PP_TEXT: {
			kr_g2_set_font(text[i].font, text[i].size);
			kr_g2_draw_string(text[i].text, pos[i].x, pos[i].y);
		} break;
		case KR_COMP_PP_COLOR_TRIS_STROKE: {
			kr_g2_draw_triangle(triangle[i].x1, triangle[i].y1, triangle[i].x2, triangle[i].y2,
			                    triangle[i].x3, triangle[i].y3, stroke[i].strength);
		} break;
		case KR_COMP_PP_COLOR_TRIS_FILL: {
			kr_g2_fill_triangle(triangle[i].x1, triangle[i].y1, triangle[i].x2, triangle[i].y2,
			                    triangle[i].x3, triangle[i].y3);
		} break;
		case KR_COMP_PP_COLOR_QUAD_STROKE: {
			kr_g2_draw_rect(pos[i].x, pos[i].y, rect[i].w, rect[i].h, stroke[i].strength);
		} break;
		case KR_COMP_PP_COLOR_QUAD_FILL: {
			kr_g2_fill_rect(pos[i].x, pos[i].y, rect[i].w, rect[i].h);
		} break;

		case KR_COMP_PP_SDF_RECT_WBORDER: {
			border_strength = border[i].strength;
			border_color = border[i].color;
		}
		case KR_COMP_PP_SDF_RECT: {
			kr_g2_draw_sdf_rect_symm(pos[i].x, pos[i].y, rect[i].w, rect[i].h, corner[i].radius,
			                         border_strength, border_color, smooth[i].px);
		} break;

		case KR_COMP_PP_SDF_RECT_ASYMM_WBORDER: {
			border_strength = border[i].strength;
			border_color = border[i].color;
		}
		case KR_COMP_PP_SDF_RECT_ASYMM: {
			kr_sdf_corner_radius_t c;
			c.bottom_left = corner_asymm[i].bl;
			c.bottom_right = corner_asymm[i].br;
			c.top_left = corner_asymm[i].tl;
			c.top_right = corner_asymm[i].tr;
			kr_g2_draw_sdf_rect(pos[i].x, pos[i].y, rect[i].w, rect[i].h, c, border_strength,
			                    border_color, smooth[i].px);
		} break;

		case KR_COMP_PP_SDF_CIRCLE_WBORDER: {
			border_strength = border[i].strength;
			border_color = border[i].color;
		}
		case KR_COMP_PP_SDF_CIRCLE: {
			kr_g2_draw_sdf_circle(pos[i].x, pos[i].y, circle[i].radius, border_strength,
			                      border_color, smooth[i].px);
		} break;
		case KR_COMP_PP_LINE: {
			kr_g2_draw_line(pos[i].x, pos[i].y, line[i].x_to, line[i].y_to, stroke[i].strength);
		} break;
		case KR_COMP_PP_SDF_LINE: // TODO
		default:
			break;
		}
	}
}

/**
 * MSB -> LSB
 * 11 bits signed depth -1024..1023
 * 5 bits unsigned pipeline 0..31
 * 16 bits extra sort upper 16 bits XOR lower 16 bits, masked sign removed
 */

#define DSHIFT 21
#define PSHIFT 16
#define DSIGN 0x80000000
#define DMASK 0x000003ff
#define PMASK 0x001f0000
#define EMASK 0x0000ffff

static int compare_render_order(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2,
                                const void *ptr2) {
	KrDrawable *d1 = (KrDrawable *)ptr1;
	KrDrawable *d2 = (KrDrawable *)ptr2;
	return ((d1->depth & DSIGN) | ((d1->depth & DMASK) << DSHIFT) |
	        ((d1->pipeline << PSHIFT) & PMASK) |
	        ((((d1->sort_extra >> (32 - PSHIFT)) & EMASK) + (d1->sort_extra & EMASK)) & EMASK)) -
	       ((d2->depth & DSIGN) | ((d2->depth & DMASK) << DSHIFT) |
	        ((d2->pipeline << PSHIFT) & PMASK) |
	        ((((d2->sort_extra >> (32 - PSHIFT)) & EMASK) + (d2->sort_extra & EMASK)) & EMASK));
}

void SystemsRenderImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsRender);

	/* Register components */
	ECS_IMPORT(world, ComponentsRender);

	ECS_SYSTEM(world, FrameTime, EcsOnLoad, $KrFrameTime);

	ecs_singleton_set(world, KrFrameTime, { kinc_time() });

	ECS_SYSTEM(world, Clear, EcsPreStore);

	ecs_term_t termbuff[22] = {
	    {ecs_id(KrDrawable)},
	    {ecs_id(KrPos2), .oper = EcsOptional},
	    {ecs_id(KrColor)},
	    {ecs_id(KrImage), .oper = EcsOptional},
	    {ecs_id(KrText), .oper = EcsOptional},
	    {ecs_id(KrStroke), .oper = EcsOptional},
	    {ecs_id(KrSmooth), .oper = EcsOptional},
	    {ecs_id(KrTriangle), .oper = EcsOptional},
	    {ecs_id(KrRect), .oper = EcsOptional},
	    {ecs_id(KrScissor), .oper = EcsOptional},
	    {ecs_id(KrCorner), .oper = EcsOptional},
	    {ecs_id(KrCornerAsymm), .oper = EcsOptional},
	    {ecs_id(KrCircle), .oper = EcsOptional},
	    {ecs_id(KrBorder), .oper = EcsOptional},
	    {ecs_id(KrLine), .oper = EcsOptional},
	    {ecs_id(KrAngle), .oper = EcsOptional},
	    {ecs_id(KrRotationCenter), .oper = EcsOptional},
	    {ecs_id(KrOpacity), .oper = EcsOptional},
	    {ecs_id(KrScaleX), .oper = EcsOptional},
	    {ecs_id(KrScaleY), .oper = EcsOptional},
	    {ecs_id(KrTranslation), .oper = EcsOptional},
	    {KrVisible},
	};

	ecs_system_init(world, &(ecs_system_desc_t){.entity = {.name = "Render", .add = {EcsOnStore}},
	                                            .query.filter.terms_buffer_count = 22,
	                                            .query.filter.terms_buffer = termbuff,
	                                            .query.order_by_component = ecs_id(KrDrawable),
	                                            .query.order_by = compare_render_order,
	                                            .callback = Render});

	ECS_SYSTEM(world, End, EcsOnStore);
}
