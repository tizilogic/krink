#include <kinc/display.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <krink/flecs/krink_flecs.h>
#include <krink/flecs/modules/animation_comp.h>
#include <krink/flecs/modules/render_comp.h>
#include <krink/graphics2/graphics.h>
#include <krink/memory.h>
#include <krink/system.h>
#include <stdlib.h>

static void *heap;

static void update(void) {
	kinc_g4_begin(0);
	kr_flecs_tick();
	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	kinc_display_init();
	int w, h;
	{
		kinc_display_mode_t dm = kinc_display_current_mode(kinc_primary_display());
#if defined(KORE_ANDROID) || defined(KORE_IOS)
		w = dm.width;
		h = dm.height;
#else
		h = (int)((float)dm.height * 0.9f);
		w = (int)((float)dm.height * 0.9f / 16.0f * 9.0f);
#endif
	}
	kinc_init("Animation", w, h, NULL, NULL);
	kinc_set_update_callback(update);
	heap = malloc(40 * 1024 * 1024);
	assert(heap != NULL);
	kr_init(heap, 40 * 1024 * 1024, NULL, 0);
	kr_flecs_init(true);
	kr_g2_init();

	ecs_entity_t e = kr_flecs_create_filled_rect(
	    kr_world, &(kr_init_filled_rect_t){.color = 0xff0000ff,
	                                       .rect = {.x = 0, .y = 0, .w = 200, .h = 100},
	                                       .depth = 0});
	ecs_set(kr_world, e, KrTranslation, {10, 10});

	kr_flecs_create_sequence(
	    kr_world, e,
	    &(kr_init_sequence_t){
	        .count = 2,
	        .loop = true,
	        .animations = {{.start = 0.0,
	                        .duration = 3.0,
	                        .ease = KR_EASE_ELASTIC_IN_OUT,
	                        .set_depth = true,
	                        .depth_during = 20,
	                        .depth_after = 0,
	                        .modifiers = {{.modifier = KR_MODIFIER_X, .value = w - 210},
	                                      {.modifier = KR_MODIFIER_Y, .value = h - 110}}},
	                       {.start = 3.0,
	                        .duration = 3.0,
	                        .ease = KR_EASE_ELASTIC_IN_OUT,
	                        .set_depth = true,
	                        .depth_during = 0,
	                        .depth_after = 20,
	                        .modifiers = {{.modifier = KR_MODIFIER_X, .value = 10},
	                                      {.modifier = KR_MODIFIER_Y, .value = 10}}}}},
	    NULL);

	e = kr_flecs_create_filled_rect(
	    kr_world, &(kr_init_filled_rect_t){.color = 0xffff0000,
	                                       .rect = {.x = 0, .y = 0, .w = 180, .h = 80},
	                                       .depth = 1});
	ecs_set(kr_world, e, KrTranslation, {20, 20});

	kr_flecs_create_sequence(
	    kr_world, e,
	    &(kr_init_sequence_t){
	        .count = 2,
	        .loop = true,
	        .animations = {{.start = 0.0,
	                        .duration = 3.0,
	                        .ease = KR_EASE_ELASTIC_IN_OUT,
	                        .set_depth = true,
	                        .depth_during = 0,
	                        .depth_after = 1,
	                        .modifiers = {{.modifier = KR_MODIFIER_X, .value = w - 200},
	                                      {.modifier = KR_MODIFIER_Y, .value = h - 100}}},
	                       {.start = 3.0,
	                        .duration = 3.0,
	                        .ease = KR_EASE_ELASTIC_IN_OUT,
	                        .set_depth = true,
	                        .depth_during = 1,
	                        .depth_after = 0,
	                        .modifiers = {{.modifier = KR_MODIFIER_X, .value = 20},
	                                      {.modifier = KR_MODIFIER_Y, .value = 20}}}}},
	    NULL);

	kinc_start();
	kr_flecs_destroy();
	return 0;
}
