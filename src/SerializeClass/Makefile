SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
VIP = ../VIPShared
GDEV = ./Gen_Devices
CXXFLAGS = -fPIC -Wall -g -ggdb3 -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE $(SNR_CPPFLAGS)
CXX = g++

sources = SerializeClass.cpp

all: libSerializeClass.so all-recursive

libSerializeClass.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

libSerializeClass.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libSerializeClass.so libSerializeClass.a >/dev/null

Sample: Sample.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $+ -L../lib -lSerializeClass -lPlutoUtils -lDCECommon

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
