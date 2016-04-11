#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I../STEngine -I/usr/include/exiv2 $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib -lexiv2
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

MYNAME = Photo_Screen_Saver
VERSION = 2.0.0
MYDEVTEMPLATEID = 1825mplateID

Gen_Dev_Files = $(GENDEV)/Photo_Screen_SaverBase.cpp
sources = Main.cpp Photo_Screen_Saver.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL `pkg-config --cflags glib-2.0`
LDFLAGS +=
XTRALDLIBS = -lGL -lGLU -lSDL -lSDL_image -lSDL_ttf -lXrender `pkg-config --libs glib-2.0` -lXmu -lSDL_gfx -lX11

Gen_Dev_Files +=
sources += AnimatedPicture.cpp \
	FadeBase.cpp \
	FadeBlending.cpp \
	FadeFactory.cpp \
	FileBrowser.cpp \
	GaleryStateMachine.cpp \
	Gallery.cpp \
	GalleryScenario.cpp \
	GUIWizardUtils.cpp \
	../STEngine/ExtensionManager.cpp \
	../STEngine/ColorDesc.cpp \
	../STEngine/GraphicImage.cpp \
	../STEngine/MathUtils.cpp \
	../STEngine/Builder.cpp \
	../STEngine/Container.cpp \
	../STEngine/Frame.cpp \
	../STEngine/Painter.cpp \
	../STEngine/Transform.cpp \
	../STEngine/Triangle.cpp \
	../STEngine/Vertex.cpp \
	../STEngine/Point3D.cpp \
	../STEngine/TextureManager.cpp \
	RefreshRates.cpp \
	SDLFrontEnd.cpp \
	WM_Event.cpp \
	ZoomBase.cpp \
	ZoomFactory.cpp \
	ZoomPanUp.cpp \
	ZoomSimple.cpp \
	../utilities/linux/window_manager/WMController/WMController.cpp \
	../utilities/linux/window_manager/wmctrl/wmctrl.cpp 
			

ifneq (,$(findstring -DVIA_OVERLAY, $(CPPFLAGS)))
	XTRALDLIBS += -lddmpeg
endif 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Photo_Screen_Saver

so: Photo_Screen_Saver.so

Photo_Screen_Saver.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Photo_Screen_Saver: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Photo_Screen_Saver.bin Photo_Screen_Saver.so >/dev/null

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
	cp Photo_Screen_Saver $(DESTPATH)/usr/pluto/bin
	cp LaunchPhoto_Screen_Saver.sh $(DESTPATH)/usr/pluto/bin
