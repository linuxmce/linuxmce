ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


all: Main.o PlutoDHCP.o
	g++ Main.o PlutoDHCP.o -o PlutoDHCP $(LDFLAGS) $(LDLIBS)
	cp PlutoDHCP $(ROOT)/bin/

clean:
	rm -f PlutoDHCP PlutoDHCP.o

install: 
	mkdir -p /usr/pluto/bin
	cp PlutoDHCP $(DESTPATH)/usr/pluto/bin
	cp PlutoDHCP.sh $(DESTPATH)/usr/pluto/bin
