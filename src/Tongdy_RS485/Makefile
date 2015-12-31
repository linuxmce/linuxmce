#<-dceag-mh-b->
GENDEV=../Gen_Devices

ROOT = ..
CXXFLAGS += -Wall -fPIC -ggdb3
CPPFLAGS += -I$(ROOT) -I$(ROOT)/DCE -I/usr/include/mysql $(SNR_CPPFLAGS)
LDFLAGS += $(SNR_LDFLAGS) -L$(ROOT)/lib 
BASELDLIBS = -lSerializeClass -lDCECommon -lPlutoUtils -lpthread

MYNAME = Tongdy_RS485
VERSION = 2.0.0
MYDEVTEMPLATEID = 2008

Gen_Dev_Files = $(GENDEV)/Tongdy_RS485Base.cpp
sources = Main.cpp Tongdy_RS485.cpp $(Gen_Dev_Files)
#<-dceag-mh-e->

CXXFLAGS +=
CPPFLAGS +=
LDFLAGS +=
XTRALDLIBS = 

Gen_Dev_Files +=
sources +=

#<-dceag-mf-b->
LDLIBS += $(XTRALDLIBS) $(BASELDLIBS)

whattodo:
	@echo "type 'make bin', 'make so' or 'make all'"

all: bin so

bin: Tongdy_RS485

so: Tongdy_RS485.so

Tongdy_RS485.so: $(sources:.cpp=.o)
	$(CXX) -shared -o $(MYNAME).so $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	chmod +x post_make.sh
	./post_make.sh $@
		
Tongdy_RS485: $(sources:.cpp=.o)
	$(CXX) -o $(MYNAME) $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ ../bin
	./post_make.sh $@

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) Tongdy_RS485.bin Tongdy_RS485.so >/dev/null

package:
	chmod +x make_package.sh
	./make_package.sh $(MYNAME) $(VERSION) $(MYDEVTEMPLATEID)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
#<-dceag-mf-e->

