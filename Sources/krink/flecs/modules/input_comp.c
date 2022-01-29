#define COMPONENTS_INPUT_IMPL
#include "input_comp.h"

void ComponentsInputImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsInput);

	ECS_META_COMPONENT(world, KrKeyState);
	ECS_META_COMPONENT(world, KrMousePos);
	ECS_META_COMPONENT(world, KrMouseState);
	ECS_META_COMPONENT(world, KrSingletonInput);
}
