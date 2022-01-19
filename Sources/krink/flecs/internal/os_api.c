#include "os_api.h"

#include "../flecs.h"
#include <assert.h>
#include <kinc/memory.h>
#include <kinc/threads/atomic.h>
#include <kinc/threads/event.h>
#include <kinc/threads/mutex.h>
#include <kinc/threads/semaphore.h>
#include <kinc/threads/thread.h>
#include <kinc/system.h>
#include <krink/memory.h>

/**
 * This unit has two licenses:
 *
 * Conditional variables implementations taken from flecs source and are licensed under the terms
 * of the MIT license -> MIT Copyright (c) 2019 Sander Mertens.
 *
 * The rest is the respective API implementations from krink and or kinc.
 */

// Memory

static void *kr_flecs_malloc(ecs_size_t size) {
	return kr_malloc((size_t)size);
}

static void *kr_flecs_calloc(ecs_size_t size) {
	void *ptr = kr_malloc((size_t)size);
	assert(ptr != NULL);
	kinc_memset(ptr, 0, (size_t)size);
	return ptr;
}

static void *kr_flecs_realloc(void *ptr, ecs_size_t size) {
	return kr_realloc(ptr, (size_t)size);
}

static void kr_flecs_free(void *ptr) {
	kr_free(ptr);
}

// Threading
static bool threading_enabled = false;

static ecs_os_thread_t kr_new_thread(ecs_os_thread_callback_t callback, void *arg) {
    if (!threading_enabled) {
        kinc_threads_init();
        threading_enabled = true;
    }
    kinc_thread_t *thread = (kinc_thread_t *)kr_malloc(sizeof(kinc_thread_t));
    assert(thread != NULL);
    kinc_thread_init(thread, (void (*)(void *))callback, arg);
	return (ecs_os_thread_t)(uintptr_t)thread;
}

static void *kr_thread_join(ecs_os_thread_t thr) {
	kinc_thread_t *thread = (kinc_thread_t *)(uintptr_t)thr;
    kinc_thread_wait_and_destroy(thread);
	kr_free(thread);
	return NULL;
}

static int32_t kr_ainc(int32_t *count) {
	return KINC_ATOMIC_INCREMENT(count);
}

static int32_t kr_adec(int32_t *count) {
	return KINC_ATOMIC_DECREMENT(count);
}

static ecs_os_mutex_t kr_mutex_new(void) {
    kinc_mutex_t *mutex = (kinc_mutex_t *)kr_malloc(sizeof(kinc_mutex_t));
    assert(mutex != NULL);
    kinc_mutex_init(mutex);
	return (ecs_os_mutex_t)(uintptr_t)mutex;
}

static void kr_mutex_free(ecs_os_mutex_t m) {
	kinc_mutex_t *mutex = (kinc_mutex_t *)(uintptr_t)m;
    kinc_mutex_destroy(mutex);
	kr_free(mutex);
}

static void kr_mutex_lock(ecs_os_mutex_t m) {
	kinc_mutex_t *mutex = (kinc_mutex_t *)(uintptr_t)m;
    kinc_mutex_lock(mutex);
}

static void kr_mutex_unlock(ecs_os_mutex_t m) {
	kinc_mutex_t *mutex = (kinc_mutex_t *)(uintptr_t)m;
    kinc_mutex_unlock(mutex);
}

// Time
static inline void kr_sleep(int32_t seconds, int32_t nanoseconds) {
    kinc_thread_sleep(seconds * 1000 + nanoseconds / 1000000);
}

static inline uint64_t kr_now(void) {
    return (uint64_t)(kinc_time() * 1000000000.0);
}

static void kr_set_high_res_clock(bool enable) {}

#ifdef ECS_TARGET_MSVC
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <windows.h>

static ecs_os_cond_t win_cond_new(void) {
	CONDITION_VARIABLE *cond = ecs_os_malloc_t(CONDITION_VARIABLE);
	InitializeConditionVariable(cond);
	return (ecs_os_cond_t)(uintptr_t)cond;
}

static void win_cond_free(ecs_os_cond_t c) {
	(void)c;
}

static void win_cond_signal(ecs_os_cond_t c) {
	CONDITION_VARIABLE *cond = (CONDITION_VARIABLE *)(intptr_t)c;
	WakeConditionVariable(cond);
}

static void win_cond_broadcast(ecs_os_cond_t c) {
	CONDITION_VARIABLE *cond = (CONDITION_VARIABLE *)(intptr_t)c;
	WakeAllConditionVariable(cond);
}

