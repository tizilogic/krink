#define COMPONENTS_FSM_IMPL
#include "fsm_comp.h"

ECS_DECLARE(KrFsmBack);
ECS_DECLARE(KrFsmRequest);
ECS_DECLARE(KrFsmRegister);
ECS_DECLARE(KrFsmCurrentState);
ECS_DECLARE(KrPrefabFsmManager);

void ComponentsFsmImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsFsm);

	ECS_TAG_DEFINE(world, KrFsmBack);
	ECS_TAG_DEFINE(world, KrFsmRequest);
	ECS_TAG_DEFINE(world, KrFsmRegister);
	ECS_TAG_DEFINE(world, KrFsmCurrentState);

    ECS_META_COMPONENT(world, KrFsmState);
	ECS_META_COMPONENT(world, KrFsmStateHistory);
	ECS_META_COMPONENT(world, KrFsmData);

	ECS_PREFAB_DEFINE(world, KrPrefabFsmManager, KrFsmState(KrFsmCurrentState), KrFsmStateHistory, KrFsmData);
}
