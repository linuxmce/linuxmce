SNR_CPPFLAGS ?= -DKDE_LMCE -DDEBUG -DTHREAD_LOG -DLOG_ALL_QUERIES

ROOT = ..
CXXFLAGS = -Wall -g 
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -I$(ROOT)/PlutoVIP -I/usr/include/mysql -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DGENERATOR
LDFLAGS = -L$(ROOT)/lib 
LDLIBS = -lRACommon -lDCECommon -lSerializeClass -lPlutoUtils -lpthread -lmysqlclient_r -lattr
CXX = g++

sources = ../PlutoVIP/PlutoVIPRequests.cpp \
	../PlutoVIP/VR_IdentifyPhone.cpp ../PlutoVIP/VR_PhoneInstalledBinary.cpp ../PlutoVIP/VR_RequestPayment.cpp ../PlutoVIP/VR_UnknownRequest.cpp \
	../PlutoVIP/VR_ManuallyMatchPhone.cpp ../PlutoVIP/VR_PhoneRespondedToRequest.cpp ../PlutoVIP/VR_RequestSecureTransaction.cpp \
	../PlutoVIP/VA_SendFileToPhone.cpp ../PlutoVIP/VA_SendMenuToPhone.cpp ../PlutoVIP/VA_UpdateTransaction.cpp

VIPServer = $(sources) VIPServer.cpp
new_thing = $(sources) TableInfo_SCVS.cpp

all: VIPServer
#all: newthing

VIPServer: $(VIPServer:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

newthing: $(new_thing:.cpp=.o)
	@echo "*** Something to do here ***"

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) VIPServer >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
