SNR_CPPFLAGS ?= <-mkr_t_compile_defines->

ROOT = ..
CXXFLAGS = -Wall -ggdb3 -DxAUDIDEMO -fmessage-length=0
CPPFLAGS = -I$(ROOT)/ -I$(ROOT)/DCE -D_REENTRANT -D_GNU_SOURCE $(SNR_CPPFLAGS) -DUSE_LZO_DATAGRID -I/usr/include/libxml2/
LDFLAGS = -L$(ROOT)/lib -lxml2

#LDLIBS = -lpthread -lmysqlclient_r -lattr -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main -lFakeEPG -lqt-mt -lmyth-0.16 -lmythtv-0.16
LDLIBS = -lpthread -lattr -lDCECommon -lSerializeClass -lPlutoUtils -lpluto_main -ldl

#<-mkr_B_use_mysql_wrapper_b->
CPPFLAGS += -I/usr/include/mysql
LDFLAGS += -lmysqlclient_r
#<-mkr_B_use_mysql_wrapper_e->

#<-mkr_b_use_mysql_wrapper_b->
CPPFLAGS += -DUSE_MYSQL_WRAPPER
#<-mkr_b_use_mysql_wrapper_e->

APPNAME=DCERouter
VERSION=2.0.0

sources = \
		CriteriaList.cpp \
		CriteriaNesting.cpp \
		EventHandler_CriteriaList.cpp \
		Main.cpp \
		DCERouter.cpp \
		DCERouter_CreatePluginHardCoded_Impl.cpp \
		../DCE/MessageXML.cpp \
		../db_wrapper/db_wrapper.cpp ../db_wrapper/ClientSocket.cpp ../db_wrapper/Socket.cpp


#sources += ../Gen_Devices/Media_PluginBase.cpp ../Media_Plugin/Media_Plugin.cpp ../Media_Plugin/MediaPluginInfo.cpp
#sources += ../Gen_Devices/Datagrid_PluginBase.cpp ../Datagrid_Plugin/Datagrid_Plugin.cpp
#sources += ../Gen_Devices/MythTV_PlugInBase.cpp  ../MythTV_PlugIn/MythTV_PlugIn.cpp ../MythTV_PlugIn/MythTvEPGWrapper.cpp
#sources += ../Gen_Devices/Xine_PluginBase.cpp ../Xine_Plugin/Xine_Plugin.cpp
#sources += ../Gen_Devices/Telecom_PluginBase.cpp ../Telecom_Plugin/Telecom_Plugin.cpp
#sources += ../Gen_Devices/Security_PluginBase.cpp ../Security_Plugin/Security_Plugin.cpp
#sources += ../Gen_Devices/Climate_PluginBase.cpp ../Climate_Plugin/Climate_Plugin.cpp
#sources += ../Gen_Devices/Lighting_PluginBase.cpp ../Lighting_Plugin/Lighting_Plugin.cpp
#sources += ../Gen_Devices/Mozilla_PluginBase.cpp ../Mozilla_Plugin/Mozilla_Plugin.cpp
#sources += ../Gen_Devices/General_Info_PluginBase.cpp ../General_Info_Plugin/General_Info_Plugin.cpp
#sources += ../Gen_Devices/Orbiter_PluginBase.cpp ../Orbiter_Plugin/Orbiter_Plugin.cpp
#sources += ../Gen_Devices/Media_Grids_PluginBase.cpp ../Media_Grids_Plugin/Media_Grids_Plugin.cpp
#sources += ../Gen_Devices/File_Grids_PluginBase.cpp ../File_Grids_Plugin/File_Grids_Plugin.cpp ../File_Grids_Plugin/FileListOps.cpp



all: DCERouter

DCERouter: $(sources:.cpp=.o) Makefile
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
