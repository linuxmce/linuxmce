ROOT=..

CXX?=g++
CC=$(CXX)
CPPFLAGS+=-W -Wall -pipe -ggdb3 $(SNR_CPPFLAGS)
CXXFLAGS+=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS+=-L$(ROOT)/lib
LDLIBS+=-lDCECommon -lSerializeClass -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


all: Main.o CreateDevice.o UserUtils.o
	g++ Main.o CreateDevice.o UserUtils.o -o CreateDevice $(LDFLAGS) $(LDLIBS)
	cp CreateDevice $(ROOT)/bin/

clean:
	rm -f CreateDevice CreateDevice.o UserUtils.o

install: 
	mkdir -p /usr/pluto/bin
	cp CreateDevice $(DESTPATH)/usr/pluto/bin
