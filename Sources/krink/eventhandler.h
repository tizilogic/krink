#pragma once

/*! \file eventhandler.h
    \brief Provides observer based event/input handling API.
*/

/*! \def KR_EVT_MAX_OBSERVER
    \brief Maximum number of observers registered at any time.
*/
#ifndef KR_EVT_MAX_OBSERVER
#define KR_EVT_MAX_OBSERVER 32
#endif

/// Event type passed to observers
typedef enum kr_evt_event_type {
	KR_EVT_KEY_DOWN,
	KR_EVT_KEY_UP,
	KR_EVT_KEY_PRESS,
	KR_EVT_MOUSE_MOVE,
	KR_EVT_MOUSE_PRESS,
	KR_EVT_MOUSE_RELEASE,
	KR_EVT_MOUSE_SCROLL,
	KR_EVT_FINGER_MOVE,
	KR_EVT_FINGER_START,
	KR_EVT_FINGER_END,
	KR_EVT_PRIMARY_MOVE,
	KR_EVT_PRIMARY_START,
	KR_EVT_PRIMARY_END,
	KR_EVT_FOREGROUND,
	KR_EVT_BACKGROUND,
	KR_EVT_PAUSE,
	KR_EVT_RESUME,
	KR_EVT_SHUTDOWN
} kr_evt_event_type_t;

typedef struct kr_evt_key_event {
	int keycode;
} kr_evt_key_event_t;

typedef struct kr_evt_key_press_event {
	unsigned character;
} kr_evt_key_event_press_t;

typedef struct kr_evt_mouse_move_event {
	int window, x, y, dx, dy;
} kr_evt_mouse_move_event_t;

typedef struct kr_evt_mouse_button_event {
	int window, button, x, y;
} kr_evt_mouse_button_event_t;

typedef struct kr_evt_mouse_scroll_event {
	int window, delta;
} kr_evt_mouse_scroll_event_t;

typedef struct kr_evt_finger_touch_event {
	int finger, x, y;
} kr_evt_finger_touch_event_t;

typedef struct kr_evt_primary_move_event {
	int x, y;
} kr_evt_primary_move_event_t;

typedef struct kr_evt_primary_button_event {
	int x, y;
} kr_evt_primary_button_event_t;

/// <summary>
/// Initialize the event handler. Needs to be called before using events.
/// </summary>
void kr_evt_init(void);

/// <summary>
/// Destroy the event handler. *Currently not implemented*.
/// </summary>
void kr_evt_destroy(void);

// WARNING: data of the void * is going out of scope immediately after calling a callback!
// Either make a copy of the data or handle immediately inside the callback.
/// <summary>
/// Add an observer to listen for events.
/// \warning data of the void * is going out of scope immediately after calling a callback!
/// Either make a copy of the data or handle immediately inside the callback.
/// </summary>
/// <param name="value">Callback function to handle events</param>
void kr_evt_add_observer(void (*value)(kr_evt_event_type_t, void *));

/// <summary>
/// Remove an observer.
/// </summary>
/// <param name="value">Previously registered callback function</param>
void kr_evt_remove_observer(void (*value)(kr_evt_event_type_t, void *));
