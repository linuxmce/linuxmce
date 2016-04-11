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

MYNAME = Game_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 1965mplateID

Gen_Dev_Files = $(GENDEV)/Game_PlayerBase.cpp
sources = Main.cpp Game_Player.cpp $(Gen_Dev_Files) \
	ImageUtils.cpp \
	../utilities/linux/window_manager/WMController/WMController.cpp \
	../utilities/linux/window_manager/wmctrl/wmctrl.cpp \
	VirtualKeyboard.cpp \
        ConfigurationWriter.cpp \
	VideoFrameGeometry.cpp \
	EmulatorFactory.cpp \
	EmulatorModel.cpp \
	EmulatorController.cpp \
	X11EmulatorModel.cpp \
	X11EmulatorController.cpp \
	INotifyEmulatorModel.cpp \
	INotifyEmulatorController.cpp \
	MAMEEmulatorModel.cpp \
	MAMEEmulatorController.cpp \
	MESSEmulatorModel.cpp \
	MESSEmulatorController.cpp \
	A2600EmulatorModel.cpp \
	A2600EmulatorController.cpp \
	StellaEmulatorModel.cpp \
	StellaEmulatorController.cpp \
	A7800EmulatorModel.cpp \
	A7800EmulatorController.cpp \
	SecondGenEmulatorModel.cpp \
	SecondGenEmulatorController.cpp \
	IntellivisionEmulatorModel.cpp \
	IntellivisionEmulatorController.cpp \
	A5200EmulatorModel.cpp \
	A5200EmulatorController.cpp \
	ColecoVisionEmulatorModel.cpp \
	ColecoVisionEmulatorController.cpp \
	NESEmulatorModel.cpp \
	NESEmulatorController.cpp \
	FamicomEmulatorModel.cpp \
	FamicomEmulatorController.cpp \
	SG1000EmulatorModel.cpp \
	SG1000EmulatorController.cpp \
	SMSEmulatorModel.cpp \
	SMSEmulatorController.cpp \
	GenesisEmulatorModel.cpp \
	GenesisEmulatorController.cpp \
	MegadriveEmulatorModel.cpp \
	MegadriveEmulatorController.cpp \
	PCEngineEmulatorModel.cpp \
	PCEngineEmulatorController.cpp \
	TG16EmulatorModel.cpp \
	TG16EmulatorController.cpp \
	SGXEmulatorModel.cpp \
	SGXEmulatorController.cpp \
	SNESEmulatorModel.cpp \
	SNESEmulatorController.cpp \
	VectrexEmulatorModel.cpp \
	VectrexEmulatorController.cpp \
	VICEEmulatorModel.cpp \
	VICEEmulatorController.cpp \
	VIC20EmulatorModel.cpp \
	VIC20EmulatorController.cpp \
	C64EmulatorModel.cpp \
	C64EmulatorController.cpp \
	Apple2EmulatorModel.cpp \
	Apple2EmulatorController.cpp \
	Atari800EmulatorModel.cpp \
	Atari800EmulatorController.cpp \
	TI994AEmulatorModel.cpp \
	TI994AEmulatorController.cpp \
	$(ROOT)/inotify/inotify_class.cpp
#<-dceag-mh-e->

CXXFLAGS += 
CPPFLAGS += `pkg-config libxml-2.0 --cflags` `pkg-config glib-2.0 --cflags` `pkg-config MagickWand --cflags`
LDFLAGS +=  -lX11 -lXmu -lXtst -lWindowUtils -lpthread `pkg-config glib-2.0 --libs` `pkg-config libxml-2.0 --libs` `pkg-config MagickWand --libs`
XTRALDLIBS = -lm

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Game_Player

so: Game_Player.so

Game_Player.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Game_Player: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Game_Player Game_Player.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

