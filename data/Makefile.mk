
if BUILD_EDJE_EXTERNAL

EDJE_CC = edje_cc
EDJE_FLAGS_VERBOSE_ =
EDJE_FLAGS_VERBOSE_0 =
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V))

edjedir = $(pkgdatadir)
edje_DATA = data/efl_svg.edj

filesdir = $(pkgdatadir)
files_DATA = \
data/efl_svg.edc \
data/example01.svg

edj_process = \
$(EDJE_CC) $(EDJE_FLAGS) \
data/efl_svg.edc \
data/efl_svg.edj

edj_verbose = $(edj_verbose_@AM_V@)
edj_verbose_ = $(edj_verbose_@AM_DEFAULT_V@)
edj_verbose_0 = @echo "  EDJ     " $@;

data/efl_svg.edj: Makefile $(files_DATA)
	@rm -f $@
	$(edj_verbose)$(edj_process)
endif

sed_edc_process = \
$(SED) \
-e 's,@PACKAGE_DATA_DIR@,$(pkgdatadir),g' \
< $< > $@ || rm $@

edc_verbose = $(edc_verbose_@AM_V@)
edc_verbose_ = $(edc_verbose_@AM_DEFAULT_V@)
edc_verbose_0 = @echo "  EDC     " $@;

data/efl_svg.edc: $(top_srcdir)/data/efl_svg.edc.in Makefile
	@rm -f $@
	$(edc_verbose)$(sed_edc_process)

EXTRA_DIST += $(files_DATA)

clean-local:
	rm -f *.edj *.edc
