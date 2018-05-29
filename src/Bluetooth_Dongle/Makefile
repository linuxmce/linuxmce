#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -lpng16 -lz

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = Bluetooth_Dongle
VERSION = 2.0.0
MYDEVTEMPLATEID = 13mplateID

Gen_Dev_Files = $(GENDEV)/Bluetooth_DongleBase.cpp
sources = Main.cpp Bluetooth_Dongle.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/SDL -DORBITER -DBLUETOOTH_DONGLE
LDFLAGS +=
XTRALDLIBS = -lVIPCommon -lBDCommon -lSDL_Helpers -lbluetooth -lSDL -lSDL_ttf -lSDL_image -lSGE -lSDL_gfx -lGL -lXrender -ljpeg


Gen_Dev_Files += $(GENDEV)/OrbiterBase.cpp
sources += \
	OrbiterBluetooth.cpp \
	OrbiterBluetoothCreator.cpp \
	OrbiterRenderer_SDL_Bluetooth.cpp \
	DataGridRenderer_Bluetooth.cpp \
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
	../Orbiter/ScreenHistory.cpp \
	../Orbiter/SDL/SDLRendererOCGHelper.cpp \
	../Orbiter/SDL/SDLGraphic.cpp \
	../Orbiter/SDL/JpegWrapper.cpp \
	../Orbiter/ScreenHandler.cpp \
	../Orbiter/CallBackData.cpp \
	../Orbiter/PlutoGraphic.cpp \
	../Orbiter/OrbiterRenderer.cpp \
	../Orbiter/OrbiterRendererFactory.cpp \
	../Orbiter/SDL/OrbiterRenderer_SDL.cpp \
	../Orbiter/DataGridRenderer.cpp \
	../Orbiter/ObjectRenderer.cpp \
	../Orbiter/ObjectRendererFactory.cpp \
	../Splitter/TextWrapper.cpp \
	../Splitter/TextSplitter.cpp \
	../LIRC_DCE/IRReceiverBase.cpp 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Bluetooth_Dongle

so: Bluetooth_Dongle.so

Bluetooth_Dongle.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Bluetooth_Dongle: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Bluetooth_Dongle.bin Bluetooth_Dongle.so >/dev/null

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
	cp BluetoothDongle.conf $(DESTPATH)/etc
	mkdir -p /etc/bluetooth
	cp hcid.conf $(DESTPATH)/etc/bluetooth
	mkdir -p /usr/pluto/bin
	cp Bluetooth_Dongle $(DESTPATH)/usr/pluto/bin
	cp SymbianPhoneInstall.sh $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sis $(DESTPATH)/usr/pluto/bin
	cp SmartphoneInstall.sh $(DESTPATH)/usr/pluto/bin
	cp Orbiter.CAB $(DESTPATH)/usr/pluto/bin
	cp TestChannel $(DESTPATH)/usr/pluto/bin
	cp pluto-pinhelper.sh $(DESTPATH)/usr/pluto/bin
	cp LaunchBluetooth_Dongle.sh $(DESTPATH)/usr/pluto/bin
	cp BluetoothDongle_BugReport.sh $(DESTPATH)/usr/pluto/bin
	cp Orbiter_Treo.CAB $(DESTPATH)/usr/pluto/bin
	cp JavaPhoneInstall.sh $(DESTPATH)/usr/pluto/bin
	cp JavaMO.jar $(DESTPATH)/usr/pluto/bin
	cp Pluto_S60.sisx $(DESTPATH)/usr/pluto/bin
