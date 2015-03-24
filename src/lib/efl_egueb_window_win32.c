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
#include "efl_egueb_private.h"

#if BUILD_ECORE_WIN32
#include "efl_egueb_main.h"
#include "efl_egueb_window.h"
#include "efl_egueb_window_win32.h"

#include "efl_egueb_window_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Egueb_Window_Win32
{
	Efl_Egueb_Window *base;
	Ecore_Win32_Window *win;
	HWND win_hwnd;
	HBITMAP bitmap;
	unsigned char *bitmap_buffer;
	int bitmap_size;
	/* the event handlers */
	Ecore_Event_Handler *handlers[10];
} Efl_Egueb_Window_Win32;

static void _update_area(Efl_Egueb_Window_Win32 *thiz, Eina_Rectangle *area)
{
	HDC win_hdc, mem_hdc;
	SetBitmapBits(thiz->bitmap, thiz->bitmap_size, thiz->bitmap_buffer);

	win_hdc = GetDC(thiz->win_hwnd);
	mem_hdc = CreateCompatibleDC(win_hdc);
	SelectObject (mem_hdc, thiz->bitmap);
	BitBlt(win_hdc, area->x, area->y, area->w, area->h, mem_hdc, area->x, area->y, SRCCOPY);
	DeleteDC (mem_hdc);
}

static Eina_Bool _efl_egueb_window_win32_buffer_update(
		Efl_Egueb_Window_Win32 *thiz, int w, int h)
{
	Efl_Egueb_Window *base = thiz->base;
	Enesim_Buffer_Sw_Data sdata;
	BITMAP bmp;
	HDC win_hdc;

	/* destroy the previous buffer */
	if (base->s)
	{
		enesim_surface_unref(base->s);
		base->s = NULL;
	}
	if (base->b)
	{
		enesim_buffer_unref(base->b);
		base->b = NULL;
	}
	if (thiz->bitmap)
	{
		DeleteObject(thiz->bitmap);
		thiz->bitmap = NULL;
		free(thiz->bitmap_buffer);
		thiz->bitmap_buffer = NULL;
	}

	/* create the win32 BITMAP */
	win_hdc = GetDC(thiz->win_hwnd);
	thiz->bitmap = CreateCompatibleBitmap(win_hdc, w, h);
	if (!thiz->bitmap)
		return EINA_FALSE;
	if (!GetObject (thiz->bitmap, sizeof (bmp), &bmp))
		return EINA_FALSE;
	thiz->bitmap_size = bmp.bmWidthBytes * h;
	thiz->bitmap_buffer = malloc(thiz->bitmap_size);
	ReleaseDC(thiz->win_hwnd, win_hdc);

	switch (bmp.bmBitsPixel)
	{
		case 32:
		sdata.xrgb8888.plane0_stride = bmp.bmWidthBytes;
		sdata.xrgb8888.plane0 = (uint32_t *)thiz->bitmap_buffer;
		break;

		default:
		/* FIXME Implement other cases */
		break;
	}

	/* create the buffer */
	base->b = enesim_buffer_new_data_from(ENESIM_FORMAT_ARGB8888, w, h, EINA_FALSE, &sdata, NULL, NULL);
	if (!base->b)
	{
		free(thiz->bitmap_buffer);
		return EINA_FALSE;
	}
	base->s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, w, h);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_buffer_setup(Efl_Egueb_Window_Win32 *thiz)
{
	Efl_Egueb_Window *base = thiz->base;

	_efl_egueb_window_win32_buffer_update(thiz, base->w, base->h);

	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _efl_egueb_window_win32_event_mouse_in(void *data,
		int type, void *event)
{
	//printf("mouse in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_mouse_out(void *data,
		int type, void *event)
{
	//printf("mouse out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_focus_in(void *data,
		int type, void *event)
{
	//printf("focus in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_focus_out(void *data,
		int type, void *event)
{
	//printf("focus out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_damage(void *data,
		int type, void *event)
{
	Efl_Egueb_Window_Win32 *thiz = data;
	Ecore_Win32_Event_Window_Damage *ev = event;
	Eina_Rectangle area;

	if (thiz->win != ev->window) return EINA_TRUE;
	eina_rectangle_coords_from(&area, ev->x, ev->y, ev->width, ev->height);
	_update_area(thiz, &area);

	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_destroy(void *data,
		int type, void *event)
{
	//printf("destroy\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_configure(void *data,
		int type, void *event)
{
	Efl_Egueb_Window_Win32 *thiz = data;
	Efl_Egueb_Window *base = thiz->base;
	Ecore_Win32_Event_Window_Configure *ev = event;

	if (thiz->win != ev->window) return EINA_TRUE;
	
	//printf("configure wm: %d %d\n", ev->from_wm, ev->override);
	/* check the size */
	if (ev->width != base->w || ev->height != base->h)
	{
		_efl_egueb_window_win32_buffer_update(thiz, ev->width, ev->height);
		efl_egueb_window_update_size(base, ev->width, ev->height);
	}
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_delete_request(void *data,
		int type, void *event)
{
	//printf("delete request\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_show(void *data,
		int type, void *event)
{
	Efl_Egueb_Window_Win32 *thiz = data;
	Efl_Egueb_Window *base = thiz->base;
	Ecore_Win32_Event_Window_Show *ev = event;

	if (thiz->win != ev->window) return EINA_TRUE;
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_win32_event_window_hide(void *data,
		int type, void *event)
{
	//printf("hide\n");
	return EINA_TRUE;
}

static void _efl_egueb_window_win32_event_register(Efl_Egueb_Window_Win32 *thiz)
{
	thiz->handlers[0] = ecore_event_handler_add(ECORE_WIN32_EVENT_MOUSE_IN,
			_efl_egueb_window_win32_event_mouse_in, thiz);
	thiz->handlers[1] = ecore_event_handler_add(ECORE_WIN32_EVENT_MOUSE_OUT,
			_efl_egueb_window_win32_event_mouse_out, thiz);
	thiz->handlers[2] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_FOCUS_IN,
			_efl_egueb_window_win32_event_window_focus_in, thiz);
	thiz->handlers[3] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_FOCUS_OUT,
			_efl_egueb_window_win32_event_window_focus_out, thiz);
	thiz->handlers[4] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_DAMAGE,
			_efl_egueb_window_win32_event_window_damage, thiz);
	thiz->handlers[5] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_DESTROY,
			_efl_egueb_window_win32_event_window_destroy, thiz);
	thiz->handlers[6] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_CONFIGURE,
			_efl_egueb_window_win32_event_window_configure, thiz);
	thiz->handlers[7] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_DELETE_REQUEST,
			_efl_egueb_window_win32_event_window_delete_request, thiz);
	thiz->handlers[8] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_SHOW,
			_efl_egueb_window_win32_event_window_show, thiz);
	thiz->handlers[9] = ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_HIDE,
			_efl_egueb_window_win32_event_window_hide, thiz);
}

