include $(top_srcdir)/common.mk

# LIBRARIES
##################################################
noinst_LTLIBRARIES = libolanetwork.la
libolanetwork_la_SOURCES = AdvancedTCPConnector.cpp \
                           FakeInterfacePicker.h \
                           HealthCheckedConnection.cpp \
                           IPV4Address.cpp \
                           Interface.cpp \
                           InterfacePicker.cpp \
                           NetworkUtils.cpp \
                           MACAddress.cpp \
                           Socket.cpp \
                           SocketAddress.cpp \
                           SocketHelper.cpp \
                           SocketHelper.h \
                           TCPConnector.cpp \
                           TCPSocket.cpp
libolanetwork_la_LIBADD = $(RESOLV_LIBS)


if USING_WIN32
libolanetwork_la_SOURCES += WindowsInterfacePicker.h WindowsInterfacePicker.cpp
else
libolanetwork_la_SOURCES += PosixInterfacePicker.h PosixInterfacePicker.cpp
endif

# TESTS
##################################################
if BUILD_TESTS
TESTS = HealthCheckedConnectionTester NetworkTester TCPConnectorTester
endif
check_PROGRAMS = $(TESTS)

# required for all tests
COMMON_TEST_LDADD = $(COMMON_TESTING_LIBS) \
                    ../libolacommon.la

HealthCheckedConnectionTester_SOURCES = HealthCheckedConnectionTest.cpp
HealthCheckedConnectionTester_CXXFLAGS = $(COMMON_TESTING_FLAGS)
HealthCheckedConnectionTester_LDADD = $(COMMON_TEST_LDADD)

NetworkTester_SOURCES = IPAddressTest.cpp \
                        InterfacePickerTest.cpp \
                        InterfaceTest.cpp \
                        MACAddressTest.cpp \
                        NetworkUtilsTest.cpp \
                        SocketAddressTest.cpp \
                        SocketTest.cpp
NetworkTester_CXXFLAGS = $(COMMON_TESTING_FLAGS)
NetworkTester_LDADD = $(COMMON_TEST_LDADD)

if USING_WIN32
NetworkTester_LDFLAGS = -no-undefined -liphlpapi -lnetapi32 -lcap -lws2_32 \
                        -ldpnet -lwsock32
endif

TCPConnectorTester_SOURCES = AdvancedTCPConnectorTest.cpp \
                             TCPConnectorTest.cpp
TCPConnectorTester_CXXFLAGS = $(COMMON_TESTING_FLAGS)
TCPConnectorTester_LDADD = $(COMMON_TEST_LDADD)
