#<-dceag-mh-b->
GENDEV=../Gen_Devices


ROOT = ..
CC = gcc
CXX = g++
CFLAGS = -Wall -fPIC -ggdb3
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS += -lSerializeClass -lDCECommon -lPlutoUtils -lpthread -ldl

MYNAME = RPC_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 2032

Gen_Dev_Files = $(GENDEV)/RPC_PluginBase.cpp
sources = Main.cpp RPC_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r

Gen_Dev_Files +=
sources += 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: RPC_Plugin

so: RPC_Plugin.so

RPC_Plugin.so: $(sources:.cpp=.o)
	$(CC) $(CFLAGS) -c mongoose.c
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) mongoose.o $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
RPC_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
	./post_make.sh $@

distclean: clean

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) RPC_Plugin.bin RPC_Plugin.so >/dev/null
	rm -f mongoose.o >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp RPC_Plugin.so $(DESTDIR)/usr/pluto/bin
