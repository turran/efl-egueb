
MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = libefl_svg.la

libefl_svg_la_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@EFL_SVG_CFLAGS@ \
@EFL_SVG_BUILD@

libefl_svg_la_SOURCES = efl_svg_smart.c
libefl_svg_la_LIBADD = @EFL_SVG_LIBS@
libefl_svg_la_LDFLAGS = -no-undefined @lt_enable_auto_import@ -version-info @version_info@
