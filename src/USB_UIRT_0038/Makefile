#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread

ifneq (,$(findstring -DARMHF, $(CPPFLAGS)))
        UUIRTDRV_SO = uuirtdrv-armhf.so
else
ifneq (,$(findstring -DARCH64, $(CPPFLAGS)))
        UUIRTDRV_SO = uuirtdrv-amd64.so
else
        UUIRTDRV_SO = uuirtdrv-i386.so
endif
endif

MYNAME = USB_UIRT_0038
VERSION = 2.0.0
MYDEVTEMPLATEID = 1783mplateID

Gen_Dev_Files = $(GENDEV)/USB_UIRT_0038Base.cpp
sources = Main.cpp USB_UIRT_0038.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lMessageTranslation -lIRBase -ldl

Gen_Dev_Files +=
sources += ../LIRC_DCE/IRReceiverBase.cpp 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: USB_UIRT_0038

so: USB_UIRT_0038.so

USB_UIRT_0038.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
USB_UIRT_0038: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	cp $(UUIRTDRV_SO) uuirtdrv.so
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) USB_UIRT_0038.bin USB_UIRT_0038.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p $(DESTDIR)/usr/pluto/bin $(DESTDIR)/usr/pluto/lib
	cp USB_UIRT_0038 $(DESTDIR)/usr/pluto/bin
	cp uuirtdrv.so $(DESTDIR)/usr/pluto/lib
