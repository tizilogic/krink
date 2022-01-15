#define SYSTEMS_ANIMATION_IMPL
#include "animation_sys.h"
#include "animation_comp.h"
#include "render_comp.h"
#include <assert.h>
#include <kinc/log.h>
#include <krink/util/tween.h>

ECS_DECLARE(KrInternalAnimationActive);
ECS_DECLARE(KrInternalAnimationDelete);
ECS_DECLARE(KrInternalAnimateFromX);
ECS_DECLARE(KrInternalAnimateFromY);
ECS_DECLARE(KrInternalAnimateFromAngle);
ECS_DECLARE(KrInternalAnimateFromScaleX);
ECS_DECLARE(KrInternalAnimateFromScaleY);
ECS_DECLARE(KrInternalAnimateFromOpacity);

static ecs_rule_t *anim_angle_rule;
static ecs_rule_t *anim_pos_rule;
static ecs_rule_t *anim_scale_x_rule;
static ecs_rule_t *anim_scale_y_rule;
static ecs_rule_t *anim_opacity_rule;

static void SystemAnimateAngle(ecs_iter_t *it) {
	ecs_iter_t rit = ecs_rule_iter(it->world, it->ctx);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	while (ecs_iter_next(&rit)) {
		bool has_angle = ecs_term_is_set(&rit, 2);
		bool has_rotation_center = ecs_term_is_set(&rit, 3);
		bool anim_has_angle_from = ecs_term_is_set(&rit, 8);
		bool anim_has_rotation_center = ecs_term_is_set(&rit, 9);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 5);
		KrAngle *angle = ecs_term(&rit, KrAngle, 2);
		KrRotationCenter *rotation_center;
		if (has_rotation_center) rotation_center = ecs_term(&rit, KrRotationCenter, 3);
		KrAngle *angle_to = ecs_term(&rit, KrAngle, 7);
		KrAngle *angle_from;
		if (anim_has_angle_from) angle_from = ecs_term(&rit, KrAngle, 8);
		KrRotationCenter *anim_rotation_center;
		if (anim_has_rotation_center) anim_rotation_center = ecs_term(&rit, KrRotationCenter, 9);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 4));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			double k = (t->time - animation[i].start) / animation[i].duration;
			float start_angle =
			    anim_has_angle_from ? angle_from[i].radians : (has_angle ? angle[i].radians : 0.0f);
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					ecs_set_pair(it->world, anim_e, KrAngle, KrInternalAnimateFromAngle,
					             {start_angle});
					if (anim_has_rotation_center)
						ecs_set(it->world, rit.entities[i], KrRotationCenter,
						        {anim_rotation_center[i].x, anim_rotation_center[i].y});
				}
			}
			else if (active && k >= 1.0) {
				ecs_remove(it->world, anim_e, KrInternalAnimationActive);
				ecs_add(it->world, anim_e, KrInternalAnimationDelete);
				angle[i].radians = angle_to[i].radians;
				continue;
			}
			else
				continue;

			float radians =
			    kr_tween(animation[i].ease, k) * (angle_to[i].radians - start_angle) + start_angle;
			if (has_angle) {
				angle[i].radians = radians;
			}
			else {
				ecs_set(it->world, rit.entities[i], KrAngle, {radians});
			}
		}
	}
}

static void SystemAnimatePos(ecs_iter_t *it) {
	ecs_iter_t rit = ecs_rule_iter(it->world, it->ctx);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	while (ecs_iter_next(&rit)) {
		bool anim_has_to_x = ecs_term_is_set(&rit, 6);
		bool anim_has_to_y = ecs_term_is_set(&rit, 7);
		if (!anim_has_to_x && !anim_has_to_y) continue;
		bool has_translation = ecs_term_is_set(&rit, 2);
		bool anim_has_from_x = ecs_term_is_set(&rit, 8);
		bool anim_has_from_y = ecs_term_is_set(&rit, 9);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrTranslation *translation = has_translation ? ecs_term(&rit, KrTranslation, 2) : NULL;
		KrModifier *to_x = anim_has_to_x ? ecs_term(&rit, KrModifier, 6) : NULL;
		KrModifier *to_y = anim_has_to_y ? ecs_term(&rit, KrModifier, 7) : NULL;
		KrModifier *from_x = anim_has_from_x ? ecs_term(&rit, KrModifier, 8) : NULL;
		KrModifier *from_y = anim_has_from_y ? ecs_term(&rit, KrModifier, 9) : NULL;

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_x =
			    anim_has_from_x ? from_x[i].v : (has_translation ? translation[i].x : 0.0f);
			float start_y =
			    anim_has_from_y ? from_y[i].v : (has_translation ? translation[i].y : 0.0f);
			float end_x = anim_has_to_x ? to_x[i].v : (has_translation ? translation[i].x : 0.0f);
			float end_y = anim_has_to_y ? to_y[i].v : (has_translation ? translation[i].y : 0.0f);

			double k = (t->time - animation[i].start) / animation[i].duration;
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					ecs_set_pair(it->world, anim_e, KrModifier, KrInternalAnimateFromX, {start_x});
					ecs_set_pair(it->world, anim_e, KrModifier, KrInternalAnimateFromY, {start_y});
					ecs_set(it->world, rit.entities[i], KrTranslation, {start_x, start_y});
				}
			}
			else if (active && k >= 1.0) {
				ecs_remove(it->world, anim_e, KrInternalAnimationActive);
				ecs_add(it->world, anim_e, KrInternalAnimationDelete);
				translation[i].x = end_x;
				translation[i].y = end_y;
				continue;
			}
			else
				continue;

			float x = kr_tween(animation[i].ease, k) * (end_x - start_x) + start_x;
			float y = kr_tween(animation[i].ease, k) * (end_y - start_y) + start_y;
			if (has_translation) {
				translation[i].x = x;
				translation[i].y = y;
			}
			else {
				ecs_set(it->world, rit.entities[i], KrTranslation, {x, y});
			}
		}
	}
}

