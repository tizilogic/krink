#include "memory.h"
#include "util/tlsf.h"
#include <assert.h>

static void* krink_heap = NULL;
static tlsf_t krink_tlsf;

void krink_memory_init(void* ptr, size_t size) {
    assert(krink_heap == NULL);
    assert(ptr != NULL);
    krink_heap = ptr;
    krink_tlsf = tlsf_create_with_pool(ptr, size);
}

void* krink_malloc(size_t size) {
    assert(krink_heap != NULL);
    return tlsf_malloc(krink_tlsf, size);
}

void krink_free(void* ptr) {
    assert(krink_heap != NULL);
    tlsf_free(krink_tlsf, ptr);
}

void* krink_calloc(size_t n, size_t size) {
    assert(0 && "Not implemented");
    return NULL;
}

void* krink_realloc(void* ptr, size_t size) {
    assert(krink_heap != NULL);
    return tlsf_realloc(krink_tlsf, ptr, size);
}
