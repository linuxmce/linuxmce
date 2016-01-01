#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = External_Media_Identifier
VERSION = 2.0.0
MYDEVTEMPLATEID = 1701

Gen_Dev_Files = $(GENDEV)/External_Media_IdentifierBase.cpp
sources = Main.cpp External_Media_Identifier.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=-std=c++11
LDFLAGS +=
XTRALDLIBS = -lmysqlclient_r -lcdio -lcddb

Gen_Dev_Files +=
sources += IdentifierBase.cpp \
	IdentifierCDDB.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: External_Media_Identifier

so: External_Media_Identifier.so

External_Media_Identifier.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	chmod +x post_make.sh
#	./post_make.sh $@

External_Media_Identifier: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	mkdir -p ../bin
	cp $@ ../bin
#	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) External_Media_Identifier External_Media_Identifier.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp External_Media_Identifier $(DESTDIR)/usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

