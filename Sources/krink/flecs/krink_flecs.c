#include "krink_flecs.h"
#include "internal/os_api.h"
#include "modules/animation_sys.h"
#include "modules/drag_comp.h"
#include "modules/drag_sys.h"
#include "modules/input_comp.h"
#include "modules/render_comp.h"
#include "modules/render_sys.h"
#include <assert.h>
#include <kinc/log.h>
#include <krink/eventhandler.h>
#include <stdbool.h>

ecs_world_t *kr_world;

/* Input */

static kr_evt_event_t evt_buffer[KR_FLECS_EVT_BUFFER_SIZE];
static int evt_count = 0;
static bool first_move = true;

static void evt_on_notify(kr_evt_event_t evt) {
	if (evt_count < KR_FLECS_EVT_BUFFER_SIZE) {
		evt_buffer[evt_count++] = evt;
		return;
	}
	kinc_log(KINC_LOG_LEVEL_ERROR, "Input buffer full");
}

static void InputSystem(ecs_iter_t *it) {
	KrSingletonInput *inp = ecs_field(it, KrSingletonInput, 1);

	for (int i = 0; i < 256; ++i) inp[0].keys[i].triggered = false;
	inp[0].mouse.primary.triggered = false;
	inp[0].mouse.right.triggered = false;

	for (int i = 0; i < evt_count; ++i) {
		switch (evt_buffer[i].event) {
		case KR_EVT_KEY_DOWN: {
			inp[0].keys[evt_buffer[i].data.key.keycode].down = true;
			inp[0].keys[evt_buffer[i].data.key.keycode].triggered = true;
		} break;
		case KR_EVT_KEY_UP: {
			inp[0].keys[evt_buffer[i].data.key.keycode].down = false;
			inp[0].keys[evt_buffer[i].data.key.keycode].triggered = true;
		} break;
		case KR_EVT_MOUSE_SCROLL: {
			inp[0].mouse.scroll.y = (float)evt_buffer[i].data.mouse_scroll.delta;
		} break;
		case KR_EVT_PRIMARY_MOVE: {
			if (!first_move) {
				inp[0].mouse.delta.x = (float)evt_buffer[i].data.primary.x - inp[0].mouse.pos.x;
				inp[0].mouse.delta.y = (float)evt_buffer[i].data.primary.y - inp[0].mouse.pos.y;
			}
			first_move = false;
			inp[0].mouse.pos.x = (float)evt_buffer[i].data.primary.x;
			inp[0].mouse.pos.y = (float)evt_buffer[i].data.primary.y;
		} break;
		case KR_EVT_PRIMARY_START: {
			inp[0].mouse.primary.down = true;
			inp[0].mouse.primary.triggered = true;
			inp[0].mouse.pos.x = (float)evt_buffer[i].data.primary.x;
			inp[0].mouse.pos.y = (float)evt_buffer[i].data.primary.y;
		} break;
		case KR_EVT_PRIMARY_END: {
			inp[0].mouse.primary.down = false;
			inp[0].mouse.primary.triggered = true;
			inp[0].mouse.pos.x = (float)evt_buffer[i].data.primary.x;
			inp[0].mouse.pos.y = (float)evt_buffer[i].data.primary.y;
		} break;
		case KR_EVT_MOUSE_PRESS: {
			if (evt_buffer[i].data.mouse_button.button != 1) break;
			inp[0].mouse.right.down = true;
			inp[0].mouse.right.triggered = true;
		} break;
		case KR_EVT_MOUSE_RELEASE: {
			if (evt_buffer[i].data.mouse_button.button != 1) break;
			inp[0].mouse.right.down = false;
			inp[0].mouse.right.triggered = true;
		} break;
		case KR_EVT_FOREGROUND:
		case KR_EVT_BACKGROUND:
		case KR_EVT_PAUSE:
		case KR_EVT_RESUME:
		case KR_EVT_SHUTDOWN:
		case KR_EVT_MOUSE_MOVE:
		case KR_EVT_FINGER_MOVE:
		case KR_EVT_FINGER_START:
		case KR_EVT_FINGER_END:
		case KR_EVT_KEY_PRESS:

		default:
			break;
		}
	}

	evt_count = 0;
}

