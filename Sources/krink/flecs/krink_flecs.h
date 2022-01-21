#pragma once

#include "flecs.h"
#include <krink/graphics2/sdfpainter.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <krink/util/tween.h>
#include <stdint.h>

typedef struct kr_rect {
	float x, y, w, h;
} kr_rect_t;

typedef struct kr_pos2 {
	float x, y;
} kr_pos2_t;

typedef struct kr_init_sprite {
	kr_image_t *image;
	kr_rect_t src, dest;
	uint32_t color;
} kr_init_sprite_t;

typedef struct kr_init_text {
	kr_pos2_t pos;
	const char *text;
	kr_ttf_font_t *font;
	int32_t size;
	uint32_t color;
} kr_init_text_t;

typedef struct kr_init_stroked_rect {
	kr_rect_t rect;
	float stroke;
	uint32_t color;
} kr_init_stroked_rect_t;

typedef struct kr_init_filled_rect {
	kr_rect_t rect;
	uint32_t color;
} kr_init_filled_rect_t;

typedef struct kr_init_stroked_triangle {
	float x1, y1, x2, y2, x3, y3, stroke;
	uint32_t color;
} kr_init_stroked_triangle_t;

typedef struct kr_init_filled_triangle {
	float x1, y1, x2, y2, x3, y3;
	uint32_t color;
} kr_init_filled_triangle_t;

typedef struct kr_init_line {
	float x1, y1, x2, y2, stroke;
	uint32_t color;
} kr_init_line_t;

typedef struct kr_init_sdf_rect {
	kr_rect_t rect;
	float corner, smooth;
	uint32_t color;
} kr_init_sdf_rect_t;

typedef struct kr_init_sdf_asymm_rect {
	kr_rect_t rect;
	kr_sdf_corner_radius_t corner;
	float smooth;
	uint32_t color;
} kr_init_sdf_asymm_rect_t;

typedef struct kr_init_sdf_rect_wborder {
	kr_init_sdf_rect_t sdf_rect;
	float border;
	uint32_t border_color;
} kr_init_sdf_rect_wborder_t;

typedef struct kr_init_sdf_asymm_rect_wborder {
	kr_init_sdf_asymm_rect_t sdf_asymm_rect;
	float border;
	uint32_t border_color;
} kr_init_sdf_asymm_rect_wborder_t;

typedef struct kr_init_sdf_circle {
	kr_pos2_t pos;
	float radius, smooth;
	uint32_t color;
} kr_init_sdf_circle_t;

typedef struct kr_init_sdf_circle_wborder {
	kr_init_sdf_circle_t sdf_circle;
	float border;
	uint32_t border_color;
} kr_init_sdf_circle_wborder_t;

extern ecs_world_t *kr_world;

void kr_flecs_init(void);
void kr_flecs_destroy(void);
void kr_flecs_tick(void);
void kr_flecs_set_depth(ecs_entity_t e, int32_t depth);
ecs_entity_t kr_flecs_create_sprite(kr_init_sprite_t args);
ecs_entity_t kr_flecs_create_text(kr_init_text_t args);
ecs_entity_t kr_flecs_create_stroked_rect(kr_init_stroked_rect_t args);
ecs_entity_t kr_flecs_create_filled_rect(kr_init_filled_rect_t args);
ecs_entity_t kr_flecs_create_stroked_triangle(kr_init_stroked_triangle_t args);
ecs_entity_t kr_flecs_create_filled_triangle(kr_init_filled_triangle_t args);
ecs_entity_t kr_flecs_create_line(kr_init_line_t args);
ecs_entity_t kr_flecs_create_sdf_rect(kr_init_sdf_rect_t args);
ecs_entity_t kr_flecs_create_sdf_asymm_rect(kr_init_sdf_asymm_rect_t args);
ecs_entity_t kr_flecs_create_sdf_rect_wborder(kr_init_sdf_rect_wborder_t args);
ecs_entity_t kr_flecs_create_sdf_asymm_rect_wborder(kr_init_sdf_asymm_rect_wborder_t args);
ecs_entity_t kr_flecs_create_sdf_circle(kr_init_sdf_circle_t args);
ecs_entity_t kr_flecs_create_sdf_circle_wborder(kr_init_sdf_circle_wborder_t args);

/* Animation */

typedef enum kr_modifier_flag {
	KR_MODIFIER_NONE = 0,
	KR_MODIFIER_ANGLE,
	KR_MODIFIER_ROTATION_CENTER,
	KR_MODIFIER_X,
	KR_MODIFIER_Y,
	KR_MODIFIER_SCALE_X,
	KR_MODIFIER_SCALE_Y,
	KR_MODIFIER_OPACITY,
} kr_modifier_flag_t;

typedef struct kr_modifier {
	kr_modifier_flag_t modifier;
	float value, opt;
} kr_modifier_t;

typedef struct kr_init_animation {
	double start, duration;
	kr_tween_ease_t ease;
	bool loop;
	kr_modifier_t modifiers[8];
} kr_init_animation_t;

typedef struct kr_init_sequence {
	int count;
	kr_init_animation_t animations[16];
	bool loop;
} kr_init_sequence_t;

void kr_flecs_create_animation(ecs_entity_t e, const kr_init_animation_t *anim);
void kr_flecs_create_sequence(ecs_entity_t e, const kr_init_sequence_t *sequence);
