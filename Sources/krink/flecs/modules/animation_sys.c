#define SYSTEMS_ANIMATION_IMPL
#include "animation_sys.h"
#include "animation_comp.h"
#include "render_comp.h"
#include <assert.h>
#include <kinc/log.h>
#include <krink/util/tween.h>

ECS_DECLARE(KrInternalAnimationActive);
ECS_DECLARE(KrInternalAnimationDelete);
ECS_DECLARE(KrInternalAnimationReset);
ECS_DECLARE(KrInternalAnimationProgress);
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

	while (ecs_iter_next(&rit)) {
		bool has_angle = ecs_term_is_set(&rit, 2);
		bool has_rotation_center = ecs_term_is_set(&rit, 3);
		bool anim_has_angle_from = ecs_term_is_set(&rit, 8);
		bool anim_has_rotation_center = ecs_term_is_set(&rit, 9);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 5);
		KrAngle *angle = ecs_term(&rit, KrAngle, 2);
		KrRotationCenter *rotation_center = ecs_term(&rit, KrRotationCenter, 3);
		KrAngle *angle_to = ecs_term(&rit, KrAngle, 7);
		KrAngle *angle_from = ecs_term(&rit, KrAngle, 8);
		KrRotationCenter *anim_rotation_center = ecs_term(&rit, KrRotationCenter, 9);
		KrOffset *anim_loop_offset = ecs_term(&rit, KrOffset, 10);
		KrModifier *mod = ecs_term(&rit, KrModifier, 11);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 4));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_angle =
			    anim_has_angle_from ? angle_from[i].radians : (has_angle ? angle[i].radians : 0.0f);
			if (mod[i].v <= 1.0f && mod[i].v >= 0.0f) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					if (!ecs_has_pair(it->world, anim_e, ecs_id(KrAngle),
					                  KrInternalAnimateFromAngle))
						ecs_set_pair(it->world, anim_e, KrAngle, KrInternalAnimateFromAngle,
						             {start_angle});

					if (anim_has_rotation_center)
						ecs_set(it->world, rit.entities[i], KrRotationCenter,
						        {anim_rotation_center[i].x, anim_rotation_center[i].y});
				}
			}
			else
				continue;

			float radians =
			    kr_tween(animation[i].ease, mod[i].v) * (angle_to[i].radians - start_angle) +
			    start_angle;
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

	while (ecs_iter_next(&rit)) {
		bool anim_has_to_x = ecs_term_is_set(&rit, 6);
		bool anim_has_to_y = ecs_term_is_set(&rit, 7);
		if (!anim_has_to_x && !anim_has_to_y) continue;
		bool has_translation = ecs_term_is_set(&rit, 2);
		bool anim_has_from_x = ecs_term_is_set(&rit, 8);
		bool anim_has_from_y = ecs_term_is_set(&rit, 9);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrTranslation *translation = ecs_term(&rit, KrTranslation, 2);
		KrModifier *to_x = ecs_term(&rit, KrModifier, 6);
		KrModifier *to_y = ecs_term(&rit, KrModifier, 7);
		KrModifier *from_x = ecs_term(&rit, KrModifier, 8);
		KrModifier *from_y = ecs_term(&rit, KrModifier, 9);
		KrOffset *anim_loop_offset = ecs_term(&rit, KrOffset, 10);
		KrModifier *mod = ecs_term(&rit, KrModifier, 11);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_x =
			    anim_has_from_x ? from_x[i].v : (has_translation ? translation[i].x : 0.0f);
			float start_y =
			    anim_has_from_y ? from_y[i].v : (has_translation ? translation[i].y : 0.0f);
			float end_x = anim_has_to_x ? to_x[i].v : (has_translation ? translation[i].x : 0.0f);
			float end_y = anim_has_to_y ? to_y[i].v : (has_translation ? translation[i].y : 0.0f);

			if (mod[i].v <= 1.0f && mod[i].v >= 0.0f) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);

					if (!ecs_has_pair(it->world, anim_e, ecs_id(KrModifier),
					                  KrInternalAnimateFromX))
						ecs_set_pair(it->world, anim_e, KrModifier, KrInternalAnimateFromX,
						             {start_x});
					if (!ecs_has_pair(it->world, anim_e, ecs_id(KrModifier),
					                  KrInternalAnimateFromY))
						ecs_set_pair(it->world, anim_e, KrModifier, KrInternalAnimateFromY,
						             {start_y});

					ecs_set(it->world, rit.entities[i], KrTranslation, {start_x, start_y});
				}
			}
			else
				continue;

			float x = kr_tween(animation[i].ease, mod[i].v) * (end_x - start_x) + start_x;
			float y = kr_tween(animation[i].ease, mod[i].v) * (end_y - start_y) + start_y;
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

	while (ecs_iter_next(&rit)) {
		bool has_scale_x = ecs_term_is_set(&rit, 2);
		bool anim_has_from_x = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrScaleX *scale_x = has_scale_x ? ecs_term(&rit, KrScaleX, 2) : NULL;
		KrScaleX *to_scale_x = ecs_term(&rit, KrScaleX, 6);
		KrScaleX *from_scale_x = anim_has_from_x ? ecs_term(&rit, KrScaleX, 7) : NULL;
		KrOffset *anim_loop_offset = ecs_term(&rit, KrOffset, 8);
		KrModifier *mod = ecs_term(&rit, KrModifier, 9);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_x =
			    anim_has_from_x ? from_scale_x[i].value : (has_scale_x ? scale_x[i].value : 1.0f);
			float end_x = to_scale_x[i].value;

			if (mod[i].v <= 1.0f && mod[i].v >= 0.0f) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					if (!ecs_has_pair(it->world, anim_e, ecs_id(KrScaleX),
					                  KrInternalAnimateFromScaleX))
						ecs_set_pair(it->world, anim_e, KrScaleX, KrInternalAnimateFromScaleX,
						             {start_x});

					ecs_set(it->world, rit.entities[i], KrScaleX, {start_x});
				}
			}
			else
				continue;

			float x = kr_tween(animation[i].ease, mod[i].v) * (end_x - start_x) + start_x;
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

	while (ecs_iter_next(&rit)) {
		bool has_scale_y = ecs_term_is_set(&rit, 2);
		bool anim_has_from_y = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrScaleY *scale_y = has_scale_y ? ecs_term(&rit, KrScaleY, 2) : NULL;
		KrScaleY *to_scale_y = ecs_term(&rit, KrScaleY, 6);
		KrScaleY *from_scale_x = anim_has_from_y ? ecs_term(&rit, KrScaleY, 7) : NULL;
		KrOffset *anim_loop_offset = ecs_term(&rit, KrOffset, 8);
		KrModifier *mod = ecs_term(&rit, KrModifier, 9);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start_y =
			    anim_has_from_y ? from_scale_x[i].value : (has_scale_y ? scale_y[i].value : 1.0f);
			float end_y = to_scale_y[i].value;

			if (mod[i].v <= 1.0f && mod[i].v >= 0.0f) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					if (!ecs_has_pair(it->world, anim_e, ecs_id(KrScaleY),
					                  KrInternalAnimateFromScaleY))
						ecs_set_pair(it->world, anim_e, KrScaleY, KrInternalAnimateFromScaleY,
						             {start_y});
					ecs_set(it->world, rit.entities[i], KrScaleY, {start_y});
				}
			}
			else
				continue;

			float y = kr_tween(animation[i].ease, mod[i].v) * (end_y - start_y) + start_y;
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

	while (ecs_iter_next(&rit)) {
		bool has_opacity = ecs_term_is_set(&rit, 2);
		bool anim_has_from_opacity = ecs_term_is_set(&rit, 7);

		KrAnimation *animation = ecs_term(&rit, KrAnimation, 4);
		KrOpacity *opacity = has_opacity ? ecs_term(&rit, KrOpacity, 2) : NULL;
		KrOpacity *to_opacity = ecs_term(&rit, KrOpacity, 6);
		KrOpacity *from_opacity = anim_has_from_opacity ? ecs_term(&rit, KrOpacity, 7) : NULL;
		KrOffset *anim_loop_offset = ecs_term(&rit, KrOffset, 8);
		KrModifier *mod = ecs_term(&rit, KrModifier, 9);

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 3));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			float start = anim_has_from_opacity ? from_opacity[i].alpha
			                                    : (has_opacity ? opacity[i].alpha : 1.0f);
			float end = to_opacity[i].alpha;

			if (mod[i].v <= 1.0f && mod[i].v >= 0.0f) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					if (ecs_has_pair(it->world, anim_e, ecs_id(KrOpacity),
					                 KrInternalAnimateFromOpacity))
						ecs_set_pair(it->world, anim_e, KrOpacity, KrInternalAnimateFromOpacity,
						             {start});
					ecs_set(it->world, rit.entities[i], KrOpacity, {start});
				}
			}
			else
				continue;

			float alpha = kr_tween(animation[i].ease, mod[i].v) * (end - start) + start;
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
		ecs_set_pair(it->world, it->entities[i], KrModifier, KrInternalAnimationProgress, {-1.0f});
	}
}