static void _efl_egueb_window_win32_event_unregister(Efl_Egueb_Window_Win32 *thiz)
{
	unsigned int i;

	for (i = 0; i < sizeof(thiz->handlers) / sizeof(Ecore_Event_Handler*); i++)
	{
		if (thiz->handlers[i])
			ecore_event_handler_del(thiz->handlers[i]);
	}
}
/*----------------------------------------------------------------------------*
 *                        Window descriptor interface                         *
 *----------------------------------------------------------------------------*/
static Ecore_Window _efl_egueb_window_win32_window_get(void *data)
{
	Efl_Egueb_Window_Win32 *thiz = data;
	return (Ecore_Window)thiz->win;
}

static void _efl_egueb_window_win32_output_update(void *data, Eina_Rectangle *area)
{
	Efl_Egueb_Window_Win32 *thiz = data;

	_update_area(thiz, area);
}

static void _efl_egueb_window_win32_free(void *data)
{
	Efl_Egueb_Window_Win32 *thiz = data;

	_efl_egueb_window_win32_event_unregister(thiz);
	if (thiz->bitmap)
	{
		DeleteObject(thiz->bitmap);
		thiz->bitmap = NULL;
		free(thiz->bitmap_buffer);
		thiz->bitmap_buffer = NULL;
	}
}

static Efl_Egueb_Window_Descriptor _descriptor = {
	/* .window_get  	= */ _efl_egueb_window_win32_window_get,
	/* .output_update 	= */ _efl_egueb_window_win32_output_update,
	/* .free 		= */ _efl_egueb_window_win32_free,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Efl_Egueb_Window * efl_egueb_window_win32_new(Egueb_Dom_Node *doc,
		const char *display, Ecore_Win32_Window *parent, int x, int y,
		int w, int h)
{
	Efl_Egueb_Window_Win32 *thiz;
	Efl_Egueb_Window *ret;
	Ecore_Win32_Window *win;

	if (!ecore_win32_init(display)) return NULL;
	thiz = calloc(1, sizeof(Efl_Egueb_Window_Win32));
	ret = efl_egueb_window_new(doc, x, y, w, h, &_descriptor, thiz);
	if (!ret)
	{
		free(thiz);
		return NULL;
	}

	win = ecore_win32_window_new (parent, x, y, ret->w, ret->h);

	thiz->win = win;
	thiz->base = ret;
	thiz->win_hwnd = ecore_win32_window_hwnd_get(win);

	_efl_egueb_window_win32_event_register(thiz);

	/* create our own buffer to use for converting the enesim surface
	 * into something the win32 GDI understands
	 */
	if (!_efl_egueb_window_win32_buffer_setup(thiz))
	{
		ecore_win32_window_free(thiz->win);
		free(thiz);
		return NULL;
	}
	ecore_win32_window_show(win);
	return ret;
}

EAPI Ecore_Win32_Window * efl_egueb_window_win32_window_get(Efl_Egueb_Window *w)
{
	return 0;
}
#endif

