ROOT=..

CXX?=g++
CC=$(CXX)
CPPFLAGS+=-W -Wall -pipe -ggdb3 $(SNR_CPPFLAGS)
CXXFLAGS+=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS+=-L$(ROOT)/lib
LDLIBS+=-lDCECommon -lSerializeClass -lPlutoUtils -lpthread

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#


all: Main.o CreateDevice.o UserUtils.o
	g++ Main.o CreateDevice.o UserUtils.o -o CreateDevice $(LDFLAGS) $(LDLIBS)
	cp CreateDevice $(ROOT)/bin/

clean:
	rm -f CreateDevice CreateDevice.o UserUtils.o

install: 
	mkdir -p /usr/pluto/bin
	cp CreateDevice $(DESTPATH)/usr/pluto/bin
