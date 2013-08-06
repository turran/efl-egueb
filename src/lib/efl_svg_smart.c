#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <math.h>
#include <libgen.h>
#include <sys/time.h>

#include <Evas.h>
#include <Ecore.h>

#include <Eina_Extra.h>

#ifdef HAVE_GL
# define ENESIM_EXTENSION
# include <Enesim.h>
/* FIXME this one is missing on the evas-gl */
typedef double		GLdouble;	/* double precision float */
# include <Evas_GL.h>
# include <Enesim_OpenGL.h>
# define GLERR {\
        GLenum err; \
        err = glGetError(); \
        printf("Error %x\n", err); \
        }
#endif

#include <Egueb_Svg.h>

#define CRI(...) EINA_LOG_DOM_CRIT(_log, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log, __VA_ARGS__)

#include "Efl_Svg_Smart.h"

/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Efl_Svg_Smart
{
	/* properties */
	char *file;
	int fps;
	Eina_Bool debug_damage;
	Eina_Bool zoom_and_pan;

	/* private */
	Evas *evas;
	Evas_Object *o;

	/* image position */
	Evas_Coord ix, iy;

	/* smart position and size */
	Evas_Coord x, y, w, h;

	/* internal smart objects */
	Evas_Object *bkg;
	Evas_Object *img;
	Evas_Object *img_clip;
	Egueb_Dom_Node *doc;
	Ecore_Idle_Enterer *idler;
	Ecore_Timer *animator;
	Enesim_Backend backend;
	Enesim_Surface *s;

	/* gl needed data */
#ifdef HAVE_GL
	Evas_GL *gl_evas;
	Evas_GL_API *gl_api;
	Evas_GL_Surface *gl_surface;
	Evas_GL_Context *gl_ctx;
	Evas_GL_Config *gl_cfg;
	Enesim_Pool *gl_pool;
#endif
	/* in case we want to debug the damages */
	Eina_List *damage_rectangles;
	Eina_List *damage_objects;
	int damage_count;

	/* app descriptor */
	char *base_dir;
	char *go_to;

	/* event interface for zoom and pan */
	Eina_Bool down;
	Evas_Coord down_x, down_y;
	Evas_Coord img_down_x, img_down_y;
	Eina_Bool scrolling;
} Efl_Svg_Smart;

static Evas_Smart *_smart = NULL;
static int _init = 0;
static int _log = -1;
/*----------------------------------------------------------------------------*
 *                           Application interface                            *
 *----------------------------------------------------------------------------*/
static const char * _efl_svg_mart_filename_get(void *user_data)
{
	Efl_Svg_Smart *thiz = user_data;
	return thiz->file;
}

#if 0
static const char * _efl_svg_smart_base_dir_get(Egueb_Dom_Node *e EINA_UNUSED, void *data)
{
	Efl_Svg_Smart *thiz = data;
	return thiz->base_dir;
}

static void _efl_svg_smart_go_to(Egueb_Dom_Node *e EINA_UNUSED, void *data, const char *uri)
{
	Efl_Svg_Smart *thiz = data;
	thiz->go_to = strdup(uri);
}

static Esvg_Element_Svg_Application_Descriptor _efl_svg_smart_descriptor = {
	/* .base_dir_get 		  = */ _efl_svg_smart_base_dir_get,
	/* .go_to 			  = */ _efl_svg_smart_go_to,
	/* .script_alert 		  = */ NULL,
	/* .video_provider_descriptor_get = */ NULL
};
#endif

