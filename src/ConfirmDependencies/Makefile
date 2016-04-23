ROOT=..

CXX=g++
CPPFLAGS=-I$(ROOT)/CreateDevice
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -ggdb3
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lPlutoUtils -lSerializeClass -lDCECommon -lDCECommon -lpthread

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
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
