SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DNOBLUETOOTH
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lDCECommon -lattr -lPlutoUtils -lSerializeClass
CXX = g++

sources = convert_color.cpp

XTRALDLIBS = 

all: convert_color

convert_color: $(sources:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) convert_color >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
