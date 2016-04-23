ROOT = ..
CXXFLAGS = -pipe -Wall -ggdb3
CPPFLAGS = -I/usr/local/include -I$(ROOT)/DCE -I.. -I. 
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -ldl -lpthread -lpluto_main -lDCECommon -lSerializeClass -lPlutoUtils

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif


APPNAME = DCEGen
VERSION = 2.0.0

sources = ../db_wrapper/db_wrapper.cpp ../db_wrapper/ClientSocket.cpp ../db_wrapper/Socket.cpp \
DCEGen.cpp

all: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o DCEGen $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp DCEGen ../bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) DCEGen >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p /usr/pluto/bin
	cp DCEGen $(DESTPATH)/usr/pluto/bin
