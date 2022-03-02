#include "system.h"
#include "graphics2/ttf.h"
#include "memory.h"
#include "eventhandler.h"
#include <kinc/threads/thread.h>

void kr_init(void *memblk, size_t size, int *glyphs, int num_glyphs) {
	kr_memory_init(memblk, size);
	kr_ttf_init(glyphs, num_glyphs);
	kr_evt_init();
	kinc_threads_init();
}

void kr_destroy(void) {
	kinc_threads_quit();
}
