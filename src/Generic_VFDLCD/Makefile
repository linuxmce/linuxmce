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

MYSQL_FLAGS = `pkg-config --cflags mysqlclient`
MYSQL_LIBS = `pkg-config --libs mysqlclient`
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	BASELDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	BASELDLIBS += -lmysqlclient_r
endif

MYNAME = Generic_VFDLCD
VERSION = 2.0.0
MYDEVTEMPLATEID = 1896mplateID

Gen_Dev_Files = $(GENDEV)/Generic_VFDLCDBase.cpp
sources = Main.cpp Generic_VFDLCD.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/libxml2/
LDFLAGS += -lxml2 -lSerial -lpluto_main
XTRALDLIBS =

Gen_Dev_Files +=
sources += \
	$(ROOT)/VFD_LCD/LCDLogic/ActionProcessor.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/InputMenuItem.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/LCDManager.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/MenuHolder.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/MenuItem.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/MenuItemAction.cpp \
	$(ROOT)/VFD_LCD/LCDLogic/MenuLoader.cpp \
	$(ROOT)/VFD_LCD/Renderers/LCDRenderer.cpp \
	$(ROOT)/VFD_LCD/InputProviders/SocketStatusInputProvider.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Generic_VFDLCD

so: Generic_VFDLCD.so

Generic_VFDLCD.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Generic_VFDLCD: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Generic_VFDLCD.bin Generic_VFDLCD.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

