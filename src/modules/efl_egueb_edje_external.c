#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Edje.h>
#include "Efl_Egueb.h"

typedef struct _Edje_Svg_Params Edje_Svg_Params;

struct _Edje_Svg_Params
{
	const char *file;
};

static Evas_Object *
_edje_egueb_add(void *data EINA_UNUSED, Evas *evas, Evas_Object *edje EINA_UNUSED, const Eina_List *params EINA_UNUSED, const char *part_name EINA_UNUSED)
{
	return efl_egueb_smart_new(evas);
}

static void
_edje_egueb_state_set(void *data EINA_UNUSED, Evas_Object *obj, const void *from_params, const void *to_params, float pos EINA_UNUSED)
{
	const Edje_Svg_Params *p;

	if (to_params) p = to_params;
	else if (from_params) p = from_params;
	else return;

	if (p->file) efl_egueb_smart_file_set(obj, p->file);
}

static Eina_Bool
_edje_egueb_param_set(void *data EINA_UNUSED, Evas_Object *obj, const Edje_External_Param *param)
{
	if (param && param->name && !strcmp(param->name, "file"))
	{
		if (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING)
		{
			efl_egueb_smart_file_set(obj, param->s);
			return EINA_TRUE;
		}

	}
	return EINA_FALSE;
}

static Eina_Bool
_edje_egueb_param_get(void *data EINA_UNUSED, const Evas_Object *obj, Edje_External_Param *param)
{
	if (param && param->name && !strcmp(param->name, "file"))
	{
		if (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING)
		{
			param->s = efl_egueb_smart_file_get((Evas_Object *)obj);
			return EINA_TRUE;
		}
	}
	return EINA_FALSE;
}

static void *
_edje_egueb_params_parse(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, const Eina_List *params)
{
	const Edje_External_Param *param;
	const Eina_List *l;
	Edje_Svg_Params *p;

	p = calloc(1, sizeof(Edje_Svg_Params));
	if (!p) return NULL;

	EINA_LIST_FOREACH(params, l, param)
	{
		if (param->name && !strcmp(param->name, "file"))
        		p->file = eina_stringshare_add(param->s);
	}
	return p;
}

static void
_edje_egueb_params_free(void *params)
{
	Edje_Svg_Params *p = params;

	eina_stringshare_del(p->file);
	free(p);
}

static const char *
_edje_egueb_label_get(void *data EINA_UNUSED)
{
	return "egueb";
}

static Edje_External_Param_Info _edje_egueb_params[] = {
	EDJE_EXTERNAL_PARAM_INFO_STRING("file"),
	EDJE_EXTERNAL_PARAM_INFO_SENTINEL
};

static const Edje_External_Type _edje_egueb_type = {
	.abi_version = EDJE_EXTERNAL_TYPE_ABI_VERSION,
	.module = "egueb",
	.module_name = "egueb",
	.add = _edje_egueb_add,
	.state_set = _edje_egueb_state_set,
	.signal_emit = NULL,
	.param_set = _edje_egueb_param_set,
	.param_get = _edje_egueb_param_get,
	.params_parse = _edje_egueb_params_parse,
	.params_free = _edje_egueb_params_free,
	.label_get = _edje_egueb_label_get,
	.description_get = NULL,
	.icon_add = NULL,
	.preview_add = NULL,
	.translate = NULL,
	.parameters_info = _edje_egueb_params,
	.data = NULL
};

static Edje_External_Type_Info _edje_egueb_types[] =
{
	{"egueb", &_edje_egueb_type},
	{NULL, NULL}
};

static Eina_Bool
edje_egueb_init(void)
{
	if (!efl_egueb_init()) return EINA_FALSE;
	edje_external_type_array_register(_edje_egueb_types);
	return EINA_TRUE;
}

static void
edje_egueb_shutdown(void)
{
	edje_external_type_array_unregister(_edje_egueb_types);
	efl_egueb_shutdown();
}

EINA_MODULE_INIT(edje_egueb_init);
EINA_MODULE_SHUTDOWN(edje_egueb_shutdown);
