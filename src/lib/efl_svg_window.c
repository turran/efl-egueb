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
#include "efl_svg_private.h"
#include "efl_svg_main.h"
#include "efl_svg_window.h"

#include "efl_svg_window_private.h"

#if BUILD_ECORE_X
#include "Efl_Svg_Window_X.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Efl_Svg_Window * efl_svg_window_new(const Efl_Svg_Window_Descriptor *d, void *data)
{
	Efl_Svg_Window *thiz;

	thiz = calloc(1, sizeof(Efl_Svg_Window));
	thiz->d = d;
	thiz->data = data;

	return thiz;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Efl_Svg_Window * efl_svg_window_auto_new(int x, int y, int w, int h)
{
	Efl_Svg_Window *thiz;

#if BUILD_ECORE_X
	thiz = efl_svg_window_x11_new(NULL, 0, x, y, w, h);
#endif
	return thiz;
}
