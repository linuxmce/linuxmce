ROOT=..

CXX=g++
CC=$(CXX)
CPPFLAGS=-W -Wall -pipe -ggdb3 -DUPDATE_MEDIA
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/taglib
LDFLAGS=-L$(ROOT)/lib -L/usr/local/lib
LDLIBS= -lpluto_media -lpluto_main -lDCECommon -lSerializeClass -lPlutoUtils -lpthread -lid3 -ltag

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

sources = Main.cpp \
UpdateMedia.cpp \
PlutoMediaFile.cpp \
MediaIdentifier.cpp \
PlutoMediaAttributes.cpp \
MediaState.cpp \
FileStatusObserver.cpp \
GenericFileHandler.cpp \
ID3FileHandler.cpp \
VDRInfoFileHandler.cpp \
FileHandlerFactory.cpp \
TagFileHandler.cpp \
RomFileHandler.cpp \
FileUtils/file_utils.cpp \
id3info/id3info.cpp \
$(ROOT)/Media_Plugin/MediaAttributes_LowLevel.cpp \
$(ROOT)/inotify/FileNotifier.cpp \
$(ROOT)/inotify/inotify_class.cpp 

all: $(sources:.cpp=.o)
	$(CXX) -o UpdateMedia $+ $(LDFLAGS) $(LDLIBS)
	cp UpdateMedia $(ROOT)/bin/

GAMEROM: GAMEROM.o GAMEROMMain.o
	$(CXX) -o GAMEROM $+ $(LDFLAGS) -llmce_game -lPlutoUtils -lSerializeClass -lDCECommon -lpthread
	cp GAMEROM $(ROOT)/bin/

clean:
	rm -f UpdateMedia $(sources:.cpp=.o) 

install: 
	mkdir -p /etc/init.d
	cp * $(DESTPATH)/etc/init.d
	mkdir -p /usr/pluto/bin
	cp UpdateMedia $(DESTPATH)/usr/pluto/bin
	cp GAMEROM $(DESTPATH)/usr/pluto/bin
