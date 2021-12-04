#include "system.h"
#include "memory.h"

void krink_init(void* memblk, size_t size) {
    krink_memory_init(memblk, size);
}
