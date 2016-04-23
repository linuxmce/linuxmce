ROOT = ..

bin_PROGRAMS = Tribunexml headendtobase rmprogs filterprogs

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

LDLIBS = $(BASELDLIBS)

MYNAME = Tribunexml
MYNAME2 = headendtobase
MYNAME3 = rmprogs
MYNAME4 = filterprogs
VERSION = 0.0.1

sources = Tribunexml.cpp
sources2 =  headendtobase.cpp
sources3 = rmprogs.cpp
sources4 = filterprogs.cpp

whattodo:
	@echo "type 'make bin' or 'make all'"

all: bin

bin: Tribunexml headendtobase rmprogs filterprogs

Tribunexml: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

headendtobase: $(sources2:.cpp=.o)
	$(CXX) -o $(MYNAME2) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin

rmprogs: $(sources3:.cpp=.o)
	$(CXX) -o $(MYNAME3) $(call cpath,$+) $(LDFLAGS)
	cp $@ ../bin

filterprogs: $(sources4:.cpp=.o)
	$(CXX) -o $(MYNAME4) $(call cpath,$+) $(LDFLAGS)
	cp $@ ../bin

clean:
	rm -f $(sources:.cpp=.o) $(sources2:.cpp=.o) $(sources3:.cpp=.o) $(sources4:.cpp=.o)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p /etc/cron.d
	cp * $(DESTPATH)/etc/cron.d
	mkdir -p /usr/pluto/bin
	cp Tribunexml $(DESTPATH)/usr/pluto/bin
	cp headendtobase $(DESTPATH)/usr/pluto/bin
	cp rmprogs $(DESTPATH)/usr/pluto/bin
	cp filterprogs $(DESTPATH)/usr/pluto/bin
