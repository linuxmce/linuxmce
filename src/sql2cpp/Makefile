SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DGENERATOR
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -lattr
CXX = g++

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

APPNAME = sql2cpp
VERSION = 2.0.0

sources = TableInfo.cpp TableInfo_Generator.cpp DatabaseInfo.cpp CommonFunctions.cpp FieldInfo.cpp
sql2cpp = $(sources) sql2cpp.cpp
new_thing = $(sources) TableInfo_SCVS.cpp

all: sql2cpp
#all: newthing

sql2cpp: $(sql2cpp:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

newthing: $(new_thing:.cpp=.o)
	@echo "*** Something to do here ***"

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) sql2cpp sql2cpp.o >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p /usr/pluto/bin
	cp sql2cpp $(DESTPATH)/usr/pluto/bin
