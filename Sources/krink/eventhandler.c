#include "eventhandler.h"
#include <assert.h>
#include <kinc/input/keyboard.h>
#include <kinc/system.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if defined(KORE_IOS) || defined(KORE_ANDROID)
#include <kinc/input/surface.h>
#else
#include <kinc/input/mouse.h>
#endif

typedef void (*observer_t)(kr_evt_event_t);
static observer_t observers[KR_EVT_MAX_OBSERVER];

static void notify(kr_evt_event_type_t event, kr_evt_data_t data) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] != NULL) observers[i]((kr_evt_event_t){event, data});
	}
}

void kr_evt_internal_trigger_ext_evt(kr_evt_event_type_t event, kr_evt_data_t data) {
	notify(event, data);
}

static void key_up(int keycode, void *unused_data) {
	kr_evt_data_t data;
	data.key.keycode = keycode;
	notify(KR_EVT_KEY_UP, data);
}

static void key_down(int keycode, void *unused_data) {
	kr_evt_data_t data;
	data.key.keycode = keycode;
	notify(KR_EVT_KEY_DOWN, data);
}

static void key_press(unsigned character, void *unused_data) {
	kr_evt_data_t data;
	data.key_press.character = character;
	notify(KR_EVT_KEY_PRESS, data);
}

static void mouse_press(int window, int button, int x, int y, void *unused_data) {
	kr_evt_data_t data;
	data.mouse_button.window = window;
	data.mouse_button.button = button;
	data.mouse_button.x = x;
	data.mouse_button.y = y;
	notify(KR_EVT_MOUSE_PRESS, data);

	if (button != 0) return;
	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_START, pdata);
}

static void mouse_release(int window, int button, int x, int y, void *unused_data) {
	kr_evt_data_t data;
	data.mouse_button.window = window;
	data.mouse_button.button = button;
	data.mouse_button.x = x;
	data.mouse_button.y = y;
	notify(KR_EVT_MOUSE_RELEASE, data);

	if (button != 0) return;
	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_END, pdata);
}

static void mouse_scroll(int window, int delta, void *unused_data) {
	kr_evt_data_t data;
	data.mouse_scroll.window = window;
	data.mouse_scroll.delta = delta;
	notify(KR_EVT_MOUSE_SCROLL, data);
}

static void mouse_move(int window, int x, int y, int dx, int dy, void *unused_data) {
	kr_evt_data_t data;
	data.mouse_move.window = window;
	data.mouse_move.x = x;
	data.mouse_move.y = y;
	data.mouse_move.dx = dx;
	data.mouse_move.dy = dy;
	notify(KR_EVT_MOUSE_MOVE, data);

	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_MOVE, pdata);
}

static void touch_start(int finger, int x, int y) {
	kr_evt_data_t data;
	data.touch.finger = finger;
	data.touch.x = x;
	data.touch.y = y;
	notify(KR_EVT_FINGER_START, data);

	if (finger != 0) return;
	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_START, pdata);
}

static void touch_end(int finger, int x, int y) {
	kr_evt_data_t data;
	data.touch.finger = finger;
	data.touch.x = x;
	data.touch.y = y;
	notify(KR_EVT_FINGER_END, data);

	if (finger != 0) return;
	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_END, pdata);
}

static void finger_move(int finger, int x, int y) {
	kr_evt_data_t data;
	data.touch.finger = finger;
	data.touch.x = x;
	data.touch.y = y;
	notify(KR_EVT_FINGER_MOVE, data);

	if (finger != 0) return;
	kr_evt_data_t pdata;
	pdata.primary.x = x;
	pdata.primary.y = y;
	notify(KR_EVT_PRIMARY_MOVE, pdata);
}

static void foreground(void *unused_data) {
	kr_evt_data_t unused;
	memset(&unused, 0, sizeof(kr_evt_data_t));
	notify(KR_EVT_FOREGROUND, unused);
}

static void background(void *unused_data) {
	kr_evt_data_t unused;
	memset(&unused, 0, sizeof(kr_evt_data_t));
	notify(KR_EVT_BACKGROUND, unused);
}

static void pause(void *unused_data) {
	kr_evt_data_t unused;
	memset(&unused, 0, sizeof(kr_evt_data_t));
	notify(KR_EVT_PAUSE, unused);
}

static void resume(void *unused_data) {
	kr_evt_data_t unused;
	memset(&unused, 0, sizeof(kr_evt_data_t));
	notify(KR_EVT_RESUME, unused);
}

static void shutdown(void *unused_data) {
	kr_evt_data_t unused;
	memset(&unused, 0, sizeof(kr_evt_data_t));
	notify(KR_EVT_SHUTDOWN, unused);
}

static void drop_files(wchar_t *file, void *udata) {
	kr_evt_data_t data;
	memset(&data, 0, sizeof(kr_evt_data_t));
	size_t len = wcstombs(data.drop.filename, file, sizeof(data.drop.filename));
	notify(KR_EVT_DROP_FILE, data);
}

void kr_evt_init(void) {
	kinc_keyboard_set_key_up_callback(key_up, NULL);
	kinc_keyboard_set_key_down_callback(key_down, NULL);
	kinc_keyboard_set_key_press_callback(key_press, NULL);

#if defined(KORE_IOS) || defined(KORE_ANDROID)
	kinc_surface_set_touch_start_callback(touch_start);
	kinc_surface_set_touch_end_callback(touch_end);
	kinc_surface_set_move_callback(finger_move);
#else
	kinc_mouse_set_press_callback(mouse_press, NULL);
	kinc_mouse_set_release_callback(mouse_release, NULL);
	kinc_mouse_set_scroll_callback(mouse_scroll, NULL);
	kinc_mouse_set_move_callback(mouse_move, NULL);
	kinc_set_drop_files_callback(drop_files, NULL);
#endif
	kinc_set_foreground_callback(foreground, NULL);
	kinc_set_background_callback(background, NULL);
	kinc_set_pause_callback(pause, NULL);
	kinc_set_resume_callback(resume, NULL);
	kinc_set_shutdown_callback(shutdown, NULL);

	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		observers[i] = NULL;
	}
}

void kr_evt_destroy(void) {
	// Maybe not needed?
}

void kr_evt_add_observer(void (*value)(kr_evt_event_t)) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] == NULL) {
			observers[i] = value;
			return;
		}
	}
	assert(0);
}

void kr_evt_remove_observer(void (*value)(kr_evt_event_t)) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] == value) {
			observers[i] = NULL;
			return;
		}
	}
	assert(0);
}
