ROOT = ..

bin_PROGRAMS = getxmls instcheck

CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/libxml2 -D_DEVEL_DEFINES
LDFLAGS = -L$(ROOT)/lib
# the library search path.
getxmls_LDADD = -lpluto_main -lPlutoUtils -lDCECommon -lSerializeClass -lxml2
instcheck_LDADD =

MYNAME_getxmls = getxmls
VERSION_getxmls = 0.0.1

MYNAME_instcheck = instcheck
VERSION_instcheck = 0.0.1

getxmls_SOURCES = getxmls.cpp DatabaseHelper.cpp IconDecoder.cpp PackageXMLParser.cpp XMLFinder.cpp ../PlutoUtils/uuencode.cpp
instcheck_SOURCES = instcheck.cpp

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	getxmls_LDADD += $(MYSQL_LIBS)
	instcheck_LDADD += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	getxmls_LDADD += -lmysqlclient
	instcheck_LDADD += -lmysqlclient
endif

whattodo:
	@echo "type 'make bin', 'make getxmls', 'make instcheck', 'make all'"

all: bin

bin: getxmls instcheck

getxmls: $(getxmls_SOURCES:.cpp=.o)
	$(CXX) -o $(MYNAME_getxmls) $(call cpath,$+) $(LDFLAGS) $(getxmls_LDADD)
	cp $@ ../bin

instcheck: $(instcheck_SOURCES:.cpp=.o)
	$(CXX) -o $(MYNAME_instcheck) $(instcheck_SOURCES) $(LDFLAGS) $(instcheck_LDADD)
	cp $@ ../bin

clean:
	rm -f $(bin_PROGRAMS) $(getxmls_SOURCES:.cpp=.o) $(instcheck_SOURCES:.cpp=.o)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif

install: 
	mkdir -p /etc/cron.d
	cp * $(DESTPATH)/etc/cron.d
	mkdir -p /etc/init.d
	cp * $(DESTPATH)/etc/init.d
	mkdir -p /usr/pluto/bin
	cp getxmls $(DESTPATH)/usr/pluto/bin
	cp instcheck $(DESTPATH)/usr/pluto/bin
