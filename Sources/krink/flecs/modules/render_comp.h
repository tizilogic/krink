#pragma once
#include <krink/flecs/flecs.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <stdbool.h>
#include <stdint.h>

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_RENDER_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kr_comp_pipeline {
	KR_COMP_PP_IMAGE = 0,
	KR_COMP_PP_TEXT,
	KR_COMP_PP_LINE,
	KR_COMP_PP_COLOR_TRIS_STROKE,
	KR_COMP_PP_COLOR_TRIS_FILL,
	KR_COMP_PP_COLOR_QUAD_STROKE,
	KR_COMP_PP_COLOR_QUAD_FILL,
	KR_COMP_PP_SDF_RECT,
	KR_COMP_PP_SDF_RECT_ASYMM,
	KR_COMP_PP_SDF_RECT_WBORDER,
	KR_COMP_PP_SDF_RECT_ASYMM_WBORDER,
	KR_COMP_PP_SDF_CIRCLE,
	KR_COMP_PP_SDF_CIRCLE_WBORDER,
	KR_COMP_PP_SDF_LINE,
} kr_comp_pipeline_t;

/* Tags */

extern ECS_DECLARE(KrCompVisible);

/* Component types */
ECS_STRUCT(KrCompDrawable, {
	int32_t depth, sort_extra;
	kr_comp_pipeline_t pipeline;
});

ECS_STRUCT(KrSingletonClearColor, { uint32_t color; });

ECS_STRUCT(KrCompColor, { uint32_t color; });

ECS_STRUCT(KrCompPos2, {
	float x;
	float y;
});

ECS_STRUCT(KrCompImage, {
	kr_image_t *image;
	float sx; float sy; float sw; float sh; float dw; float dh;
});

ECS_STRUCT(KrCompText, {
	const char *text;
	kr_ttf_font_t *font;
	int32_t size;
});

ECS_STRUCT(KrCompStroke, { float strength; });

ECS_STRUCT(KrCompSmooth, { float px; });

ECS_STRUCT(KrCompTriangle, { float x1; float y1; float x2; float y2; float x3; float y3; });

ECS_STRUCT(KrCompRect, { float w; float h; });

ECS_STRUCT(KrCompScissor, { float x; float y; float w; float h; });

ECS_STRUCT(KrCompCorner, { float radius; });

ECS_STRUCT(KrCompCornerAsymm, { float tl; float bl; float tr; float br; });

ECS_STRUCT(KrCompCircle, { float radius; });

ECS_STRUCT(KrCompBorder, {
	float strength;
	uint32_t color;
});

ECS_STRUCT(KrCompLine, { float x_to; float y_to; });

/* Prefabs */

extern ECS_DECLARE(KrPrefabSprite);
extern ECS_DECLARE(KrPrefabText);
extern ECS_DECLARE(KrPrefabStrokedRect);
extern ECS_DECLARE(KrPrefabFilledRect);
extern ECS_DECLARE(KrPrefabStrokedTriangle);
extern ECS_DECLARE(KrPrefabFilledTriangle);
extern ECS_DECLARE(KrPrefabLine);
extern ECS_DECLARE(KrPrefabSdfRect);
extern ECS_DECLARE(KrPrefabSdfRectAsymm);
extern ECS_DECLARE(KrPrefabSdfRectWborder);
extern ECS_DECLARE(KrPrefabSdfRectAsymmWborder);
extern ECS_DECLARE(KrPrefabSdfCircle);
extern ECS_DECLARE(KrPrefabSdfCircleWborder);


void ComponentsRenderImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif
