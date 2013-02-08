ROOT=..
CXXFLAGS = -fPIC -Wall -g -D_GNU_SOURCE
CPPFLAGS = -I./ -I../../ -I../ -I$(ROOT) -I$(ROOT)/DCE -I$(ROOT)/BD -I$(ROOT)/Orbiter -I$(ROOT)/VIPShared -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DUSE_MYSQL -DVIPSERVER -DXBTSOCKET -DBLUETOOTH_DONGLE
CXX = g++
LDFLAGS = -L$(ROOT)/lib
LDLIBS =

sources = \
    BDCommandProcessor_BluetoothDongle.cpp \
    BD_CP_CaptureKeyboard.cpp \
    BD_CP_Disconnect.cpp \
    BD_CP_SendFile.cpp \
    BD_CP_SendMeKeystrokes.cpp \
    BD_CP_ShowImage.cpp \
    BD_CP_ShowList.cpp \
    BD_CP_ShowVMC.cpp \
    BD_CP_UpdateVariables.cpp \
	BD_CP_ShowMenu.cpp \
    BD_PC_Disconnect.cpp \
    BD_PC_KeyWasPressed.cpp \
    BD_PC_OrderPlaced.cpp \
    BD_PC_ReportingVariables.cpp \
    BD_PC_SelectedFromList.cpp \
    BD_PC_SetVariable.cpp \
    BD_PC_VMCHidden.cpp \
    BD_PC_VMCTerminated.cpp \
	BD_PC_ReportMyVersion.cpp \
	BD_CP_SimulateEvent.cpp \
	BD_CP_CurrentSignalStrength.cpp \
	BD_PC_GetSignalStrength.cpp \
	BD_PC_SetImageQuality.cpp \
	BD_CP_MenuData.cpp \
	BD_CP_SetBkgndImage.cpp \
	BD_PC_Configure.cpp \
	BD_PC_SelectedItem.cpp \
	BD_PC_MouseEvent.cpp \
	BD_PC_GetMenuImages.cpp \
	BD_CP_SetMenuImages.cpp \
    PhoneDetection_Bluetooth_Linux.cpp \
    PhoneDetectionEngine.cpp \
    PhoneDetection_Simulate.cpp \
    PlutoPhoneCommands.cpp \
    VIPIncludes.cpp \
    MenuData.cpp \
    MenuItemInfo.cpp

all: libVIPCommon.so all-recursive

libVIPCommon.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

libVIPCommon.a: $(sources:.cpp=.o)
	$(AR) rc $@ $(call cpath,$+)
	cp $@ ../lib
		
clean: clean-recursive
	rm -f $(call cpath,$(sources:.cpp=.o) $(sources:.cpp=.d)) libVIPCommon.a libVIPCommon.so >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p pluto/lib
	cp libVIPCommon.so $(DESTPATH)pluto/lib
