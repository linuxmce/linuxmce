SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES -I/opt/libxine1-pluto/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include -I/opt/libsdl1.2-1.2.7+1.2.8cvs20041007/include/SD

ROOT = ../..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lPlutoUtils -lDCECommon -lSerializeClass

MYNAME = MediaIdentifierTest
VERSION = 2.0.0
MYDEVTEMPLATEID = 8mplateID

sources = MediaIdentifierTest.cpp ../MediaIdentifier.cpp

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
	MYSQL_LIBS = -lmysqlclient_r
endif

CXXFLAGS += -g -O0
CPPFLAGS += 
LDFLAGS += 
XTRALDLIBS = 

LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make all'"

all: bin

bin: MediaIdentifierTest

MediaIdentifierTest: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $+ $(LDFLAGS) $(LDLIBS)

clean: 
	rm -f $(sources:.cpp=.o) $(sources:.cpp=.d) MediaIdentifierTest.bin > /dev/null


