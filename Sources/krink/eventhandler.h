#pragma once

#ifndef KR_EVT_MAX_OBSERVER
#define KR_EVT_MAX_OBSERVER 32
#endif

typedef enum kr_evt_event_type {
    KEY_DOWN,
    KEY_UP,
    KEY_PRESS,
    MOUSE_MOVE,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    MOUSE_SCROLL,
    FINGER_MOVE,
    FINGER_START,
    FINGER_END,
    PRIMARY_MOVE,
    PRIMARY_START,
    PRIMARY_END,
    FOREGROUND,
    BACKGROUND,
    PAUSE,
    RESUME,
    SHUTDOWN
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

void kr_evt_init(void);
void kr_evt_destroy(void);

// WARNING: data of the void * is going out of scope immediately after calling a callback!
// Either make a copy of the data or handle immediately inside the callback.
void kr_evt_add_observer(void (*value)(kr_evt_event_type_t, void *));
void kr_evt_remove_observer(void (*value)(kr_evt_event_type_t, void *));
