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
#include "efl_svg_private.h"
#include "efl_svg_main.h"
#include "efl_egueb_window.h"

#include "efl_egueb_window_private.h"

#if BUILD_ECORE_X
#include "Efl_Egueb_Window_X.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
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

static Eina_Bool _efl_egueb_window_timer_cb(void *data)
{
	Efl_Egueb_Window *thiz = data;

	egueb_dom_feature_animation_tick(thiz->animation);
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
		if (!thiz->d->output_update)
			goto done;
		EINA_LIST_FREE(thiz->damages, r)
		{
			thiz->d->output_update(thiz->data, r);
			free(r);
		}
	}
done:
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h,
		const Efl_Egueb_Window_Descriptor *d, void *data)
{
	Efl_Egueb_Window *thiz;
	Egueb_Dom_Feature *render;
	Egueb_Dom_Feature *window;
	Egueb_Dom_Feature *animation;
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

	Egueb_Dom_Feature_Window_Type type;
	egueb_dom_feature_window_type_get(window, &type);

	/* set the content size */
	egueb_dom_feature_window_content_size_set(window, w, h);
	/* now process the document */
	egueb_dom_document_process(doc);
	egueb_dom_feature_window_content_size_get(window, &cw, &ch);
	if (cw <= 0 || ch <= 0)
	{
		printf("Invalid size of the window %d %d\n", cw, ch);
		egueb_dom_feature_unref(window);
		egueb_dom_feature_unref(render);
		egueb_dom_node_unref(doc);
		return NULL;
	}

	/* sanitize the size */
	if (w <= 0)
		w = cw;

	if (h <= 0)
		h = ch;

	printf("Using size of %d %d\n", w, h);
	animation = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_ANIMATION_NAME, NULL);

	thiz = calloc(1, sizeof(Efl_Egueb_Window));
	thiz->d = d;
	thiz->data = data;
	thiz->doc = doc;
	thiz->render = render;
	thiz->window = window;
	thiz->animation = animation;
	thiz->w = w;
	thiz->h = h;
	thiz->x = x;
	thiz->y = y;

	/* register our own idler */
	thiz->idle_enterer = ecore_idle_enterer_add(_efl_egueb_window_idle_enterer_cb, thiz);
	/* register our own timer for the anim in case we have one */
	if (thiz->animation)
	{
		int fps;

		egueb_dom_feature_animation_fps_get(thiz->animation, &fps);
		if (fps > 0)
			thiz->animator = ecore_timer_add(1.0/fps, _efl_egueb_window_timer_cb, thiz);
		//ecore_timer_interval_set(thiz->animator, 1.0/thiz->fps);
	}

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
	if (thiz->b)
		enesim_buffer_unref(thiz->b);
	if (thiz->s)
		enesim_surface_unref(thiz->s);

	ecore_idle_enterer_del(thiz->idle_enterer);
	if (thiz->animator)
		ecore_timer_del(thiz->animator);
	if (thiz->animation)
		egueb_dom_feature_unref(thiz->animation);
	if (thiz->window)
		egueb_dom_feature_unref(thiz->window);
	if (thiz->render)
		egueb_dom_feature_unref(thiz->render);
	if (thiz->doc)
		egueb_dom_node_unref(thiz->doc);
	if (thiz->d->free)
		thiz->d->free(thiz->data);
	free(thiz);
}
