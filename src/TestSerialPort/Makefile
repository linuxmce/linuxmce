ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lPlutoUtils -lSerial -lmysqlclient_r -lSerializeClass

SOURCES=Main.cpp ../Generic_Serial_Device/IOUtils.cpp

all: $(SOURCES:.cpp=.o)
	g++ $+ -o TestSerialPort $(LDFLAGS) $(LDLIBS)
	cp TestSerialPort $(ROOT)/bin/

clean:
	rm -f TestSerialPort $(SOURCES:.cpp=.o)

install: 
	mkdir -p /usr/pluto/bin
	cp TestSerialPort $(DESTPATH)/usr/pluto/bin
