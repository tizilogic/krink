#pragma once
#include <krink/flecs/flecs.h>
#include <stdbool.h>
#include <stdint.h>

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_ANIMATION_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Tags */

extern ECS_DECLARE(KrCompAnimate);
extern ECS_DECLARE(KrCompAnimateToX);
extern ECS_DECLARE(KrCompAnimateToY);
extern ECS_DECLARE(KrCompAnimateToAngle);
extern ECS_DECLARE(KrCompAnimateRotationCenter);
extern ECS_DECLARE(KrCompAnimateToScaleX);
extern ECS_DECLARE(KrCompAnimateToScaleY);
extern ECS_DECLARE(KrCompAnimateToOpacity);
extern ECS_DECLARE(KrCompAnimateFromX);
extern ECS_DECLARE(KrCompAnimateFromY);
extern ECS_DECLARE(KrCompAnimateFromAngle);
extern ECS_DECLARE(KrCompAnimateFromScaleX);
extern ECS_DECLARE(KrCompAnimateFromScaleY);
extern ECS_DECLARE(KrCompAnimateFromOpacity);

/* Component types */

ECS_STRUCT(KrCompAnimation, {
	float start;
	float duration;
	int32_t ease;
});

ECS_STRUCT(KrCompModifier, { float v; });

/* Prefabs */

void ComponentsAnimationImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
