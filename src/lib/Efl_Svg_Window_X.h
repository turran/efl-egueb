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
#ifndef _EFL_SVG_WINDOW_X_H_
#define _EFL_SVG_WINDOW_X_H_

#include <Ecore_X.h>

EAPI Efl_Svg_Window * efl_svg_window_x11_new(const char *display,
		Ecore_X_Window parent, int x, int y, int w, int h);
EAPI Ecore_X_Window efl_svg_window_x11_window_get(Efl_Svg_Window *w);

#endif

