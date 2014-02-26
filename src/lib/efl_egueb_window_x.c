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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Egueb_Window_X
{
	Efl_Egueb_Window *base;
	Ecore_X_Window win;
	Ecore_X_Screen *screen;
	Ecore_X_Visual visual;
	Ecore_X_Colormap colormap;
	Ecore_X_Image *xim;
	/* the event handlers */
	Ecore_Event_Handler *handlers[13];
	Enesim_Buffer *b;
	Enesim_Surface *s;
	Eina_Bool argb;
	int depth;
	int w;
	int h;
} Efl_Egueb_Window_X;

static int _init_count = 0;

static inline void _mask_to_offset_and_length(int value, uint8_t *offset, uint8_t *len)
{
	*len = 0;
	*offset = 0;
	printf("%08x\n", value);
	while (value)
	{
		if (value & 1)
		{
			while (value)
			{
				if (!(value & 1))
					break;
				value >>= 1;
				(*len)++;
			}
			return;
		}
		(*offset)++;
		value >>= 1;
	}
}

/*----------------------------------------------------------------------------*
 *                               Event handlers                               *
 *----------------------------------------------------------------------------*/
static Eina_Bool _efl_egueb_window_x_event_mouse_in(void *data,
		int type, void *event)
{
	printf("mouse in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_mouse_out(void *data,
		int type, void *event)
{
	printf("mouse out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_focus_in(void *data,
		int type, void *event)
{
	printf("focus in\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_focus_out(void *data,
		int type, void *event)
{
	printf("focus out\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_damage(void *data,
		int type, void *event)
{
	printf("damage\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_destroy(void *data,
		int type, void *event)
{
	printf("destroy\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_configure(void *data,
		int type, void *event)
{
	printf("configure\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_delete_request(void *data,
		int type, void *event)
{
	printf("delete request\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_show(void *data,
		int type, void *event)
{
	Efl_Egueb_Window_X *thiz = data;
	Ecore_X_Event_Window_Show *ev = event;
	Ecore_X_GC gc;

	if (thiz->win != ev->win) return EINA_TRUE;
	egueb_dom_document_process(thiz->base->doc);
	/* draw the whole area */
	egueb_dom_feature_render_draw(thiz->base->render, thiz->s, ENESIM_ROP_FILL, NULL, 0, 0, NULL);
	/* convert */
	enesim_converter_surface(thiz->s, thiz->b);
	/* upload */
	ecore_x_image_put(thiz->xim, thiz->win, NULL, 0, 0, 0, 0, thiz->w, thiz->h);
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_window_hide(void *data,
		int type, void *event)
{
	printf("hide\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_property_change(void *data,
		int type, void *event)
{
	printf("property change\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_visibility_change(void *data,
		int type, void *event)
{
	printf("visibility change\n");
	return EINA_TRUE;
}

static Eina_Bool _efl_egueb_window_x_event_client_message(void *data,
		int type, void *event)
{
	return EINA_TRUE;
}


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

static Eina_Bool _efl_egueb_window_x_visual_to_format(Ecore_X_Visual *v,
		int depth, Enesim_Buffer_Format *f)
{
	Ecore_X_Display *display;
	Visual *visual = v;
	uint8_t rlen, roffset;
	uint8_t glen, goffset;
	uint8_t blen, boffset;

	if (visual->class != TrueColor && visual->class != DirectColor)
		return EINA_FALSE;
	_mask_to_offset_and_length(visual->red_mask, &roffset, &rlen);
	_mask_to_offset_and_length(visual->green_mask, &goffset, &glen);
	_mask_to_offset_and_length(visual->blue_mask, &boffset, &blen);
	return enesim_buffer_format_rgb_components_from(f, depth, 0, 0, roffset, rlen,
			goffset, glen, boffset, blen, EINA_FALSE);
}

static Eina_Bool _efl_egueb_window_x_buffer_setup(Efl_Egueb_Window_X *thiz)
{
	Enesim_Buffer_Format format;
	Enesim_Buffer_Sw_Data sdata;
	int bpl;
	int rows;
	int bpp;
	void *data;

	/* get the format based on the x attributes */
	if (!_efl_egueb_window_x_visual_to_format(thiz->visual, thiz->depth, &format))
	{
		printf("no format\n");
		return EINA_FALSE;
	}
	/* create the ximage */
	thiz->xim = ecore_x_image_new(thiz->w, thiz->h, thiz->visual, thiz->depth);
	if (!thiz->xim)
		return EINA_FALSE;
	data = ecore_x_image_data_get(thiz->xim, &bpl, &rows, &bpp);
	printf("format %d. (%d %d) %d %d %d\n", format, thiz->w, thiz->h, bpl, rows, bpp);
	/* FIXME ecore is giving xrgb8888 format, weird ... */
	format = ENESIM_BUFFER_FORMAT_XRGB8888;
	switch (format)
	{
		case ENESIM_BUFFER_FORMAT_RGB888:
		sdata.rgb888.plane0_stride = bpl;
		sdata.rgb888.plane0 = data;
		break;

		case ENESIM_BUFFER_FORMAT_BGR888:
		sdata.bgr888.plane0_stride = bpl;
		sdata.bgr888.plane0 = data;

		case ENESIM_BUFFER_FORMAT_XRGB8888:
		sdata.xrgb8888.plane0_stride = bpl;
		sdata.xrgb8888.plane0 = data;
		break;

		default:
		break;
	}
	
	/* create the buffer */
	thiz->b = enesim_buffer_new_data_from(format, thiz->w, thiz->h, EINA_FALSE, &sdata, NULL, NULL);
	if (!thiz->b)
	{
		ecore_x_image_free(thiz->xim);
		return EINA_FALSE;
	}
	thiz->s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, thiz->w, thiz->h);

	return EINA_TRUE;
}

static void _efl_egueb_window_x_event_register(Efl_Egueb_Window_X *thiz)
{
	thiz->handlers[0] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_IN,
			_efl_egueb_window_x_event_mouse_in, thiz);
	thiz->handlers[1] = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_OUT,
			_efl_egueb_window_x_event_mouse_out, thiz);
	thiz->handlers[2] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_IN,
			_efl_egueb_window_x_event_window_focus_in, thiz);
	thiz->handlers[3] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_FOCUS_OUT,
			_efl_egueb_window_x_event_window_focus_out, thiz);
	thiz->handlers[4] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DAMAGE,
			_efl_egueb_window_x_event_window_damage, thiz);
	thiz->handlers[5] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DESTROY,
			_efl_egueb_window_x_event_window_destroy, thiz);
	thiz->handlers[6] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_CONFIGURE,
			_efl_egueb_window_x_event_window_configure, thiz);
	thiz->handlers[7] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
			_efl_egueb_window_x_event_window_delete_request, thiz);
	thiz->handlers[8] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_SHOW,
			_efl_egueb_window_x_event_window_show, thiz);
	thiz->handlers[9] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_HIDE,
			_efl_egueb_window_x_event_window_hide, thiz);
	thiz->handlers[10] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY,
			_efl_egueb_window_x_event_property_change, thiz);
	thiz->handlers[11] = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE,
			_efl_egueb_window_x_event_visibility_change, thiz);
	thiz->handlers[12] = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE,
			_efl_egueb_window_x_event_client_message, thiz);
}

