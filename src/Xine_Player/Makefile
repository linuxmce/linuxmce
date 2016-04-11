#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -ldl -lXext

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = Xine_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 5

Gen_Dev_Files = $(GENDEV)/Xine_PlayerBase.cpp
sources = Main.cpp Xine_Player.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

ARCH = $(shell dpkg-architecture -qDEB_BUILD_ARCH)

CXXFLAGS +=
CPPFLAGS += $(shell pkg-config --cflags glib-2.0) \
		-DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
LDFLAGS += -L/usr/X11R6/lib  -L$(ROOT)/Xine_Player
XTRALDLIBS = -ljpeg -lxine -lXtst -lX11 $(shell pkg-config --libs glib-2.0) -lXmu -lcdda_interface 

ifneq (,$(findstring armhf, $(ARCH)))
	XTRALDLIBS += $(shell pkg-config --libs dvdnav)
else
ifneq (,$(findstring amd64, $(ARCH)))
	XTRALDLIBS += -ldvdnav_xine_custom_64b
else
	XTRALDLIBS += -ldvdnav_xine_custom
endif
endif

Gen_Dev_Files +=
sources += Colorspace_Utils.cpp JpegEncoderDecoder.cpp Xine_Stream_Factory.cpp Xine_Stream.cpp XineMediaInfo.cpp ../utilities/linux/window_manager/WMController/WMController.cpp ../utilities/linux/window_manager/wmctrl/wmctrl.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Xine_Player

so: Xine_Player.so

Xine_Player.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Xine_Player: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Xine_Player Xine_Player.bin Xine_Player.so >/dev/null

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
	cp Xine_Player $(DESTPATH)/usr/pluto/bin
	cp nbd-client-wrapper $(DESTPATH)/usr/pluto/bin
