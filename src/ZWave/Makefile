#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= 

ROOT = ..

CXXFLAGS += -fPIC
CPPFLAGS += -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS += $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = ZWave
VERSION = 2.0.0
MYDEVTEMPLATEID = 1754

Gen_Dev_Files = $(GENDEV)/ZWaveBase.cpp
sources = Main.cpp ZWave.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS =

Gen_Dev_Files +=
sources += Serial.cpp ZWApi.cpp ../Generic_Serial_Device/IOUtils.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin

bin: ZWave

so: ZWave.so

ZWave.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
ZWave: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) ZWave ZWave.so >/dev/null

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
	cp ZWave $(DESTPATH)/usr/pluto/bin
