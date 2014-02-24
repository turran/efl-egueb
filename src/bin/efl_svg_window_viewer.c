#include <stdio.h>
#include <getopt.h>

#include "Efl_Svg.h"

typedef struct _Efl_Svg_Window_Viewer
{
	const char *file;
	int width;
	int height;
} Efl_Svg_Window_Viewer;

static void help(const char *name)
{
	printf("Usage: %s [OPTIONS] FILE\n", name);
	printf("Where OPTIONS can be one of the following:\n");
	printf("-h Print this screen\n");
	printf("-w The width of the window\n");
	printf("-e The height of the window\n");
	printf("And FILE can be a SVG file or a directory\n");
}


int main(int argc, char *argv[])
{
	Efl_Svg_Window_Viewer thiz;
	Efl_Svg_Window *w;
	char *short_options = "hw:e:";
	struct option long_options[] = {
		{ "help", 1, 0, 'h' },
		{ "width", 1, 0, 'w' },
		{ "height", 1, 0, 'e' },
	};
	int option;
	int ret;
	const char *filename;
	char *engine;
	int width;
	int height;
	int fps;
	struct stat st;

	/* default options */
	width = 640;
	height = 480;
	fps = 30;

	if (!efl_svg_init())
		goto efl_svg_failed;

	while ((ret = getopt_long(argc, argv, short_options, long_options,
			&option)) != -1)
	{
		switch (ret)
		{
			case 'h':
			help(argv[0]);
			return 0;

			case 'w':
			width = atoi(optarg);
			break;

			case 'e':
			height = atoi(optarg);
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
		printf("No SVG files found\n");
		help(argv[0]);
		return 0;
	}

	thiz.file = filename;
	thiz.width = width;
	thiz.height = height;

	w = efl_svg_window_auto_new(0, 0, width, height);
	if (!w)
		goto shutdown_egueb_svg;

	/* create the main svg object */
	ecore_main_loop_begin();
	efl_svg_shutdown();

	return 0;

shutdown_egueb_svg:
	efl_svg_shutdown();
efl_svg_failed:
	return -1;
}

