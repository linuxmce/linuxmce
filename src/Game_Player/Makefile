#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = Game_Player
VERSION = 2.0.0
MYDEVTEMPLATEID = 1965mplateID

Gen_Dev_Files = $(GENDEV)/Game_PlayerBase.cpp
sources = Main.cpp Game_Player.cpp $(Gen_Dev_Files) \
	EmulatorModel.cpp \
	EmulatorController.cpp \
	X11EmulatorModel.cpp \
	X11EmulatorController.cpp \
	MAMEEmulatorModel.cpp
#<-dceag-mh-e->

CXXFLAGS += 
CPPFLAGS += `pkg-config libxml-2.0 --cflags`
LDFLAGS +=  -lX11 -lXmu -lXtst -lWindowUtils -lmysqlclient_r -lpthread `pkg-config libxml-2.0 --libs`
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

