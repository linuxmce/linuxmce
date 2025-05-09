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

MYNAME = Proxy_Orbiter
VERSION = 2.0.0
MYDEVTEMPLATEID = 1727mplateID

Gen_Dev_Files = $(GENDEV)/OrbiterBase.cpp
sources = Main.cpp Proxy_Orbiter.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DPROXY_ORBITER
LDFLAGS +=
XTRALDLIBS = -lSDL_Helpers -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lcurl -lGL -lXrender -ljpeg -lpng -lz

sources += \
	Proxy_OrbiterRenderer_SDL.cpp \
	DataGridRenderer_Proxy.cpp \
	../Orbiter/OrbiterRenderer.cpp \
	../Orbiter/SDL/OrbiterRenderer_SDL.cpp \
	../Orbiter/OrbiterRendererFactory.cpp \
	../Orbiter/OrbiterFileBrowser.cpp \
	../Orbiter/Simulator.cpp \
	../Orbiter/DesignObj_Data.cpp \
	../Orbiter/DesignObj_Orbiter.cpp \
	../Orbiter/DesignObj_DataGrid.cpp \
	../Orbiter/Orbiter.cpp \
	../Orbiter/GraphicBuilder.cpp \
	../Orbiter/RendererMNG.cpp \
	../Orbiter/TextStyle.cpp \
	../Orbiter/CacheImageManager.cpp \
	../Orbiter/RendererOCG.cpp \
	../Orbiter/ScreenHandler.cpp \
	../Orbiter/ScreenHistory.cpp \
	../Orbiter/SDL/SDLRendererOCGHelper.cpp \
	../Orbiter/SDL/SDLGraphic.cpp \
	../Orbiter/SDL/JpegWrapper.cpp \
	../Orbiter/CallBackData.cpp \
	../Orbiter/PlutoGraphic.cpp \
	../Orbiter/DataGridRenderer.cpp \
	../Orbiter/ObjectRenderer.cpp \
	../Orbiter/ObjectRendererFactory.cpp \
	../Splitter/TextWrapper.cpp \
	../Splitter/TextSplitter.cpp \
	../PlutoUtils/HttpUtils.cpp \
	../LIRC_DCE/IRReceiverBase.cpp 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

binary: bin

all: bin so

bin: Proxy_Orbiter

so: Proxy_Orbiter.so

Proxy_Orbiter.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Proxy_Orbiter: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp Proxy_Orbiter $(DESTDIR)/usr/pluto/bin
	cp configure_proxyorbiter.pl $(DESTDIR)/usr/pluto/bin
	cp LaunchProxy_Orbiter.sh $(DESTDIR)/usr/pluto/bin
	
clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Proxy_Orbiter.bin Proxy_Orbiter.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

