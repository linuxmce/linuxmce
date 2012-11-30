#<-dceag-mh-b->
GENDEV=../Gen_Devices

SNR_CPPFLAGS = -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES
SNR_LDFLAGS = 

ROOT = ..
CXX = g++
CXXFLAGS = -Wall -fPIC -ggdb3
CPPFLAGS = -I$(ROOT) -I$(ROOT)/DCE $(SNR_CPPFLAGS)
LDFLAGS = $(SNR_LDFLAGS) -L$(ROOT)/lib
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = XML_Data_Handler_Plugin
VERSION = 2.0.0
MYDEVTEMPLATEID = 1868mplateID

Gen_Dev_Files = $(GENDEV)/XML_Data_Handler_PluginBase.cpp
sources = Main.cpp XML_Data_Handler_Plugin.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS += -I/usr/include/libxml2/
LDFLAGS += -lxml2
XTRALDLIBS =

Gen_Dev_Files +=
sources += XML_Data_Factory.cpp XML_Data_CallBack.cpp

#<-dceag-mf-b->
LDLIBS = $(XTRALDLIBS) $(BASELDLIBS)

whattodo: so
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: XML_Data_Handler_Plugin

so: XML_Data_Handler_Plugin.so

XML_Data_Handler_Plugin.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
XML_Data_Handler_Plugin: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

distclean: clean
clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) XML_Data_Handler_Plugin.bin XML_Data_Handler_Plugin.so >/dev/null

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
#	cp Basic_XML_Data_Source_Plugin.so $(DESTPATH)/usr/pluto/bin
	cp XML_Data_Handler_Plugin.so $(DESTDIR)/usr/pluto/bin
