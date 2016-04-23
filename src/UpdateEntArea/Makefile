ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-l pluto_main -lDCECommon -lSerializeClass -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


all: Main.o UpdateEntArea.o UpdateEntArea_Climate.o UpdateEntArea_Lighting.o UpdateEntArea_Media.o UpdateEntArea_Security.o UpdateEntArea_Telecom.o UpdateEntArea_Setup.o UpdateEntArea_Event.o CommandGroupArray.o Criteria.o ../CreateDevice/CreateDevice.o
	g++ Main.o UpdateEntArea.o UpdateEntArea_Climate.o UpdateEntArea_Lighting.o UpdateEntArea_Media.o UpdateEntArea_Security.o UpdateEntArea_Telecom.o UpdateEntArea_Setup.o UpdateEntArea_Event.o CommandGroupArray.o Criteria.o ../CreateDevice/CreateDevice.o -o UpdateEntArea $(LDFLAGS) $(LDLIBS)
	cp UpdateEntArea $(ROOT)/bin/

clean:
	rm -f UpdateEntArea *.o

install: 
	mkdir -p /usr/pluto/bin
	cp UpdateEntArea $(DESTPATH)/usr/pluto/bin
	cp PurgeEntAreas.sql $(DESTPATH)/usr/pluto/bin
