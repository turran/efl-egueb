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

#include "efl_egueb_io_request_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static Eina_Bool _efl_egueb_io_request_url_data_cb(void *data, int type EINA_UNUSED, void *event)
{
	Efl_Egueb_IO_Request *thiz = data;
	Ecore_Con_Event_Url_Data *ev = event;

	if (ev->url_con != thiz->conn)
		return EINA_TRUE;
	if (ev->size > 0)
		eina_binbuf_append_length(thiz->binbuf, ev->data, ev->size);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_io_request_url_completion_cb(void *data, int type EINA_UNUSED, void *event)
{
	Efl_Egueb_IO_Request *thiz = data;
	Ecore_Con_Event_Url_Complete *ev = event;
	Enesim_Stream *s;

	if (ev->url_con != thiz->conn)
		return EINA_TRUE;

	/* TODO can we avoid this dup? */
	s = enesim_stream_buffer_new(eina_binbuf_string_steal(thiz->binbuf),
			eina_binbuf_length_get(thiz->binbuf), free);
	thiz->in_event = EINA_TRUE;
	if (thiz->descriptor->completion)
		thiz->descriptor->completion(thiz, s);
	thiz->in_event = EINA_FALSE;
	enesim_stream_unref(s);

	if (thiz->destroy)
		efl_egueb_io_request_free(thiz);

	return EINA_TRUE;
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Efl_Egueb_IO_Request * efl_egueb_io_request_new(Egueb_Dom_String *location,
		const Efl_Egueb_IO_Request_Descriptor *descriptor, void *data)
{
	Efl_Egueb_IO_Request *thiz;
	const char *filename;


	if (!location) return NULL;
	filename = egueb_dom_string_chars_get(location);

	thiz = calloc(1, sizeof(Efl_Egueb_IO_Request));
	thiz->descriptor = descriptor;
	thiz->data = data;

	if (!strncmp(filename, "file://", 7))
	{
		Enesim_Stream *s;

		s = enesim_stream_file_new(filename + 7, "r");
		if (s)
		{
			DBG("Data '%s' loaded correctly", filename);
			thiz->in_event = EINA_TRUE;
			if (thiz->descriptor->completion)
				thiz->descriptor->completion(thiz, s);
			enesim_stream_unref(s);
			thiz->in_event = EINA_FALSE;
		}
	}
	else if (!strncmp(filename, "http://", 7))
	{
		thiz->conn = ecore_con_url_new(filename);
		thiz->binbuf = eina_binbuf_new();

		ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE,
				_efl_egueb_io_request_url_completion_cb,
				thiz);
		ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA,
				_efl_egueb_io_request_url_data_cb,
				thiz);
		ecore_con_url_get(thiz->conn);
	}
	else
	{
		WRN("Unsupported schema '%s'", filename);
		free(thiz);
		thiz = NULL;
		goto done;
	}

	if (thiz->destroy)
	{
		efl_egueb_io_request_free(thiz);
		return NULL;		
	}
done:
	return thiz;
}

void efl_egueb_io_request_free(Efl_Egueb_IO_Request *thiz)
{
	/* just mark it for later */
	if (thiz->in_event)
	{
		thiz->destroy = EINA_TRUE;
		thiz->in_event = EINA_FALSE;
		return;
	}

	if (thiz->data)
	{
		if (thiz->descriptor->free)
			thiz->descriptor->free(thiz);
		thiz->data = NULL;
	}

	if (thiz->binbuf)
	{
		eina_binbuf_free(thiz->data);
		thiz->data = NULL;
	}
	if (thiz->conn)
	{
		ecore_con_url_free(thiz->conn);
		thiz->conn = NULL;
	}
	free(thiz);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

