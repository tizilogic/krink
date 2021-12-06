#include "system.h"
#include "graphics2/ttf.h"
#include "memory.h"

void krink_init(void *memblk, size_t size, int *glyphs, int num_glyphs) {
	krink_memory_init(memblk, size);
	krink_ttf_init(glyphs, num_glyphs);
}
