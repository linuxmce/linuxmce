#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = Generic_Serial_Device
VERSION = 2.0.0
MYDEVTEMPLATEID = 69mplateID

Gen_Dev_Files = $(GENDEV)/Generic_Serial_DeviceBase.cpp
sources = Main.cpp Generic_Serial_Device.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I$(ROOT)/MessageTranslation
LDFLAGS +=
XTRALDLIBS = -lruby1.8 -lSerial -lrt -lMessageTranslation

ifneq (,$(findstring amd64, $(arch))) 
        CPPFLAGS += -I/usr/lib/ruby/1.8/x86_64-linux
else
	ifneq (,$(findstring armhf, $(arch)))
	        CPPFLAGS += -I/usr/lib/ruby/1.8/arm-linux-eabihf
	else
	        CPPFLAGS += -I/usr/lib/ruby/1.8/i386-linux -I/usr/lib/ruby/1.8/i486-linux -I/usr/lib/ruby/1.8/i686-linux -I/usr/lib/ruby/1.8/x86_64-linux/
	endif
endif


Gen_Dev_Files +=
sources += $(ROOT)/Serial/GenericIODevice.cpp FileCodeSupplier.cpp IOEvent.cpp IOMutex.cpp IOPool.cpp IOThread.cpp LoopStateMachine.cpp RubyEmbeder.cpp RubyEmbededClass.cpp RubyExceptions.cpp RubyStdStreamRedirector.cpp SerialInterceptorCodeSupplier.cpp SerialIOConnection.cpp NetworkIOConnection.cpp SerialIOPool.cpp NetworkIOPool.cpp StateMachine.cpp TestEmbededClass.cpp TestRubyEmbederCodeSupplier.cpp RubySerialWrapper.cpp RubySerialIOWrapper.cpp RubyDeviceWrapper.cpp RubyDCECodeSupplier.cpp RubyDCEEmbededClass.cpp RubyIOManager.cpp RubyIOPool.cpp RubyCommandWrapper.cpp BufferedIOConnection.cpp RubySerialIOConnectionWrapper.cpp IOUtils.cpp GSDMessageProcessing.cpp RubyDCEDeviceNode.cpp

#<-dceag-mf-b->!
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Generic_Serial_Device ruby

so: Generic_Serial_Device.so ruby

ruby: Ruby_$(MYNAME).so

Generic_Serial_Device.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@

Generic_Serial_Device: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@


Ruby_$(MYNAME).so: Ruby_$(MYNAME)_wrap.o
	ln -s /usr/lib/libtcl8.4.so.0 /usr/lib/libtcl8.4.so | true
	$(CXX) -shared -o Ruby_$(MYNAME).so $(LDFLAGS) $(LDLIBS) Ruby_$(MYNAME)_wrap.o RubySerialWrapper.o RubySerialIOWrapper.o RubyDeviceWrapper.o RubyCommandWrapper.o RubySerialIOConnectionWrapper.o BufferedIOConnection.o -ltcl8.4 -lstdc++
	cp $@ ../bin

Ruby_$(MYNAME)_wrap.o: Ruby_$(MYNAME)_wrap.cxx
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) Ruby_$(MYNAME)_wrap.cxx RubySerialWrapper.cpp RubySerialIOWrapper.cpp RubyDeviceWrapper.cpp RubyCommandWrapper.cpp RubySerialIOConnectionWrapper.cpp BufferedIOConnection.cpp -I/usr/include/tcl8.4

Ruby_Generic_Serial_Device_wrap.cxx:
	swig -ruby -c++ Ruby_Generic_Serial_Device.i

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Generic_Serial_Device.bin Generic_Serial_Device.so *.cxx >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

docs:
	doxygen Doxyfile

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install: 
	mkdir -p /usr/pluto/bin
	cp Generic_Serial_Device $(DESTPATH)/usr/pluto/bin
	cp Ruby_Generic_Serial_Device.so $(DESTPATH)/usr/pluto/bin
