
lib_LTLIBRARIES = src/lib/libefl_egueb.la

install_efl_egueb_headersdir = $(pkgincludedir)-@VMAJ@
dist_install_efl_egueb_headers_DATA = \
src/lib/Efl_Egueb.h \
src/lib/efl_egueb_build.h \
src/lib/efl_egueb_main.h \
src/lib/efl_egueb_smart.h \
src/lib/efl_egueb_window.h

src_lib_libefl_egueb_la_CPPFLAGS = \
-DEFL_EGUEB_BUILD \
@EFL_EGUEB_CFLAGS@

src_lib_libefl_egueb_la_SOURCES = \
src/lib/efl_egueb_document.c \
src/lib/efl_egueb_io_request.c \
src/lib/efl_egueb_io_request_private.h \
src/lib/efl_egueb_main.c \
src/lib/efl_egueb_smart.c \
src/lib/efl_egueb_smart_svg.c \
src/lib/efl_egueb_window.c

src_lib_libefl_egueb_la_LIBADD = @EFL_EGUEB_LIBS@
src_lib_libefl_egueb_la_LDFLAGS = -no-undefined -version-info @version_info@

if BUILD_ECORE_X
src_lib_libefl_egueb_la_CPPFLAGS += @EFL_EGUEB_X_CFLAGS@
src_lib_libefl_egueb_la_SOURCES += src/lib/efl_egueb_window_x.c
src_lib_libefl_egueb_la_LIBADD += @EFL_EGUEB_X_LIBS@
dist_install_efl_egueb_headers_DATA += \
src/lib/Efl_Egueb_X.h \
src/lib/efl_egueb_window_x.h
endif

if BUILD_ECORE_WIN32
src_lib_libefl_egueb_la_CPPFLAGS += @EFL_EGUEB_WIN32_CFLAGS@
src_lib_libefl_egueb_la_SOURCES += src/lib/efl_egueb_window_win32.c
src_lib_libefl_egueb_la_LIBADD += @EFL_EGUEB_WIN32_LIBS@ -lgdi32
dist_install_efl_egueb_headers_DATA += \
src/lib/Efl_Egueb_Win32.h \
src/lib/efl_egueb_window_win32.h
endif

if BUILD_EGUEB_SCRIPT
src_lib_libefl_egueb_la_CPPFLAGS += @EGUEB_SCRIPT_CFLAGS@
src_lib_libefl_egueb_la_LIBADD += @EGUEB_SCRIPT_LIBS@
endif

if BUILD_EGUEB_VIDEO
src_lib_libefl_egueb_la_CPPFLAGS += @EGUEB_VIDEO_CFLAGS@
src_lib_libefl_egueb_la_LIBADD += @EGUEB_VIDEO_LIBS@
endif

if BUILD_EGUEB_SMIL
src_lib_libefl_egueb_la_CPPFLAGS += @EGUEB_SMIL_CFLAGS@
src_lib_libefl_egueb_la_LIBADD += @EGUEB_SMIL_LIBS@
endif

if BUILD_EGUEB_SVG
src_lib_libefl_egueb_la_CPPFLAGS += @EGUEB_SVG_CFLAGS@
src_lib_libefl_egueb_la_LIBADD += @EGUEB_SVG_LIBS@
endif
