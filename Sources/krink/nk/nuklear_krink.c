#include "nuklear_krink.h"
#include <assert.h>
#include <math.h>
#include <kinc/input/keyboard.h>
#include <krink/eventhandler.h>
#include <krink/memory.h>

#define NK_IMPLEMENTATION
#ifdef NK_MAX_FLOAT_PRECISION
#undef NK_MAX_FLOAT_PRECISION
#endif
#define NK_MAX_FLOAT_PRECISION 6
#include <krink/nk/nuklear.h>

static struct nk_context nkctx;
static struct nk_user_font fnt;
static bool ctx_input_started = false;
static bool initialized = false;

static inline uint32_t nk_color_to_uint(struct nk_color color) {
	uint32_t c = 0;
	c = kr_color_set_channel(c, 'A', color.a);
	c = kr_color_set_channel(c, 'R', color.r);
	c = kr_color_set_channel(c, 'G', color.g);
	c = kr_color_set_channel(c, 'B', color.b);
	return c;
}

static inline float text_width(nk_handle handle, float h, const char *text, int len) {
	int characters[KR_NK_MAX_CHARACTERS];
	len = len <= KR_NK_MAX_CHARACTERS ? len : KR_NK_MAX_CHARACTERS;
	for (int i = 0; i < len; ++i) {
		characters[i] = (int)(unsigned)text[i];
	}
	float w = kr_ttf_width_of_characters((kr_ttf_font_t *)handle.ptr, (int)(h + 0.5f), characters,
	                                     0, len);
	return w;
}

static inline void begin_input(void) {
	assert(!ctx_input_started);
	nk_input_begin(&nkctx);
	ctx_input_started = true;
}

static enum nk_keys map_kinc_to_nk(int keycode) {
	switch (keycode) {
	case KINC_KEY_BACKSPACE:
		return NK_KEY_BACKSPACE;
	case KINC_KEY_TAB:
		return NK_KEY_TAB;
	case KINC_KEY_RETURN:
		return NK_KEY_ENTER;
	case KINC_KEY_SHIFT:
		return NK_KEY_SHIFT;
	case KINC_KEY_CONTROL:
		return NK_KEY_CTRL;
	case KINC_KEY_PAGE_UP:
		return NK_KEY_SCROLL_UP;
	case KINC_KEY_PAGE_DOWN:
		return NK_KEY_SCROLL_DOWN;
	case KINC_KEY_END:
		return NK_KEY_TEXT_END;
	case KINC_KEY_HOME:
		return NK_KEY_TEXT_START;
	case KINC_KEY_LEFT:
		return NK_KEY_LEFT;
	case KINC_KEY_UP:
		return NK_KEY_UP;
	case KINC_KEY_RIGHT:
		return NK_KEY_RIGHT;
	case KINC_KEY_DOWN:
		return NK_KEY_DOWN;
	default:
		break;
	}
	return NK_KEY_NONE;
}

static inline void notify(kr_evt_event_t evt) {
	switch (evt.event) {
	case KR_EVT_KEY_DOWN: {
		nk_input_key(&nkctx, map_kinc_to_nk(evt.data.key.keycode), 1);
	} break;
	case KR_EVT_KEY_UP: {
		nk_input_key(&nkctx, map_kinc_to_nk(evt.data.key.keycode), 0);
	} break;
	case KR_EVT_KEY_PRESS: {
		nk_input_unicode(&nkctx, evt.data.key_press.character);
	} break;
	case KR_EVT_MOUSE_SCROLL: {
		nk_input_scroll(&nkctx, (struct nk_vec2){0, -evt.data.mouse_scroll.delta});
	} break;
	case KR_EVT_PRIMARY_MOVE: {
		nk_input_motion(&nkctx, evt.data.primary.x, evt.data.primary.y);
	} break;
	case KR_EVT_PRIMARY_START: {
		nk_input_button(&nkctx, NK_BUTTON_LEFT, evt.data.primary.x, evt.data.primary.y, 1);
	} break;
	case KR_EVT_PRIMARY_END: {
		nk_input_button(&nkctx, NK_BUTTON_LEFT, evt.data.primary.x, evt.data.primary.y, 0);
	} break;
	case KR_EVT_FOREGROUND:
	case KR_EVT_BACKGROUND:
	case KR_EVT_PAUSE:
	case KR_EVT_RESUME:
	case KR_EVT_SHUTDOWN:
	case KR_EVT_MOUSE_MOVE:
	case KR_EVT_MOUSE_PRESS:
	case KR_EVT_MOUSE_RELEASE:
	case KR_EVT_FINGER_MOVE:
	case KR_EVT_FINGER_START:
	case KR_EVT_FINGER_END:

	default:
		break;
	}
}

void kr_nk_init(kr_ttf_font_t *font, float font_height, void *memory, nk_size size) {
	fnt.userdata.ptr = (void *)font;
	fnt.height = font_height;
	fnt.width = text_width;
	nk_init_fixed(&nkctx, memory, size, &fnt);
	ctx_input_started = false;
	kr_g2_set_font(font, (int)(font_height + 0.5f));
	kr_evt_add_observer(notify);
	initialized = true;
	begin_input();
}

struct nk_context *kr_nk_get_ctx(void) {
	assert(initialized);
	return &nkctx;
}

