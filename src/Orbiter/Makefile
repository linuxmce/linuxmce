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

MYNAME = Orbiter
VERSION = 2.0.0
MYDEVTEMPLATEID = 8

Gen_Dev_Files = $(GENDEV)/OrbiterBase.cpp
sources = Main.cpp Orbiter.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

#Note: For Nokia 770, do an export SNR_CPPFLAGS="-DMAEMO_NOKIA770" before doing a make clean; make bin


CXXFLAGS += -g -O0
CPPFLAGS += -I/usr/include/SDL -DORBITER -DCACHE_IMAGES -DENABLE_MOUSE_BEHAVIOR -I$(ROOT)/Splitter -I$(ROOT)/OrbiterGen -I$(ROOT)/Orbiter -I$(ROOT)/SDL_Helpers
LDFLAGS += -L/usr/X11R6/lib
XTRALDLIBS += -lDCECommon -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lXtst -lXext -lXrender -lSDL_Helpers -lX11 -lz

# Remove unnessesary libraries for Maemo Orbiter
#ifneq (MAEMO_NOKIA770,$(findstring MAEMO_NOKIA770, $(SNR_CPPFLAGS)))
#ifneq (RPI,$(findstring RPI, $(SNR_CPPFLAGS)))
#XTRALDLIBS += -lusb -lGL -lGLU
#CPPFLAGS += -DHID_REMOTE
#endif
#endif

# USE_GTK can be passed in as additional defines to SNR_CPPFLAGS to add support for GTK progress bars and prompt dialogues.
ifeq (USE_GTK,$(findstring USE_GTK, $(SNR_CPPFLAGS)))
CPPFLAGS += $(shell pkg-config --cflags gtk+-x11-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-x11-2.0)
sources += Linux/GTKPromptUser.cpp \
	   Linux/GTKProgressWnd.cpp
endif

# wmctrl wrapper
CPPFLAGS += -I/usr/X11R6/include $(shell pkg-config --cflags glib-2.0)
XTRALDLIBS += -lX11 -lXmu $(shell pkg-config --libs glib-2.0)

Gen_Dev_Files +=
sources += \
	Linux/X3DWindow.cpp \
	Linux/X3DButton.cpp \
	Linux/XPromptUser.cpp \
	Linux/XProgressWnd.cpp \
	../utilities/linux/window_manager/WMController/WMController.cpp \
	../utilities/linux/window_manager/wmctrl/wmctrl.cpp \
	../utilities/linux/wrapper/wrapper_x11.cpp \
	../utilities/linux/wrapper/image_file.cpp \
	Linux/win_list_manager.cpp \
	Linux/OrbiterRenderer_Linux.cpp \
        Linux/WindowContext.cpp \
	DesignObj_Data.cpp \
	DesignObj_Orbiter.cpp \
	DesignObj_DataGrid.cpp \
	Simulator.cpp \
	OrbiterFileBrowser.cpp \
	Linux/OrbiterLinux.cpp \
	Linux/XRecordExtensionHandler.cpp \
	RendererMNG.cpp \
	GraphicBuilder.cpp \
	TextStyle.cpp \
	CacheImageManager.cpp \
	RendererOCG.cpp \
	ScreenHandler.cpp \
	CallBackData.cpp \
	ScreenHistory.cpp \
	MouseBehavior.cpp \
	LightMouseHandler.cpp \
	SpeedMouseHandler.cpp \
	VolumeMouseHandler.cpp \
	MediaMouseHandler.cpp \
	EPGMouseHandler.cpp \
	LockedMouseHandler.cpp \
	KeyboardMouseHandler.cpp \
	HorizMenuMouseHandler.cpp \
	MediaBrowserMouseHandler.cpp \
	DatagridMouseHandlerHelper.cpp \
	MouseGovernor.cpp \
	MouseIterator.cpp \
	PlutoGraphic.cpp \
	OrbiterRenderer.cpp \
	OrbiterRendererFactory.cpp \
	DataGridRenderer.cpp \
	ObjectRenderer.cpp \
	ObjectRendererFactory.cpp \
	SDL/OrbiterRenderer_SDL.cpp \
	SDL/SDLRendererOCGHelper.cpp \
	SDL/SDLGraphic.cpp \
	SDL/StartOrbiterSDL.cpp \
	Linux/OSDScreenHandler.cpp \
	Linux/MouseBehavior_Linux.cpp \
	WizardLogic.cpp \
	../CreateDevice/UserUtils.cpp \
	$(ROOT)/Splitter/TextSplitter.cpp \
	$(ROOT)/Splitter/TextWrapper.cpp \
	../LIRC_DCE/IRReceiverBase.cpp \
	$(ROOT)/utilities/linux/transparency/transparency.cpp \


# Remove unnessesary libraries for Maemo Orbiter
#ifneq (MAEMO_NOKIA770,$(findstring MAEMO_NOKIA770, $(SNR_CPPFLAGS)))
#ifneq (RPI,$(findstring RPI, $(SNR_CPPFLAGS)))
#sources += Linux/HIDInterface.cpp \
#	   Linux/GenericHIDInterface.cpp
#endif
#endif

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Orbiter

so: Orbiter.so

Orbiter.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Orbiter: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Orbiter Orbiter.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /etc
	cp Orbiter.conf $(DESTPATH)/etc
	mkdir -p /usr/pluto/bin
	cp Orbiter $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sis $(DESTPATH)/usr/pluto/bin
	cp LaunchOrbiter.sh $(DESTPATH)/usr/pluto/bin
	cp OrbiterGL $(DESTPATH)/usr/pluto/bin
	cp UIdiag $(DESTPATH)/usr/pluto/bin
	cp logo.png $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sisx $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sisx $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/share
	cp GTKOrbiter.glade $(DESTPATH)pluto/share
	cp orbiter-gtkrc $(DESTPATH)pluto/share
