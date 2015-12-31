ROOT = ..
CXXFLAGS = -fPIC -Wall -ggdb3
CPPFLAGS = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql
CXX = g++

APPNAME = Serial
VERSION = 2.0.0

sources = SerialPort.cpp

all: libSerial.so all-recursive

libSerial.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

libSerial.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

clean:
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libSerial.so libSerial.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p pluto/lib
	cp libSerial.so $(DESTPATH)pluto/lib