static void _efl_svg_smart_benchmark(Eina_Bool start)
{
	static double last = 0;
	static double td = 0;
	struct timeval timev;
	double t;

	gettimeofday(&timev, NULL);
	t = (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
	if (start)
	{
		last = t;
		return;
	}
	INF("Drawing took %g at %g", t - last, last - td);
	td = t;
	last = t;
}

static inline double _efl_svg_smart_timestamp_get(void)
{
	struct timeval timev;
	double t;

	gettimeofday(&timev, NULL);
	t = (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
	return t;
}

static Eina_Bool _efl_svg_smart_damages(Egueb_Dom_Node *e EINA_UNUSED, Eina_Rectangle *area, void *data)
{
	Efl_Svg_Smart *thiz = data;
	Eina_Rectangle *r;

	/* FIXME instead of allocating, just get the rectangle from a pool of rectangles */
	r = malloc(sizeof(Eina_Rectangle));
	*r = *area;

	INF("Adding damage at %" EINA_EXTRA_RECTANGLE_FORMAT, EINA_EXTRA_RECTANGLE_ARGS(area));
	thiz->damage_rectangles = eina_list_append(thiz->damage_rectangles, r);
	if (thiz->debug_damage)
	{
		Evas_Object *o;

		o = eina_list_nth(thiz->damage_objects, thiz->damage_count);
		if (!o)
		{
			o = evas_object_rectangle_add(thiz->evas);
			evas_object_color_set(o, 64, 0, 0, 64);
			thiz->damage_objects = eina_list_append(thiz->damage_objects, o);
		}
		DBG("Adding rectangle object at %d %d", thiz->ix, thiz->iy);
		evas_object_move(o, r->x + thiz->ix, r->y + thiz->iy);
		evas_object_resize(o, r->w, r->h);
		evas_object_show(o);

		thiz->damage_count++;
	}

	return EINA_TRUE;
}

static void _efl_svg_smart_damage_clear(Efl_Svg_Smart *thiz)
{
	Eina_List *l;
	Evas_Object *o;
	int i = 0;

	if (!thiz->debug_damage) return;
	EINA_LIST_FOREACH(thiz->damage_objects, l, o)
	{
		if (thiz->damage_count == i)
			break;
		evas_object_hide(o);
		i++;
	}
	thiz->damage_count = 0;
}

static Eina_Bool _efl_svg_smart_animator_cb(void *data)
{
	Efl_Svg_Smart *thiz = data;
	Egueb_Dom_Node *svg = NULL;;

	egueb_dom_document_element_get(thiz->doc, &svg);
	if (!svg) goto done;

	egueb_svg_element_svg_time_tick(svg);
	egueb_dom_node_unref(svg);
done:
	return EINA_TRUE;
}

static void _efl_svg_smart_mouse_up(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Efl_Svg_Smart *thiz = (Efl_Svg_Smart *)data;
/* 	Evas_Event_Mouse_Up *ev = event_info; */
	egueb_svg_document_feed_mouse_up(thiz->doc, 0);
	thiz->down = EINA_FALSE;
	{
		Egueb_Dom_Node *svg;
		egueb_dom_document_element_get(thiz->doc, &svg);
		ERR("Setting scale to 1x");
		egueb_svg_element_svg_current_scale_set(svg, 1);
		egueb_dom_node_unref(svg);
	}
}

static void _efl_svg_smart_mouse_down(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Efl_Svg_Smart *thiz = (Efl_Svg_Smart *)data;
 	Evas_Event_Mouse_Down *ev = event_info;
	Evas_Coord ix, iy;

	egueb_svg_document_feed_mouse_down(thiz->doc, 0);

	thiz->down = EINA_TRUE;
	evas_object_geometry_get(thiz->img, &ix, &iy, NULL, NULL);
	thiz->down_x = ev->canvas.x;
	thiz->down_y = ev->canvas.y;
	thiz->img_down_x = ix;
	thiz->img_down_y = iy;

	{
		Egueb_Dom_Node *svg;
		egueb_dom_document_element_get(thiz->doc, &svg);
		ERR("Setting scale to 2x");
		egueb_svg_element_svg_current_scale_set(svg, 2);
		egueb_dom_node_unref(svg);
	}
}

static void _efl_svg_smart_mouse_move(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
	Efl_Svg_Smart *thiz = (Efl_Svg_Smart *)data;
	Evas_Event_Mouse_Move *ev = event_info;
	Evas_Coord ix;
	Evas_Coord iy;
	int svgx;
	int svgy;

	/* check if we are dragging */
	if (thiz->down && thiz->zoom_and_pan)
	{
		Evas_Coord nx, ny;
		
		/* TODO if we do, first send the mouse up on the doc */
		nx = thiz->img_down_x + (ev->cur.canvas.x - thiz->down_x);
		ny = thiz->img_down_y + (ev->cur.canvas.y - thiz->down_y);
		/* start moving the image origin */
		evas_object_move(thiz->img, nx, ny);
	}
	evas_object_geometry_get(thiz->img, &ix, &iy, NULL, NULL);
	svgx = ev->cur.canvas.x - ix;
	svgy = ev->cur.canvas.y - iy;
	egueb_svg_document_feed_mouse_move(thiz->doc, svgx, svgy);
}

static void _efl_svg_smart_reconfigure(Efl_Svg_Smart *thiz)
{
	Evas_Coord x, y, w, h;
	Evas_Coord ix, iy, iw, ih;

	w = thiz->w;
	h = thiz->h;
	x = thiz->x;
	y = thiz->y;

	/* resize and move the main background object */
	evas_object_move(thiz->bkg, x, y);
	evas_object_resize(thiz->bkg, w, h);

	/* do the same on the clip */
	evas_object_move(thiz->img_clip, x, y);
	evas_object_resize(thiz->img_clip, w, h);

	/* resize the document */
	egueb_svg_document_width_set(thiz->doc, w);
	egueb_svg_document_height_set(thiz->doc, h);

	/* center the image */
	evas_object_image_size_get(thiz->img, &iw, &ih);
	ix = x;
	if (thiz->w > iw)
		ix += (thiz->w - iw) / 2;
	iy = y;
	if (thiz->h > ih)
		iy += (thiz->h - ih) / 2;
	thiz->ix = ix;
	thiz->iy = iy;
	evas_object_move(thiz->img, ix, iy);
}

#ifdef HAVE_GL
static void _efl_svg_smart_opengl_surface_reconfigure(Efl_Svg_Smart *thiz, Evas_Coord iw, Evas_Coord ih)
{
	if (thiz->s)
	{
		enesim_surface_unref(thiz->s);
		thiz->s = NULL;

		evas_gl_surface_destroy(thiz->gl_evas, thiz->gl_surface);
		thiz->gl_surface = NULL;
	}

	if (!thiz->s)
	{
		Evas_Native_Surface ns;
		//Enesim_Buffer_OpenGL_Data *bd;
#if 1
		/* create the gl surface */
		thiz->gl_surface = evas_gl_surface_create(thiz->gl_evas, thiz->gl_cfg, iw, ih);
		evas_gl_native_surface_get(thiz->gl_evas, thiz->gl_surface, &ns);
		thiz->s = enesim_surface_new_opengl_data_from(ENESIM_FORMAT_ARGB8888, iw, ih, ns.data.opengl.texture_id);
#else
		thiz->s = enesim_surface_new_pool_from(ENESIM_FORMAT_ARGB8888, iw, ih, thiz->gl_pool);
		bd = enesim_surface_backend_data_get(thiz->s);

		ns.version = EVAS_NATIVE_SURFACE_VERSION;
		ns.type = EVAS_NATIVE_SURFACE_OPENGL;
		ns.data.opengl.texture_id = bd->texture;
		ns.data.opengl.framebuffer_id = 0;
		ns.data.opengl.format = ns.data.opengl.internal_format = GL_RGBA;
		ns.data.opengl.x = 0;
		ns.data.opengl.y = 0;
		ns.data.opengl.w = iw;
		ns.data.opengl.h = ih;
#endif
		evas_object_image_native_surface_set(thiz->img, &ns);
		printf("gl surface of size %d %d with tid %d and addr %p\n", iw, ih, ns.data.opengl.texture_id, thiz->s);
		printf("version %d %d\n", ns.version, EVAS_NATIVE_SURFACE_VERSION);
		printf("type %d %d\n", ns.version, EVAS_NATIVE_SURFACE_OPENGL);
		printf("tid %d\n", ns.data.opengl.texture_id);
		printf("fbid %d\n", ns.data.opengl.framebuffer_id);
		printf("format %d %d %d\n", ns.data.opengl.format, ns.data.opengl.internal_format, GL_RGBA);
	}
}
#endif

static void _efl_svg_smart_sw_surface_reconfigure(Efl_Svg_Smart *thiz, Evas_Coord iw,
		Evas_Coord ih)
{
	if (thiz->s)
	{
		enesim_surface_unref(thiz->s);
		thiz->s = NULL;
	}

	if (!thiz->s)
	{
		void *data;
		size_t stride;

		data = evas_object_image_data_get(thiz->img, EINA_TRUE);
		stride = evas_object_image_stride_get(thiz->img);
		thiz->s = enesim_surface_new_data_from(ENESIM_FORMAT_ARGB8888,
				iw, ih, EINA_FALSE, data, stride, NULL, NULL);
	}
}

static Eina_Bool _efl_svg_smart_surface_reconfigure(Efl_Svg_Smart *thiz)
{
	Evas_Coord iw, ih;
	Evas_Coord w, h;
	double aw;
	double ah;

	egueb_svg_document_actual_width_get(thiz->doc, &aw);
	egueb_svg_document_actual_height_get(thiz->doc, &ah);
	evas_object_image_size_get(thiz->img, &iw, &ih);

	w = ceil(aw);
	h = ceil(ah);
	if ((iw != w) || (ih != h))
	{
		evas_object_resize(thiz->img, w, h);
		evas_object_image_size_set(thiz->img, w, h);
		evas_object_image_fill_set(thiz->img, 0, 0, w, h);
		if (thiz->backend == ENESIM_BACKEND_SOFTWARE)
		{
			_efl_svg_smart_sw_surface_reconfigure(thiz, w, h);
		}
#if HAVE_GL
		else if (thiz->backend == ENESIM_BACKEND_OPENGL)
		{
			_efl_svg_smart_opengl_surface_reconfigure(thiz, w, h);
		}
#endif
		//evas_object_image_data_update_add(thiz->img, 0, 0, w, h);
		_efl_svg_smart_reconfigure(thiz);
		return EINA_TRUE;
	}
	return EINA_FALSE;
}


/* The reason to create another idler is because evas does not allow to hook
 * before the evas_render() is begin called. The smart calculate is the most similar
 * case, but whenever you enqueue your calculate during a calculate, it will get
 * triggered again on the same cycle, not the next one.
 */
static Eina_Bool _efl_svg_smart_idler_cb(void *data)
{
	Efl_Svg_Smart *thiz = (Efl_Svg_Smart *)data;
	Egueb_Dom_Node *svg = NULL;
	Enesim_Log *error = NULL;
	Eina_Rectangle *r;
	Eina_Bool new_svg = EINA_FALSE;
	Eina_Bool ret;
	/* for benchmarking */
	double draw_start, draw_end;
	double process_start, process_end;

	/* check that we have a topmost svg */
	egueb_dom_document_element_get(thiz->doc, &svg);
	if (!svg) goto done;

	/* check if we dont have to jump to another svg */
	if (thiz->go_to)
	{
		efl_svg_file_set(thiz->o, thiz->go_to);
		free(thiz->go_to);
		thiz->go_to = NULL;
		new_svg = EINA_TRUE;
	}

	/* check if we need to process the document, if not, then dont ask
	 * for the damages
	 */
	if (!egueb_dom_document_needs_process(thiz->doc))
		goto done;

	/* ok, lets process the document and check for its damages */
	process_start = _efl_svg_smart_timestamp_get();
	egueb_dom_document_process(thiz->doc);
	process_end = _efl_svg_smart_timestamp_get();
	INF("Processing took %g", process_end - process_start);

	/* if we jumped to a new svg, be sure to send the mouse move */
	if (new_svg)
	{
		Evas_Coord x;
		Evas_Coord y;
		Evas_Coord ix;
		Evas_Coord iy;

		/* feed a mouse move event to inform our current cursor position */
		evas_object_geometry_get(thiz->img, &ix, &iy, NULL, NULL);
		evas_pointer_canvas_xy_get(thiz->evas, &x, &y);
		egueb_svg_document_feed_mouse_move(svg, x - ix, y - iy);
	}

	/* check if the size has changed, if so, create a new surface */
	_efl_svg_smart_surface_reconfigure(thiz);

	/* get the damages */
	_efl_svg_smart_damage_clear(thiz);
	egueb_svg_document_damages_get(thiz->doc, _efl_svg_smart_damages, thiz);
	if (!thiz->damage_rectangles) goto done;

	/* FIXME from the docs, looks like if i put true on the second argument, the whole data
	 * will be invalidated, but then, it says that i should call update_add ... weird
	 */
	if (!thiz->s)
		goto no_surface;
#if HAVE_GL
	if (thiz->backend == ENESIM_BACKEND_OPENGL)
	{
		evas_gl_make_current(thiz->gl_evas, thiz->gl_surface, thiz->gl_ctx);
	}
#endif

	draw_start = _efl_svg_smart_timestamp_get();
	/* we use the fill variant given that we need to overwrite what is in the image */
	ret = egueb_svg_element_svg_draw_list(svg, thiz->s, ENESIM_FILL, thiz->damage_rectangles, 0, 0, &error);
	draw_end = _efl_svg_smart_timestamp_get();
	if (!ret)
	{
		ERR("Error drawing");
		if (error)
		{
			enesim_log_dump(error);
			enesim_log_delete(error);
		}
	}
	INF("Drawing took %g", draw_end - draw_start);
	/* free the list and its rectangles too, this should change whenever the rects are cached */
no_surface:
	EINA_LIST_FREE(thiz->damage_rectangles, r)
	{
		evas_object_image_data_update_add(thiz->img, r->x, r->y, r->w, r->h);
		free(r);
	}
	thiz->damage_rectangles = NULL;
	/* TODO mark evas to pick again the damaged pixels not the whole surface */
#if 0
	evas_object_image_size_get(thiz->img, &iw, &ih);
	evas_object_image_data_update_add(thiz->img, 0, 0, iw, ih);
#endif
done:
	if (svg)
	{
		egueb_dom_node_unref(svg);
	}
	
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                           Smart object interface                           *
 *----------------------------------------------------------------------------*/
static void _efl_svg_smart_calculate(Evas_Object *obj EINA_UNUSED)
{
}

static void _efl_svg_smart_add(Evas_Object *obj)
{
	Efl_Svg_Smart *thiz;
	Evas *e;
	Eina_List *engines, *l;
	Enesim_Backend backend;
	char *engine;
	int render_method;

	thiz = calloc(1, sizeof(Efl_Svg_Smart));
	thiz->o = obj;

	/* set default properties */
	thiz->debug_damage = EINA_FALSE;
	thiz->zoom_and_pan = EINA_TRUE;
	thiz->fps = 30;

	/* create the evas objects */
	e = evas_object_evas_get(obj);
	thiz->evas = e;

	thiz->bkg = evas_object_rectangle_add(e);
	evas_object_color_set(thiz->bkg, 255, 255, 255, 255);
	evas_object_smart_member_add(thiz->bkg, obj);

#if 0
	thiz->img_clip = evas_object_rectangle_add(e);
	evas_object_color_set(thiz->img_clip, 255, 255, 255, 255);
	evas_object_smart_member_add(thiz->img_clip, obj);
#endif

   	thiz->img = evas_object_image_add(e);
	evas_object_image_alpha_set(thiz->img, EINA_TRUE);
	//evas_object_clip_set(thiz->img, thiz->img_clip);
	evas_object_smart_member_add(thiz->img, obj);

	/* the idler */
	thiz->idler = ecore_idle_enterer_add(_efl_svg_smart_idler_cb, thiz);
	/* the animator */
	thiz->animator = ecore_timer_add(1.0/thiz->fps, _efl_svg_smart_animator_cb, thiz);
	/* the events */
	evas_object_event_callback_add(thiz->img, EVAS_CALLBACK_MOUSE_DOWN,
		_efl_svg_smart_mouse_down, thiz);
	evas_object_event_callback_add(thiz->img, EVAS_CALLBACK_MOUSE_UP,
		_efl_svg_smart_mouse_up, thiz);
	evas_object_event_callback_add(thiz->img, EVAS_CALLBACK_MOUSE_MOVE,
		_efl_svg_smart_mouse_move, thiz);
	evas_object_smart_data_set(obj, thiz);

	/* the backend to use */
	render_method = evas_output_method_get(e);
	engines = evas_render_method_list();
	EINA_LIST_FOREACH(engines, l, engine)
	{
		if (render_method == evas_render_method_lookup(engine))
			break;
	}
	if (engine && strstr(engine, "gl"))
	{
#ifdef HAVE_GL
		backend = ENESIM_BACKEND_OPENGL;
		/* initialize the evas gl thing */
		thiz->gl_evas = evas_gl_new(thiz->evas);
		thiz->gl_api = evas_gl_api_get(thiz->gl_evas);
		/* create the gl context */
		thiz->gl_cfg = evas_gl_config_new();
		thiz->gl_cfg->color_format = EVAS_GL_RGBA_8888;
		thiz->gl_cfg->depth_bits   = EVAS_GL_DEPTH_NONE;        // Othe config options
		thiz->gl_cfg->stencil_bits = EVAS_GL_STENCIL_NONE;
		thiz->gl_cfg->options_bits = EVAS_GL_OPTIONS_NONE;
		thiz->gl_cfg->multisample_bits = EVAS_GL_MULTISAMPLE_NONE;
		thiz->gl_ctx = evas_gl_context_create(thiz->gl_evas, NULL);
		thiz->gl_pool = enesim_pool_opengl_new();
#endif
	}
	else
	{
		backend = ENESIM_BACKEND_SOFTWARE;
	}

	thiz->backend = backend;
	evas_render_method_list_free(engines);

	/* create the document */
	thiz->doc = egueb_svg_document_new(NULL);
	/* set the different application callbacks */
	egueb_svg_document_filename_get_cb_set(thiz->doc, _efl_svg_mart_filename_get, thiz);
}

static void _efl_svg_smart_del(Evas_Object *obj)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);
	egueb_dom_node_unref(thiz->doc);
	/* the idler */
	ecore_idle_enterer_del(thiz->idler);
	/* the animator */
	ecore_timer_del(thiz->animator);
	/* TODO the gl_surface */
	/* TODO the gl_pool */
	/* TODO the damage_objects and rectangles */
	evas_object_del(thiz->bkg);
	evas_object_del(thiz->img);
	evas_object_del(thiz->img_clip);
	if (thiz->file)
		free(thiz->file);
	free(thiz);
}

static void _efl_svg_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);
	if ((thiz->x == x) && (thiz->y == y)) return;

	thiz->x = x;
	thiz->y = y;
	_efl_svg_smart_reconfigure(thiz);
}

