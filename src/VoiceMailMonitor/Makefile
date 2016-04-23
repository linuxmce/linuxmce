ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/inotify
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lDCECommon -lPlutoUtils -lmp3lame

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


sources = Main.cpp \
$(ROOT)/inotify/inotify_class.cpp \

all: $(sources:.cpp=.o)
	$(CXX) -o VoiceMailMonitor $+ $(LDFLAGS) $(LDLIBS)
	cp VoiceMailMonitor $(ROOT)/bin/

clean:
	rm -f VoiceMailMonitor $(sources:.cpp=.o) 

install:
	mkdir -p $(DESTPATH)/usr/pluto/bin
	cp VoiceMailMonitor $(DESTPATH)/usr/pluto/bin
	cp VoiceMailMonitor.sh $(DESTPATH)/usr/pluto/bin
