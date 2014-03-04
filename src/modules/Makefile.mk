
if BUILD_EDJE_EXTERNAL

pkgdir = @EDJE_MODULEDIR@/egueb/$(MODULE_ARCH)
pkg_LTLIBRARIES = src/modules/module.la

src_modules_module_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DEFL_EGUEB_BUILD \
@EFL_EGUEB_EDJE_EXTERNAL_CFLAGS@ \
@EFL_EGUEB_CFLAGS@

src_modules_module_la_SOURCES = \
src/modules/efl_egueb_edje_external.c

src_modules_module_la_LIBADD = \
$(top_builddir)/src/lib/libefl_egueb.la \
@EFL_EGUEB_LIBS@ \
@EFL_EGUEB_EDJE_EXTERNAL_LIBS@

src_modules_module_la_LDFLAGS = -no-undefined -module -avoid-version

endif
