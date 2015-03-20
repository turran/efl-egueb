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
#include "efl_egueb_smart.h"

#include "efl_egueb_document_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Egueb_Smart_Svg
{
	/* properties */
	Eina_Bool zoom_and_pan;
	/* private */
	Evas_Object *obj;
#if BUILD_EGUEB_SVG
	Eina_Bool l_down;
	Evas_Coord l_down_x, l_down_y;
	Egueb_Svg_Point current_translate;

	Eina_Bool r_down;
	Evas_Coord r_down_x, r_down_y;
	double current_scale;

	Evas_Object *focus_rect;
#endif
} Efl_Egueb_Smart_Svg;

#if BUILD_EGUEB_SVG
static void _efl_egueb_smart_svg_focus_in_cb(Egueb_Dom_Event *ev, void *data)
{
#if 0
	Efl_Egueb_Smart_Svg *thiz = data;
	Egueb_Dom_Node *target;

	/* add the rectangle object */
	target = egueb_dom_event_target_get(ev);
	egueb_dom_node_unref(target);

	/* FIXME the focus code is for testing, it should not place an object
	 * on top of every other object, it must be part of the smart object
	 * only
	 */
	if (!thiz->focus_rect)
	{
		Evas *evas;
		Evas_Object *o;

		evas = evas_object_evas_get(thiz->obj);
		o = evas_object_rectangle_add(evas);
		evas_object_color_set(o, 64, 0, 0, 64);
		evas_object_move(o, 0, 0);
		evas_object_resize(o, 128, 128);
		thiz->focus_rect = o;
	}
	/* get the position and size of the object */
	evas_object_show(thiz->focus_rect);
#endif
}

static void _efl_egueb_smart_svg_focus_out_cb(Egueb_Dom_Event *ev, void *data)
{
#if 0
	Efl_Egueb_Smart_Svg *thiz = data;
	/* remove the rectangle object */
	evas_object_hide(thiz->focus_rect);
#endif
} 

static void _efl_egueb_smart_svg_setup(Efl_Egueb_Smart_Svg *thiz)
{
	Egueb_Dom_Node *doc;

	/* register the document related events */
	doc = efl_egueb_smart_document_get(thiz->obj);
	egueb_dom_node_event_listener_add(doc,
			EGUEB_DOM_EVENT_FOCUS_IN,
			_efl_egueb_smart_svg_focus_in_cb, EINA_FALSE, thiz);
	egueb_dom_node_event_listener_add(doc,
			EGUEB_DOM_EVENT_FOCUS_OUT,
			_efl_egueb_smart_svg_focus_out_cb, EINA_FALSE, thiz);
	egueb_dom_node_unref(doc);
}

static void _efl_egueb_smart_svg_cleanup(Efl_Egueb_Smart_Svg *thiz)
{
	Egueb_Dom_Node *doc;

	/* unregister the document related events */
	doc = efl_egueb_smart_document_get(thiz->obj);
	egueb_dom_node_event_listener_remove(doc,
			EGUEB_DOM_EVENT_FOCUS_IN,
			_efl_egueb_smart_svg_focus_in_cb, EINA_FALSE, thiz);
	egueb_dom_node_event_listener_remove(doc,
			EGUEB_DOM_EVENT_FOCUS_OUT,
			_efl_egueb_smart_svg_focus_out_cb, EINA_FALSE, thiz);
	egueb_dom_node_unref(doc);
}

static void _efl_egueb_smart_svg_mouse_up(void *data, Evas *e EINA_UNUSED,
		Evas_Object *obj EINA_UNUSED, void *event_info)
{
	Efl_Egueb_Smart_Svg *thiz = data;
 	Evas_Event_Mouse_Up *ev = event_info;

	/* left click */
	if (ev->button == 1)
	{
		thiz->l_down = EINA_FALSE;
	}
	/* right click */
	else if (ev->button == 3)
	{
		thiz->r_down = EINA_FALSE;
	}
}

static void _efl_egueb_smart_svg_mouse_down(void *data, Evas *e EINA_UNUSED,
		Evas_Object *obj, void *event_info)
{
	Efl_Egueb_Smart_Svg *thiz = data;
 	Evas_Event_Mouse_Down *ev = event_info;

	/* left click */
	if (ev->button == 1)
	{
		Egueb_Dom_Node *doc;
		Egueb_Dom_Node *svg;
	
		doc = efl_egueb_smart_document_get(obj);
		svg = egueb_dom_document_document_element_get(doc);

		thiz->l_down = EINA_TRUE;
		thiz->l_down_x = ev->canvas.x;
		thiz->l_down_y = ev->canvas.y;
		egueb_svg_element_svg_current_translate_get(svg, &thiz->current_translate);
		egueb_dom_node_unref(svg);
		egueb_dom_node_unref(doc);
	}
	/* right click */
	else if (ev->button == 3)
	{
		Egueb_Dom_Node *doc;
		Egueb_Dom_Node *svg;

		doc = efl_egueb_smart_document_get(obj);
		svg = egueb_dom_document_document_element_get(doc);

		thiz->r_down = EINA_TRUE;
		thiz->r_down_x = ev->canvas.x;
		thiz->r_down_y = ev->canvas.y;
		thiz->current_scale = egueb_svg_element_svg_current_scale_get(svg);
		egueb_dom_node_unref(svg);
		egueb_dom_node_unref(doc);
	}
}