/* Render */

static inline int32_t ptr2sort(void *img) {
	return (int32_t)((uint64_t)img % 214783648);
}

ecs_entity_t kr_flecs_create_sprite(ecs_world_t *world, const kr_init_sprite_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabSprite);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth,
	         .sort_extra = ptr2sort((void *)args->image),
	         .pipeline = KR_COMP_PP_IMAGE});
	ecs_set(world, e, KrPos2, {.x = args->dest.x, .y = args->dest.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrImage,
	        {.image = args->image,
	         .sx = args->src.x,
	         .sy = args->src.y,
	         .sw = args->src.w,
	         .sh = args->src.h,
	         .dw = args->dest.w,
	         .dh = args->dest.h,
	         .bilinear = args->bilinear});
	return e;
}

ecs_entity_t kr_flecs_create_text(ecs_world_t *world, const kr_init_text_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabText);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth,
	         .sort_extra = ptr2sort((void *)args->font),
	         .pipeline = KR_COMP_PP_TEXT});
	ecs_set(world, e, KrPos2, {.x = args->pos.x, .y = args->pos.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrText, {.font = args->font, .text = args->text, .size = args->size});
	return e;
}

ecs_entity_t kr_flecs_create_stroked_rect(ecs_world_t *world, const kr_init_stroked_rect_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabStrokedRect);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_QUAD_STROKE});
	ecs_set(world, e, KrPos2, {.x = args->rect.x, .y = args->rect.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrRect, {.w = args->rect.w, .h = args->rect.h});
	ecs_set(world, e, KrStroke, {.strength = args->stroke});
	return e;
}

ecs_entity_t kr_flecs_create_filled_rect(ecs_world_t *world, const kr_init_filled_rect_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabFilledRect);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_QUAD_FILL});
	ecs_set(world, e, KrPos2, {.x = args->rect.x, .y = args->rect.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrRect, {.w = args->rect.w, .h = args->rect.h});
	return e;
}

ecs_entity_t kr_flecs_create_stroked_triangle(ecs_world_t *world,
                                              const kr_init_stroked_triangle_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabStrokedTriangle);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_TRIS_STROKE});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrTriangle,
	        {.x1 = args->x1,
	         .y1 = args->y1,
	         .x2 = args->x2,
	         .y2 = args->y2,
	         .x3 = args->x3,
	         .y3 = args->y3});
	ecs_set(world, e, KrStroke, {.strength = args->stroke});
	return e;
}

ecs_entity_t kr_flecs_create_filled_triangle(ecs_world_t *world,
                                             const kr_init_filled_triangle_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabFilledTriangle);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_COLOR_TRIS_FILL});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrTriangle,
	        {.x1 = args->x1,
	         .y1 = args->y1,
	         .x2 = args->x2,
	         .y2 = args->y2,
	         .x3 = args->x3,
	         .y3 = args->y3});
	return e;
}

