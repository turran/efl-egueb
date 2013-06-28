
if BUILD_EDJE_EXTERNAL

pkgdir = @EDJE_MODULEDIR@/svg/$(MODULE_ARCH)
pkg_LTLIBRARIES = src/modules/module.la

src_modules_module_la_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
-DEFL_SVG_BUILD \
@EFL_SVG_EDJE_EXTERNAL_CFLAGS@ \
@EFL_SVG_CFLAGS@

src_modules_module_la_SOURCES = \
src/modules/efl_svg_edje_external.c

src_modules_module_la_LIBADD = \
$(top_builddir)/src/lib/libefl_svg.la \
@EFL_SVG_LIBS@ \
@EFL_SVG_EDJE_EXTERNAL_LIBS@

src_modules_module_la_LDFLAGS = -no-undefined -module -avoid-version

endif