static void _efl_egueb_smart_svg_mouse_move(void *data, Evas *e EINA_UNUSED,
		Evas_Object *obj, void *event_info)
{
	Efl_Egueb_Smart_Svg *thiz = data;
	Evas_Event_Mouse_Move *ev = event_info;
	Egueb_Dom_Node *svg;
	Egueb_Dom_Node *doc;

	/* check if we are dragging */
	doc = efl_egueb_smart_document_get(obj);
	svg = egueb_dom_document_document_element_get(doc);
	if (thiz->l_down)
	{
		Egueb_Svg_Point p;

		p.x = (ev->cur.canvas.x - thiz->l_down_x);
		p.y = (ev->cur.canvas.y - thiz->l_down_y);

		p.x = thiz->current_translate.x + p.x;
		p.y = thiz->current_translate.y + p.y;
		egueb_svg_element_svg_current_translate_set(svg, &p);
	}
	if (thiz->r_down)
	{
		double scale;

		/* simple distance */
		scale = ((ev->cur.canvas.x - thiz->r_down_x) + (ev->cur.canvas.y - thiz->r_down_y)) / 2;
		scale = thiz->current_scale + (scale * 0.01);
		if (scale < 0)
			scale = 0;
		egueb_svg_element_svg_current_scale_set(svg, scale);
	}
	egueb_dom_node_unref(svg);
	egueb_dom_node_unref(doc);
}
#endif

static void _efl_egueb_smart_svg_del(void *data, Evas *e EINA_UNUSED,
		Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Efl_Egueb_Smart_Svg *thiz = data;
	_efl_egueb_smart_svg_cleanup(thiz);
	free(thiz);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eina_Bool efl_egueb_smart_is_svg(Evas_Object *o)
{
#if BUILD_EGUEB_SVG
	Egueb_Dom_Node *doc;
	Eina_Bool ret;

	doc = efl_egueb_smart_document_get(o);
	if (!doc) return EINA_FALSE;
	ret = egueb_svg_is_document(doc);
	egueb_dom_node_unref(doc);
	return ret;
#else
	return EINA_FALSE;
#endif
}

EAPI Eina_Bool efl_egueb_smart_svg_setup(Evas_Object *o)
{
	Efl_Egueb_Smart_Svg *thiz;

	if (!efl_egueb_smart_is_svg(o))
		return EINA_FALSE;
	/* add the callback whenever the smart object is deleted */
	thiz = calloc(1, sizeof(Efl_Egueb_Smart_Svg));
	thiz->obj = o;

	evas_object_data_set(o, "svg", thiz);
	evas_object_event_callback_add(o, EVAS_CALLBACK_DEL,
			_efl_egueb_smart_svg_del, thiz);
	_efl_egueb_smart_svg_setup(thiz);

	/* enable the zoom and pan by default */
	efl_egueb_smart_svg_zoom_and_pan_enable(o);
	return EINA_TRUE;
}

EAPI void efl_egueb_smart_svg_zoom_and_pan_enable(Evas_Object *o)
{
	Efl_Egueb_Smart_Svg *thiz;

	thiz = evas_object_data_get(o, "svg");
	if (!thiz)
	{
		ERR("Call efl_egueb_smart_svg_setup() first");
		return;
	}
	if (thiz->zoom_and_pan)
		return;

	thiz->zoom_and_pan = EINA_TRUE;
	/* TODO add the callback whenever the document has changed */
#if BUILD_EGUEB_SVG
	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
			_efl_egueb_smart_svg_mouse_down, thiz);
	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,
			_efl_egueb_smart_svg_mouse_up, thiz);
	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
			_efl_egueb_smart_svg_mouse_move, thiz);
#endif
}

EAPI void efl_egueb_smart_svg_zoom_and_pan_disable(Evas_Object *o)
{
	Efl_Egueb_Smart_Svg *thiz;

	thiz = evas_object_data_get(o, "svg");
	if (!thiz)
	{
		ERR("Call efl_egueb_smart_svg_setup() first");
		return;
	}
	if (!thiz->zoom_and_pan)
		return;

	thiz->zoom_and_pan = EINA_FALSE;
	/* TODO add the callback whenever the document has changed */
#if BUILD_EGUEB_SVG
	evas_object_event_callback_del_full(o, EVAS_CALLBACK_MOUSE_DOWN,
			_efl_egueb_smart_svg_mouse_down, thiz);
	evas_object_event_callback_del_full(o, EVAS_CALLBACK_MOUSE_UP,
			_efl_egueb_smart_svg_mouse_up, thiz);
	evas_object_event_callback_del_full(o, EVAS_CALLBACK_MOUSE_MOVE,
			_efl_egueb_smart_svg_mouse_move, thiz);
#endif
}
