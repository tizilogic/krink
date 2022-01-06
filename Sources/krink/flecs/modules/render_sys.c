#define SYSTEMS_RENDER_IMPL
#include "render_sys.h"
#include "render_comp.h"
#include <kinc/math/core.h>
#include <kinc/math/matrix.h>
#include <krink/graphics2/graphics.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>

static bool scissor_active = false;

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
	KrCompDrawable *drawable = ecs_term(it, KrCompDrawable, 1);
	KrCompPos2 *pos = ecs_term(it, KrCompPos2, 2);
	KrCompColor *color = ecs_term(it, KrCompColor, 3);
	KrCompImage *image = ecs_term(it, KrCompImage, 4);
	KrCompText *text = ecs_term(it, KrCompText, 5);
	KrCompStroke *stroke = ecs_term(it, KrCompStroke, 6);
	KrCompSmooth *smooth = ecs_term(it, KrCompSmooth, 7);
	KrCompTriangle *triangle = ecs_term(it, KrCompTriangle, 8);
	KrCompRect *rect = ecs_term(it, KrCompRect, 9);
	KrCompScissor *scissor = ecs_term(it, KrCompScissor, 10);
	KrCompCorner *corner = ecs_term(it, KrCompCorner, 11);
	KrCompCornerAsymm *corner_asymm = ecs_term(it, KrCompCornerAsymm, 12);
	KrCompCircle *circle = ecs_term(it, KrCompCircle, 13);
	KrCompBorder *border = ecs_term(it, KrCompBorder, 14);
	KrCompLine *line = ecs_term(it, KrCompLine, 15);
	KrCompAngle *angle = ecs_term(it, KrCompAngle, 16);
	KrCompRotationCenter *rotation_center = ecs_term(it, KrCompRotationCenter, 17);
	KrCompOpacity *opacity = ecs_term(it, KrCompOpacity, 18);
	KrCompScale *scale = ecs_term(it, KrCompScale, 19);
	KrCompScaleX *scale_x = ecs_term(it, KrCompScaleX, 20);
	KrCompScaleY *scale_y = ecs_term(it, KrCompScaleY, 21);
	KrCompTranslation *translation = ecs_term(it, KrCompTranslation, 22);

	for (int i = 0; i < it->count; ++i) {
		float border_strength = 0.0f;
		uint32_t border_color = 0;
		kr_matrix3x3_t transform = kr_matrix3x3_identity();

		float sx = 1.0f, sy = 1.0f;
		if (ecs_term_is_set(it, 22)) { // Translate
			kr_matrix3x3_t tmat = kr_matrix3x3_translation(translation[i].x, translation[i].y);
			transform = kr_matrix3x3_multmat(&transform, &tmat);
		}
		if (ecs_term_is_set(it, 19)) { // Scale
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale[i].value, scale[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale[i].value;
			sy = scale[i].value;
		}
		else if (ecs_term_is_set(it, 20) && ecs_term_is_set(it, 21)) { // Scale asymmetric
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale_x[i].value, scale_y[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale_x[i].value;
			sy = scale_y[i].value;
		}
		if (ecs_term_is_set(it, 16)) { // Rotate
			float cx, cy;
			if (ecs_term_is_set(it, 17)) {
				cx = rotation_center[i].x * sx;
				cy = rotation_center[i].y * sy;
			}
			else {
				float w, h;
				switch (drawable[i].pipeline) {
				case KR_COMP_PP_IMAGE: {
					w = image[i].dw * sx;
					h = image[i].dh * sy;
				} break;
				case KR_COMP_PP_TEXT: {
					w = kr_ttf_width(text[i].font, (int)(text[i].size + 0.5f), text[i].text) * sx;
					h = kr_ttf_height(text[i].font, (int)(text[i].size + 0.5f)) * sy;
				} break;
				case KR_COMP_PP_COLOR_TRIS_STROKE:
				case KR_COMP_PP_COLOR_TRIS_FILL: {
					w = kinc_max(triangle[i].x1, kinc_max(triangle[i].x2, triangle[i].x3)) -
					    kinc_min(triangle[i].x1, kinc_min(triangle[i].x2, triangle[i].x3));
					h = kinc_max(triangle[i].y1, kinc_max(triangle[i].y2, triangle[i].y3)) -
					    kinc_min(triangle[i].y1, kinc_min(triangle[i].y2, triangle[i].y3));
					w *= sx;
					h *= sy;
				} break;
				case KR_COMP_PP_COLOR_QUAD_STROKE:
				case KR_COMP_PP_COLOR_QUAD_FILL:
				case KR_COMP_PP_SDF_RECT_WBORDER:
				case KR_COMP_PP_SDF_RECT:
				case KR_COMP_PP_SDF_RECT_ASYMM_WBORDER:
				case KR_COMP_PP_SDF_RECT_ASYMM: {
					w = rect[i].w * sx;
					h = rect[i].h * sy;
				} break;
				case KR_COMP_PP_SDF_CIRCLE_WBORDER:
				case KR_COMP_PP_SDF_CIRCLE: {
					w = circle[i].radius * 2.0f * sx;
					h = circle[i].radius * 2.0f * sy;
				} break;
				case KR_COMP_PP_LINE:
				case KR_COMP_PP_SDF_LINE: {
					w = kinc_max(pos[i].x, line[i].x_to) - kinc_min(pos[i].x, line[i].x_to) * sx;
					h = kinc_max(pos[i].y, line[i].y_to) - kinc_min(pos[i].y, line[i].y_to) * sy;
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

#define DSHIFT 22
#define PSHIFT 16
#define EMOD 0x0000ffff

static int compare_render_order(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2,
                                const void *ptr2) {
	KrCompDrawable *d1 = (KrCompDrawable *)ptr1;
	KrCompDrawable *d2 = (KrCompDrawable *)ptr2;
	return (d1->depth << DSHIFT) + (d1->pipeline << PSHIFT) +
	       ((d1->sort_extra >> PSHIFT) ^ (d1->sort_extra & EMOD)) - (d2->depth << DSHIFT) +
	       (d2->pipeline << PSHIFT) + ((d2->sort_extra >> PSHIFT) ^ (d2->sort_extra & EMOD));
}

void SystemsRenderImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsRender);

	/* Register components */
	ECS_IMPORT(world, ComponentsRender);

	ECS_SYSTEM(world, Clear, EcsPreStore);

	ecs_term_t termbuff[23] = {
	    {ecs_id(KrCompDrawable)},
	    {ecs_id(KrCompPos2), .oper = EcsOptional},
	    {ecs_id(KrCompColor)},
	    {ecs_id(KrCompImage), .oper = EcsOptional},
	    {ecs_id(KrCompText), .oper = EcsOptional},
	    {ecs_id(KrCompStroke), .oper = EcsOptional},
	    {ecs_id(KrCompSmooth), .oper = EcsOptional},
	    {ecs_id(KrCompTriangle), .oper = EcsOptional},
	    {ecs_id(KrCompRect), .oper = EcsOptional},
	    {ecs_id(KrCompScissor), .oper = EcsOptional},
	    {ecs_id(KrCompCorner), .oper = EcsOptional},
	    {ecs_id(KrCompCornerAsymm), .oper = EcsOptional},
	    {ecs_id(KrCompCircle), .oper = EcsOptional},
	    {ecs_id(KrCompBorder), .oper = EcsOptional},
	    {ecs_id(KrCompLine), .oper = EcsOptional},
	    {ecs_id(KrCompAngle), .oper = EcsOptional},
	    {ecs_id(KrCompRotationCenter), .oper = EcsOptional},
	    {ecs_id(KrCompOpacity), .oper = EcsOptional},
	    {ecs_id(KrCompScale), .oper = EcsOptional},
	    {ecs_id(KrCompScaleX), .oper = EcsOptional},
	    {ecs_id(KrCompScaleY), .oper = EcsOptional},
	    {ecs_id(KrCompTranslation), .oper = EcsOptional},
	    {KrCompVisible},
	};

	ecs_system_init(world, &(ecs_system_desc_t){.entity = {.name = "Render", .add = {EcsOnStore}},
	                                            .query.filter.terms_buffer_count = 23,
	                                            .query.filter.terms_buffer = termbuff,
	                                            .query.order_by_component = ecs_id(KrCompDrawable),
	                                            .query.order_by = compare_render_order,
	                                            .callback = Render});

	ECS_SYSTEM(world, End, EcsOnStore);
}
