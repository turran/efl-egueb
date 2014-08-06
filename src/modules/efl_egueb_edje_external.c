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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Edje.h>
#include "Efl_Egueb.h"

typedef struct _Efl_Egueb_Edje_Params Efl_Egueb_Edje_Params;

struct _Efl_Egueb_Edje_Params
{
	const char *file;
};

/*----------------------------------------------------------------------------*
 *                     Edje external object interface                         *
 *----------------------------------------------------------------------------*/
static Evas_Object *
_efl_egueb_edje_add(void *data EINA_UNUSED, Evas *evas, Evas_Object *edje EINA_UNUSED, const Eina_List *params EINA_UNUSED, const char *part_name EINA_UNUSED)
{
	return efl_egueb_smart_new(evas);
}

static void
_efl_egueb_edje_state_set(void *data EINA_UNUSED, Evas_Object *obj, const void *from_params, const void *to_params, float pos EINA_UNUSED)
{
	const Efl_Egueb_Edje_Params *p;

	if (to_params) p = to_params;
	else if (from_params) p = from_params;
	else return;

	if (p->file)
	{
		Enesim_Stream *s;

		s = enesim_stream_file_new(p->file, "r");
		efl_egueb_smart_stream_set(obj, s);
	}
}

static Eina_Bool
_efl_egueb_edje_param_set(void *data EINA_UNUSED, Evas_Object *obj, const Edje_External_Param *param)
{
	if (param && param->name && !strcmp(param->name, "file"))
	{
		if (param->type == EDJE_EXTERNAL_PARAM_TYPE_STRING)
		{
			Enesim_Stream *s;

			s = enesim_stream_file_new(param->s, "r");
			efl_egueb_smart_stream_set(obj, s);
			return EINA_TRUE;
		}

	}
	return EINA_FALSE;
}

static Eina_Bool
_efl_egueb_edje_param_get(void *data EINA_UNUSED, const Evas_Object *obj, Edje_External_Param *param)
{
	return EINA_FALSE;
}

static void *
_efl_egueb_edje_params_parse(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, const Eina_List *params)
{
	const Edje_External_Param *param;
	const Eina_List *l;
	Efl_Egueb_Edje_Params *p;

	p = calloc(1, sizeof(Efl_Egueb_Edje_Params));
	if (!p) return NULL;

	EINA_LIST_FOREACH(params, l, param)
	{
		if (param->name && !strcmp(param->name, "file"))
        		p->file = eina_stringshare_add(param->s);
	}
	return p;
}

static void
_efl_egueb_edje_params_free(void *params)
{
	Efl_Egueb_Edje_Params *p = params;

	eina_stringshare_del(p->file);
	free(p);
}

static const char *
_efl_egueb_edje_label_get(void *data EINA_UNUSED)
{
	return "egueb";
}

static Edje_External_Param_Info _efl_egueb_edje_params[] = {
	EDJE_EXTERNAL_PARAM_INFO_STRING("file"),
	EDJE_EXTERNAL_PARAM_INFO_SENTINEL
};

static const Edje_External_Type _efl_egueb_edje_type = {
	.abi_version = EDJE_EXTERNAL_TYPE_ABI_VERSION,
	.module = "egueb",
	.module_name = "egueb",
	.add = _efl_egueb_edje_add,
	.state_set = _efl_egueb_edje_state_set,
	.signal_emit = NULL,
	.param_set = _efl_egueb_edje_param_set,
	.param_get = _efl_egueb_edje_param_get,
	.params_parse = _efl_egueb_edje_params_parse,
	.params_free = _efl_egueb_edje_params_free,
	.label_get = _efl_egueb_edje_label_get,
	.description_get = NULL,
	.icon_add = NULL,
	.preview_add = NULL,
	.translate = NULL,
	.parameters_info = _efl_egueb_edje_params,
	.data = NULL
};

static Edje_External_Type_Info _efl_egueb_edje_types[] =
{
	{"egueb", &_efl_egueb_edje_type},
	{NULL, NULL}
};

static Eina_Bool
efl_egueb_edje_init(void)
{
	if (!efl_egueb_init()) return EINA_FALSE;
	edje_external_type_array_register(_efl_egueb_edje_types);
	return EINA_TRUE;
}

static void
efl_egueb_edje_shutdown(void)
{
	edje_external_type_array_unregister(_efl_egueb_edje_types);
	efl_egueb_shutdown();
}

EINA_MODULE_INIT(efl_egueb_edje_init);
EINA_MODULE_SHUTDOWN(efl_egueb_edje_shutdown);
