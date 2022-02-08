#define COMPONENTS_DRAG_IMPL
#include "drag_comp.h"

ECS_DECLARE(KrDragable);
ECS_DECLARE(KrDragActive);
ECS_DECLARE(KrDragStart);
ECS_DECLARE(KrDrop);

void ComponentsDragImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsDrag);

    ECS_IMPORT(world, ComponentsRender);

	ECS_TAG_DEFINE(world, KrDragable);
	ECS_TAG_DEFINE(world, KrDragActive);
	ECS_TAG_DEFINE(world, KrDragStart);
	ECS_TAG_DEFINE(world, KrDrop);

	ECS_META_COMPONENT(world, KrDragInfo);
	ECS_META_COMPONENT(world, KrDragAABB);
}
