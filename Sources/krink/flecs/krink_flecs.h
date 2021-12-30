#pragma once

#include "flecs.h"
#include <krink/graphics2/sdfpainter.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <stdint.h>

typedef struct kr_rect {
	float x, y, w, h;
} kr_rect_t;

typedef struct kr_init_sprite {
	kr_image_t *image;
	kr_rect src, dest;
	uint32_t color;
} kr_init_sprite_t;

typedef struct kr_init_text {
	const char *text;
	kr_ttf_font_t *font;
	float size;
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
	kr_init_sdf_rect_t sdfrect;
	float border;
	uint32_t border_color;
} kr_init_sdf_rect_wborder_t;

typedef struct kr_init_sdf_asymm_rect_wborder {
	kr_init_sdf_asymm_rect_t sdfrect;
	float border;
	uint32_t border_color;
} kr_init_sdf_asymm_rect_wborder_t;

typedef struct kr_init_sdf_circle {
	float x, y, radius;
	uint32_t color;
} kr_init_sdf_circle_t;

typedef struct kr_init_sdf_circle_wborder {
	kr_init_sdf_circle_t sdfcircle;
	float border;
	uint32_t color;
} kr_init_sdf_circle_wborder_t;

extern ecs_world_t *kr_world;

void kr_flecs_init(void);
void kr_flecs_destroy(void);
void kr_flecs_tick(void);
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
