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
#ifndef _EFL_EGUEB_IO_REQUEST_PRIVATE_H_
#define _EFL_EGUEB_IO_REQUEST_PRIVATE_H_

typedef struct _Efl_Egueb_IO_Request Efl_Egueb_IO_Request;

typedef void (*Efl_Egueb_IO_Request_Completion_Cb)(Efl_Egueb_IO_Request *thiz, Enesim_Stream *s);
typedef void (*Efl_Egueb_IO_Request_Free_Cb)(Efl_Egueb_IO_Request *thiz);

typedef struct _Efl_Egueb_IO_Request_Descriptor
{
	Efl_Egueb_IO_Request_Completion_Cb completion;
	Efl_Egueb_IO_Request_Free_Cb free;
} Efl_Egueb_IO_Request_Descriptor;

struct _Efl_Egueb_IO_Request
{
	Ecore_Con_Url *conn;
	Eina_Binbuf *binbuf;
	Eina_Bool in_event;
	Eina_Bool destroy;
	const Efl_Egueb_IO_Request_Descriptor *descriptor;
	void *data;
};

Efl_Egueb_IO_Request * efl_egueb_io_request_new(Egueb_Dom_String *location,
		const Efl_Egueb_IO_Request_Descriptor *descriptor, void *data);
void efl_egueb_io_request_free(Efl_Egueb_IO_Request *thiz);

#endif
