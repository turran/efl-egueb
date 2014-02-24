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
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
int efl_svg_log = -1;
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int efl_svg_init(void)
{
	if (++_init != 1)
		return _init;
	egueb_svg_init();
	efl_svg_log = eina_log_domain_register("efl_svg", EINA_COLOR_BLUE);
	return _init;
}

EAPI int efl_svg_shutdown(void)
{
	if (--_init != 0)
		return _init;
	eina_log_domain_unregister(efl_svg_log);
	egueb_svg_shutdown();
	return _init;
}
