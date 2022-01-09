#define COMPONENTS_ANIMATION_IMPL
#include "animation_comp.h"

ECS_DECLARE(KrCompAnimate);
ECS_DECLARE(KrCompAnimateToX);
ECS_DECLARE(KrCompAnimateToY);
ECS_DECLARE(KrCompAnimateToAngle);
ECS_DECLARE(KrCompAnimateRotationCenter);
ECS_DECLARE(KrCompAnimateToScaleX);
ECS_DECLARE(KrCompAnimateToScaleY);
ECS_DECLARE(KrCompAnimateToOpacity);
ECS_DECLARE(KrCompAnimateFromX);
ECS_DECLARE(KrCompAnimateFromY);
ECS_DECLARE(KrCompAnimateFromAngle);
ECS_DECLARE(KrCompAnimateFromScaleX);
ECS_DECLARE(KrCompAnimateFromScaleY);
ECS_DECLARE(KrCompAnimateFromOpacity);

void ComponentsAnimationImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsAnimation);

    ECS_TAG_DEFINE(world, KrCompAnimate);
    ECS_TAG_DEFINE(world, KrCompAnimateToX);
    ECS_TAG_DEFINE(world, KrCompAnimateToY);
    ECS_TAG_DEFINE(world, KrCompAnimateToAngle);
    ECS_TAG_DEFINE(world, KrCompAnimateRotationCenter);
    ECS_TAG_DEFINE(world, KrCompAnimateToScaleX);
    ECS_TAG_DEFINE(world, KrCompAnimateToScaleY);
    ECS_TAG_DEFINE(world, KrCompAnimateToOpacity);
    ECS_TAG_DEFINE(world, KrCompAnimateFromX);
    ECS_TAG_DEFINE(world, KrCompAnimateFromY);
    ECS_TAG_DEFINE(world, KrCompAnimateFromAngle);
    ECS_TAG_DEFINE(world, KrCompAnimateFromScaleX);
    ECS_TAG_DEFINE(world, KrCompAnimateFromScaleY);
    ECS_TAG_DEFINE(world, KrCompAnimateFromOpacity);

	ECS_META_COMPONENT(world, KrCompAnimation);
	ECS_META_COMPONENT(world, KrCompModifier);
}
