
bin_PROGRAMS = \
src/bin/efl_svg_smart_viewer \
src/bin/efl_svg_window_viewer

src_bin_efl_svg_smart_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @EFL_SVG_BIN_CFLAGS@
src_bin_efl_svg_smart_viewer_SOURCES = src/bin/efl_svg_smart_viewer.c
src_bin_efl_svg_smart_viewer_LDADD = $(top_builddir)/src/lib/libefl_svg.la @EFL_SVG_BIN_LIBS@

src_bin_efl_svg_window_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @EFL_SVG_BIN_CFLAGS@
src_bin_efl_svg_window_viewer_SOURCES = src/bin/efl_svg_window_viewer.c
src_bin_efl_svg_window_viewer_LDADD = $(top_builddir)/src/lib/libefl_svg.la @EFL_SVG_BIN_LIBS@
