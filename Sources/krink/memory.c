#include "memory.h"

#ifndef KR_NO_TLSF

#include "util/tlsf.h"
#include <assert.h>
#include <kinc/threads/mutex.h>
#include <stdint.h>

#if !defined(NDEBUG) && !defined(KR_NO_ALLOCATION_TRACKER)

#ifndef KR_ALLOCATION_TRACKER_START_SIZE
#define KR_ALLOCATION_TRACKER_START_SIZE 1024
#endif

#include <stdlib.h>

typedef struct kr_alloc {
	void *ptr;
	int size;
} kr_alloc_t;

static int kr_pool_total = 0;
static int kr_pool_num_allocs = 0;
static int kr_pool_allocated = 0;
static int kr_pool_allocated_max = 0;
static kr_alloc_t *kr_allocs = NULL;
static int kr_allocs_count = 0;

static void kr_alloctrack_set_total(size_t size) {
	kr_pool_total = size;
}

static void kr_alloctrack_malloc(void *ptr, size_t size) {
	if (kr_allocs == NULL) {
		kr_allocs = (kr_alloc_t *)malloc(KR_ALLOCATION_TRACKER_START_SIZE * sizeof(kr_alloc_t));
		assert(kr_allocs);
		kr_allocs_count = KR_ALLOCATION_TRACKER_START_SIZE;
		kr_pool_num_allocs = 1;
		kr_pool_allocated = size;
		kr_allocs[0].ptr = ptr;
		kr_allocs[0].size = size;
		for (int i = 1; i < kr_allocs_count; ++i) {
			kr_allocs[i].ptr = NULL;
			kr_allocs[i].size = 0;
		}
		return;
	}
	for (int i = 0; i < kr_allocs_count; ++i) {
		if (kr_allocs[i].ptr == NULL) {
			kr_allocs[i].ptr = ptr;
			kr_allocs[i].size = size;
			++kr_pool_num_allocs;
			kr_pool_allocated += size;
			kr_pool_allocated_max = (kr_pool_allocated > kr_pool_allocated_max)
			                            ? kr_pool_allocated
			                            : kr_pool_allocated_max;
			return;
		}
	}
	kr_allocs = (kr_alloc_t *)realloc(kr_allocs, 2 * kr_allocs_count * sizeof(kr_alloc_t));
	assert(kr_allocs);
	for (int i = kr_allocs_count; i < 2 * kr_allocs_count; ++i) {
		kr_allocs[i].ptr = NULL;
		kr_allocs[i].size = 0;
	}
	kr_allocs[kr_allocs_count].ptr = ptr;
	kr_allocs[kr_allocs_count].size = size;
	++kr_pool_num_allocs;
	kr_allocs_count *= 2;
}

static void kr_alloctrack_free(void *ptr) {
	if (ptr == NULL) return;

	for (int i = 0; i < kr_allocs_count; ++i) {
		if (kr_allocs[i].ptr == ptr) {
			kr_allocs[i].ptr = NULL;
			kr_pool_allocated -= kr_allocs[i].size;
			kr_allocs[i].size = 0;
			--kr_pool_num_allocs;
			return;
		}
	}
	assert(0);
}

static void kr_alloctrack_realloc(void *old, void *ptr, size_t size) {
	for (int i = 0; i < kr_allocs_count; ++i) {
		if (kr_allocs[i].ptr == old) {
			kr_allocs[i].ptr = ptr;
			kr_pool_allocated += size - kr_allocs[i].size;
			kr_pool_allocated_max = (kr_pool_allocated > kr_pool_allocated_max)
			                            ? kr_pool_allocated
			                            : kr_pool_allocated_max;
			kr_allocs[i].size = size;
			return;
		}
	}
	assert(0);
}

#else

#define kr_alloctrack_set_total(A)
#define kr_alloctrack_malloc(A, B)
#define kr_alloctrack_free(A)
#define kr_alloctrack_realloc(A, B, C)

#endif

static void *kr_heap = NULL;
static tlsf_t kr_tlsf;
static kinc_mutex_t memlock;

void kr_memory_init(void *ptr, size_t size) {
	assert(kr_heap == NULL);
	assert(ptr != NULL);
	kr_heap = ptr;
	kr_tlsf = tlsf_create_with_pool(ptr, size);
	kinc_mutex_init(&memlock);
	kr_alloctrack_set_total(size);
}

void *kr_malloc(size_t size) {
	assert(kr_heap != NULL);
	kinc_mutex_lock(&memlock);
	void *ptr = tlsf_malloc(kr_tlsf, size);
	kinc_mutex_unlock(&memlock);
	assert(ptr);
	kr_alloctrack_malloc(ptr, size);
	return ptr;
}

void kr_free(void *ptr) {
	assert(kr_heap != NULL);
	kr_alloctrack_free(ptr);
	kinc_mutex_lock(&memlock);
	tlsf_free(kr_tlsf, ptr);
	kinc_mutex_unlock(&memlock);
}

void *kr_calloc(size_t n, size_t size) {
	// Naive implementation!!!
	uint8_t *ptr = (uint8_t *)kr_malloc(n * size);
	for (int i = 0; i < size; ++i) ptr[i] = 0;
	return ptr;
}

void *kr_realloc(void *ptr, size_t size) {
	assert(kr_heap != NULL);
	kinc_mutex_lock(&memlock);
	void *nptr = tlsf_realloc(kr_tlsf, ptr, size);
	kinc_mutex_unlock(&memlock);
	assert(nptr);
	kr_alloctrack_realloc(ptr, nptr, size);
	return nptr;
}

#if !defined(NDEBUG) && !defined(KR_NO_ALLOCATION_TRACKER)

int kr_allocation_count(void) {
	return kr_pool_num_allocs;
}

int kr_allocation_size(void) {
	return kr_pool_allocated;
}

int kr_allocation_available(void) {
	return kr_pool_total - kr_pool_allocated;
}

int kr_allocation_total(void) {
	return kr_pool_total;
}

int kr_allocation_max(void) {
	return kr_pool_allocated_max;
}

#else

int kr_allocation_count(void) {
	return -1;
}
int kr_allocation_size(void) {
	return -1;
}
int kr_allocation_available(void) {
	return -1;
}
int kr_allocation_total(void) {
	return -1;
}

int kr_allocation_max(void) {
	return -1;
}

#endif

#endif
