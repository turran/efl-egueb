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
#include "efl_egueb_io_request_private.h"

#if BUILD_EGUEB_JS_SM
#include <Egueb_Js_Sm.h>
#endif

#if BUILD_GST_EGUEB
#include <Gst_Egueb.h>
#endif

#if BUILD_EGUEB_SMIL
#include <Egueb_Smil.h>
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
typedef struct _Efl_Egueb_Document_IO_Data_Request
{
	Efl_Egueb_Document *thiz;
	Egueb_Dom_Event *ev;
} Efl_Egueb_Document_IO_Data_Request;
/*----------------------------------------------------------------------------*
 *                            Data event request                              *
 *----------------------------------------------------------------------------*/
static void _efl_egueb_document_io_data_completion_cb(Efl_Egueb_IO_Request *r,
		Enesim_Stream *s)
{
	Efl_Egueb_Document_IO_Data_Request *data = r->data;

	/* finish the event */
	egueb_dom_event_io_data_finish(data->ev, enesim_stream_ref(s));
	/* destroy the request */
	efl_egueb_io_request_free(r);
}

static void _efl_egueb_document_io_data_free_cb(Efl_Egueb_IO_Request *r)
{
	Efl_Egueb_Document_IO_Data_Request *data = r->data;

	/* remove the request */
	data->thiz->requests = eina_list_remove(data->thiz->requests, r);
	egueb_dom_event_unref(data->ev);
	free(data);
}

static Efl_Egueb_IO_Request_Descriptor _efl_egueb_document_io_data_descriptor = {
	/* .completion_cb 	= */ _efl_egueb_document_io_data_completion_cb,
	/* .free_cb 		= */ _efl_egueb_document_io_data_free_cb
};

/*----------------------------------------------------------------------------*
 *                         Document go to request                             *
 *----------------------------------------------------------------------------*/
static void _efl_egueb_document_go_to_completion_cb(Efl_Egueb_IO_Request *r,
		Enesim_Stream *s)
{
	Efl_Egueb_Document *thiz = r->data;
	Egueb_Dom_Node *doc = NULL;

	egueb_dom_parser_parse(enesim_stream_ref(s), &doc);
	if (doc)
	{
		INF("Swapping documents");
		efl_egueb_document_cleanup(thiz);
		efl_egueb_document_setup(thiz, doc);
	}
	/* swap the current doc with the new doc */
	efl_egueb_io_request_free(r);
}

static void _efl_egueb_document_go_to_free_cb(Efl_Egueb_IO_Request *r)
{
	Efl_Egueb_Document *thiz = r->data;
	thiz->requests = eina_list_remove(thiz->requests, r);
}

static Efl_Egueb_IO_Request_Descriptor _efl_egueb_document_go_to_descriptor = {
	/* .completion_cb 	= */ _efl_egueb_document_go_to_completion_cb,
	/* .free_cb 		= */ _efl_egueb_document_go_to_free_cb
};

static void _efl_egueb_document_io_data_load(Efl_Egueb_Document *thiz,
		Egueb_Dom_String *location,
		Egueb_Dom_Event *ev)
{
	Efl_Egueb_Document_IO_Data_Request *data;
	Efl_Egueb_IO_Request *request;

	data = calloc(1, sizeof(Efl_Egueb_Document_IO_Data_Request));
	data->thiz = thiz;
	data->ev = egueb_dom_event_ref(ev);

	request = efl_egueb_io_request_new(location,
			&_efl_egueb_document_io_data_descriptor,
			data);
	if (request)
	{
		thiz->requests = eina_list_append(thiz->requests, request);
	}
}

static void _efl_egueb_document_io_relative_data_cb(
		Efl_Egueb_Document *thiz,
		Egueb_Dom_Uri *uri,
		Egueb_Dom_Event *ev)
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Node *node;
	Egueb_Dom_String *location;
	Egueb_Dom_Uri resolved;

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

	if (!egueb_dom_uri_resolve(uri, location, &resolved))
	{
		WRN("Can not resolve the URI");
		egueb_dom_string_unref(location);
		goto beach;
	}

	_efl_egueb_document_io_data_load(thiz, resolved.location, ev);
	egueb_dom_uri_cleanup(&resolved);
	egueb_dom_string_unref(location);
beach:
	egueb_dom_node_unref(node);
}

