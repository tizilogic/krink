#pragma once
#include <krink/flecs/flecs.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file animation_comp.h
    \brief Provides flecs components for animation.
*/

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_ANIMATION_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Tags */

extern ECS_DECLARE(KrAnimate);
extern ECS_DECLARE(KrAnimateToX);
extern ECS_DECLARE(KrAnimateToY);
extern ECS_DECLARE(KrAnimateToAngle);
extern ECS_DECLARE(KrAnimateRotationCenter);
extern ECS_DECLARE(KrAnimateToScaleX);
extern ECS_DECLARE(KrAnimateToScaleY);
extern ECS_DECLARE(KrAnimateToOpacity);
extern ECS_DECLARE(KrAnimateLoop);

/* Component types */

ECS_STRUCT(KrAnimation, {
	double start;
	double duration;
	int32_t ease;
	bool set_depth;
	int32_t depth_during;
	int32_t depth_after;
});

ECS_STRUCT(KrModifier, { float v; });

ECS_STRUCT(KrOffset, { double t; });

ECS_STRUCT(KrCallback, {
	void *before;
	void *before_param;
	void *after;
	void *after_param;
});

typedef void (*kr_anim_callback)(ecs_world_t *, void *);

/* Prefabs */

void ComponentsAnimationImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
