ROOT = ..
CXXFLAGS = -Wall -D_GNU_SOURCE
CPPFLAGS = -I/../.. -I/.. -I/usr/include/mysql -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =  -lmysqlclient_r

APPNAME = pluto_main
VERSION = 2.0.0
sources = Database_FakeEPG.cpp  \
Table_Listing.cpp

all: libFakeEPG.a all-recursive

libFakeEPG.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libFakeEPG.a >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
