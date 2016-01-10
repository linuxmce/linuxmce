ROOT = ../..
CXXFLAGS = -Wall -g
CPPFLAGS = -I$(ROOT)
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lpthread -lDCECommon -lIRBase -lSerial

sources = gc100.cpp

all: gc_seriald gc_serial_client

gc_seriald: gc_seriald.cpp
	$(CXX) -o $@ $+  $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

gc_serial_client: gc_serial_client.cpp
	$(CXX) -o $@ $+  $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f *.o *.d >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