static void _efl_egueb_window_x_event_unregister(Efl_Egueb_Window_X *thiz)
{
	unsigned int i;

	for (i = 0; i < sizeof(thiz->handlers) / sizeof(Ecore_Event_Handler*); i++)
	{
		if (thiz->handlers[i])
			ecore_event_handler_del(thiz->handlers[i]);
	}
}

static void _efl_egueb_window_x_init(void)
{
	_init_count++;
	if (_init_count > 1) return;
}

static void _efl_egueb_window_x_deinit(void)
{
	if (!_init_count) return;
	if (_init_count == 1)
	{
	}
	_init_count--;
}
/*----------------------------------------------------------------------------*
 *                        Window descriptor interface                         *
 *----------------------------------------------------------------------------*/
static void _efl_egueb_window_x_free(void *data)
{
	Efl_Egueb_Window_X *thiz = data;
	_efl_egueb_window_x_event_unregister(thiz);
	ecore_x_image_free(thiz->xim);
	enesim_buffer_unref(thiz->b);
}

static Efl_Egueb_Window_Descriptor _descriptor = {
	/* .free 	= */ _efl_egueb_window_x_free,
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
	Efl_Egueb_Window *ret;
	Ecore_X_Window win;
	Ecore_X_Screen *screen;
	Ecore_X_Window_Attributes at;
	Eina_Bool argb = EINA_FALSE;

	if (!ecore_x_init(display)) return NULL;
	_efl_egueb_window_x_init();

	if (parent)
		argb = ecore_x_window_argb_get(parent);

	if (argb)
		w = ecore_x_window_argb_new(parent, x, y, w, h);

	win = ecore_x_window_new(parent, x, y, w, h);
	ecore_x_window_defaults_set(win);

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

	_efl_egueb_window_x_event_register(thiz);

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

	ret = efl_egueb_window_new(doc, &_descriptor, thiz);
	thiz->base = ret;

	return ret;
}

EAPI Ecore_X_Window efl_egueb_window_x_window_get(Efl_Egueb_Window *w)
{
	return 0;
}
#endif
