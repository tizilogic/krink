#define SYSTEMS_ANIMATION_IMPL
#include "animation_sys.h"
#include "animation_comp.h"

ECS_DECLARE(KrInternalAnimationActive);
ECS_DECLARE(KrInternalAnimationDelete);

void SystemsAnimationImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsAnimation);
	ECS_IMPORT(world, ComponentsAnimation);

    /* Register internal components */
    ECS_TAG_DEFINE(world, KrInternalAnimationActive);
    ECS_TAG_DEFINE(world, KrInternalAnimationDelete);

	/* Register systems */

}
