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

MYNAME = SimplePhone
VERSION = 2.0.0
MYDEVTEMPLATEID = 1759mplateID

Gen_Dev_Files = $(GENDEV)/SimplePhoneBase.cpp
sources = Main.cpp SimplePhone.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += $(shell pkg-config --cflags glib-2.0 linphone)
CPPFLAGS += $(shell pkg-config --cflags glib-2.0 linphone) -DHAVE_CONFIG_H -DLINPHONE_CONST=$(LINPHONE_CONST)
LDFLAGS += $(shell pkg-config --libs-only-L glib-2.0 linphone)
XTRALDLIBS = $(shell pkg-config --libs-only-l glib-2.0 linphone)

Gen_Dev_Files +=
sources += SIP_Thread.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: SimplePhone

so: SimplePhone.so

SimplePhone.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	chmod +x post_make.sh
#	./post_make.sh $@
		
SimplePhone: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) SimplePhone SimplePhone.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp SimplePhone $(DESTDIR)/usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

install: 
	mkdir -p /usr/pluto/bin
	cp SimplePhone $(DESTPATH)/usr/pluto/bin
	mkdir -p pluto/templates
	cp simplephone.conf $(DESTPATH)pluto/templates
