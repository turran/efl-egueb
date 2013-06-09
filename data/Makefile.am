MAINTAINERCLEANFILES = Makefile.in

if BUILD_EDJE_EXTERNAL

EDJE_CC = edje_cc
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V))

edjedir = $(pkgdatadir)
edje_DATA = efl_svg.edj

filesdir = $(pkgdatadir)
files_DATA = \
efl_svg.edc \
example01.svg

efl_svg.edj: Makefile $(files_DATA)
	$(EDJE_CC) $(EDJE_FLAGS) \
	$(top_srcdir)/data/efl_svg.edc \
	$(top_builddir)/data/efl_svg.edj
endif

efl_svg.edc:
	@echo "  SED    efl_svg.edc"
	@rm -f efl_svg.edc
	@sed "s#@PACKAGE_DATA_DIR@#$(pkgdatadir)#" < efl_svg.edc.in > efl_svg.edc

EXTRA_DIST = $(files_DATA)

clean-local:
	rm -f *.edj

distclean-local:
	rm -f *.edc
