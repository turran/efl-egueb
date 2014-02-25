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
#ifndef _EFL_EGUEB_WINDOW_X_H_
#define _EFL_EGUEB_WINDOW_X_H_

#include <Ecore_X.h>

EAPI Efl_Egueb_Window * efl_egueb_window_x_new(Egueb_Dom_Node *doc,
		const char *display, Ecore_X_Window parent, int x, int y,
		int w, int h);
EAPI Ecore_X_Window efl_egueb_window_x_window_get(Efl_Egueb_Window *w);

#endif

