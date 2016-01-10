#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXX = g++
GCC = gcc
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = EnOcean_TCM120
VERSION = 0.0.1
MYDEVTEMPLATEID = 1948

Gen_Dev_Files = $(GENDEV)/EnOcean_TCM120Base.cpp
sources = Main.cpp EnOcean_TCM120.cpp $(Gen_Dev_Files)
libenocean_sources = libEnOcean/TCM120.c libEnOcean/EnOceanPort.c libEnOcean/cssl.c
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r

Gen_Dev_Files +=
sources += 

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: EnOcean_TCM120

so: EnOcean_TCM120.so

EnOcean_TCM120.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
EnOcean_TCM120: $(sources:.cpp=.o) cssl.o EnOceanPort.o TCM120.o
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

cssl.o:
	$(GCC) -c -fPIC -D_GNU_SOURCE -o cssl.o libEnOcean/cssl.c

EnOceanPort.o:
	$(GCC) -c -fPIC -o EnOceanPort.o libEnOcean/EnOceanPort.c

TCM120.o:
	$(GCC) -c -fPIC -o TCM120.o libEnOcean/TCM120.c

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) EnOcean_TCM120.bin EnOcean_TCM120.so cssl.o EnOceanPort.o TCM120.o  >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->


install:
	mkdir -p /usr/pluto/bin
	cp EnOcean_TCM120 $(DESTPATH)/usr/pluto/bin
