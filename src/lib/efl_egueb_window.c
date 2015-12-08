/* EFL-Egueb Egueb based EFL extensions
 * Copyright (C) 2013 - 2014 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "efl_egueb_private.h"
#include "efl_egueb_main.h"
#include "efl_egueb_window.h"

#if BUILD_ECORE_X
#include "efl_egueb_window_x.h"
#endif

#if BUILD_ECORE_WIN32
#include "efl_egueb_window_win32.h"
#endif

#include "efl_egueb_window_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Egueb_Timeout
{
	Ecore_Timer *timer;
	Egueb_Dom_Window_Descriptor_Timeout_Cb cb;
	Egueb_Dom_Window *win;
	void *user_data;
} Efl_Egueb_Timeout;

static Eina_Bool _efl_egueb_window_timout_cb(void *data)
{
	Efl_Egueb_Timeout *to = data;
	to->cb(to->win, to->user_data);
	return EINA_TRUE;
}

static inline Eina_Bool _check_window(Efl_Egueb_Window *thiz, Ecore_Window w)
{
	Ecore_Window real;

	real = thiz->desc->window_get(thiz->data);
	if (real == w) return EINA_TRUE;
	else return EINA_FALSE;
}

static void _efl_egueb_window_transform_key(Ecore_Event_Key *ev,
		Egueb_Dom_String **key, Egueb_Dom_String **code,
		Egueb_Dom_Key_Location *location)
{
	if (ev->string)
	{
		*code = egueb_dom_string_new_with_string(ev->keyname);
		if (!strncmp(ev->key, "KP_", 3))
		{
			*key = egueb_dom_string_new_with_string(ev->string);
			*location = EGUEB_DOM_KEY_LOCATION_NUMPAD;
		}
		else if (!strcmp(ev->key, "Tab"))
		{
			*key = egueb_dom_string_new_with_string(ev->key);
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Return"))
		{
			*key = egueb_dom_string_new_with_string("Enter");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Escape"))
		{
			*key = egueb_dom_string_new_with_string(ev->keyname);
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "BackSpace"))
		{
			*key = egueb_dom_string_new_with_string(ev->keyname);
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else
		{
			*key = egueb_dom_string_new_with_string(ev->string);
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
	}
	else
	{
		if (!strcmp(ev->key, "Control_L"))
		{
			*key = egueb_dom_string_new_with_string("Control");
			*code = egueb_dom_string_new_with_string("ControlLeft");
			*location = EGUEB_DOM_KEY_LOCATION_LEFT;
		}
		else if (!strcmp(ev->key, "Control_R"))
		{
			*key = egueb_dom_string_new_with_string("Control");
			*code = egueb_dom_string_new_with_string("ControlRight");
			*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
		}
		else if (!strcmp(ev->key, "Shift_L"))
		{
			*key = egueb_dom_string_new_with_string("Shift");
			*code = egueb_dom_string_new_with_string("ShiftLeft");
			*location = EGUEB_DOM_KEY_LOCATION_LEFT;
		}
		else if (!strcmp(ev->key, "Shift_R"))
		{
			*key = egueb_dom_string_new_with_string("Shift");
			*code = egueb_dom_string_new_with_string("ShiftRight");
			*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
		}
		else if (!strcmp(ev->key, "Alt_L"))
		{
			*key = egueb_dom_string_new_with_string("Alt");
			*code = egueb_dom_string_new_with_string("AltLeft");
			*location = EGUEB_DOM_KEY_LOCATION_LEFT;
		}
		else if (!strcmp(ev->key, "Alt_R"))
		{
			*key = egueb_dom_string_new_with_string("Alt");
			*code = egueb_dom_string_new_with_string("AltRight");
			*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
		}
		else if (!strcmp(ev->key, "Super_L"))
		{
			*key = egueb_dom_string_new_with_string("Super");
			*code = egueb_dom_string_new_with_string("SuperLeft");
			*location = EGUEB_DOM_KEY_LOCATION_LEFT;
		}
		else if (!strcmp(ev->key, "Super_R"))
		{
			*key = egueb_dom_string_new_with_string("Super");
			*code = egueb_dom_string_new_with_string("SuperRight");
			*location = EGUEB_DOM_KEY_LOCATION_RIGHT;
		}
		else if (!strcmp(ev->key, "ISO_Level3_Shift"))
		{
			*key = egueb_dom_string_new_with_string("AltGraph");
			*code = egueb_dom_string_new_with_string("AltGraph");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Left"))
		{
			*key = egueb_dom_string_new_with_string("ArrowLeft");
			*code = egueb_dom_string_new_with_string("ArrowLeft");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Right"))
		{
			*key = egueb_dom_string_new_with_string("ArrowRight");
			*code = egueb_dom_string_new_with_string("ArrowRight");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Down"))
		{
			*key = egueb_dom_string_new_with_string("ArrowDown");
			*code = egueb_dom_string_new_with_string("ArrowDown");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Up"))
		{
			*key = egueb_dom_string_new_with_string("ArrowUp");
			*code = egueb_dom_string_new_with_string("ArrowUp");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Prior"))
		{
			*key = egueb_dom_string_new_with_string("PageUp");
			*code = egueb_dom_string_new_with_string("PageUp");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strcmp(ev->key, "Next"))
		{
			*key = egueb_dom_string_new_with_string("PageDown");
			*code = egueb_dom_string_new_with_string("PageDown");
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
		else if (!strncmp(ev->key, "KP_", 3))
		{
			*key = egueb_dom_string_new_with_string(ev->key + 3);
			*code = egueb_dom_string_new_with_string(ev->key + 3);
			*location = EGUEB_DOM_KEY_LOCATION_NUMPAD;
		}
		else
		{
			*key = egueb_dom_string_new_with_string(ev->key);
			*code = egueb_dom_string_new_with_string(ev->keyname);
			*location = EGUEB_DOM_KEY_LOCATION_STANDARD;
		}
	}
}

/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _efl_egueb_window_key_down(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Key *ev = event;
	Egueb_Dom_String *key = NULL;
	Egueb_Dom_String *code = NULL;
	Egueb_Dom_Key_Location location;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;
	if (!thiz->input)
		return EINA_TRUE;

	_efl_egueb_window_transform_key(ev, &key, &code, &location);
	egueb_dom_input_feed_key_down(thiz->input, key, code, location);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_key_up(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Key *ev = event;
	Egueb_Dom_String *key, *code;
	Egueb_Dom_Key_Location location;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;
	if (!thiz->input)
		return EINA_TRUE;

	_efl_egueb_window_transform_key(ev, &key, &code, &location);
	egueb_dom_input_feed_key_up(thiz->input, key, code, location);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_button_down(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Button *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->input)
		egueb_dom_input_feed_mouse_down(thiz->input, ev->buttons);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_button_up(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Button *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->input)
		egueb_dom_input_feed_mouse_up(thiz->input, ev->buttons);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_move(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Move *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->input) egueb_dom_input_feed_mouse_move(thiz->input, ev->x, ev->y);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_wheel(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Wheel *ev = event;
	int deltax = 0, deltay = 0, deltaz = 0;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;
	switch (ev->direction)
	{
		/* up/down */
		case 0:
		deltay = ev->z;
		break;

		default:
		ERR("Unsupported direction");
		break;
	}
	if (thiz->input) egueb_dom_input_feed_mouse_wheel(thiz->input, deltax, deltay, deltaz);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_in(void *data,
		int type, void *event)
{
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_out(void *data,
		int type, void *event)
{
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_damages(Egueb_Dom_Feature *e EINA_UNUSED,
		Eina_Rectangle *area, void *data)
{
	Efl_Egueb_Window *thiz = data;
	Eina_Rectangle *r;

	/* TODO clip the rectangle against the window area */
	/* FIXME instead of allocating, just get the rectangle from a pool of rectangles */
	r = malloc(sizeof(Eina_Rectangle));
	*r = *area;

	DBG("Adding damage area of %" EINA_RECTANGLE_FORMAT, EINA_RECTANGLE_ARGS(area));
	thiz->damages = eina_list_append(thiz->damages, r);
	return EINA_TRUE;
}

static void _efl_egueb_window_topmost_cleanup(Efl_Egueb_Window *thiz)
{
	if (thiz->input)
		egueb_dom_input_unref(thiz->input);
	if (thiz->window)
		egueb_dom_feature_unref(thiz->window);
	if (thiz->render)
		egueb_dom_feature_unref(thiz->render);
	if (thiz->topmost)
		egueb_dom_node_unref(thiz->topmost);
}

static Eina_Bool _efl_egueb_window_topmost_setup(Efl_Egueb_Window *thiz,
		Egueb_Dom_Node *topmost)
{
	Egueb_Dom_Feature *render;
	Egueb_Dom_Feature *window;
	Egueb_Dom_Feature *ui;
	Eina_Bool ret = EINA_FALSE;

	/* check it it has the render feature */
	render = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!render)
	{
		ERR("The topmost element does not have a render feature");
		goto no_render;
	}

	window = egueb_dom_node_feature_get(topmost, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!window)
	{
		ERR("The topmost element does not have a window feature");
		goto no_window;
	}

	thiz->topmost = egueb_dom_node_ref(topmost);
	thiz->render = egueb_dom_feature_ref(render);
	thiz->window = egueb_dom_feature_ref(window);

	/* optional features */
	ui = egueb_dom_node_feature_get(topmost,
			EGUEB_DOM_FEATURE_UI_NAME, NULL);
	if (ui)
	{
		thiz->input = egueb_dom_feature_ui_input_get(ui);
		egueb_dom_feature_unref(ui);
	}
	ret = EINA_TRUE;

	egueb_dom_feature_unref(window);
no_window:
	egueb_dom_feature_unref(render);
no_render:
	return ret;
}

static void _efl_egueb_window_cleanup(Efl_Egueb_Window *thiz)
{
	_efl_egueb_window_topmost_cleanup(thiz);
	if (thiz->doc)
		egueb_dom_node_unref(thiz->doc);
}

static Eina_Bool _efl_egueb_window_setup(Efl_Egueb_Window *thiz, Egueb_Dom_Node *doc)
{
	Egueb_Dom_Node *topmost;
	Eina_Bool ret = EINA_FALSE;

	/* check if it is a doc */
	if (egueb_dom_node_type_get(doc) != EGUEB_DOM_NODE_TYPE_DOCUMENT)
	{
		ERR("The provided node is not a document");
		goto no_doc;
	}

	topmost = egueb_dom_document_document_element_get(doc);
	if (!topmost)
	{
		ERR("No topmost element found");
		goto no_doc;
	}

	if (!_efl_egueb_window_topmost_setup(thiz, topmost))
		goto no_setup;

	thiz->doc = egueb_dom_node_ref(doc);
	ret = EINA_TRUE;
no_setup:
	egueb_dom_node_unref(topmost);
no_doc:
	egueb_dom_node_unref(doc);
	return ret;
}

static Eina_Bool _efl_egueb_window_idle_enterer_cb(void *data)
{
	Efl_Egueb_Window *thiz = data;

	/* check that the document has not changed */
	if (thiz->doc != thiz->edoc.doc)
	{
		_efl_egueb_window_cleanup(thiz);
		if (thiz->edoc.doc)
			_efl_egueb_window_setup(thiz, egueb_dom_node_ref(thiz->edoc.doc));
	}

	egueb_dom_document_process(thiz->doc);
	if (!thiz->render)
		goto done;

	/* if need to draw, get the damages, and draw */
	if (thiz->b && thiz->s)
	{
		Eina_Rectangle *r;

		egueb_dom_feature_render_damages_get(thiz->render, thiz->s,
				_efl_egueb_window_damages, thiz);
		if (!thiz->damages) goto done;

		enesim_renderer_draw_list(thiz->background, thiz->s,
				ENESIM_ROP_FILL, thiz->damages, 0, 0, NULL);
		egueb_dom_feature_render_draw_list(thiz->render, thiz->s,
				ENESIM_ROP_BLEND, thiz->damages, 0, 0, NULL);
		/* convert */
		enesim_converter_surface(thiz->s, thiz->b);
		if (!thiz->desc->output_update)
		{
			EINA_LIST_FREE(thiz->damages, r)
				free(r);
		}
		else
		{
			EINA_LIST_FREE(thiz->damages, r)
			{
				thiz->desc->output_update(thiz->data, r);
				free(r);
			}
		}
	}
done:
	return EINA_TRUE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void efl_egueb_window_update_size(Efl_Egueb_Window *thiz, int w, int h)
{
	thiz->w = w;
	thiz->h = h;

	/* TODO do we actually need this function? */
	if (thiz->window)
	{
		egueb_dom_feature_window_size_set(thiz->window,
				thiz->w, thiz->h);
	}
	egueb_dom_window_resize_notify(thiz->win);
}

Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		int x, int y, int *w, int *h,
		const Efl_Egueb_Window_Descriptor *d, void *data)
{
	Efl_Egueb_Window *thiz;

	if (!doc) return NULL;

	thiz = calloc(1, sizeof(Efl_Egueb_Window));
	if (!_efl_egueb_window_setup(thiz, egueb_dom_node_ref(doc)))
		return NULL;
	efl_egueb_document_setup(&thiz->edoc, doc);
	thiz->desc = d;
	thiz->data = data;

	/* in case the user does not provide a size */
	if (*w <= 0 || *h <= 0)
	{
		Egueb_Dom_Feature_Window_Hint_Data wdata;
		int whints;

		whints = egueb_dom_feature_window_hints_get(thiz->window, &wdata);
		if (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_PREFERRED)
		{
			if (wdata.pref_width != -1 && *w == -1)
				*w = wdata.pref_width;
			if (wdata.pref_height != -1 && *h == -1)
				*h = wdata.pref_height;
		}

		if (whints & EGUEB_DOM_FEATURE_WINDOW_HINT_MIN_MAX)
		{
			if (wdata.min_width != -1 && *w < wdata.min_width)
				*w = wdata.min_width;
			if (wdata.min_height != -1 && *h < wdata.min_height)
				*h = wdata.min_height;

			if (wdata.max_width != -1 && *w > wdata.max_width)
				*w = wdata.max_width;
			if (wdata.max_height != -1 && *h > wdata.max_height)
				*h = wdata.max_height;
		}
	}

	if (*w <= 0 || *h <= 0)
	{
		ERR("Invalid size of the window %d %d", *w, *h);
		efl_egueb_window_free(thiz);
		return NULL;
	}
	/* set the window size */
	egueb_dom_feature_window_size_set(thiz->window, *w, *h);

	DBG("Using size of %d %d", *w, *h);
	thiz->w = *w;
	thiz->h = *h;
	thiz->x = x;
	thiz->y = y;

	/* register our own idler */
	thiz->idle_enterer = ecore_idle_enterer_add(_efl_egueb_window_idle_enterer_cb, thiz);
	/* register the event handlers */
	thiz->handlers[0] = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
			_efl_egueb_window_key_down, thiz);
	thiz->handlers[1] = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
			_efl_egueb_window_key_up, thiz);
	thiz->handlers[2] = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN,
			_efl_egueb_window_mouse_button_down, thiz);
	thiz->handlers[3] = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP,
			_efl_egueb_window_mouse_button_up, thiz);
	thiz->handlers[4] = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE,
			_efl_egueb_window_mouse_move, thiz);
	thiz->handlers[5] = ecore_event_handler_add(ECORE_EVENT_MOUSE_WHEEL,
			_efl_egueb_window_mouse_wheel, thiz);
	thiz->handlers[6] = ecore_event_handler_add(ECORE_EVENT_MOUSE_IN,
			_efl_egueb_window_mouse_in, thiz);
	thiz->handlers[7] = ecore_event_handler_add(ECORE_EVENT_MOUSE_OUT,
			_efl_egueb_window_mouse_out, thiz);

	/* create the background renderer */
	thiz->background = enesim_renderer_background_new();
	enesim_renderer_background_color_set(thiz->background, 0xffffffff);

	return thiz;
}

