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

#include "efl_egueb_window_private.h"

#if BUILD_ECORE_X
#include "Efl_Egueb_Window_X.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static inline Eina_Bool _check_window(Efl_Egueb_Window *thiz, Ecore_Window w)
{
	Ecore_Window real;

	real = thiz->desc->window_get(thiz->data);
	if (real == w) return EINA_TRUE;
	else return EINA_FALSE;
}

/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _efl_egueb_window_key_down(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;

	printf("key down\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_key_up(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;

	printf("key up\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_button_down(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Button *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->ui) egueb_dom_feature_ui_feed_mouse_down(thiz->ui, ev->buttons);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_button_up(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Button *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->ui) egueb_dom_feature_ui_feed_mouse_up(thiz->ui, ev->buttons);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_move(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;
	Ecore_Event_Mouse_Move *ev = event;

	if (!_check_window(thiz, ev->window))
		return EINA_TRUE;

	if (thiz->ui) egueb_dom_feature_ui_feed_mouse_move(thiz->ui, ev->x, ev->y);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_wheel(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;

	printf("mouse wheel\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_in(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;

	printf("mouse in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_mouse_out(void *data,
		int type, void *event)
{
	Efl_Egueb_Window *thiz = data;

	printf("mouse out\n");
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

	thiz->damages = eina_list_append(thiz->damages, r);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_idle_enterer_cb(void *data)
{
	Efl_Egueb_Window *thiz = data;

	egueb_dom_document_process(thiz->doc);
	/* if need to draw, get the damages, and draw */
	if (thiz->b && thiz->s)
	{
		Eina_Rectangle *r;

		egueb_dom_feature_render_damages_get(thiz->render, thiz->s,
				_efl_egueb_window_damages, thiz);
		if (!thiz->damages) goto done;

		egueb_dom_feature_render_draw_list(thiz->render, thiz->s,
				ENESIM_ROP_FILL, thiz->damages, 0, 0, NULL);
		/* convert */
		enesim_converter_surface(thiz->s, thiz->b);
		if (!thiz->desc->output_update)
			goto done;
		EINA_LIST_FREE(thiz->damages, r)
		{
			thiz->desc->output_update(thiz->data, r);
			free(r);
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
	if (thiz->window)
	{
		egueb_dom_feature_window_content_size_set(thiz->window,
				thiz->w, thiz->h);
	}
}

Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h,
		const Efl_Egueb_Window_Descriptor *d, void *data)
{
	Efl_Egueb_Window *thiz;
	Egueb_Dom_Feature *render;
	Egueb_Dom_Feature *window;
	int cw, ch;

	if (!doc) return NULL;

	/* check if it is a doc */
	if (egueb_dom_node_type_get(doc) != EGUEB_DOM_NODE_TYPE_DOCUMENT_NODE)
	{
		egueb_dom_node_unref(doc);
		return NULL;
	}
	/* check it it has the render feature */
	render = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_RENDER_NAME, NULL);
	if (!render)
	{
		egueb_dom_node_unref(doc);
		return NULL;
	}

	window = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_WINDOW_NAME, NULL);
	if (!window)
	{
		egueb_dom_feature_unref(render);
		egueb_dom_node_unref(doc);
		return NULL;
	}

	thiz = calloc(1, sizeof(Efl_Egueb_Window));
	efl_egueb_document_setup(&thiz->edoc, egueb_dom_node_ref(doc));
	thiz->desc = d;
	thiz->data = data;
	thiz->doc = doc;
	thiz->render = render;
	thiz->window = window;

	/* set the content size */
	egueb_dom_feature_window_content_size_set(thiz->window, w, h);
	/* now process the document */
	egueb_dom_document_process(doc);
	egueb_dom_feature_window_content_size_get(thiz->window, &cw, &ch);
	if (cw <= 0 || ch <= 0)
	{
		ERR("Invalid size of the window %d %d", cw, ch);
		efl_egueb_window_free(thiz);
		return NULL;
	}

	/* sanitize the size */
	if (w <= 0)
		w = cw;

	if (h <= 0)
		h = ch;

	DBG("Using size of %d %d", w, h);
	thiz->w = w;
	thiz->h = h;
	thiz->x = x;
	thiz->y = y;

	/* optional features */
	thiz->ui = egueb_dom_node_feature_get(thiz->doc,
			EGUEB_DOM_FEATURE_UI_NAME, NULL);
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

	return thiz;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Efl_Egueb_Window * efl_egueb_window_auto_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h)
{
	Efl_Egueb_Window *thiz;

#if BUILD_ECORE_X
	thiz = efl_egueb_window_x_new(doc, NULL, 0, x, y, w, h);
#endif
	return thiz;
}

EAPI void efl_egueb_window_free(Efl_Egueb_Window *thiz)
{
	efl_egueb_document_cleanup(&thiz->edoc);
	if (thiz->b)
		enesim_buffer_unref(thiz->b);
	if (thiz->s)
		enesim_surface_unref(thiz->s);

	if (thiz->idle_enterer)
		ecore_idle_enterer_del(thiz->idle_enterer);

	if (thiz->ui)
		egueb_dom_feature_unref(thiz->ui);
	if (thiz->window)
		egueb_dom_feature_unref(thiz->window);
	if (thiz->render)
		egueb_dom_feature_unref(thiz->render);
	if (thiz->doc)
		egueb_dom_node_unref(thiz->doc);
	if (thiz->desc->free)
		thiz->desc->free(thiz->data);
	free(thiz);
}
