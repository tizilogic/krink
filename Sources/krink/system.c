#include "system.h"
#include "graphics2/ttf.h"
#include "memory.h"

void kr_init(void *memblk, size_t size, int *glyphs, int num_glyphs) {
	kr_memory_init(memblk, size);
	kr_ttf_init(glyphs, num_glyphs);
}
