#pragma once
#include "render_comp.h"
#include <krink/flecs/flecs.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file drag_comp.h
    \brief Provides flecs components for drag and drop.
*/

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_DRAG_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Tags */

extern ECS_DECLARE(KrDragable);
extern ECS_DECLARE(KrDragActive);
extern ECS_DECLARE(KrDragStart);
extern ECS_DECLARE(KrDrop);

/* Component types */

ECS_STRUCT(KrDragInfo, {
	KrPos2 start_pos;
	KrTranslation start_trans;
	KrPos2 offset;
});

ECS_STRUCT(KrDragAABB, {
    float x;
    float y;
    float hw;
    float hh;
});

void ComponentsDragImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
