SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE $(SNR_CPPFLAGS)
CXX = g++

sources = SerializeClass.cpp

header = *.h

all: libSerializeClass.so all-recursive

libSerializeClass.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

libSerializeClass.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	mkdir -p ../lib
	cp $@ ../lib

distclean: clean

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libSerializeClass.so libSerializeClass.a >/dev/null

Sample: Sample.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $+ -L../lib -lSerializeClass -lPlutoUtils -lDCECommon

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	# We put it into pluto/lib as well as usr/pluto/lib to make sure older and newer routines
	# work.
	mkdir -p $(DESTDIR)/pluto/lib $(DESTDIR)/usr/include/lmce $(DESTDIR)/usr/pluto/lib
	cp libSerializeClass.so $(DESTDIR)/pluto/lib
	cp libSerializeClass.so $(DESTDIR)/usr/pluto/lib
	cp $(header) $(DESTDIR)/usr/include/lmce
