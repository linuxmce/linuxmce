ROOT = ..
CXXFLAGS = -pipe -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I.
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lSerializeClass -lPlutoUtils

sources = Main.cpp

all: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o ColorSplitter $(call cpath,$+) $(LDFLAGS) $(LDLIBS)

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) ColorSplitter >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
