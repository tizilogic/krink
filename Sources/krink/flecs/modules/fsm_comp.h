#pragma once
#include <krink/flecs/flecs.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file fsm_comp.h
    \brief Provides flecs components for FSM.
*/

/*! \def KRINK_FSM_MAX_HISTORY
    \brief Maximum history size. Will wrap and overwrite.
*/
#ifndef KRINK_FSM_MAX_HISTORY
#define KRINK_FSM_MAX_HISTORY 128
#endif

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_FSM_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Tags */

extern ECS_DECLARE(KrFsmBack);
extern ECS_DECLARE(KrFsmRequest);
extern ECS_DECLARE(KrFsmRegister);
extern ECS_DECLARE(KrFsmCurrentState);

/* Component types */

ECS_STRUCT(KrFsmFunc, { void (*f)(ecs_world_t *); });

ECS_STRUCT(KrFsmState, {
	KrFsmFunc update_state;
	KrFsmFunc enter_state;
	KrFsmFunc exit_state;
});


ECS_STRUCT(KrFsmStateHistory, {
    ecs_id_t history[KRINK_FSM_MAX_HISTORY];
    int16_t cursor;
    int16_t start;
    bool wrapped;
});

ECS_STRUCT(KrFsmData, { void *data; int32_t type_id; });

/* Prefabs */

extern ECS_DECLARE(KrPrefabFsmManager);

void ComponentsFsmImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
