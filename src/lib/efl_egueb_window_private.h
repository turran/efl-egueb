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

typedef void (*Efl_Egueb_Window_Descriptor_Free)(void *data);

typedef struct _Efl_Egueb_Window_Descriptor
{
	Efl_Egueb_Window_Descriptor_Free free;
} Efl_Egueb_Window_Descriptor;


struct _Efl_Egueb_Window
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Feature *render;
	Ecore_Idle_Enterer *idle_enterer;
	void *data;
	const Efl_Egueb_Window_Descriptor *d;
};

Efl_Egueb_Window * efl_egueb_window_new(Egueb_Dom_Node *doc,
		const Efl_Egueb_Window_Descriptor *d, void *data);

#endif
