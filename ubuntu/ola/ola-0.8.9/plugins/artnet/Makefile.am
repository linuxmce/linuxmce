include $(top_srcdir)/common.mk
libdir = $(plugindir)
SUBDIRS = messages
EXTRA_DIST = ArtNetDevice.h ArtNetNode.h ArtNetPackets.h ArtNetPlugin.h \
             ArtNetPort.h MockUdpSocket.h

# This is a library which doesn't contain any coupling to olad
noinst_LTLIBRARIES = libolaartnetnode.la
libolaartnetnode_la_SOURCES = ArtNetNode.cpp

lib_LTLIBRARIES = libolaartnet.la
libolaartnet_la_SOURCES = ArtNetPlugin.cpp ArtNetDevice.cpp ArtNetPort.cpp
libolaartnet_la_LIBADD = ./libolaartnetnode.la messages/libolaartnetconf.la

# Test programs
TESTS = ArtNetTester
check_PROGRAMS = $(TESTS)
ArtNetTester_SOURCES = ArtNetNodeTest.cpp \
                       ArtNetTester.cpp \
                       MockUdpSocket.cpp

ArtNetTester_CXXFLAGS = $(CPPUNIT_CFLAGS)
ArtNetTester_LDFLAGS = $(CPPUNIT_LIBS)
ArtNetTester_LDADD = -lprotobuf \
                     ./libolaartnetnode.la \
                     ../../common/libolacommon.la
