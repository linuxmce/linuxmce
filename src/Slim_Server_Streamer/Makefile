#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = Slim_Server_Streamer
VERSION = 2.0.0
MYDEVTEMPLATEID = 53mplateID

Gen_Dev_Files = $(GENDEV)/Slim_Server_StreamerBase.cpp
sources = Main.cpp Slim_Server_Streamer.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -fmessage-length=0
CPPFLAGS += -I/usr/include/mysql
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r

Gen_Dev_Files +=
sources += ../Xine_Player/XineMediaInfo.cpp SqueezeBox_Player.cpp $(GENDEV)/SqueezeBox_PlayerBase.cpp


#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Slim_Server_Streamer

so: Slim_Server_Streamer.so

Slim_Server_Streamer.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Slim_Server_Streamer: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Slim_Server_Streamer.bin Slim_Server_Streamer.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /usr/pluto/bin
	cp Slim_Server_Streamer $(DESTPATH)/usr/pluto/bin
	cp LaunchSlimServer.sh $(DESTPATH)/usr/pluto/bin
