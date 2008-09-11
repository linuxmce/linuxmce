ROOT=..

CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lSerializeClass -lmysqlclient_r -lPlutoUtils -lpluto_main

all: Main.o PlutoDHCP.o
	g++ Main.o PlutoDHCP.o -o PlutoDHCP $(LDFLAGS) $(LDLIBS)
	cp PlutoDHCP $(ROOT)/bin/

clean:
	rm -f PlutoDHCP PlutoDHCP.o
