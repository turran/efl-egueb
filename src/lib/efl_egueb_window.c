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
static Eina_Bool _efl_egueb_window_idle_enterer_cb(void *data)
{
	Efl_Egueb_Window *thiz = data;

	/* PROCESS and be sure to draw */
	//egueb_dom_document_process(thiz->doc);
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		const Efl_Egueb_Window_Descriptor *d, void *data)
{
	Efl_Egueb_Window *thiz;
	Egueb_Dom_Feature *render;
	Egueb_Dom_Feature *window;

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
	thiz->d = d;
	thiz->data = data;
	thiz->doc = doc;
	thiz->render = render;
	thiz->window = window;

	/* register our own idler */
	thiz->idle_enterer = ecore_idle_enterer_add(_efl_egueb_window_idle_enterer_cb, thiz);

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
	ecore_idle_enterer_del(thiz->idle_enterer);
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
