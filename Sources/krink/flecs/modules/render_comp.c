#define COMPONENTS_RENDER_IMPL
#include "render_comp.h"

ECS_DECLARE(KrVisible);
ECS_DECLARE(KrPrefabSprite);
ECS_DECLARE(KrPrefabText);
ECS_DECLARE(KrPrefabStrokedRect);
ECS_DECLARE(KrPrefabFilledRect);
ECS_DECLARE(KrPrefabStrokedTriangle);
ECS_DECLARE(KrPrefabFilledTriangle);
ECS_DECLARE(KrPrefabLine);
ECS_DECLARE(KrPrefabSdfRect);
ECS_DECLARE(KrPrefabSdfRectAsymm);
ECS_DECLARE(KrPrefabSdfRectWborder);
ECS_DECLARE(KrPrefabSdfRectAsymmWborder);
ECS_DECLARE(KrPrefabSdfCircle);
ECS_DECLARE(KrPrefabSdfCircleWborder);

void ComponentsRenderImport(ecs_world_t *world) {
	ECS_MODULE(world, ComponentsRender);

	ECS_TAG_DEFINE(world, KrVisible);
	ECS_META_COMPONENT(world, KrPipeline);
	ECS_META_COMPONENT(world, KrDrawable);
	ECS_META_COMPONENT(world, KrSingletonClearColor);
	ECS_META_COMPONENT(world, KrColor);
	ECS_META_COMPONENT(world, KrPos2);
	ECS_META_COMPONENT(world, KrImage);
	ECS_META_COMPONENT(world, KrText);
	ECS_META_COMPONENT(world, KrStroke);
	ECS_META_COMPONENT(world, KrSmooth);
	ECS_META_COMPONENT(world, KrTriangle);
	ECS_META_COMPONENT(world, KrRect);
	ECS_META_COMPONENT(world, KrScissor);
	ECS_META_COMPONENT(world, KrCorner);
	ECS_META_COMPONENT(world, KrCornerAsymm);
	ECS_META_COMPONENT(world, KrCircle);
	ECS_META_COMPONENT(world, KrBorder);
	ECS_META_COMPONENT(world, KrLine);

	ECS_META_COMPONENT(world, KrAngle);
	ECS_META_COMPONENT(world, KrRotationCenter);
	ECS_META_COMPONENT(world, KrOpacity);
	ECS_META_COMPONENT(world, KrScaleX);
	ECS_META_COMPONENT(world, KrScaleY);
	ECS_META_COMPONENT(world, KrTranslation);
	ECS_META_COMPONENT(world, KrAnchor);
	ECS_META_COMPONENT(world, KrSetDepth);

	ECS_META_COMPONENT(world, KrFrameTime);

	ECS_PREFAB_DEFINE(world, KrPrefabSprite, KrDrawable, KrPos2, KrColor, KrImage);
	ECS_PREFAB_DEFINE(world, KrPrefabText, KrDrawable, KrPos2, KrColor, KrText);
	ECS_PREFAB_DEFINE(world, KrPrefabStrokedRect, KrDrawable, KrPos2, KrColor, KrRect, KrStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabFilledRect, KrDrawable, KrPos2, KrColor, KrRect);
	ECS_PREFAB_DEFINE(world, KrPrefabStrokedTriangle, KrDrawable, KrColor, KrTriangle, KrStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabFilledTriangle, KrDrawable, KrColor, KrTriangle);
	ECS_PREFAB_DEFINE(world, KrPrefabLine, KrDrawable, KrPos2, KrColor, KrLine, KrStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRect, KrDrawable, KrPos2, KrColor, KrRect, KrCorner,
	                  KrSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectAsymm, KrDrawable, KrPos2, KrColor, KrRect,
	                  KrCornerAsymm, KrSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectWborder, KrDrawable, KrPos2, KrColor, KrRect, KrCorner,
	                  KrBorder, KrSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectAsymmWborder, KrDrawable, KrPos2, KrColor, KrRect,
	                  KrCornerAsymm, KrBorder, KrSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfCircle, KrDrawable, KrPos2, KrColor, KrCircle, KrSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfCircleWborder, KrDrawable, KrPos2, KrColor, KrCircle,
	                  KrBorder, KrSmooth);
}
