#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib 
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread -lSerial

MYNAME = HAL
VERSION = 2.0.0
MYDEVTEMPLATEID = 1808mplateID

Gen_Dev_Files = $(GENDEV)/HALBase.cpp
sources = Main.cpp HAL.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CPPFLAGS += -I/usr/include/dbus-1.0/ -I/usr/include/hal \
	-I/usr/lib/i386-linux-gnu/dbus-1.0/include \
	-I/usr/lib/i386-linux-gnu/glib-2.0/include \
	-I/usr/lib/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include \
	-DDBUS_API_SUBJECT_TO_CHANGE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	$(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -D_GNU_SOURCE
CXXFLAGS += -Wno-deprecated
LDFLAGS +=
XTRALDLIBS = -lSerial -ldbus-glib-1 -lgthread-2.0 -ldbus-1 -lglib-2.0 -lhal

Gen_Dev_Files +=
sources += pluto-hald.cpp seriald.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: HAL

so: HAL.so

HAL.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
HAL: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) HAL.bin HAL.so >/dev/null

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
	cp HAL $(DESTPATH)/usr/pluto/bin
