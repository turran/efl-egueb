MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDJE_EXTERNAL
pkgdir = @EDJE_MODULEDIR@/svg/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la

module_la_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@EFL_SVG_EDJE_EXTERNAL_CFLAGS@ \
@EFL_SVG_CFLAGS@ \
@EFL_SVG_BUILD@

module_la_SOURCES = \
efl_svg_edje_external.c

module_la_LIBADD = \
$(top_builddir)/src/lib/libefl_svg.la \
@EFL_SVG_LIBS@ \
@EFL_SVG_EDJE_EXTERNAL_LIBS@

module_la_LDFLAGS = -no-undefined -module -avoid-version
endif
