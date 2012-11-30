#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS = -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS = 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread 

MYNAME = Shoutcast_Radio_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 1929mplateID

Gen_Dev_Files = $(GENDEV)/Shoutcast_Radio_PluginBase.cpp
sources = Main.cpp Shoutcast_Radio_Plugin.cpp $(Gen_Dev_Files) ../PlutoUtils/HttpUtils.cpp
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/libxml2
LDFLAGS +=
XTRALDLIBS = -lcurl

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Shoutcast_Radio_Plugin

so: Shoutcast_Radio_Plugin.so

Shoutcast_Radio_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Shoutcast_Radio_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Shoutcast_Radio_Plugin.bin Shoutcast_Radio_Plugin.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp Shoutcast_Radio_Plugin.so $(DESTDIR)/usr/pluto/bin
