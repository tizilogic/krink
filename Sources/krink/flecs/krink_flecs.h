#pragma once

#include "flecs.h"
#include <krink/graphics2/sdfpainter.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <krink/util/tween.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file krink_flecs.h
    \brief Provides the flecs integration into krink.
*/

/*! \def KR_FLECS_EVT_BUFFER_SIZE
    \brief Size of the internal eventbuffer that stores events between frames.
*/
#ifndef KR_FLECS_EVT_BUFFER_SIZE
#define KR_FLECS_EVT_BUFFER_SIZE 256
#endif

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
	bool bilinear;
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

/*! \var ecs_world_t *kr_world
 * 	\brief The flecs world.
 */
extern ecs_world_t *kr_world;

/// <summary>
/// Call before using flecs.
/// \note The REST API will only activate in DEBUG mode
/// </summary>
/// <param name="with_flecs_rest">Whether to activate the flecs REST API</param>
void kr_flecs_init(bool with_flecs_rest);

/// <summary>
/// Call at end to cleanly shutdown flecs.
/// </summary>
void kr_flecs_destroy(void);

/// <summary>
/// Call inside your update callback.
/// </summary>
void kr_flecs_tick(void);

/// <summary>
/// Convenience function to change the depth value of a drawable entity.
/// </summary>
/// <param name="world">The world</param>
/// <param name="e">The entity</param>
/// <param name="depth">Draw depth value in the range -1024..1024</param>
void kr_flecs_set_depth(ecs_world_t *world, ecs_entity_t e, int32_t depth);

/// <summary>
/// Factrory function to create a sprite.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sprite(ecs_world_t *world, kr_init_sprite_t args);

/// <summary>
/// Factrory function to create a text.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_text(ecs_world_t *world, kr_init_text_t args);

/// <summary>
/// Factrory function to create a stroked rect.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_stroked_rect(ecs_world_t *world, kr_init_stroked_rect_t args);

/// <summary>
/// Factrory function to create a filled rect.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_filled_rect(ecs_world_t *world, kr_init_filled_rect_t args);

/// <summary>
/// Factrory function to create a stroked triangle.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_stroked_triangle(ecs_world_t *world, kr_init_stroked_triangle_t args);

/// <summary>
/// Factrory function to create a filled triangle.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_filled_triangle(ecs_world_t *world, kr_init_filled_triangle_t args);

/// <summary>
/// Factrory function to create a line.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_line(ecs_world_t *world, kr_init_line_t args);

/// <summary>
/// Factrory function to create a SDF rect.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_rect(ecs_world_t *world, kr_init_sdf_rect_t args);

/// <summary>
/// Factrory function to create a SDF rect with asymmetric corner radii.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_asymm_rect(ecs_world_t *world, kr_init_sdf_asymm_rect_t args);

/// <summary>
/// Factrory function to create a SDF rect with a border.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_rect_wborder(ecs_world_t *world, kr_init_sdf_rect_wborder_t args);

/// <summary>
/// Factrory function to create a SDF rect with asymmetric corner radii and border.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_asymm_rect_wborder(ecs_world_t *world, kr_init_sdf_asymm_rect_wborder_t args);

/// <summary>
/// Factrory function to create a SDF circle.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_circle(ecs_world_t *world, kr_init_sdf_circle_t args);

/// <summary>
/// Factrory function to create a SDF circle with border.
/// </summary>
/// <param name="world">The world</param>
/// <param name="args">Initializer</param>
/// <returns>A fully set up entity</returns>
ecs_entity_t kr_flecs_create_sdf_circle_wborder(ecs_world_t *world, kr_init_sdf_circle_wborder_t args);

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

/// <summary>
/// Factrory function to create an animation for an entity.
/// </summary>
/// <param name="world">The world</param>
/// <param name="e">The entity</param>
/// <param name="anim">Initializer</param>
void kr_flecs_create_animation(ecs_world_t *world, ecs_entity_t e, const kr_init_animation_t *anim);

/// <summary>
/// Factrory function to create a sequence of animations for an entity.
/// </summary>
/// <param name="world">The world</param>
/// <param name="e">The entity</param>
/// <param name="sequence">Initializer</param>
void kr_flecs_create_sequence(ecs_world_t *world, ecs_entity_t e, const kr_init_sequence_t *sequence);
