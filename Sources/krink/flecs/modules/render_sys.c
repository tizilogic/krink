#define SYSTEMS_RENDER_IMPL
#include "render_sys.h"
#include "render_comp.h"
#include <krink/graphics2/graphics.h>

/* Implement a simple move system */
static void Render(ecs_iter_t *it) {
	ecs_world_t *world = it->world;
	kr_g2_begin(0);
	const KrSingletonClearColor *clear_color =
	    ecs_get(world, ecs_id(KrSingletonClearColor), KrSingletonClearColor);
	kr_g2_clear(clear_color->color);

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

	bool scissor_active = false;

	for (int i = 0; i < it->count; ++i) {
		float border_strength = 0.0f;
		uint32_t border_color = 0;

		kr_g2_set_color(color[i].color);
		if (ecs_term_is_set(it, 10)) {
			kr_g2_scissor(scissor[i].x, scissor[i].y, scissor[i].w, scissor[i].h);
		}
		else if (scissor_active)
			kr_g2_disable_scissor();

		// TODO: add and handle transform component

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
		case KR_COMP_PP_SDF_LINE: // TODO
		default:
			break;
		}
	}
	kr_g2_disable_scissor();
	kr_g2_end();
}

static int compare_render_order(ecs_entity_t e1, KrCompDrawable *d1, ecs_entity_t e2,
                                KrCompDrawable *d2) {
	return d1->depth < d2->depth
	           ? -1
	           : (d1->depth > d2->depth
	                  ? 1
	                  : (d1->pipeline < d2->pipeline
	                         ? -1
	                         : (d1->pipeline > d2->pipeline ? 1
	                                                        : d1->sort_extra - d2->sort_extra)));
}

void SystemsRenderImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsRender);

	/* Register components */
	ECS_IMPORT(world, ComponentsRender);

	ecs_system_init(world,
	                &(ecs_system_desc_t){.entity = {.name = "Render", .add = {EcsOnStore}},
	                                     .query.filter.terms =
	                                         {
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
	                                             {KrCompVisible},
	                                         },
	                                     .callback = Render});
}
