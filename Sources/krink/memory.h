#pragma once
#include <stddef.h>

/*! \file memory.h
    \brief Internal memory allocator used by krink.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initialize the TLSF memory allocator.
/// </summary>
/// <param name="ptr"></param>
/// <param name="size"></param>
void kr_memory_init(void *ptr, size_t size);

/// <summary>
/// `malloc` equivalent.
/// </summary>
/// <param name="size"></param>
void *kr_malloc(size_t size);

/// <summary>
/// `free` equivalent.
/// </summary>
/// <param name="ptr"></param>
void kr_free(void *ptr);

/// <summary>
/// `calloc` equivalent.
/// </summary>
/// <param name="n"></param>
/// <param name="size"></param>
void *kr_calloc(size_t n, size_t size);

/// <summary>
/// `realloc` equivalent.
/// </summary>
/// <param name="ptr"></param>
/// <param name="size"></param>
void *kr_realloc(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif
