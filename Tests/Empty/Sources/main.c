#include <kinc/graphics4/graphics.h>
#include <kinc/display.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <krink/eventhandler.h>
#include <krink/flecs/krink_flecs.h>
#include <krink/flecs/modules/render_comp.h>
#include <krink/graphics2/graphics.h>
#include <krink/graphics2/ttf.h>
#include <krink/memory.h>
#include <krink/nk/nuklear_krink.h>
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
	kinc_init("Empty", w, h, NULL, NULL);
	kinc_set_update_callback(update);
	heap = malloc(20 * 1024 * 1024);
	assert(heap != NULL);
	kr_init(heap, 20 * 1024 * 1024, NULL, 0);
	kr_flecs_init(false);
	kr_evt_init();
	kr_g2_init(0, 1080, 1920);
	kr_ttf_font_t font;
	kr_ttf_font_init(&font, "SpaceMono.ttf", 0);

	kr_nk_init(&font, 15, kr_malloc(5 * 1024 * 1024), 5 * 1024 * 1024);
	struct nk_context *kos_nkctx = kr_nk_get_ctx();
	ecs_entity_t e = kr_flecs_create_filled_rect(
	    kr_world,
	    (kr_init_filled_rect_t){.color = 0xff0000ff, .rect = {.x = 0, .y = 0, .w = 200, .h = 100}});
	ecs_set(kr_world, e, KrTranslation, {10, 10});
	kr_flecs_set_depth(kr_world, e, -5);

	kr_flecs_create_text(kr_world, (kr_init_text_t){.color = 0xffffffff,
	                                                .font = &font,
	                                                .size = 30,
	                                                .pos = {.x = 100, .y = 700},
	                                                .text = "Empty Project Test"});

	e = kr_flecs_create_sdf_asymm_rect_wborder(
	    kr_world, (kr_init_sdf_asymm_rect_wborder_t){
	                  .sdf_asymm_rect = {.color = 0xff008800,
	                                     .rect = {.x = 0, .y = 0, .w = 100, .h = 60},
	                                     .corner = {.top_left = 30, .top_right = 10, .bottom_left = 10, .bottom_right = 30},
	                                     .smooth = 2.2f}, .border = 8, .border_color = 0xffffffff});
	ecs_set(kr_world, e, KrTranslation, {20, 20});
	kr_flecs_set_depth(kr_world, e, 5);
	
	kinc_start();
	kr_flecs_destroy();
	return 0;
}
