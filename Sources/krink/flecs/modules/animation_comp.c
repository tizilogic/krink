#define COMPONENTS_ANIMATION_IMPL
#include "animation_comp.h"

ECS_DECLARE(KrAnimate);
ECS_DECLARE(KrAnimateToX);
ECS_DECLARE(KrAnimateToY);
ECS_DECLARE(KrAnimateToAngle);
ECS_DECLARE(KrAnimateRotationCenter);
ECS_DECLARE(KrAnimateToScaleX);
ECS_DECLARE(KrAnimateToScaleY);
ECS_DECLARE(KrAnimateToOpacity);

void ComponentsAnimationImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsAnimation);

	ECS_TAG_DEFINE(world, KrAnimate);
	ECS_TAG_DEFINE(world, KrAnimateToX);
	ECS_TAG_DEFINE(world, KrAnimateToY);
	ECS_TAG_DEFINE(world, KrAnimateToAngle);
	ECS_TAG_DEFINE(world, KrAnimateRotationCenter);
	ECS_TAG_DEFINE(world, KrAnimateToScaleX);
	ECS_TAG_DEFINE(world, KrAnimateToScaleY);
	ECS_TAG_DEFINE(world, KrAnimateToOpacity);

	ECS_META_COMPONENT(world, KrAnimation);
	ECS_META_COMPONENT(world, KrModifier);
}
