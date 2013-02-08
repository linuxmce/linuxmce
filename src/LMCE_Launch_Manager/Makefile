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

MYNAME = LMCE_Launch_Manager
VERSION = 2.0.0
MYDEVTEMPLATEID = 1888mplateID

Gen_Dev_Files = $(GENDEV)/LMCE_Launch_ManagerBase.cpp
sources = Main.cpp LMCE_Launch_Manager.cpp LM.cpp List.cpp DB.cpp UI.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r -lX11 -lXext -lXrandr

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: LMCE_Launch_Manager

so: LMCE_Launch_Manager.so

LMCE_Launch_Manager.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
LMCE_Launch_Manager: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) LMCE_Launch_Manager.bin LMCE_Launch_Manager.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /usr/share/applications/
	cp lmce_launch_manager.desktop $(DESTPATH)/usr/share/applications/
	mkdir -p /usr/pluto/bin
	cp LMCE_Launch_Manager $(DESTPATH)/usr/pluto/bin
	cp StartCoreServices.sh $(DESTPATH)/usr/pluto/bin
	cp StopCoreServices.sh $(DESTPATH)/usr/pluto/bin
	cp GetVideoDriver.py $(DESTPATH)/usr/pluto/bin
	cp lmce_launch_manager.sh $(DESTPATH)/usr/pluto/bin
