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
#ifndef _EFL_EGUEB_WINDOW_H_
#define _EFL_EGUEB_WINDOW_H_

EAPI Egueb_Dom_Window * efl_egueb_window_auto_new(Egueb_Dom_Node *doc,
		int x, int y, int w, int h);
EAPI void efl_egueb_window_color_set(Egueb_Dom_Window *w,
		Enesim_Color color);

#endif /*_EFL_EGUEB_WINDOW_H*/
