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
#ifndef _EFL_SVG_SMART_H_
#define _EFL_SVG_SMART_H_

EAPI Evas_Object * efl_svg_smart_new(Evas *e);
EAPI Egueb_Dom_Node * efl_svg_smart_document_get(Evas_Object *o);
EAPI void efl_svg_smart_file_set(Evas_Object *o, const char *file);
EAPI const char * efl_svg_smart_file_get(Evas_Object *o);
EAPI void efl_svg_smart_debug_damage_set(Evas_Object *o, Eina_Bool debug);
EAPI void efl_svg_smart_fps_set(Evas_Object *o, int fps);

#endif