ecs_entity_t kr_flecs_create_line(ecs_world_t *world, const kr_init_line_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabLine);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_LINE});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrPos2, {.x = args->x1, .y = args->y1});
	ecs_set(world, e, KrLine, {.x_to = args->x2, .y_to = args->y2});
	ecs_set(world, e, KrStroke, {.strength = args->stroke});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_rect(ecs_world_t *world, const kr_init_sdf_rect_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabSdfRect);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT});
	ecs_set(world, e, KrPos2, {.x = args->rect.x, .y = args->rect.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrRect, {.w = args->rect.w, .h = args->rect.h});
	ecs_set(world, e, KrCorner, {.radius = args->corner});
	ecs_set(world, e, KrSmooth, {.px = args->smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_asymm_rect(ecs_world_t *world,
                                            const kr_init_sdf_asymm_rect_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabSdfRectAsymm);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_ASYMM});
	ecs_set(world, e, KrPos2, {.x = args->rect.x, .y = args->rect.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrRect, {.w = args->rect.w, .h = args->rect.h});
	ecs_set(world, e, KrCornerAsymm,
	        {.tl = args->corner.top_left,
	         .bl = args->corner.bottom_left,
	         .tr = args->corner.top_right,
	         .br = args->corner.bottom_right});
	ecs_set(world, e, KrSmooth, {.px = args->smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_rect_wborder(ecs_world_t *world,
                                              const kr_init_sdf_rect_wborder_t *args) {
	ecs_entity_t e = kr_flecs_create_sdf_rect(world, &args->sdf_rect);
	ecs_add(world, e, KrBorder);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_WBORDER});
	ecs_set(world, e, KrBorder, {.strength = args->border, .color = args->border_color});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_asymm_rect_wborder(ecs_world_t *world,
                                                    const kr_init_sdf_asymm_rect_wborder_t *args) {
	ecs_entity_t e = kr_flecs_create_sdf_asymm_rect(world, &args->sdf_asymm_rect);
	ecs_add(world, e, KrBorder);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_RECT_ASYMM_WBORDER});
	ecs_set(world, e, KrBorder, {.strength = args->border, .color = args->border_color});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_circle(ecs_world_t *world, const kr_init_sdf_circle_t *args) {
	ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, KrPrefabSdfCircle);
	ecs_add(world, e, KrVisible);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_CIRCLE});
	ecs_set(world, e, KrPos2, {.x = args->pos.x, .y = args->pos.y});
	ecs_set(world, e, KrColor, {.color = args->color});
	ecs_set(world, e, KrCircle, {.radius = args->radius});
	ecs_set(world, e, KrSmooth, {.px = args->smooth});
	return e;
}

ecs_entity_t kr_flecs_create_sdf_circle_wborder(ecs_world_t *world,
                                                const kr_init_sdf_circle_wborder_t *args) {
	ecs_entity_t e = kr_flecs_create_sdf_circle(world, &args->sdf_circle);
	ecs_add(world, e, KrBorder);

	ecs_set(world, e, KrDrawable,
	        {.depth = args->depth, .sort_extra = 0, .pipeline = KR_COMP_PP_SDF_CIRCLE_WBORDER});
	ecs_set(world, e, KrBorder, {.strength = args->border, .color = args->border_color});
	return e;
}

void kr_flecs_set_depth(ecs_world_t *world, ecs_entity_t e, int32_t depth) {
	bool is_added;
	KrDrawable *d = ecs_get_mut(world, e, KrDrawable);
	assert(!is_added);
	d->depth = depth;
	ecs_modified(world, e, KrDrawable);
}

/* Animation */

static ecs_entity_t entity_buffer[16];
static int entity_buffer_top = 0;

static void fill_entity_buffer(ecs_world_t *world) {
	if (entity_buffer_top == 16) return;
	while (entity_buffer_top < 16) {
		entity_buffer[entity_buffer_top++] = ecs_new_id(world);
	}

	// TODO: replace bubble with other sort algo if necessary
	for (int i = 0; i < 16; ++i)
		for (int j = 0; j < 15 - i; ++j)
			if (entity_buffer[j] < entity_buffer[j + 1]) {
				ecs_entity_t tmp = entity_buffer[j];
				entity_buffer[j] = entity_buffer[j + 1];
				entity_buffer[j + 1] = tmp;
			}
}

