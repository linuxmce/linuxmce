ROOT=..
CXXFLAGS = -Wall -ggdb3 -fPIC
CPPFLAGS = -I./ -I../ -I../DCE/
CXX = g++
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpthread -lDCECommon -lSerial -lSerializeClass -lPlutoUtils

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

APPNAME = IRBase
VERSION = 2.0.0

sources = MessageTranslation.cpp DefaultMessageTranslation.cpp AVMessageTranslation.cpp

headers = *.h

all: libMessageTranslation.so all-recursive

libMessageTranslation.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	mkdir -p $(ROOT)/lib
	cp $@ $(ROOT)/lib

libMessageTranslation.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	mkdir -p $(ROOT)/lib
	cp $@ $(ROOT)/lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libMessageTranslation.so libMessageTranslation.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include/lmce
	cp libMessageTranslation.so $(DESTDIR)/usr/pluto/lib
	cp $(headers) $(DESTDIR)/usr/include/lmce
