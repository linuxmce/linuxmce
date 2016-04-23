#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS ?= 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = External_Media_Identifier
VERSION = 2.0.0
MYDEVTEMPLATEID = 1701

Gen_Dev_Files = $(GENDEV)/External_Media_IdentifierBase.cpp
sources = Main.cpp External_Media_Identifier.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=-std=c++0x
LDFLAGS +=
XTRALDLIBS = -lcdio -lcddb -lcurl -ldvdid

Gen_Dev_Files +=
sources += HTTPPicture.cpp \
	WMMCDI.cpp \
	IdentifierBase.cpp \
	IdentifierDVDBase.cpp \
	IdentifierDVDMDR.cpp \
	IdentifierDVDXML.cpp \
	IdentifierCDMDR.cpp \
	DVDXMLParser.cpp \
	DVDXMLQuery.cpp \
	DVDID.cpp \
	MDRCDParser.cpp \
	MDRCDQuery.cpp \
	MDRDVDParser.cpp \
	MDRDVDQuery.cpp \
	pugixml.cpp \
	IdentifierCDDB.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: all
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin 

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

