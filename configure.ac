
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

requirements_pc="egueb-dom >= 0.0.23 enesim >= 0.0.15 ecore-input >= 1.2.0 ecore >= 1.2.0 ecore-con >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0"
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
  EDJE_MODULEARCH=$(${PKG_CONFIG} --variable=module_arch edje)
  AC_SUBST([EDJE_MODULEDIR])
  AC_SUBST([EDJE_MODULEARCH])
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

### Ecore Win32 window
AC_ARG_ENABLE([ecore-win32], [AS_HELP_STRING([--enable-ecore-win32], [Enable Ecore Win32 window])],
  [
    if test "x$enableval" = "xyes" ; then
       want_ecore_win32="yes"
    else
       want_ecore_win32="no"
    fi
  ],
  [want_ecore_win32="yes"]
)
PKG_CHECK_MODULES([EFL_EGUEB_WIN32], [ecore-win32], [have_ecore_win32="yes"], [have_ecore_win32="no"])
build_ecore_win32="no"

if test "x${have_ecore_win32}" = "xyes" -a "x${want_ecore_win32}" = "xyes"; then
  build_ecore_win32="yes"
  requirements_pc="${requirements_pc} ecore-win32 >= 1.7.99"
fi

if test "x${build_ecore_win32}" = "xyes"; then
  AC_DEFINE([BUILD_ECORE_WIN32], [1], [Have support for ecore_win32])
fi
AM_CONDITIONAL([BUILD_ECORE_WIN32], [test "x${build_ecore_win32}" = "xyes"])

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

### SVG based features
AC_ARG_ENABLE([egueb-svg], [AS_HELP_STRING([--enable-egueb-svg], [Enable SVG animations support])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_svg="yes"
    else
       want_egueb_svg="no"
    fi
  ],
  [want_egueb_svg="yes"]
)

PKG_CHECK_MODULES([EGUEB_SVG], [egueb-svg], [have_egueb_svg="yes"], [have_egueb_svg="no"])
build_egueb_svg="no"

if test "x${have_egueb_svg}" = "xyes" -a "x${want_egueb_svg}" = "xyes"; then
  build_egueb_svg="yes"
  requirements_pc="${requirements_pc} egueb-svg"
fi

if test "x${build_egueb_svg}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_SVG], [1], [Have support for egueb_svg])
fi
AM_CONDITIONAL([BUILD_EGUEB_SVG], [test "x${build_egueb_svg}" = "xyes"])

### script support
AC_ARG_ENABLE([egueb-script], [AS_HELP_STRING([--enable-egueb-script], [Script support])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_script="yes"
    else
       want_egueb_script="no"
    fi
  ],
  [want_egueb_script="yes"]
)

PKG_CHECK_MODULES([EGUEB_SCRIPT], [egueb-script], [have_egueb_script="yes"], [have_egueb_script="no"])
build_egueb_script="no"

if test "x${have_egueb_script}" = "xyes" -a "x${want_egueb_script}" = "xyes"; then
  build_egueb_script="yes"
  requirements_pc="${requirements_pc} egueb-script"
fi

if test "x${build_egueb_script}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_SCRIPT], [1], [Have support for egueb_script])
fi
AM_CONDITIONAL([BUILD_EGUEB_SCRIPT], [test "x${build_egueb_script}" = "xyes"])

### GStreamer based video provider
AC_ARG_ENABLE([egueb-video], [AS_HELP_STRING([--enable-egueb-video], [Enable GStreamer based video provider])],
  [
    if test "x$enableval" = "xyes" ; then
       want_egueb_video="yes"
    else
       want_egueb_video="no"
    fi
  ],
  [want_egueb_video="yes"]
)

PKG_CHECK_MODULES([EGUEB_VIDEO], [egueb-video], [have_egueb_video="yes"], [have_egueb_video="no"])
build_egueb_video="no"

if test "x${have_egueb_video}" = "xyes" -a "x${want_egueb_video}" = "xyes"; then
  build_egueb_video="yes"
  requirements_pc="${requirements_pc} egueb-video"
fi

if test "x${build_egueb_video}" = "xyes"; then
  AC_DEFINE([BUILD_EGUEB_VIDEO], [1], [Have support for egueb_video])
fi
AM_CONDITIONAL([BUILD_EGUEB_VIDEO], [test "x${build_egueb_video}" = "xyes"])

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
echo "  SVG support...............................: ${build_egueb_svg}"
echo "  OpenGL support............................: ${have_gl}"
echo "  Edje external support.....................: ${build_edje_external}"
echo "  Window support:"
echo "    Ecore X.................................: ${build_ecore_x}"
echo "    Ecore Win32.............................: ${build_ecore_win32}"
echo "  Scripting support.........................: ${build_egueb_script}"
echo "  Video provider support:...................: ${build_egueb_video}"
echo
echo "Compilation.................................: make (or gmake)"
echo "  CPPFLAGS..................................: $CPPFLAGS"
echo "  CFLAGS....................................: $CFLAGS"
echo "  LDFLAGS...................................: $LDFLAGS"
echo
echo "Installation................................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix....................................: $prefix"
echo
