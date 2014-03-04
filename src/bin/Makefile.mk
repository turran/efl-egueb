
bin_PROGRAMS = \
src/bin/efl_egueb_smart_viewer \
src/bin/efl_egueb_window_viewer

src_bin_efl_egueb_smart_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @EFL_EGUEB_BIN_CFLAGS@
src_bin_efl_egueb_smart_viewer_SOURCES = src/bin/efl_egueb_smart_viewer.c
src_bin_efl_egueb_smart_viewer_LDADD = $(top_builddir)/src/lib/libefl_egueb.la @EFL_EGUEB_BIN_LIBS@

src_bin_efl_egueb_window_viewer_CPPFLAGS = -I$(top_srcdir)/src/lib @EFL_EGUEB_BIN_CFLAGS@
src_bin_efl_egueb_window_viewer_SOURCES = src/bin/efl_egueb_window_viewer.c
src_bin_efl_egueb_window_viewer_LDADD = $(top_builddir)/src/lib/libefl_egueb.la @EFL_EGUEB_BIN_LIBS@
