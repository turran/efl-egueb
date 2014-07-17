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

#if BUILD_EGUEB_JS_SM
#include <Egueb_Js_Sm.h>
#endif

#if BUILD_GST_EGUEB
#include <Gst_Egueb.h>
#endif

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
typedef struct _Efl_Egueb_Document_Http_Request
{
	Ecore_Con_Url *conn;
	Eina_Binbuf *data;
	Egueb_Dom_Event *ev;
} Efl_Egueb_Document_Http_Request;

static Eina_Bool _efl_egueb_document_url_data_cb(void *data, int type EINA_UNUSED, void *event)
{
	Ecore_Con_Event_Url_Data *ev = event;
	Efl_Egueb_Document_Http_Request *d = data;

	if (ev->url_con != d->conn)
		return EINA_TRUE;
	if (ev->size > 0)
		eina_binbuf_append_length(d->data, ev->data, ev->size);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_document_url_completion_cb(void *data, int type EINA_UNUSED, void *event)
{
	Ecore_Con_Event_Url_Complete *ev = event;
	Efl_Egueb_Document_Http_Request *d = data;
	Enesim_Stream *s;

	if (ev->url_con != d->conn)
		return EINA_TRUE;

	s = enesim_stream_buffer_new(eina_binbuf_string_steal(d->data),
			eina_binbuf_length_get(d->data));
	egueb_dom_event_io_data_finish(d->ev, s);

	egueb_dom_event_unref(d->ev);
	eina_binbuf_free(d->data);
	ecore_con_url_free(d->conn);
	free(d);

	return EINA_TRUE;
}

static void _efl_egueb_document_io_data_load(Egueb_Dom_String *location,
		Egueb_Dom_Event *ev)
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
			egueb_dom_event_io_data_finish(ev, s);
		}
	}
	else if (!strncmp(filename, "http://", 7))
	{
		Efl_Egueb_Document_Http_Request *data;

		data = calloc(1, sizeof(Efl_Egueb_Document_Http_Request));
		data->conn = ecore_con_url_new(filename);
		data->data = eina_binbuf_new();
		data->ev = egueb_dom_event_ref(ev);

		ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE,
				_efl_egueb_document_url_completion_cb,
				data);
		ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA,
				_efl_egueb_document_url_data_cb,
				data);
		ecore_con_url_get(data->conn);
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
	char *s_location;
	char *filename;
	char *dir;
	int ret;

	node = egueb_dom_event_target_get(ev);
	doc = egueb_dom_node_owner_document_get(node);
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
	_efl_egueb_document_io_data_load(location, ev);
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
		_efl_egueb_document_io_data_load(uri.location, ev);
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

	egueb_dom_event_io_image_finish(ev, src);
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

static void _efl_egueb_document_script_scripter_cb(Egueb_Dom_Event *ev, void *data)
{
	Efl_Egueb_Document *thiz = data;
	Egueb_Dom_Scripter *scripter;
	Egueb_Dom_String *type;
	const char *stype;

	/* in case the application has set an scripter, just skip */
	scripter = egueb_dom_event_script_scripter_get(ev);
	if (scripter)
		return;

	/* check for our own scripters */
	type = egueb_dom_event_script_type_get(ev);
	stype = egueb_dom_string_string_get(type);

	DBG("Requesting a script for '%s'", stype);
	/* normalize the type */
	if (!strcmp(stype, "application/ecmascript") ||
			!strcmp(stype, "text/ecmascript") ||
			!strcmp(stype, "text/javascript"))
		stype = "application/ecmascript";

	scripter = eina_hash_find(thiz->scripters, stype);
	if (scripter)
	{
		egueb_dom_event_script_scripter_set(ev, scripter);
		goto done;
	}

#if BUILD_EGUEB_JS_SM
	if (!strcmp(stype, "application/ecmascript"))
	{
		DBG("Creating new scripter for '%s'", stype);
		scripter = egueb_js_sm_scripter_new();
	}
#endif
	if (scripter)
	{
		DBG("Registering scripter for '%s'", stype);
		eina_hash_add(thiz->scripters, stype, scripter);
		/* TODO add any global object we might need, like the window object? */
		egueb_dom_event_script_scripter_set(ev, scripter);
	}
done:
	egueb_dom_string_unref(type);
}

static void _efl_egueb_document_multimedia_video_cb(Egueb_Dom_Event *ev, void *data)
{
	Egueb_Dom_Video_Provider *vp = NULL;
	Egueb_Dom_Node *n;
	Enesim_Renderer *r;
	const Egueb_Dom_Video_Provider_Notifier *notifier = NULL;

	n = egueb_dom_event_target_get(ev);
	r = egueb_dom_event_multimedia_video_renderer_get(ev);
#if BUILD_GST_EGUEB
	vp = gst_egueb_video_provider_new(NULL, r, n);
#endif
	egueb_dom_event_multimedia_video_provider_set(ev, vp);
	egueb_dom_node_unref(n);
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
		ecore_con_init();
		ecore_con_url_init();
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

	/* check for the script feature */
	thiz->script = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_SCRIPT_NAME, NULL);
	if (thiz->script)
	{
		egueb_dom_node_event_listener_add(thiz->doc,
				EGUEB_DOM_EVENT_SCRIPT_SCRIPTER,
				_efl_egueb_document_script_scripter_cb, EINA_TRUE, thiz);
		thiz->scripters = eina_hash_string_superfast_new(EINA_FREE_CB(egueb_dom_scripter_free));
	}

	/* check for the multimedia feature */
	thiz->multimedia = egueb_dom_node_feature_get(doc, EGUEB_DOM_FEATURE_MULTIMEDIA_NAME, NULL);
	if (thiz->multimedia)
	{
		egueb_dom_node_event_listener_add(thiz->doc,
				EGUEB_DOM_EVENT_MULTIMEDIA_VIDEO,
				_efl_egueb_document_multimedia_video_cb, EINA_TRUE, thiz);
	}
}

void efl_egueb_document_cleanup(Efl_Egueb_Document *thiz)
{
	/* first remove every global object to release any
	 * reference that blocks the destruction of the document
	 * like a reference to the document itself
	 */
	if (thiz->script)
	{
		Eina_Iterator *it;
		Egueb_Dom_Scripter *scripter;

		it = eina_hash_iterator_data_new(thiz->scripters);
		EINA_ITERATOR_FOREACH(it, scripter)
		{
			egueb_dom_scripter_global_clear(scripter);
		}
		eina_iterator_free(it);
	}

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
		ecore_con_url_shutdown();
		ecore_con_shutdown();
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
	if (thiz->script)
	{
		eina_hash_free(thiz->scripters);
		egueb_dom_feature_unref(thiz->script);
	}
	if (thiz->multimedia)
		egueb_dom_feature_unref(thiz->multimedia);
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

