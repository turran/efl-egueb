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
#ifndef _EFL_EGUEB_SMART_H_
#define _EFL_EGUEB_SMART_H_

EAPI Evas_Object * efl_egueb_smart_new(Evas *e);
EAPI Egueb_Dom_Node * efl_egueb_smart_document_get(Evas_Object *o);
EAPI void efl_egueb_smart_document_set(Evas_Object *o, Egueb_Dom_Node *doc);
EAPI void efl_egueb_smart_stream_set(Evas_Object *o, Enesim_Stream *s);
EAPI void efl_egueb_smart_debug_damage_set(Evas_Object *o, Eina_Bool debug);
EAPI void efl_egueb_smart_fps_set(Evas_Object *o, int fps);

/* The SVG extension */
EAPI Eina_Bool efl_egueb_smart_is_svg(Evas_Object *o);
EAPI Eina_Bool efl_egueb_smart_svg_setup(Evas_Object *o);
EAPI void efl_egueb_smart_svg_zoom_and_pan_enable(Evas_Object *o);
EAPI void efl_egueb_smart_svg_zoom_and_pan_disable(Evas_Object *o);

#endif