static void SystemAnimateScaleX(ecs_iter_t *it) {
	ecs_iter_t rit = ecs_rule_iter(it->world, it->ctx);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	while (ecs_iter_next(&rit)) {
		bool has_scale_x = ecs_term_is_set(&rit, 2);
		bool anim_has_from_x = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrScaleX *scale_x = has_scale_x ? ecs_term(&rit, KrScaleX, 2) : NULL;
		KrScaleX *to_scale_x = ecs_term(&rit, KrScaleX, 6);
		KrScaleX *from_scale_x = anim_has_from_x ? ecs_term(&rit, KrScaleX, 7) : NULL;

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_x =
			    anim_has_from_x ? from_scale_x[i].value : (has_scale_x ? scale_x[i].value : 1.0f);
			float end_x = to_scale_x[i].value;

			double k = (t->time - animation[i].start) / animation[i].duration;
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					ecs_set_pair(it->world, anim_e, KrScaleX, KrInternalAnimateFromScaleX,
					             {start_x});
					ecs_set(it->world, rit.entities[i], KrScaleX, {start_x});
				}
			}
			else if (active && k >= 1.0) {
				ecs_remove(it->world, anim_e, KrInternalAnimationActive);
				ecs_add(it->world, anim_e, KrInternalAnimationDelete);
				scale_x[i].value = end_x;
				continue;
			}
			else
				continue;

			float x = kr_tween(animation[i].ease, k) * (end_x - start_x) + start_x;
			if (has_scale_x) {
				scale_x[i].value = x;
			}
			else {
				ecs_set(it->world, rit.entities[i], KrScaleX, {x});
			}
		}
	}
}

static void SystemAnimateScaleY(ecs_iter_t *it) {
	ecs_iter_t rit = ecs_rule_iter(it->world, it->ctx);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	while (ecs_iter_next(&rit)) {
		bool has_scale_y = ecs_term_is_set(&rit, 2);
		bool anim_has_from_y = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrScaleY *scale_y = has_scale_y ? ecs_term(&rit, KrScaleY, 2) : NULL;
		KrScaleY *to_scale_y = ecs_term(&rit, KrScaleY, 6);
		KrScaleY *from_scale_x = anim_has_from_y ? ecs_term(&rit, KrScaleY, 7) : NULL;

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_y =
			    anim_has_from_y ? from_scale_x[i].value : (has_scale_y ? scale_y[i].value : 1.0f);
			float end_y = to_scale_y[i].value;

			double k = (t->time - animation[i].start) / animation[i].duration;
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					ecs_set_pair(it->world, anim_e, KrScaleY, KrInternalAnimateFromScaleY,
					             {start_y});
					ecs_set(it->world, rit.entities[i], KrScaleY, {start_y});
				}
			}
			else if (active && k >= 1.0) {
				ecs_remove(it->world, anim_e, KrInternalAnimationActive);
				ecs_add(it->world, anim_e, KrInternalAnimationDelete);
				scale_y[i].value = end_y;
				continue;
			}
			else
				continue;

			float y = kr_tween(animation[i].ease, k) * (end_y - start_y) + start_y;
			if (has_scale_y) {
				scale_y[i].value = y;
			}
			else {
				ecs_set(it->world, rit.entities[i], KrScaleY, {y});
			}
		}
	}
}

#define clamp(x) ((x) < 0.0f) ? 0.0f : (((x) > 1.0f) ? 1.0f : (x))