static void add_animation(ecs_world_t *world, ecs_entity_t e, ecs_entity_t anim_e,
                          const kr_init_animation_t *anim) {
	int i = 0;
	ecs_set(world, anim_e, KrAnimation,
	        {.start = anim->start,
	         .duration = anim->duration,
	         .ease = anim->ease,
	         .set_depth = anim->set_depth,
	         .depth_during = anim->depth_during,
	         .depth_after = anim->depth_after});

	for (int i = 0; i < 8; ++i) {
		switch (anim->modifiers[i].modifier) {
		case KR_MODIFIER_ANGLE: {
			ecs_set_pair(world, anim_e, KrAngle, KrAnimateToAngle, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_ROTATION_CENTER: {
			ecs_set_pair(world, anim_e, KrRotationCenter, KrAnimateRotationCenter,
			             {anim->modifiers[i].value, anim->modifiers[i].opt});
		} break;
		case KR_MODIFIER_X: {
			ecs_set_pair(world, anim_e, KrModifier, KrAnimateToX, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_Y: {
			ecs_set_pair(world, anim_e, KrModifier, KrAnimateToY, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_SCALE_X: {
			ecs_set_pair(world, anim_e, KrScaleX, KrAnimateToScaleX, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_SCALE_Y: {
			ecs_set_pair(world, anim_e, KrScaleY, KrAnimateToScaleY, {anim->modifiers[i].value});
		} break;
		case KR_MODIFIER_OPACITY: {
			ecs_set_pair(world, anim_e, KrOpacity, KrAnimateToOpacity, {anim->modifiers[i].value});
		} break;

		default:
			break;
		}
	}
	ecs_add_pair(world, e, KrAnimate, anim_e);
}

void kr_flecs_create_animation(ecs_world_t *world, ecs_entity_t e, const kr_init_animation_t *anim,
                               const KrCallback *callback) {
	fill_entity_buffer(world);
	ecs_entity_t anim_e = entity_buffer[--entity_buffer_top];
	if (anim->loop) {
		ecs_set_pair(world, anim_e, KrOffset, KrAnimateLoop, {anim->duration});
	}
	if (callback != NULL) {
		ecs_set(world, anim_e, KrCallback,
		        {.before = callback->before,
		         .before_param = callback->before_param,
		         .after = callback->after,
		         .after_param = callback->after_param});
	}
	add_animation(world, e, anim_e, anim);
}

void kr_flecs_create_sequence(ecs_world_t *world, ecs_entity_t e,
                              const kr_init_sequence_t *sequence, const KrCallback *callback) {
	assert(sequence->count > 0 && sequence->count <= 16);
	fill_entity_buffer(world);
	double offset = 0.0;
	if (sequence->loop) {
		kr_init_animation_t a = sequence->animations[sequence->count - 1];
		offset = a.start + a.duration;
	}
	for (int i = 0; i < sequence->count; ++i) {
		ecs_entity_t anim_e = entity_buffer[--entity_buffer_top];
		kr_init_animation_t a = sequence->animations[i];
		a.loop = sequence->loop;
		if (a.loop) {
			ecs_set_pair(world, anim_e, KrOffset, KrAnimateLoop, {offset});
		}
		if (callback != NULL && i == 0 && callback->before != NULL) {
			ecs_set(world, anim_e, KrCallback,
			        {.before = callback->before,
			         .before_param = callback->before_param,
			         .after = NULL,
			         .after_param = NULL});
		}
		if (callback != NULL && i == sequence->count - 1 && callback->after != NULL) {
			ecs_set(world, anim_e, KrCallback,
			        {.before = NULL,
			         .before_param = NULL,
			         .after = callback->after,
			         .after_param = callback->after_param});
		}
		add_animation(world, e, anim_e, &a);
	}
}

/* Init/Destroy/Tick */

void kr_flecs_init(bool with_flecs_rest) {
#if !defined(__EMSCRIPTEN__)
	kr_set_flecs_os_api();
#endif
	kr_world = ecs_init();
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
	if (with_flecs_rest) ecs_singleton_set(kr_world, EcsRest, {0});
#endif
	ECS_IMPORT(kr_world, ComponentsInput);
	ECS_IMPORT(kr_world, ComponentsRender);
	ECS_IMPORT(kr_world, SystemsRender);
	ECS_IMPORT(kr_world, ComponentsDrag);
	ECS_IMPORT(kr_world, SystemsDrag);
	ECS_IMPORT(kr_world, ComponentsAnimation);
	ECS_IMPORT(kr_world, SystemsAnimation);
	ecs_singleton_set(kr_world, KrSingletonClearColor, {.color = 0});

	kr_evt_add_observer(evt_on_notify);
	ecs_singleton_set(kr_world, KrSingletonInput, {0});
	ECS_SYSTEM(kr_world, InputSystem, EcsOnLoad, KrSingletonInput);
	kinc_log(KINC_LOG_LEVEL_INFO, "flecs initialized");
}

void kr_flecs_destroy(void) {
	ecs_fini(kr_world);
	kr_world = NULL;
}

void kr_flecs_tick(void) {
	ecs_progress(kr_world, 0);
}
