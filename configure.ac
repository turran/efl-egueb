
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [3])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([efl_egueb], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign 1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

VMAJ=v_maj
AC_SUBST([VMAJ])

### Needed information

AC_CANONICAL_HOST

MODULE_ARCH="$host_os-$host_cpu-1.0.0"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

### Additional options to configure


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if ! $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   AC_MSG_ERROR([pkg-config >= 0.22 is required])
fi


### Checks for libraries

requirements_pc="egueb-dom >= 0.0.23 enesim >= 0.0.15 ecore >= 1.2.0 ecore-con >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0"
PKG_CHECK_EXISTS([evas-opengl-x11 >= 1.2.0 enesim_opengl >= 0.0.15],
   [
    have_gl="yes"
    requirements_pc="${requirements_pc} enesim_opengl >= 0.0.15"
    AC_DEFINE([HAVE_GL], [1], [Have OpenGL support])
   ],
   [have_gl="no"])
AC_SUBST([requirements_pc])
PKG_CHECK_MODULES([EFL_EGUEB], [${requirements_pc}])

PKG_CHECK_MODULES([EFL_EGUEB_BIN], [egueb-dom >= 0.0.23 enesim >= 0.0.15 ecore-evas >= 1.2.0 ecore >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0])

PKG_CHECK_MODULES([EFL_EGUEB_EDJE_EXTERNAL], [edje >= 1.0.99], [have_edje="yes"], [have_edje="no"])

build_edje_external="no"
if test "x${have_edje}" = "xyes" ; then
  build_edje_external="yes"
  EDJE_MODULEDIR=$(${PKG_CONFIG} --variable=modules edje)
  AC_SUBST([EDJE_MODULEDIR])
fi

AM_CONDITIONAL([BUILD_EDJE_EXTERNAL], [test "x${build_edje_external}" = "xyes"])

### Ecore X window
AC_ARG_ENABLE([ecore-x], [AS_HELP_STRING([--enable-ecore-x], [Enable Ecore X window])],
  [
    if test "x$enableval" = "xyes" ; then
       want_ecore_x="yes"
    else
       want_ecore_x="no"
    fi
  ],
  [want_ecore_x="yes"]
)
PKG_CHECK_MODULES([EFL_EGUEB_X], [ecore-x], [have_ecore_x="yes"], [have_ecore_x="no"])
build_ecore_x="no"

if test "x${have_ecore_x}" = "xyes" -a "x${want_ecore_x}" = "xyes"; then
  build_ecore_x="yes"
  requirements_pc="${requirements_pc} ecore-x >= 1.7.99"
fi

if test "x${build_ecore_x}" = "xyes"; then
  AC_DEFINE([BUILD_ECORE_X], [1], [Have support for ecore_x])
fi
AM_CONDITIONAL([BUILD_ECORE_X], [test "x${build_ecore_x}" = "xyes"])

### SMIL based features
AC_ARG_ENABLE([egueb-smil], [AS_HELP_STRING([--enable-egueb-smil], [Enable SMIL animations support])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_smil="yes"
    else
       want_egueb_smil="no"
    fi
  ],
  [want_egueb_smil="yes"]
)

PKG_CHECK_MODULES([EGUEB_SMIL], [egueb-smil], [have_egueb_smil="yes"], [have_egueb_smil="no"])
build_egueb_smil="no"

if test "x${have_egueb_smil}" = "xyes" -a "x${want_egueb_smil}" = "xyes"; then
  build_egueb_smil="yes"
  requirements_pc="${requirements_pc} egueb-smil"
fi

if test "x${build_egueb_smil}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_SMIL], [1], [Have support for egueb_smil])
fi
AM_CONDITIONAL([BUILD_EGUEB_SMIL], [test "x${build_egueb_smil}" = "xyes"])

### JavaScript (SpiderMonkey based) script support
AC_ARG_ENABLE([egueb-js-sm], [AS_HELP_STRING([--enable-egueb-js-sm], [JavaScript (SpiderMonkey based) scripter])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_js_sm="yes"
    else
       want_egueb_js_sm="no"
    fi
  ],
  [want_egueb_js_sm="yes"]
)

PKG_CHECK_MODULES([EGUEB_JS_SM], [egueb-js-sm], [have_egueb_js_sm="yes"], [have_egueb_js_sm="no"])
build_egueb_js_sm="no"

if test "x${have_egueb_js_sm}" = "xyes" -a "x${want_egueb_js_sm}" = "xyes"; then
  build_egueb_js_sm="yes"
  requirements_pc="${requirements_pc} egueb-js-sm"
fi

if test "x${build_egueb_js_sm}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_JS_SM], [1], [Have support for egueb_js_sm])
fi
AM_CONDITIONAL([BUILD_EGUEB_JS_SM], [test "x${build_egueb_js_sm}" = "xyes"])

### GStreamer based video provider
AC_ARG_ENABLE([gst-egueb], [AS_HELP_STRING([--enable-gst-egueb], [Enable GStreamer based video provider])],
  [
    if test "x$enableval" = "xyes" ; then
       want_gst_egueb="yes"
    else
       want_gst_egueb="no"
    fi
  ],
  [want_gst_egueb="yes"]
)

PKG_CHECK_MODULES([GST_EGUEB], [gst-egueb], [have_gst_egueb="yes"], [have_gst_egueb="no"])
build_gst_egueb="no"

if test "x${have_gst_egueb}" = "xyes" -a "x${want_gst_egueb}" = "xyes"; then
  build_gst_egueb="yes"
  requirements_pc="${requirements_pc} gst-egueb"
fi

if test "x${build_gst_egueb}" = "xyes"; then
  AC_DEFINE([BUILD_GST_EGUEB], [1], [Have support for gst_egueb])
fi
AM_CONDITIONAL([BUILD_GST_EGUEB], [test "x${build_gst_egueb}" = "xyes"])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O

EFL_CHECK_COMPILER_FLAGS([EFL_EGUEB], [-Wall])


### Checks for linker characteristics


### Checks for library functions

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "  SMIL support..............................: ${build_egueb_smil}"
echo "  OpenGL support............................: ${have_gl}"
echo "  Edje external support.....................: ${build_edje_external}"
echo "  Window support:"
echo "    Ecore X.................................: ${build_ecore_x}"
echo "  Scripting support:"
echo "    JavaScript (SpiderMonkey)...............: ${build_egueb_js_sm}"
echo "  Video provider support:"
echo "    GStreamer...............................: ${build_gst_egueb}"
echo
echo "Compilation.................................: make (or gmake)"
echo "  CPPFLAGS..................................: $CPPFLAGS"
echo "  CFLAGS....................................: $CFLAGS"
echo "  LDFLAGS...................................: $LDFLAGS"
echo
echo "Installation................................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix....................................: $prefix"
echo
