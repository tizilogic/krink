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

		ecs_entity_t anim_e = ecs_pair_object(it->world, ecs_term_id(&rit, 5));
		bool active = ecs_has(it->world, anim_e, KrInternalAnimationActive);

		for (int i = 0; i < rit.count; ++i) {
			double k = (t->time - animation[i].start) / animation[i].duration;
			float start_angle = anim_has_angle_from ? angle_from[i].radians : 0.0f;
			if (k < 1.0 && k >= 0.0) {
				if (!active) {
					ecs_add(it->world, anim_e, KrInternalAnimationActive);
					start_angle = has_angle ? angle[i].radians : 0.0;
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
				k = 1.0;
				angle[i].radians = angle_to[i].radians;
				continue;
			}
			else
				continue;
			angle[i].radians =
			    kr_tween(animation[i].ease, k) * (angle_to[i].radians - start_angle) + start_angle;
		}
	}
}

static void SystemAnimatePos(ecs_iter_t *it) {}

static void SystemAnimateScaleX(ecs_iter_t *it) {}

static void SystemAnimateScaleY(ecs_iter_t *it) {}

static void SystemAnimateOpacity(ecs_iter_t *it) {}

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
	       ",components.animation.KrAnimation(_A, components.animation.KrAnimate)"
	       ",?components.render.KrTranslation"
	       ",(components.animation.KrAnimate, _A)"
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
	       ",components.animation.KrAnimation(_A, components.animation.KrAnimate)"
	       ",?components.render.KrScaleX"
	       ",(components.animation.KrAnimate, _A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",?components.render.KrScaleX(_A, components.animation.KrAnimateToScaleX)"
	       ",?components.render.KrScaleX(_A, systems.animation.KrInternalAnimateFromScaleX)";
	anim_scale_x_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleX", .add = EcsOnUpdate},
	                                .ctx = anim_scale_x_rule,
	                                .callback = SystemAnimateScaleX});

	expr = "components.render.KrDrawable"
	       ",components.animation.KrAnimation(_A, components.animation.KrAnimate)"
	       ",?components.render.KrScaleY"
	       ",(components.animation.KrAnimate, _A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",?components.render.KrScaleY(_A, components.animation.KrAnimateToScaleY)"
	       ",?components.render.KrScaleY(_A, systems.animation.KrInternalAnimateFromScaleY)";
	anim_scale_y_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(
	    world, &(ecs_system_desc_t){.entity = {.name = "SystemAnimationScaleY", .add = EcsOnUpdate},
	                                .ctx = anim_scale_y_rule,
	                                .callback = SystemAnimateScaleY});

	expr = "components.render.KrDrawable"
	       ",components.animation.KrAnimation(_A, components.animation.KrAnimate)"
	       ",?components.render.KrOpacity"
	       ",(components.animation.KrAnimate, _A)"
	       ",?(_A, systems.animation.KrInternalAnimationActive)"
	       ",?components.render.KrOpacity(_A, components.animation.KrAnimateToOpacity)"
	       ",?components.render.KrOpacity(_A, systems.animation.KrInternalAnimateFromOpacity)";
	anim_opacity_rule = ecs_rule_init(world, &(ecs_filter_desc_t){.expr = expr});
	ecs_system_init(world, &(ecs_system_desc_t){
	                           .entity = {.name = "SystemAnimationOpacity", .add = EcsOnUpdate},
	                           .ctx = anim_opacity_rule,
	                           .callback = SystemAnimateOpacity});
}
