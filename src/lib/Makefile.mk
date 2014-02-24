
lib_LTLIBRARIES = src/lib/libefl_svg.la

install_efl_svg_headersdir = $(pkgincludedir)-@VMAJ@
dist_install_efl_svg_headers_DATA = \
src/lib/Efl_Svg.h \
src/lib/efl_svg_main.h \
src/lib/efl_svg_smart.h \
src/lib/efl_svg_window.h

src_lib_libefl_svg_la_CPPFLAGS = \
-DEFL_SVG_BUILD \
@EFL_SVG_CFLAGS@

src_lib_libefl_svg_la_SOURCES = \
src/lib/efl_svg_main.c \
src/lib/efl_svg_smart.c \
src/lib/efl_svg_window.c

src_lib_libefl_svg_la_LIBADD = @EFL_SVG_LIBS@
src_lib_libefl_svg_la_LDFLAGS = -no-undefined -version-info @version_info@

if BUILD_ECORE_X
src_lib_libefl_svg_la_CPPFLAGS += @EFL_SVG_X_CFLAGS@
src_lib_libefl_svg_la_SOURCES += src/lib/efl_svg_window_x.c
src_lib_libefl_svg_la_LIBADD += @EFL_SVG_X_LIBS@
endif

