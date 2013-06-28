
bin_PROGRAMS = src/bin/efl_svg_viewer

src_bin_efl_svg_viewer_CPPFLAGS = \
-I$(top_srcdir)/src/lib \
@EFL_SVG_BIN_CFLAGS@

src_bin_efl_svg_viewer_SOURCES = src/bin/efl_svg_viewer.c

src_bin_efl_svg_viewer_LDADD = \
$(top_builddir)/src/lib/libefl_svg.la \
@EFL_SVG_BIN_LIBS@
