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
#ifndef _EFL_EGUEB_PRIVATE_H_
#define _EFL_EGUEB_PRIVATE_H_

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define CRI(...) EINA_LOG_DOM_CRIT(efl_egueb_log, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(efl_egueb_log, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(efl_egueb_log, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(efl_egueb_log, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(efl_egueb_log, __VA_ARGS__)

extern int efl_egueb_log;

/* Include external headers before including our own efl_egueb_build.h */
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Con.h>
#include <Ecore_Input.h>

#include <Egueb_Dom.h>

#if BUILD_EGUEB_SCRIPT
#include <Egueb_Script.h>
#endif

#if BUILD_EGUEB_VIDEO
#include <Egueb_Video.h>
#endif

#if BUILD_EGUEB_SMIL
#include <Egueb_Smil.h>
#endif

#if BUILD_EGUEB_SVG
#include "Egueb_Svg.h"
#endif

#if BUILD_ECORE_X
#include <Ecore_X.h>
#endif

#if BUILD_ECORE_WIN32
#include <Ecore_Win32.h>
#endif

#include "efl_egueb_build.h"

#endif
