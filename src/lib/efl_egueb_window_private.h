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
#ifndef _EFL_EGUEB_WINDOW_PRIVATE_H_
#define _EFL_EGUEB_WINDOW_PRIVATE_H_

#include "efl_egueb_document_private.h"
#include <Ecore_Input.h>

typedef void (*Efl_Egueb_Window_Descriptor_Free)(void *data);
typedef void (*Efl_Egueb_Window_Descriptor_Output_Update)(void *data, Eina_Rectangle *area);
typedef Ecore_Window (*Efl_Egueb_Window_Descriptor_Window_Get)(void *data);

typedef struct _Efl_Egueb_Window_Descriptor
{
	Efl_Egueb_Window_Descriptor_Window_Get window_get;
	Efl_Egueb_Window_Descriptor_Output_Update output_update;
	Efl_Egueb_Window_Descriptor_Free free;
} Efl_Egueb_Window_Descriptor;

struct _Efl_Egueb_Window
{
	Efl_Egueb_Document edoc;
	Egueb_Dom_Node *doc;
	Egueb_Dom_Feature *render;
	Egueb_Dom_Feature *window;
	Egueb_Dom_Input *input;
	int x;
	int y;
	int w;
	int h;
	/* data the backend should set */
	Enesim_Surface *s;
	Enesim_Buffer *b;
	/* private */
	Ecore_Idle_Enterer *idle_enterer;
	Eina_List *damages;
	/* the event handlers */
	Ecore_Event_Handler *handlers[8];

	void *data;
	const Efl_Egueb_Window_Descriptor *desc;
};

Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h,
		const Efl_Egueb_Window_Descriptor *d, void *data);

void efl_egueb_window_update_size(Efl_Egueb_Window *thiz, int w, int h);

#endif
