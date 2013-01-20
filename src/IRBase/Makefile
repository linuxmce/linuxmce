ROOT=..
CXXFLAGS = -W -Wall -g -ggdb3 -fPIC
CPPFLAGS = -I./ -I../ -I../DCE/ -I/usr/include/mysql
CXX = g++
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpthread -lDCECommon -lSerial -lSerializeClass -lPlutoUtils -lMessageTranslation

APPNAME = IRBase
VERSION = 2.0.0

headers = *.h

sources = IRBase.cpp GenerateRcX.cpp

all: libIRBase.so all-recursive

libIRBase.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	mkdir -p $(ROOT)/lib
	cp $@ $(ROOT)/lib

libIRBase.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
	mkdir -p $(ROOT)/lib
	cp $@ $(ROOT)/lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libIRBase.so libIRBase.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p $(DESTDIR)/usr/pluto/lib $(DESTDIR)/usr/include/lmce
	cp libIRBase.so $(DESTDIR)/usr/pluto/lib
	cp $(headers) $(DESTDIR)/usr/include/lmce
#	cp IRBase-r.lib $(DESTDIR)pluto/lib