static void _efl_svg_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);
	if ((thiz->w == w) && (thiz->h == h)) return;

	thiz->w = w;
	thiz->h = h;
	_efl_svg_smart_reconfigure(thiz);
}

static void _efl_svg_smart_show(Evas_Object *obj)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);

  	evas_object_show(thiz->bkg);
  	evas_object_show(thiz->img);
  	evas_object_show(thiz->img_clip);
}

static void _efl_svg_smart_hide(Evas_Object *obj)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);

  	evas_object_hide(thiz->bkg);
  	evas_object_hide(thiz->img);
  	evas_object_hide(thiz->img_clip);
}

static void _efl_svg_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(obj);
	evas_object_color_set(thiz->bkg, r, g, b, a);
}

static void _efl_svg_smart_clip_set(Evas_Object *obj EINA_UNUSED,
		Evas_Object * clip EINA_UNUSED)
{
#if 0
   E_Smart_Data *sd;

   if (!(sd = evas_object_smart_data_get(obj))) return;
   evas_object_clip_set(sd->obj, clip);
   evas_object_clip_set(sd->eventarea, clip);
#endif
}

static void _efl_svg_smart_clip_unset(Evas_Object *obj EINA_UNUSED)
{
#if 0
   E_Smart_Data *sd;

   if (!(sd = evas_object_smart_data_get(obj))) return;
   evas_object_clip_unset(sd->obj);
   evas_object_clip_unset(sd->eventarea);
#endif
}


