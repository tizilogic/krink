#define SYSTEMS_DRAG_IMPL
#include "drag_sys.h"
#include "drag_comp.h"
#include "input_comp.h"
#include "render_comp.h"
#include <assert.h>
#include <kinc/log.h>
#include <kinc/math/core.h>
#include <krink/math/matrix.h>
#include <krink/math/vector.h>
#include <stdbool.h>

ECS_DECLARE(KrInternalCanDrag);

static inline bool point_in_triangle(float px, float py, const KrTriangle *tr) {
	float s = (tr->x1 - tr->x3) * (py - tr->y3) - (tr->y1 - tr->y3) * (px - tr->x3);
	float t = (tr->x2 - tr->x1) * (py - tr->y1) - (tr->y2 - tr->y1) * (px - tr->x1);

	if ((s < 0) != (t < 0) && s != 0 && t != 0) return false;

	float d = (tr->x3 - tr->x2) * (py - tr->y2) - (tr->y3 - tr->y2) * (px - tr->x2);
	return d == 0 || (d < 0) == (s + t <= 0);
}

static inline bool point_in_rect(float px, float py, const KrPos2 *pos, const KrRect *rect) {
	float s = pos->x - px;
	float t = pos->y - py;
	return s >= 0.0f && s <= rect->w && t >= 0.0f && t <= rect->h;
}

static inline bool point_in_circle(float px, float py, const KrPos2 *pos, const KrCircle *circle) {
	float mag_r = circle->radius * circle->radius;
	float mag_p = (px - pos->x) * (px - pos->x) + (py - pos->y) * (py - pos->y);
	return mag_p <= mag_r;
}

#define MAXF 3.4028327e38

static inline KrDragAABB make_aabb(const kr_vec2_t *points, int count) {
	float xmin, xmax, ymin, ymax;
	xmin = ymin = MAXF;
	xmax = ymax = -MAXF;

	for (int i = 0; i < count; ++i) {
		xmin = kinc_min(xmin, points[i].x);
		xmax = kinc_max(xmax, points[i].x);
		ymin = kinc_min(ymin, points[i].y);
		ymax = kinc_max(ymax, points[i].y);
	}

	float hw = (xmax - xmin) * 0.5f;
	float hh = (ymax - ymin) * 0.5f;
	return (KrDragAABB){.x = xmin + hw, .y = ymin + hh, .hw = hw, .hh = hh};
}

