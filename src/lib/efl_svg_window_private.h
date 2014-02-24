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
#ifndef _EFL_SVG_WINDOW_PRIVATE_H_
#define _EFL_SVG_WINDOW_PRIVATE_H_

typedef struct _Efl_Svg_Window_Descriptor
{
	
} Efl_Svg_Window_Descriptor;


struct _Efl_Svg_Window
{
	void *data;
	const Efl_Svg_Window_Descriptor *d;
};

Efl_Svg_Window * efl_svg_window_new(const Efl_Svg_Window_Descriptor *d, void *data);

#endif
