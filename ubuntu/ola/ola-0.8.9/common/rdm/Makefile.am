include $(top_srcdir)/common.mk

EXTRA_DIST = Pids.proto

noinst_LTLIBRARIES = libolardm.la
libolardm_la_SOURCES = RDMAPI.cpp RDMCommand.cpp RDMHelper.cpp \
                       QueueingRDMController.cpp

TESTS = RDMTester
check_PROGRAMS = $(TESTS)
RDMTester_SOURCES = RDMAPITest.cpp RDMCommandTest.cpp RDMTester.cpp \
                    QueueingRDMControllerTest.cpp UIDTest.cpp
RDMTester_CPPFLAGS = $(CPPUNIT_CFLAGS) $(AM_CPPFLAGS)
RDMTester_LDFLAGS = $(CPPUNIT_LIBS)
RDMTester_LDADD = libolardm.la ../logging/liblogging.la \
                  ../network/libolanetwork.la ../utils/libolautils.la
