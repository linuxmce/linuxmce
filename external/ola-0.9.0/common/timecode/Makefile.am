include $(top_srcdir)/common.mk

# LIBRARIES
##################################################
noinst_LTLIBRARIES = libtimecode.la
libtimecode_la_SOURCES = TimeCode.cpp

# TESTS
##################################################
if BUILD_TESTS
TESTS = TimeCodeTester
endif
check_PROGRAMS = $(TESTS)

TimeCodeTester_SOURCES = TimeCodeTest.cpp
TimeCodeTester_CXXFLAGS = $(COMMON_TESTING_FLAGS)
TimeCodeTester_LDADD = $(COMMON_TESTING_LIBS) \
                       ./libtimecode.la \
                       ../base/libolabase.la

