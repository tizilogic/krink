#include "memory.h"

#ifndef KR_NO_TLSF

#include "util/tlsf.h"
#include <assert.h>
#include <kinc/threads/mutex.h>

static void *kr_heap = NULL;
static tlsf_t kr_tlsf;
static kinc_mutex_t memlock;

void kr_memory_init(void *ptr, size_t size) {
	assert(kr_heap == NULL);
	assert(ptr != NULL);
	kr_heap = ptr;
	kr_tlsf = tlsf_create_with_pool(ptr, size);
	kinc_mutex_init(&memlock);
}

void *kr_malloc(size_t size) {
	assert(kr_heap != NULL);
	kinc_mutex_lock(&memlock);
	void *ptr = tlsf_malloc(kr_tlsf, size);
	kinc_mutex_unlock(&memlock);
	return ptr;
}

void kr_free(void *ptr) {
	assert(kr_heap != NULL);
	kinc_mutex_lock(&memlock);
	tlsf_free(kr_tlsf, ptr);
	kinc_mutex_unlock(&memlock);
}

void *kr_calloc(size_t n, size_t size) {
	assert(0 && "Not implemented");
	return NULL;
}

void *kr_realloc(void *ptr, size_t size) {
	assert(kr_heap != NULL);
	kinc_mutex_lock(&memlock);
	ptr = tlsf_realloc(kr_tlsf, ptr, size);
	kinc_mutex_unlock(&memlock);
	return ptr;
}

#endif
