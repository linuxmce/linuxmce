ROOT = ..
CXXFLAGS = -pipe -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I. -I/usr/include/mysql
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -ldl -lmysqlclient -lpthread -lpluto_main -lDCECommon -lSerializeClass -lPlutoUtils

APPNAME = MakeRelease_PrepFiles
VERSION = 2.0.0

sources = MakeRelease_PrepFiles.cpp

all: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o MakeRelease_PrepFiles $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp MakeRelease_PrepFiles ../bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) MakeRelease_PrepFiles >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
