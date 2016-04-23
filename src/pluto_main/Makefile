ROOT = ..
CXXFLAGS = -pipe -Wall -O2 -D_GNU_SOURCE -fPIC -g -ggdb3
CPPFLAGS =  -I.. -I../.. -I../DCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DMULTI_THREADED -DNO_SQL_THREAD_LOG
CXX = g++
LDFLAGS = -L$(ROOT)/lib
sources = Database_pluto_main.cpp TableRow.cpp

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq ($(strip $(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources +=  \
Table_Array.cpp \
Table_Broadcast.cpp \
Table_Button.cpp \
Table_CachedScreens.cpp \
Table_CannedEvents.cpp \
Table_CannedEvents_CriteriaParmList.cpp \
Table_City.cpp \
Table_CommMethod.cpp \
Table_Command.cpp \
Table_CommandCategory.cpp \
Table_CommandGroup.cpp \
Table_CommandGroup_Command.cpp \
Table_CommandGroup_Command_CommandParameter.cpp \
Table_CommandGroup_D.cpp \
Table_CommandGroup_D_Command.cpp \
Table_CommandGroup_D_Command_CommandParameter.cpp \
Table_CommandGroup_EntertainArea.cpp \
Table_CommandGroup_Room.cpp \
Table_CommandParameter.cpp \
Table_Command_CommandParameter.cpp \
Table_Command_Pipe.cpp \
Table_ConfigType.cpp \
Table_ConfigType_File.cpp \
Table_ConfigType_Setting.cpp \
Table_ConfigType_Token.cpp \
Table_ConnectorType.cpp \
Table_Country.cpp \
Table_Criteria.cpp \
Table_CriteriaList.cpp \
Table_CriteriaList_CriteriaParmList.cpp \
Table_CriteriaParm.cpp \
Table_CriteriaParmList.cpp \
Table_CriteriaParmNesting.cpp \
Table_DHCPDevice.cpp \
Table_DHCPDevice_DeviceData.cpp \
Table_DataGrid.cpp \
Table_DesignObj.cpp \
Table_DesignObjCategory.cpp \
Table_DesignObjParameter.cpp \
Table_DesignObjType.cpp \
Table_DesignObjType_DesignObjParameter.cpp \
Table_DesignObjVariation.cpp \
Table_DesignObjVariation_DesignObj.cpp \
Table_DesignObjVariation_DesignObjParameter.cpp \
Table_DesignObjVariation_DesignObj_Skin_Language.cpp \
Table_DesignObjVariation_Text.cpp \
Table_DesignObjVariation_Text_Skin_Language.cpp \
Table_DesignObjVariation_Zone.cpp \
Table_Device.cpp \
Table_DeviceCategory.cpp \
Table_DeviceCategory_DeviceData.cpp \
Table_DeviceCategory_Event.cpp \
Table_DeviceCommandGroup.cpp \
Table_DeviceCommandGroup_Command.cpp \
Table_DeviceCommandGroup_DeviceCommandGroup_Parent.cpp \
Table_DeviceData.cpp \
Table_DeviceGroup.cpp \
Table_DeviceTemplate.cpp \
Table_DeviceTemplate_AV.cpp \
Table_DeviceTemplate_DSPMode.cpp \
Table_DeviceTemplate_DesignObj.cpp \
Table_DeviceTemplate_DeviceCategory_ControlledVia.cpp \
Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe.cpp \
Table_DeviceTemplate_DeviceCommandGroup.cpp \
Table_DeviceTemplate_DeviceData.cpp \
Table_DeviceTemplate_DeviceTemplate_ControlledVia.cpp \
Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe.cpp \
Table_DeviceTemplate_DeviceTemplate_Related.cpp \
Table_DeviceTemplate_Event.cpp \
Table_DeviceTemplate_InfraredGroup.cpp \
Table_DeviceTemplate_Input.cpp \
Table_DeviceTemplate_MediaType.cpp \
Table_DeviceTemplate_MediaType_DesignObj.cpp \
Table_DeviceTemplate_Output.cpp \
Table_DeviceTemplate_PageSetup.cpp \
Table_Device_Command.cpp \
Table_Device_CommandGroup.cpp \
Table_Device_DeviceData.cpp \
Table_Device_DeviceGroup.cpp \
Table_Device_Device_Pipe.cpp \
Table_Device_Device_Related.cpp \
Table_Device_EntertainArea.cpp \
Table_Device_HouseMode.cpp \
Table_Device_MRU.cpp \
Table_Device_Orbiter.cpp \
Table_Device_QuickStart.cpp \
Table_Device_StartupScript.cpp \
Table_Device_Users.cpp \
Table_Direction.cpp \
Table_Directory.cpp \
Table_Distro.cpp \
Table_Document.cpp \
Table_Document_Comment.cpp \
Table_Effect.cpp \
Table_EffectType.cpp \
Table_EffectType_Effect_Skin.cpp \
Table_EntertainArea.cpp \
Table_Event.cpp \
Table_EventCategory.cpp \
Table_EventHandler.cpp \
Table_EventParameter.cpp \
Table_Event_EventParameter.cpp \
Table_FAQ.cpp \
Table_Firewall.cpp \
Table_Floorplan.cpp \
Table_FloorplanObjectType.cpp \
Table_FloorplanObjectType_Color.cpp \
Table_FloorplanType.cpp \
Table_HorizAlignment.cpp \
Table_HouseMode.cpp \
Table_Household.cpp \
Table_Household_Installation.cpp \
Table_Icon.cpp \
Table_Image.cpp \
Table_InfraredGroup.cpp \
Table_InfraredGroup_Command.cpp \
Table_InfraredGroup_Command_Preferred.cpp \
Table_InstallWizard.cpp \
Table_InstallWizard_Distro.cpp \
Table_Installation.cpp \
Table_Installation_RepositorySource_URL.cpp \
Table_Installation_Users.cpp \
Table_Language.cpp \
Table_License.cpp \
Table_Licensing.cpp \
Table_Manufacturer.cpp \
Table_MediaType.cpp \
Table_MediaType_Broadcast.cpp \
Table_MediaType_DesignObj.cpp \
Table_News.cpp \
Table_OperatingSystem.cpp \
Table_Orbiter.cpp \
Table_Orbiter_Users_PasswordReq.cpp \
Table_Orbiter_Variable.cpp \
Table_Package.cpp \
Table_PackageType.cpp \
Table_Package_Compat.cpp \
Table_Package_Device.cpp \
Table_Package_Directory.cpp \
Table_Package_Directory_File.cpp \
Table_Package_Package.cpp \
Table_Package_Source.cpp \
Table_Package_Source_Compat.cpp \
Table_Package_Users.cpp \
Table_Package_Version.cpp \
Table_PageSetup.cpp \
Table_PaidLicense.cpp \
Table_ParameterType.cpp \
Table_PhoneLineType.cpp \
Table_Pipe.cpp \
Table_PnpLevel.cpp \
Table_PnpProtocol.cpp \
Table_PnpQueue.cpp \
Table_PostalCode.cpp \
Table_QuickStartCategory.cpp \
Table_QuickStartTemplate.cpp \
Table_Region.cpp \
Table_RemoteControl.cpp \
Table_RemoteMapping.cpp \
Table_RepositorySource.cpp \
Table_RepositorySource_URL.cpp \
Table_RepositoryType.cpp \
Table_Room.cpp \
Table_RoomType.cpp \
Table_Room_Users.cpp \
Table_Schema.cpp \
Table_Screen.cpp \
Table_Screen_CommandParameter.cpp \
Table_Screen_DesignObj.cpp \
Table_SetupStep.cpp \
Table_Size.cpp \
Table_Skin.cpp \
Table_Software.cpp \
Table_Software_Device.cpp \
Table_Software_Source.cpp \
Table_StabilityStatus.cpp \
Table_StartupScript.cpp \
Table_Style.cpp \
Table_StyleVariation.cpp \
Table_System.cpp \
Table_Template.cpp \
Table_Text.cpp \
Table_TextCategory.cpp \
Table_Text_LS.cpp \
Table_Text_LS_AltVersions.cpp \
Table_TimeZone.cpp \
Table_UI.cpp \
Table_UnknownDevices.cpp \
Table_UserMode.cpp \
Table_Users.cpp \
Table_Variable.cpp \
Table_Version.cpp \
Table_VertAlignment.cpp \
Table_psc_constants_batdet.cpp \
Table_psc_constants_bathdr.cpp \
Table_psc_constants_batuser.cpp \
Table_psc_constants_repset.cpp \
Table_psc_constants_schema.cpp \
Table_psc_constants_tables.cpp \
Table_psc_dce_batdet.cpp \
Table_psc_dce_bathdr.cpp \
Table_psc_dce_batuser.cpp \
Table_psc_dce_repset.cpp \
Table_psc_dce_schema.cpp \
Table_psc_dce_tables.cpp \
Table_psc_designer_batdet.cpp \
Table_psc_designer_bathdr.cpp \
Table_psc_designer_batuser.cpp \
Table_psc_designer_repset.cpp \
Table_psc_designer_schema.cpp \
Table_psc_designer_tables.cpp \
Table_psc_document_batdet.cpp \
Table_psc_document_bathdr.cpp \
Table_psc_document_batuser.cpp \
Table_psc_document_repset.cpp \
Table_psc_document_schema.cpp \
Table_psc_document_tables.cpp \
Table_psc_ir_batdet.cpp \
Table_psc_ir_bathdr.cpp \
Table_psc_ir_batuser.cpp \
Table_psc_ir_repset.cpp \
Table_psc_ir_schema.cpp \
Table_psc_ir_tables.cpp \
Table_psc_local_batdet.cpp \
Table_psc_local_bathdr.cpp \
Table_psc_local_batuser.cpp \
Table_psc_local_repset.cpp \
Table_psc_local_schema.cpp \
Table_psc_local_tables.cpp \
Table_psc_website_batdet.cpp \
Table_psc_website_bathdr.cpp \
Table_psc_website_batuser.cpp \
Table_psc_website_repset.cpp \
Table_psc_website_schema.cpp \
Table_psc_website_tables.cpp

all: libpluto_main.so all-recursive

libpluto_main.so: $(sources:.cpp=.o)
	$(CXX) -shared $(CXXFLAGS) $(CPPFLAGS) -o $@ $(call cpath,$+)
	cp $@ ../lib

clean: clean-recursive
	rm -f *.o *.d libpluto_main.so >/dev/null

install:
	mkdir -p $(DESTDIR)/usr/pluto/bin
	cp $@ /usr/pluto/bin

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)
