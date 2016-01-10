ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -I$(ROOT)/inotify
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lDCECommon -lmysqlclient_r -lPlutoUtils -lmp3lame

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
