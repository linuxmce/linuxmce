ROOT=..
CXXFLAGS = -Wall -ggdb3 -fPIC
CPPFLAGS = -I./ -I../ -I../DCE/
CXX = g++
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpthread -lDCECommon -lSerial -lSerializeClass -lPlutoUtils

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#

APPNAME = IRBase
VERSION = 2.0.0

sources = MessageTranslation.cpp DefaultMessageTranslation.cpp AVMessageTranslation.cpp

all: libMessageTranslation.so all-recursive

libMessageTranslation.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ $(ROOT)/lib

libMessageTranslation.a: $(sources:.cpp=.o)
	$(AR) rc $@ $+
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
	mkdir -p pluto/lib
	cp libMessageTranslation.so $(DESTPATH)pluto/lib
