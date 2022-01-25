#include <kinc/graphics4/graphics.h>
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
	kr_flecs_tick();
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	kinc_init("Empty", 1080, 1920, NULL, NULL);
	kinc_set_update_callback(update);
	heap = malloc(20 * 1024 * 1024);
	assert(heap != NULL);
	kr_init(heap, 20 * 1024 * 1024, NULL, 0);
	kr_flecs_init();
	kr_evt_init();
	kr_g2_init(0, 1080, 1920);
	kr_ttf_font_t font;
	kr_ttf_font_init(&font, "SpaceMono.ttf", 0);

	kr_nk_init(&font, 15, kr_malloc(5 * 1024 * 1024), 5 * 1024 * 1024);
	struct nk_context *kos_nkctx = kr_nk_get_ctx();
	ecs_entity_t e = kr_flecs_create_filled_rect(
	    (kr_init_filled_rect_t){.color = 0xff0000ff, .rect = {.x = 0, .y = 0, .w = 200, .h = 100}});
	ecs_set(kr_world, e, KrTranslation, {10, 10});
	kr_flecs_set_depth(e, -5);

	kr_flecs_create_text((kr_init_text_t){.color = 0xffffffff,
	                                      .font = &font,
	                                      .size = 30,
	                                      .pos = {.x = 100, .y = 700},
	                                      .text = "Empty Project Test"});

	kinc_start();
	kr_flecs_destroy();
	return 0;
}