static void UpdateAABB(ecs_iter_t *it) {
	if (!ecs_term_is_set(it, 15)) return;
	KrDrawable *drawable = ecs_term(it, KrDrawable, 1);
	KrPos2 *pos = ecs_term(it, KrPos2, 2);
	KrImage *image = ecs_term(it, KrImage, 3);
	KrTriangle *triangle = ecs_term(it, KrTriangle, 4);
	KrRect *rect = ecs_term(it, KrRect, 5);
	KrCircle *circle = ecs_term(it, KrCircle, 6);
	KrAngle *angle = ecs_term(it, KrAngle, 7);
	KrRotationCenter *rotation_center = ecs_term(it, KrRotationCenter, 8);
	KrScaleX *scale_x = ecs_term(it, KrScaleX, 9);
	KrScaleY *scale_y = ecs_term(it, KrScaleY, 10);
	KrTranslation *translation = ecs_term(it, KrTranslation, 11);
	KrDragAABB *drag_aabb = ecs_term(it, KrDragAABB, 12);

	for (int i = 0; i < it->count; ++i) {
		kr_matrix3x3_t transform = kr_matrix3x3_identity();

		float sx = 1.0f, sy = 1.0f;
		if (ecs_term_is_set(it, 11)) { // Translate
			kr_matrix3x3_t tmat = kr_matrix3x3_translation(translation[i].x, translation[i].y);
			transform = kr_matrix3x3_multmat(&transform, &tmat);
		}
		else {
			ecs_set(it->world, it->entities[i], KrTranslation, {.x = 0.0f, .y = 0.0f});
		}
		if (ecs_term_is_set(it, 9) && ecs_term_is_set(it, 10)) { // Scale asymmetric
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale_x[i].value, scale_y[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale_x[i].value;
			sy = scale_y[i].value;
		}
		else if (ecs_term_is_set(it, 9)) { // ScaleX
			kr_matrix3x3_t smat = kr_matrix3x3_scale(scale_x[i].value, 1.0f);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = scale_x[i].value;
			sy = 1.0f;
		}
		else if (ecs_term_is_set(it, 10)) { // ScaleY
			kr_matrix3x3_t smat = kr_matrix3x3_scale(1.0f, scale_y[i].value);
			transform = kr_matrix3x3_multmat(&transform, &smat);
			sx = 1.0f;
			sy = scale_y[i].value;
		}
		if (ecs_term_is_set(it, 7)) { // Rotate
			float cx, cy;
			if (ecs_term_is_set(it, 8)) {
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
				case KR_COMP_PP_COLOR_TRIS_STROKE:
				case KR_COMP_PP_COLOR_TRIS_FILL: {
					w = kinc_max(triangle[i].x1, kinc_max(triangle[i].x2, triangle[i].x3)) -
					    kinc_min(triangle[i].x1, kinc_min(triangle[i].x2, triangle[i].x3));
					h = kinc_max(triangle[i].y1, kinc_max(triangle[i].y2, triangle[i].y3)) -
					    kinc_min(triangle[i].y1, kinc_min(triangle[i].y2, triangle[i].y3));
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
				default: {
					kinc_log(KINC_LOG_LEVEL_INFO, "dropped through");
					continue;
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

		kr_vec2_t points[4];
		int count = 0;

		switch (drawable[i].pipeline) {
		case KR_COMP_PP_IMAGE: {
			points[0].x = pos[i].x;
			points[0].y = pos[i].y;
			points[1].x = pos[i].x + image[i].dw;
			points[1].y = pos[i].y;
			points[2].x = pos[i].x + image[i].dw;
			points[2].y = pos[i].y + image[i].dh;
			points[3].x = pos[i].x;
			points[3].y = pos[i].y + image[i].dh;
			count = 4;
		} break;
		case KR_COMP_PP_COLOR_TRIS_STROKE:
		case KR_COMP_PP_COLOR_TRIS_FILL: {
			points[0].x = triangle[i].x1;
			points[0].y = triangle[i].y1;
			points[1].x = triangle[i].x2;
			points[1].y = triangle[i].y2;
			points[2].x = triangle[i].x3;
			points[2].y = triangle[i].y3;
			count = 3;
		} break;
		case KR_COMP_PP_COLOR_QUAD_STROKE:
		case KR_COMP_PP_COLOR_QUAD_FILL:
		case KR_COMP_PP_SDF_RECT_WBORDER:
		case KR_COMP_PP_SDF_RECT:
		case KR_COMP_PP_SDF_RECT_ASYMM_WBORDER:
		case KR_COMP_PP_SDF_RECT_ASYMM: {
			points[0].x = pos[i].x;
			points[0].y = pos[i].y;
			points[1].x = pos[i].x + rect[i].w;
			points[1].y = pos[i].y;
			points[2].x = pos[i].x + rect[i].w;
			points[2].y = pos[i].y + rect[i].h;
			points[3].x = pos[i].x;
			points[3].y = pos[i].y + rect[i].h;
			count = 4;
		} break;
		case KR_COMP_PP_SDF_CIRCLE_WBORDER:
		case KR_COMP_PP_SDF_CIRCLE: {
			points[0].x = pos[i].x - circle[i].radius;
			points[0].y = pos[i].y - circle[i].radius;
			points[1].x = pos[i].x + circle[i].radius;
			points[1].y = pos[i].y - circle[i].radius;
			points[2].x = pos[i].x + circle[i].radius;
			points[2].y = pos[i].y + circle[i].radius;
			points[3].x = pos[i].x - circle[i].radius;
			points[3].y = pos[i].y + circle[i].radius;
			count = 4;
		} break;
		default:
			kinc_log(KINC_LOG_LEVEL_INFO, "dropped through 2");
			continue;
		}

		for (int j = 0; j < count; ++j) {
			points[j] = kr_matrix3x3_multvec(&transform, points[j]);
		}

		KrDragAABB aabb = make_aabb(points, count);

		if (ecs_term_is_set(it, 12)) {
			drag_aabb[i] = aabb;
		}
		else {
			ecs_set(it->world, it->entities[i], KrDragAABB,
			        {.x = aabb.x, .y = aabb.y, .hw = aabb.hw, .hh = aabb.hh});
		}
	}
}

static inline bool point_in_aabb(float px, float py, const KrDragAABB *aabb) {
	float s = kinc_abs(aabb->x - px);
	float t = kinc_abs(aabb->y - py);
	return s <= aabb->hw && t <= aabb->hh;
}

static void CheckDrag(ecs_iter_t *it) {
	if (!ecs_term_is_set(it, 6)) return;
	const KrSingletonInput *inp = ecs_singleton_get(it->world, KrSingletonInput);
	if (!(inp->mouse.primary.down && inp->mouse.primary.triggered)) return;

	KrDrawable *drawable = ecs_term(it, KrDrawable, 1);
	KrDragAABB *aabb = ecs_term(it, KrDragAABB, 2);
	KrTranslation *trans = ecs_term(it, KrTranslation, 3);

	for (int i = 0; i < it->count; ++i) {
		if (point_in_aabb(inp->mouse.pos.x, inp->mouse.pos.y, &aabb[i])) {
			ecs_set(it->world, it->entities[i], KrDragInfo,
			        {.start_pos.x = inp->mouse.pos.x,
			         .start_pos.y = inp->mouse.pos.y,
			         .start_trans = trans[i]});
			ecs_add(it->world, it->entities[i], KrDragActive);
			ecs_add(it->world, it->entities[i], KrDragStart);
			ecs_singleton_remove(it->world, KrInternalCanDrag);
			return;
		}
	}
}

static int compare_depth(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2, const void *ptr2) {
	return ((KrDrawable *)ptr2)->depth - ((KrDrawable *)ptr1)->depth;
}

static void UpdateDrag(ecs_iter_t *it) {
	const KrSingletonInput *inp = ecs_singleton_get(it->world, KrSingletonInput);
	KrDrawable *drawable = ecs_term(it, KrDrawable, 1);
	KrDragInfo *info = ecs_term(it, KrDragInfo, 2);
	KrTranslation *trans = ecs_term(it, KrTranslation, 3);

	if (ecs_has(it->world, it->entities[0], KrDragStart))
		ecs_remove(it->world, it->entities[0], KrDragStart);

	if (!ecs_term_is_set(it, 5)) {
		ecs_remove(it->world, it->entities[0], KrDragInfo);
		ecs_singleton_add(it->world, KrInternalCanDrag);
		trans[0].x = info[0].start_trans.x;
		trans[0].y = info[0].start_trans.y;
		return;
	}
	else if (!inp->mouse.primary.down) {
		ecs_remove(it->world, it->entities[0], KrDragActive);
		ecs_add(it->world, it->entities[0], KrDrop);
		ecs_singleton_add(it->world, KrInternalCanDrag);
	}
	trans[0].x = info[0].start_trans.x + (inp->mouse.pos.x - info[0].start_pos.x);
	trans[0].y = info[0].start_trans.y + (inp->mouse.pos.y - info[0].start_pos.y);
}

static void CleanDropped(ecs_iter_t *it) {
	KrDragInfo *info = ecs_term(it, KrDragInfo, 1);

	ecs_remove(it->world, it->entities[0], KrDragInfo);
	ecs_remove(it->world, it->entities[0], KrDrop);
}

void SystemsDragImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsDrag);
	ECS_IMPORT(world, ComponentsDrag);
	ECS_IMPORT(world, ComponentsInput);
	ECS_IMPORT(world, ComponentsRender);

	/* Define Internals */
	ECS_TAG_DEFINE(world, KrInternalCanDrag);

	/* Register systems */
	ECS_SYSTEM(world, CleanDropped, EcsOnUpdate, components.drag.KrDragInfo,
	           components.drag.KrDrop);

	ecs_term_t termbuff[15] = {
	    /*  1 */ {ecs_id(KrDrawable), .inout = EcsIn},
	    /*  2 */ {ecs_id(KrPos2), .oper = EcsOptional, .inout = EcsIn},
	    /*  3 */ {ecs_id(KrImage), .oper = EcsOptional, .inout = EcsIn},
	    /*  4 */ {ecs_id(KrTriangle), .oper = EcsOptional, .inout = EcsIn},
	    /*  5 */ {ecs_id(KrRect), .oper = EcsOptional, .inout = EcsIn},
	    /*  6 */ {ecs_id(KrCircle), .oper = EcsOptional, .inout = EcsIn},
	    /*  7 */ {ecs_id(KrAngle), .oper = EcsOptional, .inout = EcsIn},
	    /*  8 */ {ecs_id(KrRotationCenter), .oper = EcsOptional, .inout = EcsIn},
	    /*  9 */ {ecs_id(KrScaleX), .oper = EcsOptional, .inout = EcsIn},
	    /* 10 */ {ecs_id(KrScaleY), .oper = EcsOptional, .inout = EcsIn},
	    /* 11 */ {ecs_id(KrTranslation), .oper = EcsOptional, .inout = EcsIn},
	    /* 12 */ {ecs_id(KrDragAABB), .oper = EcsOptional, .inout = EcsInOut},
	    /* 13 */ {KrDragable},
	    /* 14 */ {KrVisible},
	    /* 15 */
	    {ecs_id(KrInternalCanDrag), .subj.entity = ecs_id(KrInternalCanDrag), .oper = EcsOptional},
	};

	ecs_system_init(world,
	                &(ecs_system_desc_t){.entity = {.name = "UpdateAABB", .add = {EcsOnUpdate}},
	                                     .query.filter.terms_buffer_count = 15,
	                                     .query.filter.terms_buffer = termbuff,
	                                     .callback = UpdateAABB});

	ecs_system_init(
	    world, &(ecs_system_desc_t){
	               .entity = {.name = "CheckDrag", .add = {EcsOnUpdate}},
	               .query.order_by_component = ecs_id(KrDrawable),
	               .query.order_by = compare_depth,
	               .query.filter.expr =
	                   "[in] components.render.KrDrawable, [in] components.drag.KrDragAABB, "
	                   "[in] components.render.KrTranslation, [in] components.render.KrVisible, "
	                   "[in] components.drag.KrDragable, ?KrInternalCanDrag(KrInternalCanDrag)",
	               .callback = CheckDrag});

	ecs_system_init(
	    world, &(ecs_system_desc_t){
	               .entity = {.name = "UpdateDrag", .add = {EcsOnUpdate}},
	               .query.filter.expr =
	                   "[in] components.render.KrDrawable, [in] components.drag.KrDragInfo, "
	                   "[out] components.render.KrTranslation, [in] components.render.KrVisible, "
	                   "[in] ?components.drag.KrDragActive, !components.drag.KrDrop",
	               .callback = UpdateDrag});

	/* Enable Dragging */
	ecs_singleton_add(world, KrInternalCanDrag);
}