static void win_cond_wait(ecs_os_cond_t c, ecs_os_mutex_t m) {
	CRITICAL_SECTION *mutex = (CRITICAL_SECTION *)(intptr_t)m;
	CONDITION_VARIABLE *cond = (CONDITION_VARIABLE *)(intptr_t)c;
	SleepConditionVariableCS(cond, mutex, INFINITE);
}


void kr_set_flecs_os_api(void) {
	ecs_os_set_api_defaults();

	ecs_os_api_t api = ecs_os_api;

    api.malloc_ = kr_flecs_malloc;
    api.realloc_ = kr_flecs_realloc;
    api.calloc_ = kr_flecs_calloc;
    api.free_ = kr_flecs_free;
	api.thread_new_ = kr_new_thread;
	api.thread_join_ = kr_thread_join;
	api.ainc_ = kr_ainc;
	api.adec_ = kr_adec;
	api.mutex_new_ = kr_mutex_new;
	api.mutex_free_ = kr_mutex_free;
	api.mutex_lock_ = kr_mutex_lock;
	api.mutex_unlock_ = kr_mutex_unlock;
	api.cond_new_ = win_cond_new;
	api.cond_free_ = win_cond_free;
	api.cond_signal_ = win_cond_signal;
	api.cond_broadcast_ = win_cond_broadcast;
	api.cond_wait_ = win_cond_wait;
	api.sleep_ = kr_sleep;
	api.now_ = kr_now;
	api.enable_high_timer_resolution_ = kr_set_high_res_clock;
    api.abort_ = kinc_stop;

	ecs_os_set_api(&api);
}

#else
#include "pthread.h"

// Maybe no need?
// #if defined(__APPLE__) && defined(__MACH__)
// #include <mach/mach_time.h>
// #elif defined(__EMSCRIPTEN__)
// #include <emscripten.h>
// #else
// #include <time.h>
// #endif

static ecs_os_cond_t posix_cond_new(void) {
	pthread_cond_t *cond = ecs_os_malloc(sizeof(pthread_cond_t));
	if (pthread_cond_init(cond, NULL)) {
		kinc_stop();
	}
	return (ecs_os_cond_t)(uintptr_t)cond;
}

static void posix_cond_free(ecs_os_cond_t c) {
	pthread_cond_t *cond = (pthread_cond_t *)(intptr_t)c;
	if (pthread_cond_destroy(cond)) {
		kinc_stop();
	}
	ecs_os_free(cond);
}

static void posix_cond_signal(ecs_os_cond_t c) {
	pthread_cond_t *cond = (pthread_cond_t *)(intptr_t)c;
	if (pthread_cond_signal(cond)) {
		kinc_stop();
	}
}

static void posix_cond_broadcast(ecs_os_cond_t c) {
	pthread_cond_t *cond = (pthread_cond_t *)(intptr_t)c;
	if (pthread_cond_broadcast(cond)) {
		kinc_stop();
	}
}

static void posix_cond_wait(ecs_os_cond_t c, ecs_os_mutex_t m) {
	pthread_cond_t *cond = (pthread_cond_t *)(intptr_t)c;
	pthread_mutex_t *mutex = (pthread_mutex_t *)(intptr_t)m;
	if (pthread_cond_wait(cond, mutex)) {
		kinc_stop();
	}
}

void kr_set_flecs_os_api(void) {
	ecs_os_set_api_defaults();

	ecs_os_api_t api = ecs_os_api;

    api.malloc_ = kr_flecs_malloc;
    api.realloc_ = kr_flecs_realloc;
    api.calloc_ = kr_flecs_calloc;
    api.free_ = kr_flecs_free;
	api.thread_new_ = kr_new_thread;
	api.thread_join_ = kr_thread_join;
	api.ainc_ = kr_ainc;
	api.adec_ = kr_adec;
	api.mutex_new_ = kr_mutex_new;
	api.mutex_free_ = kr_mutex_free;
	api.mutex_lock_ = kr_mutex_lock;
	api.mutex_unlock_ = kr_mutex_unlock;
	api.cond_new_ = posix_cond_new;
	api.cond_free_ = posix_cond_free;
	api.cond_signal_ = posix_cond_signal;
	api.cond_broadcast_ = posix_cond_broadcast;
	api.cond_wait_ = posix_cond_wait;
	api.sleep_ = kr_sleep;
	api.now_ = kr_now;
	api.enable_high_timer_resolution_ = kr_set_high_res_clock;
    api.abort_ = kinc_stop;

	ecs_os_set_api(&api);
}

#endif
