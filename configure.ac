
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([efl_svg], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])
VMAJ=v_maj
AC_SUBST([VMAJ])

### Needed information

AC_CANONICAL_HOST

EFL_SVG_BUILD=""
case "$host_os" in
   mingw*)
      EFL_SVG_BUILD="-DEFL_SVG_BUILD"
   ;;
esac
AC_SUBST([EFL_SVG_BUILD])

MODULE_ARCH="$host_os-$host_cpu-1.0.0"
AC_SUBST(MODULE_ARCH)
AC_DEFINE_UNQUOTED(MODULE_ARCH, "$MODULE_ARCH", "Module architecture")

### Additional options to configure


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

# Check whether pkg-config supports Requires.private
if $PKG_CONFIG --atleast-pkgconfig-version 0.22; then
   pkgconfig_requires_private="Requires.private"
else
   pkgconfig_requires_private="Requires"
fi
AC_SUBST([pkgconfig_requires_private])


### Checks for libraries

requirements_efl_svg="egueb-svg >= 0.0.21 ender >= 0.0.6 enesim >= 0.0.15 ecore >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0"
PKG_CHECK_EXISTS([evas-opengl-x11 >= 1.2.0 enesim_opengl >= 0.0.15],
   [
    have_gl="yes"
    requirements_efl_svg="${requirements_efl_svg} enesim_opengl >= 0.0.15"
    AC_DEFINE([HAVE_GL], [1], [Have OpenGL support])
   ],
   [have_gl="no"])
AC_SUBST([requirements_efl_svg])
PKG_CHECK_MODULES([EFL_SVG], [${requirements_efl_svg}])

PKG_CHECK_MODULES([EFL_SVG_BIN], [egueb-svg >= 0.0.21 enesim >= 0.0.15 ecore-evas >= 1.2.0 ecore >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0])

PKG_CHECK_MODULES([EFL_SVG_EDJE_EXTERNAL], [edje >= 1.0.99], [have_edje="yes"], [have_edje="no"])

build_edje_external="no"
if test "x${have_edje}" = "xyes" ; then
  build_edje_external="yes"
  EDJE_MODULEDIR=$(${PKG_CONFIG} --variable=modules edje)
  AC_SUBST([EDJE_MODULEDIR])
fi

AM_CONDITIONAL([BUILD_EDJE_EXTERNAL], [test "x${build_edje_external}" = "xyes"])


### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

EFL_CHECK_COMPILER_FLAGS([EFL_SVG], [-Wall -Wextra -Wshadow])


### Checks for linker characteristics

lt_enable_auto_import=""
case "${host_os}" in
   mingw*)
      lt_enable_auto_import="-Wl,--enable-auto-import"
   ;;
esac
AC_SUBST([lt_enable_auto_import])


### Checks for library functions

AC_CONFIG_FILES([
Makefile
src/Makefile
src/include/Makefile
src/lib/Makefile
src/bin/Makefile
src/modules/Makefile
data/Makefile
efl_svg.pc
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
echo "  OpenGL support............................: ${have_gl}"
echo "  Edje external support.....................: ${build_edje_external}"
echo
echo "Compilation.................................: make (or gmake)"
echo "  CPPFLAGS..................................: $CPPFLAGS"
echo "  CFLAGS....................................: $CFLAGS"
echo "  LDFLAGS...................................: $LDFLAGS"
echo
echo "Installation................................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix....................................: $prefix"
echo
