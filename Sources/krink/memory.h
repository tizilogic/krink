#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void kr_memory_init(void *ptr, size_t size);
void *kr_malloc(size_t size);
void kr_free(void *ptr);
void *kr_calloc(size_t n, size_t size);
void *kr_realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
