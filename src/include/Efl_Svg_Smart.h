#ifndef _EFL_SVG_SMART_H_
#define _EFL_SVG_SMART_H_

#include <Eina.h>
#include <Evas.h>
#include <Egueb_Dom.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_SVG_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_SVG_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

EAPI int efl_svg_init(void);
EAPI int efl_svg_shutdown(void);
EAPI Evas_Object * efl_svg_new(Evas *e);
EAPI Egueb_Dom_Node * efl_svg_document_get(Evas_Object *o);
EAPI void efl_svg_file_set(Evas_Object *o, const char *file);
EAPI const char * efl_svg_file_get(Evas_Object *o);
EAPI void efl_svg_debug_damage_set(Evas_Object *o, Eina_Bool debug);
EAPI void efl_svg_fps_set(Evas_Object *o, int fps);

#endif /* _EFL_SVG_SMART_H_ */
