#pragma once
#include <stddef.h>

/*! \file memory.h
    \brief Internal memory allocator used by krink.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KR_NO_TLSF

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

/// <summary>
/// When compiled in debug mode, tracks the number of allocations.
/// </summary>
/// <returns>Number of total allocations</returns>
int kr_allocation_count(void);

/// <summary>
/// When compiled in debug mode, tracks the amount of theoretically used memory.
/// Note: Only tracks requested size!
/// </summary>
/// <returns>Size of all allocated memory</returns>
int kr_allocation_size(void);

/// <summary>
/// When compiled in debug mode, tracks the amount of theoretically available memory.
/// Note: Only tracks requested size!
/// </summary>
/// <returns>Size of theoretically free memory</returns>
int kr_allocation_available(void);

/// <summary>
/// Returns the total pool size
/// </summary>
/// <returns>The size the pool was initially allocated</returns>
int kr_allocation_total(void);

/// <summary>
/// When compiled in debug mode, tracks the maximum total allocated memory from the pool.
/// Note: Only sums up requested size!
/// </summary>
/// <returns>The maximum used memory at runtime</returns>
int kr_allocation_max(void);

#else
#include <stdlib.h>

#define kr_memory_init(A, B)
#define kr_malloc(SIZE) malloc(SIZE)
#define kr_free(PTR) free(PTR)
#define kr_calloc(N, SIZE) calloc((N), (SIZE))
#define kr_realloc(PTR, SIZE) realloc((PTR), (SIZE))

#define kr_allocation_count() -1
#define kr_allocation_size() -1
#define kr_allocation_available() -1
#define kr_allocation_total() -1
#define kr_allocation_max() -1;

#endif

#ifdef __cplusplus
}
#endif
