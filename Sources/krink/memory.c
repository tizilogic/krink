#include "memory.h"
#include "util/tlsf.h"
#include <assert.h>

static void *kr_heap = NULL;
static tlsf_t kr_tlsf;

void kr_memory_init(void *ptr, size_t size) {
	assert(kr_heap == NULL);
	assert(ptr != NULL);
	kr_heap = ptr;
	kr_tlsf = tlsf_create_with_pool(ptr, size);
}

void *kr_malloc(size_t size) {
	assert(kr_heap != NULL);
	return tlsf_malloc(kr_tlsf, size);
}

void kr_free(void *ptr) {
	assert(kr_heap != NULL);
	tlsf_free(kr_tlsf, ptr);
}

void *kr_calloc(size_t n, size_t size) {
	assert(0 && "Not implemented");
	return NULL;
}

void *kr_realloc(void *ptr, size_t size) {
	assert(kr_heap != NULL);
	return tlsf_realloc(kr_tlsf, ptr, size);
}
