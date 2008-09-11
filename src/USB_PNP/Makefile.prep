SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

ROOT = ..
CXXFLAGS = -Wall -g -pipe
LDFLAGS = -L$(ROOT)/lib
LDLIBS = -lpthread -lmysqlclient_r -lhttp_fetcher -ldbus-glib-1 -lgthread-2.0 -ldbus-1 -lglib-2.0 -lhal

APPNAME = usbpnpd
VERSION = 1.0.0

#sources = MessageSend.cpp ParamGet.cpp
BaseCPP = -I$(ROOT) -I.. -I$(ROOT)/DCE
BaseSources = hal-test.cpp

SerClsCPP = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql -D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE_SOURCE $(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID
SerClsPfx = $(ROOT)/SerializeClass/
SerClsSrc = SerializeClass.cpp

DCECPP = -I./ -I../ -I../DCE/ -I../VIPShared/ -I/usr/include/mysql -I/usr/include/dbus-1.0/ -I/usr/include/hal \
	-I/usr/lib/dbus-1.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include \
	-DDBUS_API_SUBJECT_TO_CHANGE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE $(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -D_GNU_SOURCE
DCEPfx = $(ROOT)/DCE/
DCESrc = DataGrid.cpp ClientSocket.cpp PlainClientSocket.cpp Logger.cpp Message.cpp \
	Socket.cpp ServerSocket.cpp SocketListener.cpp ServerLogger.cpp \
	AlarmManager.cpp Command_Impl.cpp Event_Impl.cpp \
	HandleRequestSocket.cpp \
	DeviceData_Impl.cpp DeviceData_Base.cpp \
	DCEConfig.cpp DCEMySqlConfig.cpp \
	Virtual_Device_Translator.cpp arp-cache.cpp

PUCPP = -I./ -I../../ -I../ -I../DCE/ -I../../DCE -I../VIPShared/ -I../../VIPShared -I/usr/include/mysql \
	-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DUSE_LZO_DATAGRID $(SNR_CPPFLAGS)
PUPfx = $(ROOT)/PlutoUtils/
PUSrc = CommonIncludes.cpp FileUtils.cpp MultiThreadIncludes.cpp Other.cpp ProcessUtils.cpp \
	StringUtils.cpp DatabaseUtils.cpp getch.cpp md5c.cpp minilzo.cpp uuencode.cpp LinuxSerialUSB.cpp

sources = $(addprefix $(SerClsPfx),$(SerClsSrc)) $(addprefix $(DCEPfx),$(DCESrc)) $(addprefix $(PUPfx),$(PUSrc)) $(BaseSources)
CPPFLAGS = $(BaseCPP) $(SerClsCPP) $(DCECPP) $(PUCPP)

all: USB_PNP 
	cp $+ $(ROOT)/bin

USB_PNP: $(sources:.cpp=.o)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $(call cpath,$+)  $(LDFLAGS) $(LDLIBS)

clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) USB_PNP >/dev/null

package:
	./make_package.sh $(APPNAME) $(VERSION)

include $(ROOT)/PlutoUtils/make.rules
ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(sources:.cpp=.d)
endif


