AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
autom4te.cache \
config.guess \
config.sub \
configure \
depcomp \
install-sh \
ltmain.sh \
missing \
m4/ltsugar.m4 \
m4/libtool.m4 \
m4/ltversion.m4 \
m4/lt~obsolete.m4 \
m4/ltoptions.m4

EXTRA_DIST =

### Library

include src/lib/Makefile.mk

### Binary

include src/bin/Makefile.mk

### Module

include src/modules/Makefile.mk

### Data

include data/Makefile.mk

EXTRA_DIST += \
AUTHORS \
COPYING \
README \
autogen.sh\
efl-egueb.pc.in \
m4/efl_compiler.m4

sed_process = \
$(SED) \
-e 's,@VERSION\@,$(VERSION),g' \
-e 's,@VMAJ\@,$(VMAJ),g' \
-e 's,@prefix\@,$(prefix),g' \
-e 's,@exec_prefix\@,$(exec_prefix),g' \
-e 's,@libdir\@,$(libdir),g' \
-e 's,@includedir\@,$(includedir),g' \
-e 's,@pkgincludedir\@,$(pkgincludedir),g' \
-e 's,@requirements_pc\@,$(requirements_pc),g' \
< $< > $@ || rm $@

pc_verbose = $(pc_verbose_@AM_V@)
pc_verbose_ = $(pc_verbose_@AM_DEFAULT_V@)
pc_verbose_0 = @echo "  PC      " $@;

efl-egueb.pc: $(top_srcdir)/efl-egueb.pc.in Makefile
	@rm -f $@
	$(pc_verbose)$(sed_process)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = efl-egueb.pc
