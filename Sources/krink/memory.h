#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void krink_memory_init(void *ptr, size_t size);
void *krink_malloc(size_t size);
void krink_free(void *ptr);
void *krink_calloc(size_t n, size_t size);
void *krink_realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
