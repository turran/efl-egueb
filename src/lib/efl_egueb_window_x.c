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

#if BUILD_ECORE_X
#include "efl_svg_main.h"
#include "efl_egueb_window.h"

#include "efl_egueb_window_private.h"
#include "Efl_Egueb_Window_X.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Egueb_Window_X
{
	Ecore_X_Window win;
	Ecore_X_Screen *screen;
	Ecore_X_Visual visual;
	Ecore_X_Colormap colormap;
	Ecore_X_Image *xim;
	Enesim_Buffer *buffer;
	Eina_Bool argb;
	int depth;
	int w;
	int h;
} Efl_Egueb_Window_X;

static int _init_count = 0;
static Ecore_Event_Handler *_handlers[13];

static Ecore_X_Screen * _efl_egueb_window_x_screen_get(Ecore_X_Window w)
{
	Ecore_X_Screen *screen;

	screen = ecore_x_default_screen_get();
	if (ecore_x_screen_count_get() > 1)
	{
		Ecore_X_Window *roots;
		int num, i;

		num = 0;
		roots = ecore_x_window_root_list(&num);
		if (roots)
		{
			Ecore_X_Window root;

			root = ecore_x_window_root_get(w);
			for (i = 0; i < num; i++)
			{
				if (root == roots[i])
				{
					screen = ecore_x_screen_get(i);
					break;
				}
			}
		}
		free(roots);
	}
	return screen;
}

static Eina_Bool _efl_egueb_window_x_buffer_setup(Efl_Egueb_Window_X *thiz)
{
	Enesim_Buffer *b;
	Enesim_Buffer_Format format;

	/* get the format based on the x attributes */
	/* create the buffer */
	/* create the ximage */
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _efl_egueb_window_x11_event_mouse_in(void *data,
		int type, void *event)
{
	printf("mouse in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_mouse_out(void *data,
		int type, void *event)
{
	printf("mouse out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_focus_in(void *data,
		int type, void *event)
{
	printf("focus in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_focus_out(void *data,
		int type, void *event)
{
	printf("focus out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_damage(void *data,
		int type, void *event)
{
	printf("damage\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_destroy(void *data,
		int type, void *event)
{
	printf("destroy\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_configure(void *data,
		int type, void *event)
{
	printf("configure\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_delete_request(void *data,
		int type, void *event)
{
	printf("delete request\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_show(void *data,
		int type, void *event)
{
	printf("show\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_window_hide(void *data,
		int type, void *event)
{
	printf("hide\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_property_change(void *data,
		int type, void *event)
{
	printf("property change\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_visibility_change(void *data,
		int type, void *event)
{
	printf("visibility change\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x11_event_client_message(void *data,
		int type, void *event)
{
	return EINA_TRUE;
}

static void _efl_egueb_window_x11_init(void)
{
	_init_count++;
	if (_init_count > 1) return;

	_handlers[0] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_IN,
			_efl_egueb_window_x11_event_mouse_in, NULL);
	_handlers[1] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT,
			_efl_egueb_window_x11_event_mouse_out, NULL);
	_handlers[2] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN,
			_efl_egueb_window_x11_event_window_focus_in, NULL);
	_handlers[3] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT,
			_efl_egueb_window_x11_event_window_focus_out, NULL);
	_handlers[4] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE,
			_efl_egueb_window_x11_event_window_damage, NULL);
	_handlers[5] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY,
			_efl_egueb_window_x11_event_window_destroy, NULL);
	_handlers[6] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE,
			_efl_egueb_window_x11_event_window_configure, NULL);
	_handlers[7] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
			_efl_egueb_window_x11_event_window_delete_request, NULL);
	_handlers[8] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_SHOW,
			_efl_egueb_window_x11_event_window_show, NULL);
	_handlers[9] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE,
			_efl_egueb_window_x11_event_window_hide, NULL);
	_handlers[10] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY,
			_efl_egueb_window_x11_event_property_change, NULL);
	_handlers[11] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE,
			_efl_egueb_window_x11_event_visibility_change, NULL);
	_handlers[12] = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE,
			_efl_egueb_window_x11_event_client_message, NULL);
}


static void _efl_egueb_window_x11_deinit(void)
{
	if (!_init_count) return;
	if (_init_count == 1)
	{
		unsigned int i;

		for (i = 0; i < sizeof(_handlers) / sizeof(Ecore_Event_Handler*); i++)
		{
			if (_handlers[i])
				ecore_event_handler_del(_handlers[i]);
		}
	}
	_init_count--;
}
/*----------------------------------------------------------------------------*
 *                        Window descriptor interface                         *
 *----------------------------------------------------------------------------*/
static Efl_Egueb_Window_Descriptor _descriptor = {

};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Efl_Egueb_Window * efl_egueb_window_x_new(Egueb_Dom_Node *doc,
		const char *display, Ecore_X_Window parent, int x, int y,
		int w, int h)
{
	Efl_Egueb_Window_X *thiz;
	Ecore_X_Window win;
	Ecore_X_Screen *screen;
	Ecore_X_Window_Attributes at;
	Enesim_Buffer *b;
	Eina_Bool argb = EINA_FALSE;

	if (!ecore_x_init(display)) return NULL;
	_efl_egueb_window_x11_init();

	if (parent)
		argb = ecore_x_window_argb_get(parent);

	if (argb)
		w = ecore_x_window_argb_new(parent, x, y, w, h);

	win = ecore_x_window_new(parent, x, y, w, h);
	screen = _efl_egueb_window_x_screen_get(parent);
	ecore_x_window_attributes_get(win, &at);

	thiz = calloc(1, sizeof(Efl_Egueb_Window_X));
	thiz->win = win;
	thiz->screen = screen;
	thiz->visual = at.visual;
	thiz->colormap = at.colormap;
	thiz->depth = at.depth;
	thiz->argb = argb;
	thiz->w = w;
	thiz->h = h;

	/* create our own buffer to use for converting the enesim surface
	 * into something X understands
	 */
	if (!_efl_egueb_window_x_buffer_setup(thiz))
	{
		ecore_x_window_free(thiz->win);
		free(thiz);
		return NULL;
	}
	ecore_x_window_show(win);

	return efl_egueb_window_new(doc, &_descriptor, thiz);
}

EAPI Ecore_X_Window efl_egueb_window_x_window_get(Efl_Egueb_Window *w)
{
	return 0;
}
#endif
