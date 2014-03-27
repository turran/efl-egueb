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

#include "efl_egueb_document_private.h"

#include <libgen.h>
#include <stdio.h>
/* Put the common interfaces with EFL here, like:
 * - Animation
 * - IO
 * - Render?
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _efl_egueb_document_io_data_load(Egueb_Dom_String *location,
		Egueb_Dom_Event_IO_Data_Cb cb, Egueb_Dom_Node *node)
{
	const char *filename;

	filename = egueb_dom_string_string_get(location);
	/* check the scheme */
	if (!strncmp(filename, "file://", 7))
	{
		Enesim_Stream *s;

		s = enesim_stream_file_new(filename + 7, "r");
		if (s)
		{
			DBG("Data '%s' loaded correctly", filename);
			cb(node, s);
		}
	}
	else
	{
		WRN("Unsupported schema '%s'", filename);
	}
}

static void _efl_egueb_document_io_relative_data_cb(Egueb_Dom_Uri *uri,
		Egueb_Dom_Event *ev)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *node;
	Egueb_Dom_String *location;
	Egueb_Dom_Event_IO_Data_Cb cb;
	char *s_location;
	char *filename;
	char *dir;
	int ret;

	node = egueb_dom_event_target_get(ev);
	doc = egueb_dom_node_document_get(node);
	if (!doc)
	{
		WRN("No document available");
		goto beach;
	}

	location = egueb_dom_document_uri_get(doc);
	egueb_dom_node_unref(doc);

	if (!location)
	{
		WRN("No URI set on the document");
		goto beach;
	}

	s_location =  strdup(egueb_dom_string_string_get(location));
	egueb_dom_string_unref(location);

	dir = dirname(s_location);
	ret = asprintf(&filename, "%s/%s", dir,
			egueb_dom_string_string_get(uri->location));

	free(s_location);
	if (ret < 0)
		goto beach;

	location = egueb_dom_string_steal(filename);
	cb = egueb_dom_event_io_data_cb_get(ev);
	_efl_egueb_document_io_data_load(location, cb, node);
	egueb_dom_string_unref(location);
beach:
	egueb_dom_node_unref(node);
}

/* TODO the idea is to use async file loading (either http://, file:// etc) */
static void _efl_egueb_document_io_data_cb(Egueb_Dom_Event *ev, void *data EINA_UNUSED)
{
	Egueb_Dom_Uri uri;

	egueb_dom_event_io_uri_get(ev, &uri);
	if (uri.fragment != NULL)
		goto has_fragment;

	DBG("Data requested '%s' (%d)", egueb_dom_string_string_get(
			uri.location), uri.type);
	if (uri.type == EGUEB_DOM_URI_TYPE_ABSOLUTE)
	{
		Egueb_Dom_Node *node;
		Egueb_Dom_Event_IO_Data_Cb cb;

		node = egueb_dom_event_target_get(ev);
		cb = egueb_dom_event_io_data_cb_get(ev);
		_efl_egueb_document_io_data_load(uri.location, cb, node);
	}
	else
	{
		_efl_egueb_document_io_relative_data_cb(&uri, ev);
	}

has_fragment:
	egueb_dom_uri_cleanup(&uri);
}

static void _efl_egueb_document_io_image_async_cb(Enesim_Buffer *b, void *data,
		int error)
{
	Egueb_Dom_Event *ev = data;
	Egueb_Dom_Node *n;
	Egueb_Dom_Event_IO_Image_Cb cb;
	Enesim_Surface *src = NULL;

	n = egueb_dom_event_target_get(ev);
	if (!b)
	{
		ERR("Can not load image, error: %s", eina_error_msg_get(error));
	}
	else
	{
		DBG("Image loaded correectly");
		src = enesim_surface_new_buffer_from(b);
	}

	cb = egueb_dom_event_io_image_cb_get(ev);
	cb(n, src);
	egueb_dom_node_unref(n);
}

static void _efl_egueb_document_io_image_cb(Egueb_Dom_Event *ev, void *data EINA_UNUSED)
{
	Enesim_Stream *s;

	s = egueb_dom_event_io_stream_get(ev);
	if (!s) return;

	enesim_image_load_async(s, NULL, NULL, NULL,
			_efl_egueb_document_io_image_async_cb,
			egueb_dom_event_ref(ev),
			NULL);
	enesim_stream_unref(s);
}

static Eina_Bool _efl_egueb_document_timer_cb(void *data)
{
	Efl_Egueb_Document *thiz = data;

	egueb_dom_feature_animation_tick(thiz->animation);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_document_idle_enterer_cb(void *data)
{
	Efl_Egueb_Document *thiz = data;

	/* process the enesim image context */
	if (thiz->io)
	{
		enesim_image_dispatch();
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void efl_egueb_document_setup(Efl_Egueb_Document *thiz, Egueb_Dom_Node *doc)
{
	thiz->doc = doc;
	/* check for IO feature */
	thiz->io = egueb_dom_node_feature_get(thiz->doc,
			EGUEB_DOM_FEATURE_IO_NAME, NULL);
	if (thiz->io)
	{
		egueb_dom_node_event_listener_add(thiz->doc,
				EGUEB_DOM_EVENT_IO_DATA,
				_efl_egueb_document_io_data_cb, EINA_TRUE, thiz);
		egueb_dom_node_event_listener_add(thiz->doc,
				EGUEB_DOM_EVENT_IO_IMAGE,
				_efl_egueb_document_io_image_cb, EINA_TRUE, thiz);
		thiz->idle_enterer = ecore_idle_enterer_add(
				_efl_egueb_document_idle_enterer_cb, thiz);
	}

	/* check for animation feature */
	thiz->animation = egueb_dom_node_feature_get(thiz->doc,
			EGUEB_DOM_FEATURE_ANIMATION_NAME, NULL);
	/* register our own timer for the anim in case we have one */
	if (thiz->animation)
	{
		int fps;

		egueb_dom_feature_animation_fps_get(thiz->animation, &fps);
		if (fps > 0)
			thiz->animator = ecore_timer_add(1.0/fps, _efl_egueb_document_timer_cb, thiz);
	}
}

void efl_egueb_document_cleanup(Efl_Egueb_Document *thiz)
{
	if (thiz->io)
	{
		egueb_dom_node_event_listener_remove(thiz->doc,
				EGUEB_DOM_EVENT_IO_DATA,
				_efl_egueb_document_io_data_cb,
				EINA_TRUE, thiz);
		egueb_dom_node_event_listener_remove(thiz->doc,
				EGUEB_DOM_EVENT_IO_IMAGE,
				_efl_egueb_document_io_image_cb,
				EINA_TRUE, thiz);
		egueb_dom_feature_unref(thiz->io);
	}
	if (thiz->doc)
	{
		egueb_dom_node_unref(thiz->doc);
		thiz->doc = NULL;
	}

	if (thiz->animator)
		ecore_timer_del(thiz->animator);
	if (thiz->animation)
		egueb_dom_feature_unref(thiz->animation);
}

void efl_egueb_document_fps_set(Efl_Egueb_Document *thiz, int fps)
{
	if (fps < 0) return;
	if (!thiz->animation) return;

	egueb_dom_feature_animation_fps_set(thiz->animation, fps);
	ecore_timer_interval_set(thiz->animator, 1.0/fps);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