/* the idea is to use async file loading (either http://, file:// etc) */
static void _efl_egueb_document_io_data_cb(Egueb_Dom_Event *ev, void *data)
{
	Efl_Egueb_Document *thiz = data;
	Egueb_Dom_Uri uri;

	egueb_dom_event_io_uri_get(ev, &uri);
	if (uri.fragment != NULL)
		goto has_fragment;

	DBG("Data requested '%s' (%d)", egueb_dom_string_string_get(
			uri.location), uri.type);
	if (uri.type == EGUEB_DOM_URI_TYPE_ABSOLUTE)
	{
		_efl_egueb_document_io_data_load(thiz, uri.location, ev);
	}
	else
	{
		_efl_egueb_document_io_relative_data_cb(thiz, &uri, ev);
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
		DBG("Image loaded correctly");
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

static void _efl_egueb_document_navigation_load(Efl_Egueb_Document *thiz,
		Egueb_Dom_String *location)
{
	Efl_Egueb_IO_Request *request;

	request = efl_egueb_io_request_new(location,
			&_efl_egueb_document_go_to_descriptor,
			thiz);
	if (request)
	{
		thiz->requests = eina_list_append(thiz->requests, request);
	}
}

static void _efl_egueb_document_navigation_relative_load(Efl_Egueb_Document *thiz,
		Egueb_Dom_Uri *uri)
{
	Egueb_Dom_String *location;
	Egueb_Dom_Uri resolved;

	location = egueb_dom_document_uri_get(thiz->doc);
	if (!location)
	{
		WRN("No URI set on the document");
		return;
	}

	if (!egueb_dom_uri_resolve(uri, location, &resolved))
	{
		WRN("Can not resolve the URI");
		egueb_dom_string_unref(location);
		goto no_resolve;
	}

	DBG("Resolving relative uri for document '%s' at '%s'",
			egueb_dom_string_string_get(location),
			egueb_dom_string_string_get(resolved.location));
	_efl_egueb_document_navigation_load(thiz, resolved.location);
	egueb_dom_uri_cleanup(&resolved);
no_resolve:
	egueb_dom_string_unref(location);
}

static void _efl_egueb_document_navigation_go_to_cb(Egueb_Dom_Event *ev, void *data)
{
	Efl_Egueb_Document *thiz = data;
	Egueb_Dom_Uri uri;

	egueb_dom_event_navigation_uri_get(ev, &uri);
	/* TODO in case we dont have any uri, but only fragment ? */
	if (uri.fragment != NULL)
		goto has_fragment;

	DBG("Going to '%s' (%d)", egueb_dom_string_string_get(
			uri.location), uri.type);
	if (uri.type == EGUEB_DOM_URI_TYPE_ABSOLUTE)
	{
		_efl_egueb_document_navigation_load(thiz, uri.location);
	}
	else
	{
		_efl_egueb_document_navigation_relative_load(thiz, &uri);
	}
has_fragment:
	egueb_dom_uri_cleanup(&uri);
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

#if BUILD_EGUEB_SMIL
static Eina_Bool _efl_egueb_document_timer_cb(void *data)
{
	Efl_Egueb_Document *thiz = data;

	egueb_smil_feature_animation_tick(thiz->animation);
	return EINA_TRUE;
}
#endif

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
/*----------------------------------------------------------------------------*
 *                             Window descriptor                              *
 *----------------------------------------------------------------------------*/
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

	/* check the navigation feature */
	thiz->navigation = egueb_dom_node_feature_get(thiz->doc,
			EGUEB_DOM_FEATURE_NAVIGATION_NAME, NULL);
	if (thiz->navigation)
	{
		egueb_dom_node_event_listener_add(thiz->doc,
				EGUEB_DOM_EVENT_NAVIGATION_GO_TO,
				_efl_egueb_document_navigation_go_to_cb,
				EINA_TRUE, thiz);
	}

#if BUILD_EGUEB_SMIL
	/* check for animation feature */
	thiz->animation = egueb_dom_node_feature_get(thiz->doc,
			EGUEB_SMIL_FEATURE_ANIMATION_NAME, NULL);
	/* register our own timer for the anim in case we have one */
	if (thiz->animation)
	{
		int fps;

		egueb_smil_feature_animation_fps_get(thiz->animation, &fps);
		if (fps > 0)
			thiz->animator = ecore_timer_add(1.0/fps, _efl_egueb_document_timer_cb, thiz);
	}
#endif
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
	Efl_Egueb_IO_Request *request;

	/* remove every pending request */
	EINA_LIST_FREE(thiz->requests, request)
		efl_egueb_io_request_free(request);
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

	if (thiz->navigation)
	{
		egueb_dom_node_event_listener_remove(thiz->doc,
				EGUEB_DOM_EVENT_NAVIGATION_GO_TO,
				_efl_egueb_document_navigation_go_to_cb,
				EINA_TRUE, thiz);
	}

	if (thiz->doc)
	{
		egueb_dom_node_unref(thiz->doc);
		thiz->doc = NULL;
	}

#if BUILD_EGUEB_SMIL
	if (thiz->animator)
		ecore_timer_del(thiz->animator);
	if (thiz->animation)
		egueb_dom_feature_unref(thiz->animation);
#endif
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
#if BUILD_EGUEB_SMIL
	if (fps < 0) return;
	if (!thiz->animation) return;

	egueb_smil_feature_animation_fps_set(thiz->animation, fps);
	ecore_timer_interval_set(thiz->animator, 1.0/fps);
#endif
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

