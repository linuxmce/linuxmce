#<-dceag-mh-b->!
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = Linphone
VERSION = 2.0.0
MYDEVTEMPLATEID = 1602mplateID

Gen_Dev_Files = $(GENDEV)/LinphoneBase.cpp
sources = Main.cpp Linphone.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -I../ -I/usr/pluto/include -I/usr/pluto/include/linphone -I/usr/pluto/include/ortp -I/usr/pluto/include/osipua -DHAVE_GLIB $(shell pkg-config gdk-2.0 --cflags)
CPPFLAGS +=
LDFLAGS += -L/usr/pluto/lib
XTRALDLIBS = -llinphone -lraw1394 -ldc1394_control

Gen_Dev_Files +=
sources += LinphoneManager.cpp

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Linphone

so: Linphone.so

Linphone.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Linphone: $(sources:.cpp=.o)
	LD_LIBRARY_PATH=/usr/pluto/lib $(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	cp Linphone ../bin
	cp Linphone_Wrapper.sh ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Linphone.bin Linphone.so >/dev/null

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
	cp Linphone $(DESTPATH)/usr/pluto/bin
	cp Linphone_Wrapper.sh $(DESTPATH)/usr/pluto/bin