static void _efl_svg_smart_init(void)
{
	if (_smart) return;
	{

		static Evas_Smart_Class sc = EVAS_SMART_CLASS_INIT_NAME_VERSION("_efl_svg");
		if (!sc.add)
		{
			sc.add = _efl_svg_smart_add;
			sc.del = _efl_svg_smart_del;
			sc.move = _efl_svg_smart_move;
			sc.resize = _efl_svg_smart_resize;
			sc.show = _efl_svg_smart_show;
			sc.hide = _efl_svg_smart_hide;
			sc.color_set = _efl_svg_smart_color_set;
			sc.clip_set = _efl_svg_smart_clip_set;
			sc.clip_unset = _efl_svg_smart_clip_unset;
			sc.calculate = _efl_svg_smart_calculate;
		}
		_smart = evas_smart_class_new(&sc);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI int efl_svg_init(void)
{
	if (++_init != 1)
		return _init;
	egueb_svg_init();
	_log = eina_log_domain_register("efl_svg", EINA_COLOR_BLUE);
	return _init;
}

EAPI int efl_svg_shutdown(void)
{
	if (--_init != 0)
		return _init;
	eina_log_domain_unregister(_log);
	egueb_svg_shutdown();
	return _init;
}

EAPI Evas_Object * efl_svg_new(Evas *e)
{
	/* initialize our own smart class */
	_efl_svg_smart_init();
	return evas_object_smart_add(e, _smart);
}

EAPI Egueb_Dom_Node * efl_svg_document_get(Evas_Object *o)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(o);
	return egueb_dom_node_ref(thiz->doc);	
}

EAPI void efl_svg_file_set(Evas_Object *o, const char *file)
{
	Efl_Svg_Smart *thiz;
	Enesim_Stream *im;
	char base_dir[PATH_MAX];
	char *tmp;

	thiz = evas_object_smart_data_get(o);
	if (thiz->file)
	{
		free(thiz->file);
		thiz->file = NULL;
	}
	if (thiz->base_dir)
	{
		free(thiz->base_dir);
		thiz->base_dir = NULL;
	}

	if (!file) return;

	im = enesim_stream_file_new(file, "r+");
	if (!im)
	{
		return;
	}
	egueb_dom_parser_parse(im, thiz->doc);
	enesim_stream_free(im);
	thiz->file = strdup(file);
	strncpy(base_dir, thiz->file, PATH_MAX);
	tmp = dirname(base_dir);
	if (tmp)
	{
		if (!strcmp(tmp, "."))
			tmp = "./";
		thiz->base_dir = strdup(tmp);
	}
	evas_object_smart_changed(o);
}

EAPI const char * efl_svg_file_get(Evas_Object *o)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(o);
	return thiz->file;
}

EAPI void efl_svg_debug_damage_set(Evas_Object *o, Eina_Bool debug)
{
	Efl_Svg_Smart *thiz;

	thiz = evas_object_smart_data_get(o);
	thiz->debug_damage = debug;
}

EAPI void efl_svg_fps_set(Evas_Object *o, int fps)
{
	Efl_Svg_Smart *thiz;
	Egueb_Dom_Node *svg = NULL;

	if (fps < 0) return;
	thiz = evas_object_smart_data_get(o);
	/* remove the animtor and add another one with the correct fps */
	thiz->fps = fps;
	ecore_timer_interval_set(thiz->animator, 1.0/thiz->fps);
	egueb_dom_document_element_get(thiz->doc, &svg);
	if (svg)
	{
		egueb_svg_element_svg_animations_fps_set(svg, thiz->fps);
		egueb_dom_node_unref(svg);
	}
}

EAPI void efl_svg_zoom_and_pan_enable(Evas_Object *o, Eina_Bool enable)
{
	Efl_Svg_Smart *thiz;
	thiz = evas_object_smart_data_get(o);
	thiz->zoom_and_pan = enable;
}