NK_API void kr_nk_render(int window, struct nk_color clear) {
	const struct nk_command *cmd;

	if (ctx_input_started) {
		nk_input_end(&nkctx);
		ctx_input_started = false;
	}

	kr_g2_begin(window);
	nk_foreach(cmd, &nkctx) {
		switch (cmd->type) {
		case NK_COMMAND_NOP:
			break;
		case NK_COMMAND_SCISSOR: {
			const struct nk_command_scissor *s = (const struct nk_command_scissor *)cmd;
			if (s->x < 0) {
				kr_g2_disable_scissor();
			}
			else {
				kr_g2_scissor((float)s->x, (float)s->y, (float)s->w, (float)s->h);
			}
		} break;
		case NK_COMMAND_LINE: {
			const struct nk_command_line *l = (const struct nk_command_line *)cmd;
			kr_g2_set_color(nk_color_to_uint(l->color));
			kr_g2_draw_line((float)l->begin.x, (float)l->begin.y, (float)l->end.x, (float)l->end.y,
			                (float)l->line_thickness);
		} break;
		case NK_COMMAND_RECT: {
			const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
			kr_g2_set_color(0);
			kr_g2_draw_sdf_rect_symm((float)r->x, (float)r->y, (float)r->w, (float)r->h,
			                         (float)r->rounding, (float)r->line_thickness,
			                         nk_color_to_uint(r->color), 2.2f);
		} break;
		case NK_COMMAND_RECT_FILLED: {
			const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;
			kr_g2_set_color(nk_color_to_uint(r->color));
			kr_g2_draw_sdf_rect_symm((float)r->x, (float)r->y, (float)r->w, (float)r->h,
			                         (float)r->rounding, 0.0f, 0, 2.2f);
		} break;
		case NK_COMMAND_CIRCLE: {
			const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
			kr_g2_set_color(0);
			float r = (float)(c->h > c->w ? c->h : c->w) / 2.0f;
			kr_g2_draw_sdf_circle((float)c->x + r, (float)c->y + r, r, (float)c->line_thickness,
			                      nk_color_to_uint(c->color), 2.2f);
		} break;
		case NK_COMMAND_CIRCLE_FILLED: {
			const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
			kr_g2_set_color(nk_color_to_uint(c->color));
			float r = (float)(c->h > c->w ? c->h : c->w) / 2.0f;
			kr_g2_draw_sdf_circle((float)c->x + r, (float)c->y + r, r, 0.0f, 0, 2.2f);
		} break;
		case NK_COMMAND_TRIANGLE: {
			const struct nk_command_triangle *t = (const struct nk_command_triangle *)cmd;
			kr_g2_set_color(nk_color_to_uint(t->color));
			kr_g2_draw_triangle((float)t->a.x, (float)t->a.y, (float)t->b.x, (float)t->b.y,
			                    (float)t->c.x, (float)t->c.y, (float)t->line_thickness);
		} break;
		case NK_COMMAND_TRIANGLE_FILLED: {
			const struct nk_command_triangle_filled *t =
			    (const struct nk_command_triangle_filled *)cmd;
			kr_g2_set_color(nk_color_to_uint(t->color));
			kr_g2_fill_triangle((float)t->a.x, (float)t->a.y, (float)t->b.x, (float)t->b.y,
			                    (float)t->c.x, (float)t->c.y);
		} break;
		case NK_COMMAND_TEXT: {
			const struct nk_command_text *t = (const struct nk_command_text *)cmd;
			kr_ttf_font_t *fnt = (kr_ttf_font_t *)t->font->userdata.ptr;
			kr_g2_set_color(nk_color_to_uint(t->background));
			kr_g2_fill_rect((float)t->x, (float)t->y, (float)t->w, (float)t->h);
			kr_g2_set_color(nk_color_to_uint(t->foreground));
			kr_g2_set_font(fnt, (int)(t->font->height + 0.5f));
			kr_g2_draw_string(t->string, (float)t->x,
			                  (float)(t->y + kr_ttf_baseline(fnt, (int)(t->font->height + 0.5f))));
		} break;
		case NK_COMMAND_IMAGE: {
			const struct nk_command_image *i = (const struct nk_command_image *)cmd;
			kr_image_t *img = (kr_image_t *)i->img.handle.ptr;
			kr_g2_set_color(nk_color_to_uint(i->col));
			kr_g2_draw_scaled_sub_image(img, (float)(i->img.region[0]), (float)(i->img.region[1]),
			                            (float)(i->img.region[2]), (float)(i->img.region[3]),
			                            (float)i->x, (float)i->y, (float)i->w, (float)i->h);
		} break;
		case NK_COMMAND_CURVE:{
			const struct nk_command_curve *c = (const struct nk_command_curve *)cmd;
			kr_g2_set_color(nk_color_to_uint(c->color));
			double px = 0.0;double py = 0.0;double u = 0.0;
			for(u = 0.0 ; u <= 1.0 ; u += 0.0001){
				px = pow(1-u,3)*c->begin.x+3*u*pow(1-u,2)*c->ctrl[0].x+3*pow(u,2)*(1-u)*c->ctrl[1].x+pow(u,3)*c->end.x;
				py = pow(1-u,3)*c->begin.y+3*u*pow(1-u,2)*c->ctrl[0].y+3*pow(u,2)*(1-u)*c->ctrl[1].y+pow(u,3)*c->end.y;
				kr_g2_fill_rect(px,py,c->line_thickness,c->line_thickness);
			}
		}break;
		case NK_COMMAND_POLYGON:
		case NK_COMMAND_POLYGON_FILLED:
		case NK_COMMAND_POLYLINE:
		case NK_COMMAND_RECT_MULTI_COLOR:
		case NK_COMMAND_ARC:
		case NK_COMMAND_ARC_FILLED:
		case NK_COMMAND_CUSTOM:
		default:
			break;
		}
	}
	nk_clear(&nkctx);
	begin_input();
	kr_g2_disable_scissor();
	kr_g2_end();
}
