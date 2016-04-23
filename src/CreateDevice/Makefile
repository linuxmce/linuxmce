ROOT=..

CXX?=g++
CC=$(CXX)
CPPFLAGS+=-W -Wall -pipe -ggdb3 $(SNR_CPPFLAGS)
CXXFLAGS+=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS+=-L$(ROOT)/lib
LDLIBS+=-lDCECommon -lSerializeClass -lPlutoUtils -lpthread

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
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
