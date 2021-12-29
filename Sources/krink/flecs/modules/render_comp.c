#include "render_comp.h"

/* Implement a simple move system */
void Move(ecs_iter_t *it) {}

void SimpleModuleImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SimpleModule);

	/* Register components */
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Velocity);

	/* Define a system called Move that is executed every frame, and subscribes
	 * for the 'Position' and 'Velocity' components */
	ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

	/* Export handles to module contents */
	ECS_SET_COMPONENT(Position);
	ECS_SET_COMPONENT(Velocity);
	ECS_SET_ENTITY(Move);
}