void efl_egueb_window_free(Efl_Egueb_Window *thiz)
{
	if (!thiz) return;

	efl_egueb_document_cleanup(&thiz->edoc);
	if (thiz->b)
		enesim_buffer_unref(thiz->b);
	if (thiz->s)
		enesim_surface_unref(thiz->s);

	if (thiz->idle_enterer)
		ecore_idle_enterer_del(thiz->idle_enterer);

	_efl_egueb_window_cleanup(thiz);
	if (thiz->desc->free)
		thiz->desc->free(thiz->data);

	enesim_renderer_unref(thiz->background);
	free(thiz);
}
/*----------------------------------------------------------------------------*
 *                        DOM  Window descriptor interface                    *
 *----------------------------------------------------------------------------*/
void efl_egueb_window_destroy(void *data)
{
	Efl_Egueb_Window *thiz = data;
	efl_egueb_window_free(thiz);
}

void * efl_egueb_window_timeout_set(void *data,
		Egueb_Dom_Window_Descriptor_Timeout_Cb cb,
		int64_t delay, void *user_data)
{
	Efl_Egueb_Window *thiz = data;
	Efl_Egueb_Timeout *to;

	to = calloc(1, sizeof(Efl_Egueb_Timeout));
	to->cb = cb;
	to->user_data = user_data;
	to->timer = ecore_timer_add(delay, _efl_egueb_window_timout_cb, to);
	to->win = thiz->win;

	/* TODO Add it to the list of timers of the window */

	return to;
}

void efl_egueb_window_timeout_clear(void *data, void *timeout)
{
	Efl_Egueb_Timeout *to = data;

	ecore_timer_del(to->timer);
	free(to);
}

int efl_egueb_window_width_get(void *data)
{
	Efl_Egueb_Window *thiz = data;
	return thiz->w;
}

int efl_egueb_window_height_get(void *data)
{
	Efl_Egueb_Window *thiz = data;
	return thiz->h;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Window * efl_egueb_window_auto_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h)
{
	Egueb_Dom_Window *ret = NULL;

#if BUILD_ECORE_X
	ret = efl_egueb_window_x_new(doc, NULL, 0, x, y, w, h);
#endif
#if BUILD_ECORE_WIN32
	if (!ret)
		ret = efl_egueb_window_win32_new(doc, NULL, 0, x, y, w, h);
#endif
	return ret;
}

EAPI void efl_egueb_window_color_set(Egueb_Dom_Window *w,
		Enesim_Color color)
{
	Efl_Egueb_Window *thiz;

	thiz = egueb_dom_window_data_get(w);
	enesim_renderer_background_color_set(thiz->background, color);
}
