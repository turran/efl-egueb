
MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = \
-I$(top_srcdir)/src/include \
-I$(top_builddir)/src/include \
@EFL_SVG_BIN_CFLAGS@

bin_PROGRAMS = efl_svg_viewer

efl_svg_viewer_SOURCES = efl_svg_viewer.c
efl_svg_viewer_LDADD = $(top_builddir)/src/lib/libefl_svg.la @EFL_SVG_BIN_LIBS@
efl_svg_viewer_LDFLAGS = @lt_enable_auto_import@
