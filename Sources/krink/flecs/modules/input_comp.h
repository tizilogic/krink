#pragma once
#include <krink/flecs/flecs.h>
#include <stdint.h>

/*! \file input_comp.h
    \brief Provides flecs components for input handling.
*/

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_INPUT_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Component types */

ECS_STRUCT(KrKeyState, {
	bool down;
	bool triggered;
}); ///< `down` indicates whether the key is currently down, `triggered` indicates whether the state
    ///< changed this frame

ECS_STRUCT(KrMousePos, {
    float x;
    float y;
});

ECS_STRUCT(KrMouseState, {
    KrKeyState primary;
    KrKeyState right;
    KrMousePos pos;
    KrMousePos delta;
    KrMousePos scroll;
});

ECS_STRUCT(KrSingletonInput, {
    KrKeyState keys[256];
    KrMouseState mouse;
});

void ComponentsInputImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
