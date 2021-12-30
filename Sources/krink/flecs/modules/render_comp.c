#include "render_comp.h"

void ComponentsRenderImport(ecs_world_t *world) {
	/* Define module */
	ECS_MODULE(world, ComponentsRender);

	/* Register components */
	ECS_COMPONENT(world, kr_comp_drawable_t);
	ECS_COMPONENT(world, kr_comp_color_t);
	ECS_COMPONENT(world, kr_comp_image_t);
	ECS_COMPONENT(world, kr_comp_text_t);
	ECS_COMPONENT(world, kr_comp_stroke_t);
	ECS_COMPONENT(world, kr_comp_smooth_t);
	ECS_TAG(world, kr_comp_fill_t);
	ECS_COMPONENT(world, kr_comp_triangle_t);
	ECS_COMPONENT(world, kr_comp_rect_t);
	ECS_COMPONENT(world, kr_comp_scissor_t);
	ECS_COMPONENT(world, kr_comp_corner_t);
	ECS_COMPONENT(world, kr_comp_corner_asymm_t);
	ECS_COMPONENT(world, kr_comp_circle_t);
	ECS_COMPONENT(world, kr_comp_border_t);
	ECS_COMPONENT(world, kr_comp_line_t);
	ECS_PREFAB(world, kr_prefab_sprite_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_image_t);
	ECS_PREFAB(world, kr_prefab_text_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_text_t);
	ECS_PREFAB(world, kr_prefab_stroked_rect_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_rect_t,
	           kr_comp_stroke_t);
	ECS_PREFAB(world, kr_prefab_filled_rect_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_rect_t,
	           kr_comp_fill_t);
	ECS_PREFAB(world, kr_prefab_stroked_triangle_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_triangle_t, kr_comp_stroke_t);
	ECS_PREFAB(world, kr_prefab_filled_triangle_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_triangle_t, kr_comp_fill_t);
	ECS_PREFAB(world, kr_prefab_line_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_line_t,
	           kr_comp_stroke_t);
	ECS_PREFAB(world, kr_prefab_sdf_rect_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_rect_t,
	           kr_comp_corner_t, kr_comp_smooth_t);
	ECS_PREFAB(world, kr_prefab_sdf_asymm_rect_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_rect_t, kr_comp_corner_asymm_t, kr_comp_smooth_t);
	ECS_PREFAB(world, kr_prefab_sdf_rect_wborder_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_rect_t, kr_comp_corner_t, kr_comp_border_t, kr_comp_smooth_t);
	ECS_PREFAB(world, kr_prefab_sdf_asymm_rect_wborder_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_rect_t, kr_comp_corner_asymm_t, kr_comp_border_t, kr_comp_smooth_t);
	ECS_PREFAB(world, kr_prefab_sdf_circle_t, kr_comp_drawable_t, kr_comp_color_t, kr_comp_circle_t,
	           kr_comp_smooth_t);
	ECS_PREFAB(world, kr_prefab_sdf_circle_wborder_t, kr_comp_drawable_t, kr_comp_color_t,
	           kr_comp_circle_t, kr_comp_border_t, kr_comp_smooth_t);

	/* Export handles to module contents */
	ECS_EXPORT_COMPONENT(kr_comp_drawable_t);
	ECS_EXPORT_COMPONENT(kr_comp_color_t);
	ECS_EXPORT_COMPONENT(kr_comp_image_t);
	ECS_EXPORT_COMPONENT(kr_comp_text_t);
	ECS_EXPORT_COMPONENT(kr_comp_stroke_t);
	ECS_EXPORT_COMPONENT(kr_comp_smooth_t);
	ECS_EXPORT_ENTITY(kr_comp_fill_t);
	ECS_EXPORT_COMPONENT(kr_comp_triangle_t);
	ECS_EXPORT_COMPONENT(kr_comp_rect_t);
	ECS_EXPORT_COMPONENT(kr_comp_scissor_t);
	ECS_EXPORT_COMPONENT(kr_comp_corner_t);
	ECS_EXPORT_COMPONENT(kr_comp_corner_asymm_t);
	ECS_EXPORT_COMPONENT(kr_comp_circle_t);
	ECS_EXPORT_COMPONENT(kr_comp_border_t);
	ECS_EXPORT_COMPONENT(kr_comp_line_t);

	ECS_EXPORT_ENTITY(kr_prefab_sprite_t);
	ECS_EXPORT_ENTITY(kr_prefab_text_t);
	ECS_EXPORT_ENTITY(kr_prefab_stroked_rect_t);
	ECS_EXPORT_ENTITY(kr_prefab_filled_rect_t);
	ECS_EXPORT_ENTITY(kr_prefab_stroked_triangle_t);
	ECS_EXPORT_ENTITY(kr_prefab_filled_triangle_t);
	ECS_EXPORT_ENTITY(kr_prefab_line_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_rect_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_asymm_rect_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_rect_wborder_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_asymm_rect_wborder_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_circle_t);
	ECS_EXPORT_ENTITY(kr_prefab_sdf_circle_wborder_t);
}
