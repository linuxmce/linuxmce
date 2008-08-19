SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

ROOT = ..
CXXFLAGS = -Wall -ggdb3 -DxAUDIDEMO -fmessage-length=0
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -I/usr/include/libxml2/
LDFLAGS = -L$(ROOT)/lib -lxml2

LDLIBS = -lpthread -lattr -lDCECommon -lSerializeClass -lPlutoUtils 

APPNAME=HillcrestBeacon
VERSION=2.0.0

sources = Main.cpp

all: HillcrestBeacon

HillcrestBeacon: $(sources:.cpp=.o) Makefile
	$(CXX) -o $@ $(call cpath,$(sources:.cpp=.o)) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) PlutoServer >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif
