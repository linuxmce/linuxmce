ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lPlutoUtils -lSerializeClass -lpthread -lusb

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources = Main.cpp ../LIRC_DCE/IRReceiverBase.cpp

all: Main.o ../LIRC_DCE/IRReceiverBase.o
	g++ $(sources:.cpp=.o) -o WatchGyroRemote $(LDFLAGS) $(LDLIBS)
	cp WatchGyroRemote $(ROOT)/bin/

clean:
	rm -f WatchGyroRemote $(sources:.cpp=.o)
