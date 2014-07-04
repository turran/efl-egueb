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
#ifndef _EFL_EGUEB_DOCUMENT_PRIVATE_H_
#define _EFL_EGUEB_DOCUMENT_PRIVATE_H_

typedef struct _Efl_Egueb_Document
{
	Egueb_Dom_Node *doc;
	Egueb_Dom_Feature *io;
	Egueb_Dom_Feature *animation;
	Egueb_Dom_Feature *script;

	Ecore_Timer *animator;
	Ecore_Idle_Enterer *idle_enterer;
	Eina_Hash *scripters;
} Efl_Egueb_Document;

void efl_egueb_document_setup(Efl_Egueb_Document *thiz, Egueb_Dom_Node *doc);
void efl_egueb_document_cleanup(Efl_Egueb_Document *thiz);
void efl_egueb_document_fps_set(Efl_Egueb_Document *thiz, int fps);

#endif
