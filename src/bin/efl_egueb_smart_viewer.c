#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <getopt.h>
#include <unistd.h>

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#include "Efl_Egueb.h"

typedef struct _Efl_Svg_Viewer
{
	Eina_List *files;
	Eina_List *current;
	char *location;
	Evas_Object *svg;
	int width;
	int height;
} Efl_Svg_Viewer;

typedef struct _Efl_Svg_Smart_Dir_Data
{
	Eina_List *files;
	const char *path;
} Efl_Svg_Smart_Dir_Data;

static double
_time_get(void)
{
  struct timeval timev;

  gettimeofday(&timev, NULL);
  return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

static void help(const char *name)
{
	printf("Usage: %s [OPTIONS] FILE\n", name);
	printf("Where OPTIONS can be one of the following:\n");
	printf("-h Print this screen\n");
	printf("-d Draw the damage retangles\n");
	printf("-w The width of the window\n");
	printf("-e The height of the window\n");
	printf("-n The evas engine to use (list to list them all)\n");
	printf("And FILE can be a SVG file or a directory\n");
}

static void _cb_delete(Ecore_Evas *ee)
{
	ecore_main_loop_quit();
}

static void _cb_resize(Ecore_Evas *ee)
{
	Evas *evas;
	Evas_Object *o;
	int width;
	int height;

	ecore_evas_geometry_get(ee, NULL, NULL, &width, &height);
	evas = ecore_evas_get(ee);
	/* resize the svg object */
	o = evas_object_name_find(evas, "svg");
	evas_object_resize(o, width, height);
	/* TODO put the controls on the same position */
}

static void _cb_dir(const char *name, const char *path, void *user_data)
{
	Efl_Svg_Smart_Dir_Data *data = user_data;
	Enesim_Stream *enesim_stream;
	char file[PATH_MAX];
	const char *mime;

	snprintf(file, PATH_MAX, "%s/%s", path, name);
	/* check if the file is a svg */
	enesim_stream = enesim_stream_file_new(file, "rb");
	mime = enesim_image_mime_data_from(enesim_stream);
	enesim_stream_unref(enesim_stream);
	if (!mime) return;
	if (strcmp(mime, "image/svg+xml"))
		return;

	data->files = eina_list_append(data->files, strdup(file));
}

int main(int argc, char *argv[])
{
	Efl_Svg_Viewer thiz;
	Enesim_Stream *s;
	Ecore_Evas *ee;
	Evas *evas;
	Evas_Object *o;
	Eina_Bool damages;
	Eina_Bool free_file = EINA_FALSE;
	char *short_options = "dhw:e:n:f:";
	struct option long_options[] = {
		{ "help", 1, 0, 'h' },
		{ "damages", 0, 0, 'd' },
		{ "fps", 1, 0, 'f' },
		{ "width", 1, 0, 'w' },
		{ "height", 1, 0, 'e' },
		{ "engine", 1, 0, 'n' },
	};
	int option;
	int ret;
	char *filename;
	char *engine;
	int width;
	int height;
	int fps;
	struct stat st;

	/* default options */
	width = 640;
	height = 480;
	fps = 30;
	damages = EINA_FALSE;
#ifdef _WIN32
	engine = "software_gdi";
#else
	engine = "software_x11";
#endif

	if (!ecore_evas_init())
		return -1;

	if (!efl_egueb_init())
		goto shutdown_ecore_evas;

	while ((ret = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (ret)
		{
			case 'h':
			help(argv[0]);
			return 0;

			case 'd':
			damages = EINA_TRUE;
			break;

			case 'f':
			fps = atoi(optarg);
			break;

			case 'w':
			width = atoi(optarg);
			break;

			case 'e':
			height = atoi(optarg);
			break;

			case 'n':
			if (!strcmp(optarg, "list"))
			{
				Eina_List *engines;
				Eina_List *l;
				const char *e;

				engines = ecore_evas_engines_get();
				EINA_LIST_FOREACH (engines, l, e)
				{
					printf("%s\n", e);
				}
				return 0;
			}
			else
				engine = optarg;
			break;

			default:
			break;
		}
	}

	if (argc - 1 != optind)
	{
		help(argv[0]);
		return 0;
	}

	filename = argv[optind];
	if (stat(filename, &st) < 0)
	{
		help(argv[0]);
		goto done;
	}
	/* sanitize the filename */
	if (*filename != '.' && *filename != '/')
	{
		char wd[PATH_MAX];

		if (!getcwd(wd, PATH_MAX))
			goto done;
		if (asprintf(&filename, "%s/%s", wd, filename) < 0)
			goto done;
		free_file = EINA_TRUE;
	}

	s = enesim_stream_file_new(filename, "r");
	if (!s)
	{
		printf("Failed to read file %s\n", filename);
		help(argv[0]);
		goto read_error;
	}

	thiz.width = width;
	thiz.height = height;

	ee = ecore_evas_new(engine, 0, 0, 0, 0, NULL);
	if (!ee)
		goto shutdown_esvg;

	evas = ecore_evas_get(ee);
	if (!evas)
		goto free_ecore_evas;
	ecore_evas_callback_delete_request_set(ee, _cb_delete);
	ecore_evas_callback_resize_set(ee, _cb_resize);

	/* create the main svg object */
	o = efl_egueb_smart_new(evas);
	efl_egueb_smart_stream_set(o, s);
	efl_egueb_smart_debug_damage_set(o, damages);
	efl_egueb_smart_fps_set(o, fps);
	/* enable the svg extension */
	efl_egueb_smart_svg_setup(o);

	evas_object_move(o, 0, 0);
	evas_object_resize(o, width, height);
	evas_object_show(o);
	evas_object_name_set(o, "svg");
	/* make the object the focused one */
	evas_object_focus_set(o, EINA_TRUE);
	thiz.svg = o;

	ecore_evas_resize(ee, width, height);
	ecore_evas_show(ee);

	ecore_main_loop_begin();

	ecore_evas_shutdown();
	efl_egueb_shutdown();

	return 0;

free_ecore_evas:
	ecore_evas_free(ee);
shutdown_esvg:
	efl_egueb_shutdown();
shutdown_ecore_evas:
	ecore_evas_shutdown();
read_error:
	if (free_file)
		free(filename);
done:
	return -1;
}
