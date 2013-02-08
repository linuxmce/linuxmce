ROOT=..

CXX=g++
CPPFLAGS=-I$(ROOT)/CreateDevice
CXXFLAGS=-I. -I.. -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql -ggdb3
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lpluto_main -lPlutoUtils -lSerializeClass -lDCECommon -lmysqlclient_r -lDCECommon

OBJECTS=ConfirmDependencies.o ../CreateDevice/CreateDevice.o

all: ConfirmDependencies
	cp ConfirmDependencies ../bin

ConfirmDependencies: $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $+ -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ConfirmDependencies $(OBJECTS)
