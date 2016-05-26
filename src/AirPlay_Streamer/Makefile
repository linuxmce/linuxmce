#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = AirPlay_Streamer
VERSION = 2.0.0
MYDEVTEMPLATEID = 2246mplateID

Gen_Dev_Files = $(GENDEV)/AirPlay_StreamerBase.cpp
sources = Main.cpp AirPlay_Streamer.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += $(shell pkg-config --cflags avahi-client) $(shell pkg-config --cflags glib-2.0)
CPPFLAGS +=
LDFLAGS += $(shell pkg-config --libs avahi-client) $(shell pkg-config --libs glib-2.0)
XTRALDLIBS = -lshairport -lao -lplist -lvlc -lX11 -lXmu

Gen_Dev_Files +=
sources += AirPlay_Protocol_Factory.cpp \
		AirPlay_Service.cpp \
		AirPlay_Protocol.cpp \
		AirPlay_Protocol_AirPlay.cpp \
		AirPlay_Protocol_AirTunes.cpp \
	        ../HttpParser/HttpParser.cpp \
		AirPlay_Videolan.cpp \
		../utilities/linux/window_manager/WMController/WMController.cpp \
		../utilities/linux/window_manager/wmctrl/wmctrl.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: AirPlay_Streamer

so: AirPlay_Streamer.so

AirPlay_Streamer.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
AirPlay_Streamer: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) AirPlay_Streamer AirPlay_Streamer.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

