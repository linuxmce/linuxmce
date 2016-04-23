ROOT=..

CXX=g++
CPPFLAGS=-I$(ROOT)/CreateDevice
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -ggdb3
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lPlutoUtils -lSerializeClass -lDCECommon -lDCECommon -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

OBJECTS=ConfirmDependencies.o ../CreateDevice/CreateDevice.o

all: ConfirmDependencies
	cp ConfirmDependencies ../bin

ConfirmDependencies: $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $+ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ConfirmDependencies $(OBJECTS)
