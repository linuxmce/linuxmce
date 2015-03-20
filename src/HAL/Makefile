#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CC = gcc
CFLAGS = -Wall -fPIC -ggdb3
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib 
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lmysqlclient_r -lpthread -lSerial

MYNAME = HAL
VERSION = 2.0.0
MYDEVTEMPLATEID = 1808

Gen_Dev_Files = $(GENDEV)/HALBase.cpp
sources = Main.cpp HAL.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CFLAGS += $(shell pkg-config --cflags gthread-2.0 glib-2.0)
CPPFLAGS += $(shell pkg-config --cflags gthread-2.0 glib-2.0) \
	-DDBUS_API_SUBJECT_TO_CHANGE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	$(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -D_GNU_SOURCE
CXXFLAGS += -Wno-deprecated
LDFLAGS +=
XTRALDLIBS = $(shell pkg-config --libs gthread-2.0 glib-2.0) -ludev

Gen_Dev_Files +=
sources += lmce-udevd.cpp seriald.cpp
c_sources = udi-helper.c

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
#	./post_make.sh $@

udi-helper.o: udi-helper.c
	$(CC) -c udi-helper.c $(CFLAGS)
		
HAL: $(sources:.cpp=.o) udi-helper.o
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
#	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) udi-helper.o HAL HAL.so >/dev/null

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
