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

MYNAME = Advanced_IP_Camera
VERSION = 2.0.0
MYDEVTEMPLATEID = 2208mplateID

Gen_Dev_Files = $(GENDEV)/Advanced_IP_CameraBase.cpp
sources = Main.cpp Advanced_IP_Camera.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS += -DWITH_OPENSSL
CPPFLAGS_SOAP =  -Ionvif -I/usr/share/gsoap -I/usr/share/gsoap/plugin -I/usr/share/gsoap/import -Iopenssl
CPPFLAGS += -Ionvif -I/usr/share/gsoap -I/usr/share/gsoap/plugin -I/usr/share/gsoap/import -Iopenssl
LDFLAGS +=
XTRALDLIBS = -lcurl -lmysqlclient_r -lgsoap++ -lgsoapssl++ -lssl -lcrypto

Gen_Dev_Files +=
sources += EventMethod.cpp CameraDevice.cpp OnvifDevice.cpp URLAccessDevice.cpp onvif/soapDeviceBindingProxy.cpp onvif/soapPTZBindingProxy.cpp onvif/soapMediaBindingProxy.cpp onvif/soapC.cpp gsoap/duration.cpp gsoap/wsseapi.cpp gsoap/mecevp.cpp gsoap/wsaapi.cpp gsoap/smdevp.cpp

gsoap_dir:
	mkdir -p gsoap

gsoap/duration.cpp:
	cp /usr/share/gsoap/custom/duration.c gsoap/duration.cpp

gsoap/wsseapi.cpp:
	cp /usr/share/gsoap/plugin/wsseapi.* gsoap/.

gsoap/wsaapi.cpp:
	cp /usr/share/gsoap/plugin/wsaapi.c gsoap/wsaapi.cpp

gsoap/mecevp.cpp:
	cp /usr/share/gsoap/plugin/mecevp.c gsoap/mecevp.cpp

gsoap/smdevp.cpp:
	cp /usr/share/gsoap/plugin/smdevp.c gsoap/smdevp.cpp

gsoap/%.o:
	$(CXX) -c $(CPPFLAGS_SOAP) $(CXXFLAGS) $< -o $@ gsoap/$*.cpp

bin: onvif

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Advanced_IP_Camera

so: Advanced_IP_Camera.so

Advanced_IP_Camera.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Advanced_IP_Camera: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Advanced_IP_Camera.bin Advanced_IP_Camera.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

# needs gsoap >=2.8.16
onvif-get:
	wsdl2h -P -x -I/usr/share/gsoap/WS -o onvif/onvif.h http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl
	sed -i '/#import "wsa5.h"/i#import "wsse.h"' onvif/onvif.h

onvif-gen: onvif-get
	soapcpp2 -j -C -I/usr/share/gsoap -I/usr/share/gsoap/import -d onvif -i onvif/onvif.h

onvif: gsoap_dir onvif-get onvif-gen
