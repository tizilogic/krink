#include "eventhandler.h"
#include <assert.h>
#include <kinc/input/keyboard.h>
#include <kinc/system.h>
#include <stddef.h>
#include <stdint.h>
//#define KORE_IOS
#if defined(KORE_IOS) || defined(KORE_ANDROID)
#include <kinc/input/surface.h>
#else
#include <kinc/input/mouse.h>
#endif

typedef void (*observer_t)(kr_evt_event_type_t, void *);
static observer_t observers[KR_EVT_MAX_OBSERVER];

static void notify(kr_evt_event_type_t event, void *data) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] != NULL) observers[i](event, data);
	}
}

static void key_up(int keycode) {
	kr_evt_key_event_t data;
	data.keycode = keycode;
	notify(KR_EVT_KEY_UP, (void *)&data);
}

static void key_down(int keycode) {
	kr_evt_key_event_t data;
	data.keycode = keycode;
	notify(KR_EVT_KEY_DOWN, (void *)&data);
}

static void key_press(unsigned character) {
	kr_evt_key_event_press_t data;
	data.character = character;
	notify(KR_EVT_KEY_PRESS, (void *)&data);
}

static void mouse_press(int window, int button, int x, int y) {
	kr_evt_mouse_button_event_t data;
	data.window = window;
	data.button = button;
	data.x = x;
	data.y = y;
	notify(KR_EVT_MOUSE_PRESS, (void *)&data);

	if (button != 0) return;
	kr_evt_primary_button_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_START, (void *)&pdata);
}

static void mouse_release(int window, int button, int x, int y) {
	kr_evt_mouse_button_event_t data;
	data.window = window;
	data.button = button;
	data.x = x;
	data.y = y;
	notify(KR_EVT_MOUSE_RELEASE, (void *)&data);

	if (button != 0) return;
	kr_evt_primary_button_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_END, (void *)&pdata);
}

static void mouse_scroll(int window, int delta) {
	kr_evt_mouse_scroll_event_t data;
	data.window = window;
	data.delta = delta;
	notify(KR_EVT_MOUSE_SCROLL, (void *)&data);
}

static void mouse_move(int window, int x, int y, int dx, int dy) {
	kr_evt_mouse_move_event_t data;
	data.window = window;
	data.x = x;
	data.y = y;
	data.dx = dx;
	data.dy = dy;
	notify(KR_EVT_MOUSE_MOVE, (void *)&data);

	kr_evt_primary_move_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_MOVE, (void *)&pdata);
}

static void touch_start(int finger, int x, int y) {
	kr_evt_finger_touch_event_t data;
	data.finger = finger;
	data.x = x;
	data.y = y;
	notify(KR_EVT_FINGER_START, (void *)&data);

	if (finger != 0) return;
	kr_evt_primary_button_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_START, (void *)&pdata);
}

static void touch_end(int finger, int x, int y) {
	kr_evt_finger_touch_event_t data;
	data.finger = finger;
	data.x = x;
	data.y = y;
	notify(KR_EVT_FINGER_END, (void *)&data);

	if (finger != 0) return;
	kr_evt_primary_button_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_END, (void *)&pdata);
}

static void finger_move(int finger, int x, int y) {
	kr_evt_finger_touch_event_t data;
	data.finger = finger;
	data.x = x;
	data.y = y;
	notify(KR_EVT_FINGER_MOVE, (void *)&data);

	if (finger != 0) return;
	kr_evt_primary_move_event_t pdata;
	pdata.x = x;
	pdata.y = y;
	notify(KR_EVT_PRIMARY_MOVE, (void *)&pdata);
}

static void foreground(void) {
	notify(KR_EVT_FOREGROUND, NULL);
}

static void background(void) {
	notify(KR_EVT_BACKGROUND, NULL);
}

static void pause(void) {
	notify(KR_EVT_PAUSE, NULL);
}

static void resume(void) {
	notify(KR_EVT_RESUME, NULL);
}

static void shutdown(void) {
	notify(KR_EVT_SHUTDOWN, NULL);
}

void kr_evt_init(void) {
	kinc_keyboard_set_key_up_callback(key_up);
	kinc_keyboard_set_key_down_callback(key_down);
	kinc_keyboard_set_key_press_callback(key_press);

#if defined(KORE_IOS) || defined(KORE_ANDROID)
	kinc_surface_set_touch_start_callback(touch_start);
	kinc_surface_set_touch_end_callback(touch_end);
	kinc_surface_set_move_callback(finger_move);
#else
	kinc_mouse_set_press_callback(mouse_press);
	kinc_mouse_set_release_callback(mouse_release);
	kinc_mouse_set_scroll_callback(mouse_scroll);
	kinc_mouse_set_move_callback(mouse_move);
#endif
	kinc_set_foreground_callback(foreground);
	kinc_set_background_callback(background);
	kinc_set_pause_callback(pause);
	kinc_set_resume_callback(resume);
	kinc_set_shutdown_callback(shutdown);

	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		observers[0] = NULL;
	}
}

void kr_evt_destroy(void) {
	// Maybe not needed?
}

void kr_evt_add_observer(void (*value)(kr_evt_event_type_t, void *)) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] == NULL) {
			observers[i] = value;
			return;
		}
	}
	assert(0);
}

void kr_evt_remove_observer(void (*value)(kr_evt_event_type_t, void *)) {
	for (int i = 0; i < KR_EVT_MAX_OBSERVER; ++i) {
		if (observers[i] == value) {
			observers[i] = NULL;
			return;
		}
	}
	assert(0);
}
