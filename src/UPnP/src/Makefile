SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ../..
CXX = g++
CXXFLAGS = -Wall -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/Serial -I/libupnp/upnp/inc $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
EXTERNAL_LIBS_NAMES = pthread
INTERNAL_LIBS_NAMES = SerializeClass DCECommon Serial 

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	EXTERNAL_LIBS_NAMES += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	EXTERNAL_LIBS_NAMES += -lmysqlclient_r
endif

MYNAME = upnp
VERSION = 1.0.0
MYDEVTEMPLATEID = 5mplateID

CXXFLAGS += 
CPPFLAGS +=
LDFLAGS += -L$(ROOT)/lib -L/usr/lib/
EXTERNAL_LIBS_NAMES += SerializeClass DCECommon Serial PlutoUtils upnp

sources =  main.cpp ControlPoint.cpp ControlPointMediaBrowser.cpp Device.cpp 
sources += DeviceMediaServer.cpp
	

LDLIBS_NAMES = $(EXTERNAL_LIBS_NAMES)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: upnp

so: upnp.so

upnp.so: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

upnp: $(sources:.cpp=.o) $(INTERNAL_LIBS_NAMES:%=$(ROOT)/lib/lib%.so) Makefile
	$(CXX) -o $(MYNAME) $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS_NAMES:%=-l%)
#	mv ../bin/$@ ../bin/$@.bak
	cp $@ ../../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) ZWaveTest.bin ZWaveTest.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
