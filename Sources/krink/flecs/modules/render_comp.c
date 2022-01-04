#define COMPONENTS_RENDER_IMPL
#include "render_comp.h"

ECS_DECLARE(KrCompVisible);
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

	ECS_TAG_DEFINE(world, KrCompVisible);
	ECS_META_COMPONENT(world, KrCompPipeline);
	ECS_META_COMPONENT(world, KrCompDrawable);
	ECS_META_COMPONENT(world, KrSingletonClearColor);
	ECS_META_COMPONENT(world, KrCompColor);
	ECS_META_COMPONENT(world, KrCompPos2);
	ECS_META_COMPONENT(world, KrCompImage);
	ECS_META_COMPONENT(world, KrCompText);
	ECS_META_COMPONENT(world, KrCompStroke);
	ECS_META_COMPONENT(world, KrCompSmooth);
	ECS_META_COMPONENT(world, KrCompTriangle);
	ECS_META_COMPONENT(world, KrCompRect);
	ECS_META_COMPONENT(world, KrCompScissor);
	ECS_META_COMPONENT(world, KrCompCorner);
	ECS_META_COMPONENT(world, KrCompCornerAsymm);
	ECS_META_COMPONENT(world, KrCompCircle);
	ECS_META_COMPONENT(world, KrCompBorder);
	ECS_META_COMPONENT(world, KrCompLine);

	ECS_META_COMPONENT(world, KrCompAngle);
	ECS_META_COMPONENT(world, KrCompRotationCenter);
	ECS_META_COMPONENT(world, KrCompOpacity);
	ECS_META_COMPONENT(world, KrCompScale);
	ECS_META_COMPONENT(world, KrCompScaleX);
	ECS_META_COMPONENT(world, KrCompScaleY);
	ECS_META_COMPONENT(world, KrCompTranslation);

	ECS_PREFAB_DEFINE(world, KrPrefabSprite, KrCompDrawable, KrCompPos2, KrCompColor, KrCompImage);
	ECS_PREFAB_DEFINE(world, KrPrefabText, KrCompDrawable, KrCompPos2, KrCompColor, KrCompText);
	ECS_PREFAB_DEFINE(world, KrPrefabStrokedRect, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompRect, KrCompStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabFilledRect, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompRect);
	ECS_PREFAB_DEFINE(world, KrPrefabStrokedTriangle, KrCompDrawable, KrCompColor, KrCompTriangle,
	                  KrCompStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabFilledTriangle, KrCompDrawable, KrCompColor, KrCompTriangle);
	ECS_PREFAB_DEFINE(world, KrPrefabLine, KrCompDrawable, KrCompPos2, KrCompColor, KrCompLine,
	                  KrCompStroke);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRect, KrCompDrawable, KrCompPos2, KrCompColor, KrCompRect,
	                  KrCompCorner, KrCompSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectAsymm, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompRect, KrCompCornerAsymm, KrCompSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectWborder, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompRect, KrCompCorner, KrCompBorder, KrCompSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfRectAsymmWborder, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompRect, KrCompCornerAsymm, KrCompBorder, KrCompSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfCircle, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompCircle, KrCompSmooth);
	ECS_PREFAB_DEFINE(world, KrPrefabSdfCircleWborder, KrCompDrawable, KrCompPos2, KrCompColor,
	                  KrCompCircle, KrCompBorder, KrCompSmooth);
}
