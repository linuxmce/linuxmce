ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lDCECommon -lPlutoUtils -lSerial -lSerializeClass

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif



SOURCES=Main.cpp ../Generic_Serial_Device/IOUtils.cpp

all: $(SOURCES:.cpp=.o)
	g++ $+ -o TestSerialPort $(LDFLAGS) $(LDLIBS)
	cp TestSerialPort $(ROOT)/bin/

clean:
	rm -f TestSerialPort $(SOURCES:.cpp=.o)

install: 
	mkdir -p /usr/pluto/bin
	cp TestSerialPort $(DESTPATH)/usr/pluto/bin
