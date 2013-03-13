ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lPlutoUtils -lpthread -lusb

sources = Main.cpp ../LIRC_DCE/IRReceiverBase.cpp

all: Main.o ../LIRC_DCE/IRReceiverBase.o
	g++ $(sources:.cpp=.o) -o WatchGyroRemote $(LDFLAGS) $(LDLIBS)
	cp WatchGyroRemote $(ROOT)/bin/

clean:
	rm -f WatchGyroRemote $(sources:.cpp=.o)
