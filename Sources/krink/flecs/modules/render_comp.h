#pragma once
#include <krink/flecs/flecs.h>
#include <krink/graphics2/ttf.h>
#include <krink/image.h>
#include <stdbool.h>
#include <stdint.h>

/*! \file render_comp.h
    \brief Provides flecs components for rendering.
*/

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef COMPONENTS_RENDER_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

ECS_ENUM(KrPipeline, {
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
                     });

/* Tags */

extern ECS_DECLARE(KrVisible);

/* Component types */
ECS_STRUCT(KrDrawable, {
	int32_t depth;
	int32_t sort_extra;
	KrPipeline pipeline;
});

ECS_STRUCT(KrSingletonClearColor, { uint32_t color; });

ECS_STRUCT(KrColor, { uint32_t color; });

ECS_STRUCT(KrPos2, {
	float x;
	float y;
});

ECS_STRUCT(KrImage, {
	kr_image_t *image;
	float sx;
	float sy;
	float sw;
	float sh;
	float dw;
	float dh;
	bool bilinear;
});

ECS_STRUCT(KrText, {
	const char *text;
	kr_ttf_font_t *font;
	int32_t size;
});

ECS_STRUCT(KrStroke, { float strength; });

ECS_STRUCT(KrSmooth, { float px; });

ECS_STRUCT(KrTriangle, {
	float x1;
	float y1;
	float x2;
	float y2;
	float x3;
	float y3;
});

ECS_STRUCT(KrRect, {
	float w;
	float h;
});

ECS_STRUCT(KrScissor, {
	float x;
	float y;
	float w;
	float h;
});

ECS_STRUCT(KrCorner, { float radius; });

ECS_STRUCT(KrCornerAsymm, {
	float tl;
	float bl;
	float tr;
	float br;
});

ECS_STRUCT(KrCircle, { float radius; });

ECS_STRUCT(KrBorder, {
	float strength;
	uint32_t color;
});

ECS_STRUCT(KrLine, {
	float x_to;
	float y_to;
});

// Optional Components
ECS_STRUCT(KrAngle, { float radians; });
ECS_STRUCT(KrRotationCenter, {
	float x;
	float y;
});
ECS_STRUCT(KrOpacity, { float alpha; });
ECS_STRUCT(KrScaleX, { float value; });
ECS_STRUCT(KrScaleY, { float value; });
ECS_STRUCT(KrTranslation, {
	float x;
	float y;
});

ECS_STRUCT(KrFrameTime, { double time; });

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
