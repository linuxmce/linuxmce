# This is a non-recursive Makefile, see
# https://github.com/OpenLightingProject/ola/issues/397 for links to useful
# documentation.
#
# Rather than defining everything in a single Makefile we include a Makefile.mk
# from each directory. This gives the build rules a bit more structure.
auxdir                  = @ac_aux_dir@
AUX_DIST                = $(auxdir)/install-sh $(auxdir)/missing \
                          $(auxdir)/mkinstalldirs

# As suggested by libtoolize
ACLOCAL         = aclocal -I $(auxdir)
ACLOCAL_AMFLAGS = -I config

include aminclude.am

# When running distcheck we want to enable the Python modules.
AM_DISTCHECK_CONFIGURE_FLAGS = --enable-python-libs

# C++ FLAGS variables
# ------------------------------------------------------------------------------
# COMMON_CXXFLAGS
# The common C++ flags. This may include -Werror if fatal-warning was enabled
# in ./configure. AM_CXXFLAGS defaults to this, and it should be preferred.
#
# COMMON_CXXFLAGS_ONLY_WARNINGS
# The common C++ flags with (non-fatal) warnings enabled. Use this for 3rd
# party code that does not build correctly with COMMON_CXXFLAGS.
#
# COMMON_TESTING_FLAGS
# The common C++ flags for testing.
COMMON_CXXFLAGS_ONLY_WARNINGS = \
    -I$(top_srcdir)/include \
    -I$(top_builddir)/include \
    -Wall -Wformat -W -fvisibility-inlines-hidden \
    $(libprotobuf_CFLAGS)

COMMON_CXXFLAGS = $(COMMON_CXXFLAGS_ONLY_WARNINGS)

# The genererated protobuf files don't compile with -Werror on win32 so we
# disable fatal warnings on WIN32.
if ! USING_WIN32
if FATAL_WARNINGS
  COMMON_CXXFLAGS += -Werror
endif
endif

# AM_CXXFLAGS is used when target_CXXFLAGS isn't specified.
AM_CXXFLAGS = $(COMMON_CXXFLAGS)

COMMON_TESTING_FLAGS = $(COMMON_CXXFLAGS) $(CPPUNIT_CFLAGS) \
                       -DTEST_SRC_DIR=\"$(srcdir)\" \
                       -DTEST_BUILD_DIR=\"$(builddir)\"

# LIBS variables
# ------------------------------------------------------------------------------
#
# COMMON_TESTING_LIBS
# The set of libraries used in the unittests.
COMMON_TESTING_LIBS = $(CPPUNIT_LIBS) \
                      common/testing/libolatesting.la \
                      common/testing/libtestmain.la \
                      common/libolacommon.la

# Due to MinGW's handling of library archives, we need to append this.
if USING_WIN32
COMMON_TESTING_LIBS += $(CPPUNIT_LIBS)
endif

# Setup pkgconfigdir, the path where .pc files are installed.
pkgconfigdir = $(libdir)/pkgconfig
oladincludedir = $(includedir)/olad

# Define a custom runner for .py tests
TESTS_ENVIRONMENT = PYTHONPATH=${top_srcdir}/python
TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

# Variables used by the included Makefile.mk(s)
# ------------------------------------------------------------------------------

# Common automake variables.
CLEANFILES =
BUILT_SOURCES =
EXTRA_DIST =

bin_PROGRAMS =
check_PROGRAMS =
noinst_PROGRAMS =

pkgconfig_DATA =
noinst_DATA =
dist_noinst_DATA =

lib_LTLIBRARIES =
noinst_LTLIBRARIES =

check_SCRIPTS =
dist_check_SCRIPTS =
dist_noinst_SCRIPTS =

dist_man_MANS =

pkginclude_HEADERS =
noinst_HEADERS =
nodist_pkginclude_HEADERS =

# Append to this to define an install-exec-hook.
INSTALL_EXEC_HOOKS =

# Test programs, these are added to check_PROGRAMS and TESTS if BUILD_TESTS is
# true.
test_programs =

# Files in built_sources are included in BUILT_SOURCES and CLEANFILES
built_sources =

# Test scripts are run if BUILD_TESTS is true.
test_scripts =

# The includes
# -----------------------------------------------------------------------------

# Since `make install` relinks the libraries, this should be in the order in
# which libraries depend on each other. e.g. common, plugins, server.
include common/Makefile.mk
include data/Makefile.mk
include debian/Makefile.mk
include doxygen/Makefile.mk
include examples/Makefile.mk
include include/Makefile.mk
include javascript/Makefile.mk
include man/Makefile.mk
include ola/Makefile.mk
include plugins/Makefile.mk
include olad/Makefile.mk
include protoc/Makefile.mk
include python/Makefile.mk
include tools/Makefile.mk

# -----------------------------------------------------------------------------

# Now some post-processing
BUILT_SOURCES += $(built_sources)
CLEANFILES += $(built_sources)

if BUILD_TESTS
TESTS = $(test_programs) $(test_scripts)
endif
check_PROGRAMS += $(test_programs)

install-exec-hook: $(INSTALL_EXEC_HOOKS)

# -----------------------------------------------------------------------------

# This gives us a rather hacky method to build the files normally built during
# the build, so they are present for Doxygen to run against
builtfiles : Makefile.am $(built_sources)
.PHONY : builtfiles

# I can't figure out how to safely execute a command (mvn) in a subdirectory,
# so this is recursive for now.
SUBDIRS = java

pkgconfig_DATA += libola.pc libolaserver.pc

# dvi is run from make distcheck. Do nothing.
dvi:

EXTRA_DIST += \
  CONTRIBUTING \
  Doxyfile \
  GPL \
  LGPL \
  LICENCE \
  README.debian \
  README.developer \
  README.mingw32 \
  README.rpm \
  config/ac_prog_java_cc.m4 \
  config/ac_python_devel.m4 \
  config/ac_saleae.m4 \
  config/ax_python_module.m4 \
  config/cppunit.m4 \
  config/maven.m4 \
  config/ola.m4 \
  config/ola_version.m4 \
  config/pkg.m4 \
  config/resolv.m4 \
  config/stl_hash.m4 \
  mac_build.sh \
  ola.spec \
  tests/recvdmx.py \
  tests/regression.py
