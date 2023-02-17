#include <assert.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/system.h>
#include <krink/flecs/krink_flecs.h>
#include <krink/flecs/modules/drag_comp.h>
#include <krink/flecs/modules/render_comp.h>
#include <krink/graphics2/graphics.h>
#include <krink/memory.h>
#include <krink/system.h>
#include <stdlib.h>

static void *heap;

static void update(void *unused) {
	kinc_g4_begin(0);
	kr_flecs_tick();
	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	kinc_init("DragDrop", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update, NULL);
	heap = malloc(20 * 1024 * 1024);
	assert(heap != NULL);
	kr_init(heap, 20 * 1024 * 1024, NULL, 0);
	kr_flecs_init(false);
	kr_g2_init();
	ecs_entity_t parent = kr_flecs_create_filled_rect(
	    kr_world, &(kr_init_filled_rect_t){.color = 0xff0000ff,
	                                       .rect = {.x = 0, .y = 0, .w = 200, .h = 100}});
	ecs_set(kr_world, parent, KrTranslation, {10, 10});
	kr_flecs_set_depth(kr_world, parent, 0);

	ecs_entity_t child = kr_flecs_create_sdf_asymm_rect_wborder(
	    kr_world,
	    &(kr_init_sdf_asymm_rect_wborder_t){
	        .sdf_asymm_rect =
	            {.color = 0xff008800,
	             .rect = {.x = 0, .y = 0, .w = 100, .h = 60},
	             .corner = {.top_left = 30, .top_right = 10, .bottom_left = 10, .bottom_right = 30},
	             .smooth = 2.2f},
	        .border = 8,
	        .border_color = 0xffffffff});
	ecs_set(kr_world, child, KrTranslation, {20, 20});
	kr_flecs_set_depth(kr_world, child, 1);
	ecs_set(kr_world, child, KrAnchor, {.parent = parent});

	ecs_entity_t grandchild = kr_flecs_create_sdf_circle_wborder(
	    kr_world, &(kr_init_sdf_circle_wborder_t){
	                  .sdf_circle = {.color = 0xffffffff, .radius = 25, .smooth = 2.2f},
	                  .border = 8,
	                  .border_color = 0xff000044});
	ecs_set(kr_world, grandchild, KrTranslation, {55, 55});
	kr_flecs_set_depth(kr_world, grandchild, 2);
	ecs_set(kr_world, grandchild, KrAnchor, {.parent = child});

	ecs_add(kr_world, parent, KrDragable);
	ecs_add(kr_world, child, KrDragable);
	ecs_add(kr_world, grandchild, KrDragable);

	kinc_start();
	kr_flecs_destroy();
	return 0;
}
