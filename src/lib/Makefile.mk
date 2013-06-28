
lib_LTLIBRARIES = src/lib/libefl_svg.la

install_efl_svg_headersdir = $(pkgincludedir)-@VMAJ@
dist_install_efl_svg_headers_DATA = src/lib/Efl_Svg_Smart.h

src_lib_libefl_svg_la_CPPFLAGS = \
-DEFL_SVG_BUILD \
@EFL_SVG_CFLAGS@

src_lib_libefl_svg_la_SOURCES = src/lib/efl_svg_smart.c

src_lib_libefl_svg_la_LIBADD = @EFL_SVG_LIBS@

src_lib_libefl_svg_la_LDFLAGS = -no-undefined -version-info @version_info@