static void DeleteAnimation(ecs_iter_t *it) {
	for (int i = 0; i < it->count; ++i) ecs_delete(it->world, it->entities[i]);
}

static void ResetAnimation(ecs_iter_t *it) {
	KrAnimation *anim = ecs_term(it, KrAnimation, 1);
	for (int i = 0; i < it->count; ++i) {
		ecs_remove(it->world, it->entities[i], KrInternalAnimationReset);
		const KrOffset *offset = ecs_get_pair(it->world, it->entities[i], KrOffset, KrAnimateLoop);
		anim[i].start += offset->t;
	}
}

void UpdateProgress(ecs_iter_t *it) {
	KrAnimation *animation = ecs_term(it, KrAnimation, 1);
	KrModifier *mod = ecs_term(it, KrModifier, 2);
	KrOffset *offset = ecs_term(it, KrOffset, 3);
	KrCallback *callback = ecs_term(it, KrCallback, 4);
	const KrFrameTime *t = ecs_singleton_get(it->world, KrFrameTime);

	for (int i = 0; i < it->count; ++i) {
		mod[i].v = (t->time - animation[i].start) / animation[i].duration;
		if (mod[i].v >= 1.0f) {
			mod[i].v = 1.0f;
			if (ecs_term_is_set(it, 3)) {
				ecs_add(it->world, it->entities[i], KrInternalAnimationReset);
				// Make sure correct progress is saved when looping animations
				if (t->time >= animation[i].start + offset[i].t &&
				    t->time <= animation[i].start + offset[i].t + animation[i].duration) {
					mod[i].v =
					    (t->time - (animation[i].start + offset[i].t)) / animation[i].duration;
				}
			}
			else {
				ecs_add(it->world, it->entities[i], KrInternalAnimationDelete);
			}
		}
		// Handle callbacks
		if (ecs_term_is_set(it, 4)) {
			if (mod[i].v > 0.0f &&
			    !ecs_has(it->world, it->entities[i], KrInternalAnimationActive)) {
				if (callback[i].before)
					((kr_anim_callback)callback[i].before)(it->world, callback[i].before_param);
			}
			else if (mod[i].v >= 1.0f && callback[i].after)
				((kr_anim_callback)callback[i].after)(it->world, callback[i].after_param);
		}
	}
}

void SystemsAnimationImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsAnimation);
	ECS_IMPORT(world, ComponentsAnimation);
	ECS_IMPORT(world, ComponentsRender);

	/* Register internal components */
	ECS_TAG_DEFINE(world, KrInternalAnimationActive);
	ECS_TAG_DEFINE(world, KrInternalAnimationDelete);
	ECS_TAG_DEFINE(world, KrInternalAnimationReset);
	ECS_TAG_DEFINE(world, KrInternalAnimationProgress);
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
	ECS_SYSTEM(world, ResetAnimation, EcsPostUpdate, components.animation.KrAnimation,
	           KrInternalAnimationReset);
	ecs_system_init(world,
	                &(ecs_system_desc_t){
	                    .entity = {.name = "UpdateProgress", .add = EcsOnUpdate},
	                    .query.filter.terms =
	                        {
	                            {ecs_id(KrAnimation)},
	                            {ecs_pair(ecs_id(KrModifier), KrInternalAnimationProgress)},
	                            {ecs_pair(ecs_id(KrOffset), KrAnimateLoop), .oper = EcsOptional},
	                            {ecs_id(KrCallback), .oper = EcsOptional},
	                        },
	                    .callback = UpdateProgress});

	const char *expr =
	    "[in] components.render.KrDrawable"
	    ",?components.render.KrAngle"
	    ",?components.render.KrRotationCenter"
	    ",(components.animation.KrAnimate, _A)"
	    ",components.animation.KrAnimation(_A)"
	    ",?(_A, systems.animation.KrInternalAnimationActive)"
	    ",components.render.KrAngle(_A, components.animation.KrAnimateToAngle)"
	    ",?components.render.KrAngle(_A, systems.animation.KrInternalAnimateFromAngle)"
	    ",?components.render.KrRotationCenter(_A, components.animation.KrAnimateRotationCenter)"
	    ",?components.animation.KrOffset(_A, components.animation.KrAnimateLoop)"
	    ",components.animation.KrModifier(_A, systems.animation.KrInternalAnimationProgress)";
	anim_angle_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationAngle", .add = EcsOnUpdate},
	                                .ctx = anim_angle_rule,
	                                .callback = SystemAnimateAngle});

	expr = "[in] components.render.KrDrawable"
	       ",?components.render.KrTranslation"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",?components.animation.KrModifier(_A, components.animation.KrAnimateToX)"
	       ",?components.animation.KrModifier(_A, components.animation.KrAnimateToY)"
	       ",?components.animation.KrModifier(_A, systems.animation.KrInternalAnimateFromX)"
	       ",?components.animation.KrModifier(_A, systems.animation.KrInternalAnimateFromY)"
	       ",?components.animation.KrOffset(_A, components.animation.KrAnimateLoop)"
	       ",components.animation.KrModifier(_A, systems.animation.KrInternalAnimationProgress)";
	anim_pos_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationPos", .add = EcsOnUpdate},
	                                .ctx = anim_pos_rule,
	                                .callback = SystemAnimatePos});

	expr = "[in] components.render.KrDrawable"
	       ",?components.render.KrScaleX"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrScaleX(_A, components.animation.KrAnimateToScaleX)"
	       ",?components.render.KrScaleX(_A, systems.animation.KrInternalAnimateFromScaleX)"
	       ",?components.animation.KrOffset(_A, components.animation.KrAnimateLoop)"
	       ",components.animation.KrModifier(_A, systems.animation.KrInternalAnimationProgress)";
	anim_scale_x_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleX", .add = EcsOnUpdate},
	                                .ctx = anim_scale_x_rule,
	                                .callback = SystemAnimateScaleX});

	expr = "[in] components.render.KrDrawable"
	       ",?components.render.KrScaleY"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrScaleY(_A, components.animation.KrAnimateToScaleY)"
	       ",?components.render.KrScaleY(_A, systems.animation.KrInternalAnimateFromScaleY)"
	       ",?components.animation.KrOffset(_A, components.animation.KrAnimateLoop)"
	       ",components.animation.KrModifier(_A, systems.animation.KrInternalAnimationProgress)";
	anim_scale_y_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleY", .add = EcsOnUpdate},
	                                .ctx = anim_scale_y_rule,
	                                .callback = SystemAnimateScaleY});

	expr = "[in] components.render.KrDrawable"
	       ",?components.render.KrOpacity"
	       ",(components.animation.KrAnimate, _A)"
	       ",components.animation.KrAnimation(_A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",components.render.KrOpacity(_A, components.animation.KrAnimateToOpacity)"
	       ",?components.render.KrOpacity(_A, systems.animation.KrInternalAnimateFromOpacity)"
	       ",?components.animation.KrOffset(_A, components.animation.KrAnimateLoop)"
	       ",components.animation.KrModifier(_A, systems.animation.KrInternalAnimationProgress)";
	anim_opacity_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(world, &(ecs_system_desc_t){
	                           .entity = {.name = "SystemAnimationOpacity", .add = EcsOnUpdate},
	                           .ctx = anim_opacity_rule,
	                           .callback = SystemAnimateOpacity});
}
