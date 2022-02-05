#define SYSTEMS_FSM_IMPL
#include "fsm_sys.h"
#include "fsm_comp.h"

ECS_DECLARE(KrInternalFsmActive);
ECS_DECLARE(KrInternalFsmEnter);
ECS_DECLARE(KrInternalFsmExit);

static void RequestState(ecs_iter_t *it) {
}

static void ExitState(ecs_iter_t *it) {
}

static void EnterState(ecs_iter_t *it) {}

static void UpdateState(ecs_iter_t *it) {}

void SystemsFsmImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, SystemsFsm);

	/* Register components */
	ECS_IMPORT(world, ComponentsFsm);

	ECS_TAG_DEFINE(world, KrInternalFsmActive);
	ECS_TAG_DEFINE(world, KrInternalFsmEnter);
	ECS_TAG_DEFINE(world, KrInternalFsmExit);

	/* Register Systems/Triggers */

	//ECS_SYSTEM(world, RequestState, EcsOnLoad, components.fsm.KrFsmState(components.fsm.KrFsmRegister, components.fsm.KrFsmRequest);
	ECS_SYSTEM(world, ExitState, EcsPostLoad, components.fsm.KrFsmState, KrInternalFsmExit);
	ECS_SYSTEM(world, EnterState, EcsPostLoad, components.fsm.KrFsmState, KrInternalFsmEnter);
	ECS_SYSTEM(world, UpdateState, EcsOnUpdate, components.fsm.KrFsmState, KrInternalFsmActive);
}
