#include <stdio.h>
#include <getopt.h>

#include "Efl_Egueb.h"

typedef struct _Efl_Egueb_Window_Viewer
{
	const char *file;
	int width;
	int height;
} Efl_Egueb_Window_Viewer;

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
	Efl_Egueb_Window_Viewer thiz;
	Efl_Egueb_Window *w;
	Egueb_Dom_Node *doc = NULL;
	Enesim_Stream *s;
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
	width = -1;
	height = -1;
	fps = 30;

	if (!efl_egueb_init())
		goto efl_egueb_failed;

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
	s = enesim_stream_file_new(filename, "rb");
	if (!s)
	{
		printf("Failed to read file %s\n", filename);
		help(argv[0]);
		return 0;
	}
	egueb_dom_parser_parse(s, &doc);
	enesim_stream_unref(s);

	thiz.file = filename;
	thiz.width = width;
	thiz.height = height;

	w = efl_egueb_window_auto_new(doc, 0, 0, width, height);
	if (!w)
		goto shutdown_egueb_svg;

	ecore_main_loop_begin();

	efl_egueb_window_free(w);
	efl_egueb_shutdown();

	return 0;

shutdown_egueb_svg:
	efl_egueb_shutdown();
efl_egueb_failed:
	return -1;
}