static void SystemAnimateOpacity(ecs_iter_t *it) {
	ecs_iter_t rit = ecs_rule_iter(it->world, it->ctx);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	while (ecs_iter_next(&rit)) {
		bool has_opacity = ecs_term_is_set(&rit, 2);
		bool anim_has_from_opacity = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrOpacity *opacity = has_opacity ? ecs_term(&rit, KrOpacity, 2) : NULL;
		KrOpacity *to_opacity = ecs_term(&rit, KrOpacity, 6);
		KrOpacity *from_opacity = anim_has_from_opacity ? ecs_term(&rit, KrOpacity, 7) : NULL;

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start = anim_has_from_opacity ? from_opacity[i].alpha
			                                    : (has_opacity ? opacity[i].alpha : 1.0f);
			float end = to_opacity[i].alpha;

			double k = (t->time - animation[i].start) / animation[i].duration;
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					ecs_set_pair(it->world, anim_e, KrOpacity, KrInternalAnimateFromOpacity,
					             {start});
					ecs_set(it->world, rit.entities[i], KrOpacity, {start});
				}
			}
			else if (active && k >= 1.0) {
				ecs_remove(it->world, anim_e, KrInternalAnimationActive);
				ecs_add(it->world, anim_e, KrInternalAnimationDelete);
				opacity[i].alpha = end;
				continue;
			}
			else
				continue;

			float alpha = kr_tween(animation[i].ease, k) * (end - start) + start;
			alpha = clamp(alpha);
			if (has_opacity) {
				opacity[i].alpha = alpha;
			}
			else {
				ecs_set(it->world, rit.entities[i], KrOpacity, {alpha});
			}
		}
	}
}

static void AddAnimation(ecs_iter_t *it) {
	KrAnimation *anim = ecs_term(it, KrAnimation, 1);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	for (int i = 0; i < it->count; ++i) {
		anim[i].start += t->time;
	}
}

static void DeleteAnimation(ecs_iter_t *it) {
	for (int i = 0; i < it->count; ++i) ecs_delete(it->world, it->entities[i]);
}

void SystemsAnimationImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsAnimation);
	ECS_IMPORT(world, ComponentsAnimation);
	ECS_IMPORT(world, ComponentsRender);

	/* Register internal components */
	ECS_TAG_DEFINE(world, KrInternalAnimationActive);
	ECS_TAG_DEFINE(world, KrInternalAnimationDelete);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromX);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromY);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromAngle);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromScaleX);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromScaleY);
	ECS_TAG_DEFINE(world, KrInternalAnimateFromOpacity);

	/* Register systems */
	ECS_TRIGGER(world, AddAnimation, EcsOnSet, components.animation.KrAnimation);
	ECS_SYSTEM(world, DeleteAnimation, EcsPostUpdate, components.animation.KrAnimation,
	           KrInternalAnimationDelete);

	const char *expr =
	    "components.render.KrDrawable"
	    ",?components.render.KrAngle"
	    ",?components.render.KrRotationCenter"
	    ",(components.animation.KrAnimate, _A)"
	    ",components.animation.KrAnimation(_A)"
	    ",?(_A, systems.animation.KrInternalAnimationActive)"
	    ",components.render.KrAngle(_A, components.animation.KrAnimateToAngle)"
	    ",?components.render.KrAngle(_A, systems.animation.KrInternalAnimateFromAngle)"
	    ",?components.render.KrRotationCenter(_A, components.animation.KrAnimateRotationCenter)";
	anim_angle_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationAngle", .add = EcsOnUpdate},
	                                .ctx = anim_angle_rule,
	                                .callback = SystemAnimateAngle});

	expr = "components.render.KrDrawable"
	       ",?components.render.KrTranslation"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",?components.animation.KrModifier(_A, components.animation.KrAnimateToX)"
	       ",?components.animation.KrModifier(_A, components.animation.KrAnimateToY)"
	       ",?components.animation.KrModifier(_A, systems.animation.KrInternalAnimateFromX)"
	       ",?components.animation.KrModifier(_A, systems.animation.KrInternalAnimateFromY)";
	anim_pos_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationPos", .add = EcsOnUpdate},
	                                .ctx = anim_pos_rule,
	                                .callback = SystemAnimatePos});

	expr = "components.render.KrDrawable"
	       ",?components.render.KrScaleX"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrScaleX(_A, components.animation.KrAnimateToScaleX)"
	       ",?components.render.KrScaleX(_A, systems.animation.KrInternalAnimateFromScaleX)";
	anim_scale_x_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleX", .add = EcsOnUpdate},
	                                .ctx = anim_scale_x_rule,
	                                .callback = SystemAnimateScaleX});

	expr = "components.render.KrDrawable"
	       ",?components.render.KrScaleY"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrScaleY(_A, components.animation.KrAnimateToScaleY)"
	       ",?components.render.KrScaleY(_A, systems.animation.KrInternalAnimateFromScaleY)";
	anim_scale_y_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleY", .add = EcsOnUpdate},
	                                .ctx = anim_scale_y_rule,
	                                .callback = SystemAnimateScaleY});

	expr = "components.render.KrDrawable"
	       ",?components.render.KrOpacity"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrOpacity(_A, components.animation.KrAnimateToOpacity)"
	       ",?components.render.KrOpacity(_A, systems.animation.KrInternalAnimateFromOpacity)";
	anim_opacity_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(world, &(ecs_system_desc_t){
	                           .entity = {.name = "SystemAnimationOpacity", .add = EcsOnUpdate},
	                           .ctx = anim_opacity_rule,
	                           .callback = SystemAnimateOpacity});
}
