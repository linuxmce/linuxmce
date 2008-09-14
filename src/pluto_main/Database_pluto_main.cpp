#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_main.h"


#include "DCEConfig.h"


using namespace DCE;
Database_pluto_main::Database_pluto_main(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblArray=NULL;
tblArray_pschist=NULL;
tblArray_pschmask=NULL;
tblBroadcast=NULL;
tblBroadcast_pschist=NULL;
tblBroadcast_pschmask=NULL;
tblButton=NULL;
tblButton_pschist=NULL;
tblButton_pschmask=NULL;
tblCachedScreens=NULL;
tblCachedScreens_pschist=NULL;
tblCachedScreens_pschmask=NULL;
tblCannedEvents=NULL;
tblCannedEvents_CriteriaParmList=NULL;
tblCannedEvents_CriteriaParmList_pschist=NULL;
tblCannedEvents_CriteriaParmList_pschmask=NULL;
tblCannedEvents_pschist=NULL;
tblCannedEvents_pschmask=NULL;
tblCity=NULL;
tblCommMethod=NULL;
tblCommMethod_pschist=NULL;
tblCommMethod_pschmask=NULL;
tblCommand=NULL;
tblCommandCategory=NULL;
tblCommandCategory_pschist=NULL;
tblCommandCategory_pschmask=NULL;
tblCommandGroup=NULL;
tblCommandGroup_Command=NULL;
tblCommandGroup_Command_CommandParameter=NULL;
tblCommandGroup_Command_CommandParameter_pschist=NULL;
tblCommandGroup_Command_CommandParameter_pschmask=NULL;
tblCommandGroup_Command_pschist=NULL;
tblCommandGroup_Command_pschmask=NULL;
tblCommandGroup_D=NULL;
tblCommandGroup_D_Command=NULL;
tblCommandGroup_D_Command_CommandParameter=NULL;
tblCommandGroup_D_Command_CommandParameter_pschist=NULL;
tblCommandGroup_D_Command_CommandParameter_pschmask=NULL;
tblCommandGroup_D_Command_pschist=NULL;
tblCommandGroup_D_Command_pschmask=NULL;
tblCommandGroup_D_pschist=NULL;
tblCommandGroup_D_pschmask=NULL;
tblCommandGroup_EntertainArea=NULL;
tblCommandGroup_EntertainArea_pschist=NULL;
tblCommandGroup_EntertainArea_pschmask=NULL;
tblCommandGroup_Room=NULL;
tblCommandGroup_Room_pschist=NULL;
tblCommandGroup_Room_pschmask=NULL;
tblCommandGroup_pschist=NULL;
tblCommandGroup_pschmask=NULL;
tblCommandParameter=NULL;
tblCommandParameter_pschist=NULL;
tblCommandParameter_pschmask=NULL;
tblCommand_CommandParameter=NULL;
tblCommand_CommandParameter_pschist=NULL;
tblCommand_CommandParameter_pschmask=NULL;
tblCommand_Pipe=NULL;
tblCommand_Pipe_pschist=NULL;
tblCommand_Pipe_pschmask=NULL;
tblCommand_pschist=NULL;
tblCommand_pschmask=NULL;
tblConfigType=NULL;
tblConfigType_File=NULL;
tblConfigType_File_pschist=NULL;
tblConfigType_File_pschmask=NULL;
tblConfigType_Setting=NULL;
tblConfigType_Setting_pschist=NULL;
tblConfigType_Setting_pschmask=NULL;
tblConfigType_Token=NULL;
tblConfigType_Token_pschist=NULL;
tblConfigType_Token_pschmask=NULL;
tblConfigType_pschist=NULL;
tblConfigType_pschmask=NULL;
tblConnectorType=NULL;
tblConnectorType_pschist=NULL;
tblConnectorType_pschmask=NULL;
tblCountry=NULL;
tblCriteria=NULL;
tblCriteriaList=NULL;
tblCriteriaList_CriteriaParmList=NULL;
tblCriteriaList_CriteriaParmList_pschist=NULL;
tblCriteriaList_CriteriaParmList_pschmask=NULL;
tblCriteriaList_pschist=NULL;
tblCriteriaList_pschmask=NULL;
tblCriteriaParm=NULL;
tblCriteriaParmList=NULL;
tblCriteriaParmList_pschist=NULL;
tblCriteriaParmList_pschmask=NULL;
tblCriteriaParmNesting=NULL;
tblCriteriaParmNesting_pschist=NULL;
tblCriteriaParmNesting_pschmask=NULL;
tblCriteriaParm_pschist=NULL;
tblCriteriaParm_pschmask=NULL;
tblCriteria_pschist=NULL;
tblCriteria_pschmask=NULL;
tblDHCPDevice=NULL;
tblDHCPDevice_DeviceData=NULL;
tblDHCPDevice_DeviceData_pschist=NULL;
tblDHCPDevice_DeviceData_pschmask=NULL;
tblDHCPDevice_pschist=NULL;
tblDHCPDevice_pschmask=NULL;
tblDataGrid=NULL;
tblDataGrid_pschist=NULL;
tblDataGrid_pschmask=NULL;
tblDesignObj=NULL;
tblDesignObjCategory=NULL;
tblDesignObjCategory_pschist=NULL;
tblDesignObjCategory_pschmask=NULL;
tblDesignObjParameter=NULL;
tblDesignObjParameter_pschist=NULL;
tblDesignObjParameter_pschmask=NULL;
tblDesignObjType=NULL;
tblDesignObjType_DesignObjParameter=NULL;
tblDesignObjType_DesignObjParameter_pschist=NULL;
tblDesignObjType_DesignObjParameter_pschmask=NULL;
tblDesignObjType_pschist=NULL;
tblDesignObjType_pschmask=NULL;
tblDesignObjVariation=NULL;
tblDesignObjVariation_DesignObj=NULL;
tblDesignObjVariation_DesignObjParameter=NULL;
tblDesignObjVariation_DesignObjParameter_pschist=NULL;
tblDesignObjVariation_DesignObjParameter_pschmask=NULL;
tblDesignObjVariation_DesignObj_Skin_Language=NULL;
tblDesignObjVariation_DesignObj_Skin_Language_pschist=NULL;
tblDesignObjVariation_DesignObj_Skin_Language_pschmask=NULL;
tblDesignObjVariation_DesignObj_pschist=NULL;
tblDesignObjVariation_DesignObj_pschmask=NULL;
tblDesignObjVariation_Text=NULL;
tblDesignObjVariation_Text_Skin_Language=NULL;
tblDesignObjVariation_Text_Skin_Language_pschist=NULL;
tblDesignObjVariation_Text_Skin_Language_pschmask=NULL;
tblDesignObjVariation_Text_pschist=NULL;
tblDesignObjVariation_Text_pschmask=NULL;
tblDesignObjVariation_Zone=NULL;
tblDesignObjVariation_Zone_pschist=NULL;
tblDesignObjVariation_Zone_pschmask=NULL;
tblDesignObjVariation_pschist=NULL;
tblDesignObjVariation_pschmask=NULL;
tblDesignObj_pschist=NULL;
tblDesignObj_pschmask=NULL;
tblDevice=NULL;
tblDeviceCategory=NULL;
tblDeviceCategory_DeviceData=NULL;
tblDeviceCategory_DeviceData_pschist=NULL;
tblDeviceCategory_DeviceData_pschmask=NULL;
tblDeviceCategory_Event=NULL;
tblDeviceCategory_Event_pschist=NULL;
tblDeviceCategory_Event_pschmask=NULL;
tblDeviceCategory_pschist=NULL;
tblDeviceCategory_pschmask=NULL;
tblDeviceCommandGroup=NULL;
tblDeviceCommandGroup_Command=NULL;
tblDeviceCommandGroup_Command_pschist=NULL;
tblDeviceCommandGroup_Command_pschmask=NULL;
tblDeviceCommandGroup_DeviceCommandGroup_Parent=NULL;
tblDeviceCommandGroup_DeviceCommandGroup_Parent_pschist=NULL;
tblDeviceCommandGroup_DeviceCommandGroup_Parent_pschmask=NULL;
tblDeviceCommandGroup_pschist=NULL;
tblDeviceCommandGroup_pschmask=NULL;
tblDeviceData=NULL;
tblDeviceData_pschist=NULL;
tblDeviceData_pschmask=NULL;
tblDeviceGroup=NULL;
tblDeviceGroup_pschist=NULL;
tblDeviceGroup_pschmask=NULL;
tblDeviceTemplate=NULL;
tblDeviceTemplate_AV=NULL;
tblDeviceTemplate_AV_pschist=NULL;
tblDeviceTemplate_AV_pschmask=NULL;
tblDeviceTemplate_DSPMode=NULL;
tblDeviceTemplate_DSPMode_pschist=NULL;
tblDeviceTemplate_DSPMode_pschmask=NULL;
tblDeviceTemplate_DesignObj=NULL;
tblDeviceTemplate_DesignObj_pschist=NULL;
tblDeviceTemplate_DesignObj_pschmask=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschist=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschmask=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_pschist=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_pschmask=NULL;
tblDeviceTemplate_DeviceCommandGroup=NULL;
tblDeviceTemplate_DeviceCommandGroup_pschist=NULL;
tblDeviceTemplate_DeviceCommandGroup_pschmask=NULL;
tblDeviceTemplate_DeviceData=NULL;
tblDeviceTemplate_DeviceData_pschist=NULL;
tblDeviceTemplate_DeviceData_pschmask=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschist=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschmask=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_pschist=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_pschmask=NULL;
tblDeviceTemplate_DeviceTemplate_Related=NULL;
tblDeviceTemplate_DeviceTemplate_Related_pschist=NULL;
tblDeviceTemplate_DeviceTemplate_Related_pschmask=NULL;
tblDeviceTemplate_Event=NULL;
tblDeviceTemplate_Event_pschist=NULL;
tblDeviceTemplate_Event_pschmask=NULL;
tblDeviceTemplate_InfraredGroup=NULL;
tblDeviceTemplate_InfraredGroup_pschist=NULL;
tblDeviceTemplate_InfraredGroup_pschmask=NULL;
tblDeviceTemplate_Input=NULL;
tblDeviceTemplate_Input_pschist=NULL;
tblDeviceTemplate_Input_pschmask=NULL;
tblDeviceTemplate_MediaType=NULL;
tblDeviceTemplate_MediaType_DesignObj=NULL;
tblDeviceTemplate_MediaType_DesignObj_pschist=NULL;
tblDeviceTemplate_MediaType_DesignObj_pschmask=NULL;
tblDeviceTemplate_MediaType_pschist=NULL;
tblDeviceTemplate_MediaType_pschmask=NULL;
tblDeviceTemplate_Output=NULL;
tblDeviceTemplate_Output_pschist=NULL;
tblDeviceTemplate_Output_pschmask=NULL;
tblDeviceTemplate_PageSetup=NULL;
tblDeviceTemplate_PageSetup_pschist=NULL;
tblDeviceTemplate_PageSetup_pschmask=NULL;
tblDeviceTemplate_pschist=NULL;
tblDeviceTemplate_pschmask=NULL;
tblDevice_Command=NULL;
tblDevice_CommandGroup=NULL;
tblDevice_CommandGroup_pschist=NULL;
tblDevice_CommandGroup_pschmask=NULL;
tblDevice_Command_pschist=NULL;
tblDevice_Command_pschmask=NULL;
tblDevice_DeviceData=NULL;
tblDevice_DeviceData_pschist=NULL;
tblDevice_DeviceData_pschmask=NULL;
tblDevice_DeviceGroup=NULL;
tblDevice_DeviceGroup_pschist=NULL;
tblDevice_DeviceGroup_pschmask=NULL;
tblDevice_Device_Pipe=NULL;
tblDevice_Device_Pipe_pschist=NULL;
tblDevice_Device_Pipe_pschmask=NULL;
tblDevice_Device_Related=NULL;
tblDevice_Device_Related_pschist=NULL;
tblDevice_Device_Related_pschmask=NULL;
tblDevice_EntertainArea=NULL;
tblDevice_EntertainArea_pschist=NULL;
tblDevice_EntertainArea_pschmask=NULL;
tblDevice_HouseMode=NULL;
tblDevice_HouseMode_pschist=NULL;
tblDevice_HouseMode_pschmask=NULL;
tblDevice_MRU=NULL;
tblDevice_MRU_pschist=NULL;
tblDevice_MRU_pschmask=NULL;
tblDevice_Orbiter=NULL;
tblDevice_Orbiter_pschist=NULL;
tblDevice_Orbiter_pschmask=NULL;
tblDevice_QuickStart=NULL;
tblDevice_QuickStart_pschist=NULL;
tblDevice_QuickStart_pschmask=NULL;
tblDevice_StartupScript=NULL;
tblDevice_StartupScript_pschist=NULL;
tblDevice_StartupScript_pschmask=NULL;
tblDevice_Users=NULL;
tblDevice_Users_pschist=NULL;
tblDevice_Users_pschmask=NULL;
tblDevice_pschist=NULL;
tblDevice_pschmask=NULL;
tblDirection=NULL;
tblDirection_pschist=NULL;
tblDirection_pschmask=NULL;
tblDirectory=NULL;
tblDirectory_pschist=NULL;
tblDirectory_pschmask=NULL;
tblDistro=NULL;
tblDistro_pschist=NULL;
tblDistro_pschmask=NULL;
tblDocument=NULL;
tblDocument_Comment=NULL;
tblDocument_Comment_pschist=NULL;
tblDocument_Comment_pschmask=NULL;
tblDocument_pschist=NULL;
tblDocument_pschmask=NULL;
tblEffect=NULL;
tblEffectType=NULL;
tblEffectType_Effect_Skin=NULL;
tblEffectType_Effect_Skin_pschist=NULL;
tblEffectType_Effect_Skin_pschmask=NULL;
tblEffectType_pschist=NULL;
tblEffectType_pschmask=NULL;
tblEffect_pschist=NULL;
tblEffect_pschmask=NULL;
tblEntertainArea=NULL;
tblEntertainArea_pschist=NULL;
tblEntertainArea_pschmask=NULL;
tblEvent=NULL;
tblEventCategory=NULL;
tblEventCategory_pschist=NULL;
tblEventCategory_pschmask=NULL;
tblEventHandler=NULL;
tblEventHandler_pschist=NULL;
tblEventHandler_pschmask=NULL;
tblEventParameter=NULL;
tblEventParameter_pschist=NULL;
tblEventParameter_pschmask=NULL;
tblEvent_EventParameter=NULL;
tblEvent_EventParameter_pschist=NULL;
tblEvent_EventParameter_pschmask=NULL;
tblEvent_pschist=NULL;
tblEvent_pschmask=NULL;
tblFAQ=NULL;
tblFAQ_pschist=NULL;
tblFAQ_pschmask=NULL;
tblFirewall=NULL;
tblFirewall_pschist=NULL;
tblFirewall_pschmask=NULL;
tblFloorplan=NULL;
tblFloorplanObjectType=NULL;
tblFloorplanObjectType_Color=NULL;
tblFloorplanObjectType_Color_pschist=NULL;
tblFloorplanObjectType_Color_pschmask=NULL;
tblFloorplanObjectType_pschist=NULL;
tblFloorplanObjectType_pschmask=NULL;
tblFloorplanType=NULL;
tblFloorplanType_pschist=NULL;
tblFloorplanType_pschmask=NULL;
tblFloorplan_pschist=NULL;
tblFloorplan_pschmask=NULL;
tblHorizAlignment=NULL;
tblHorizAlignment_pschist=NULL;
tblHorizAlignment_pschmask=NULL;
tblHouseMode=NULL;
tblHouseMode_pschist=NULL;
tblHouseMode_pschmask=NULL;
tblHousehold=NULL;
tblHousehold_Installation=NULL;
tblHousehold_Installation_pschist=NULL;
tblHousehold_Installation_pschmask=NULL;
tblHousehold_pschist=NULL;
tblHousehold_pschmask=NULL;
tblIcon=NULL;
tblIcon_pschist=NULL;
tblIcon_pschmask=NULL;
tblImage=NULL;
tblImage_pschist=NULL;
tblImage_pschmask=NULL;
tblInfraredGroup=NULL;
tblInfraredGroup_Command=NULL;
tblInfraredGroup_Command_Preferred=NULL;
tblInfraredGroup_Command_Preferred_pschist=NULL;
tblInfraredGroup_Command_Preferred_pschmask=NULL;
tblInfraredGroup_Command_pschist=NULL;
tblInfraredGroup_Command_pschmask=NULL;
tblInfraredGroup_pschist=NULL;
tblInfraredGroup_pschmask=NULL;
tblInstallWizard=NULL;
tblInstallWizard_Distro=NULL;
tblInstallWizard_Distro_pschist=NULL;
tblInstallWizard_Distro_pschmask=NULL;
tblInstallWizard_pschist=NULL;
tblInstallWizard_pschmask=NULL;
tblInstallation=NULL;
tblInstallation_RepositorySource_URL=NULL;
tblInstallation_RepositorySource_URL_pschist=NULL;
tblInstallation_RepositorySource_URL_pschmask=NULL;
tblInstallation_Users=NULL;
tblInstallation_Users_pschist=NULL;
tblInstallation_Users_pschmask=NULL;
tblInstallation_pschist=NULL;
tblInstallation_pschmask=NULL;
tblLanguage=NULL;
tblLanguage_pschist=NULL;
tblLanguage_pschmask=NULL;
tblLicense=NULL;
tblLicense_pschist=NULL;
tblLicense_pschmask=NULL;
tblLicensing=NULL;
tblLicensing_pschist=NULL;
tblLicensing_pschmask=NULL;
tblManufacturer=NULL;
tblManufacturer_pschist=NULL;
tblManufacturer_pschmask=NULL;
tblMediaType=NULL;
tblMediaType_Broadcast=NULL;
tblMediaType_Broadcast_pschist=NULL;
tblMediaType_Broadcast_pschmask=NULL;
tblMediaType_DesignObj=NULL;
tblMediaType_DesignObj_pschist=NULL;
tblMediaType_DesignObj_pschmask=NULL;
tblMediaType_pschist=NULL;
tblMediaType_pschmask=NULL;
tblNews=NULL;
tblNews_pschist=NULL;
tblNews_pschmask=NULL;
tblOperatingSystem=NULL;
tblOperatingSystem_pschist=NULL;
tblOperatingSystem_pschmask=NULL;
tblOrbiter=NULL;
tblOrbiter_Users_PasswordReq=NULL;
tblOrbiter_Users_PasswordReq_pschist=NULL;
tblOrbiter_Users_PasswordReq_pschmask=NULL;
tblOrbiter_Variable=NULL;
tblOrbiter_Variable_pschist=NULL;
tblOrbiter_Variable_pschmask=NULL;
tblOrbiter_pschist=NULL;
tblOrbiter_pschmask=NULL;
tblPackage=NULL;
tblPackageType=NULL;
tblPackageType_pschist=NULL;
tblPackageType_pschmask=NULL;
tblPackage_Compat=NULL;
tblPackage_Compat_pschist=NULL;
tblPackage_Compat_pschmask=NULL;
tblPackage_Device=NULL;
tblPackage_Device_pschist=NULL;
tblPackage_Device_pschmask=NULL;
tblPackage_Directory=NULL;
tblPackage_Directory_File=NULL;
tblPackage_Directory_File_pschist=NULL;
tblPackage_Directory_File_pschmask=NULL;
tblPackage_Directory_pschist=NULL;
tblPackage_Directory_pschmask=NULL;
tblPackage_Package=NULL;
tblPackage_Package_pschist=NULL;
tblPackage_Package_pschmask=NULL;
tblPackage_Source=NULL;
tblPackage_Source_Compat=NULL;
tblPackage_Source_Compat_pschist=NULL;
tblPackage_Source_Compat_pschmask=NULL;
tblPackage_Source_pschist=NULL;
tblPackage_Source_pschmask=NULL;
tblPackage_Users=NULL;
tblPackage_Users_pschist=NULL;
tblPackage_Users_pschmask=NULL;
tblPackage_Version=NULL;
tblPackage_Version_pschist=NULL;
tblPackage_Version_pschmask=NULL;
tblPackage_pschist=NULL;
tblPackage_pschmask=NULL;
tblPageSetup=NULL;
tblPageSetup_pschist=NULL;
tblPageSetup_pschmask=NULL;
tblPaidLicense=NULL;
tblPaidLicense_pschist=NULL;
tblPaidLicense_pschmask=NULL;
tblParameterType=NULL;
tblParameterType_pschist=NULL;
tblParameterType_pschmask=NULL;
tblPhoneLineType=NULL;
tblPhoneLineType_pschist=NULL;
tblPhoneLineType_pschmask=NULL;
tblPipe=NULL;
tblPipe_pschist=NULL;
tblPipe_pschmask=NULL;
tblPnpLevel=NULL;
tblPnpLevel_pschist=NULL;
tblPnpLevel_pschmask=NULL;
tblPnpProtocol=NULL;
tblPnpProtocol_pschist=NULL;
tblPnpProtocol_pschmask=NULL;
tblPnpQueue=NULL;
tblPostalCode=NULL;
tblQuickStartCategory=NULL;
tblQuickStartCategory_pschist=NULL;
tblQuickStartCategory_pschmask=NULL;
tblQuickStartTemplate=NULL;
tblQuickStartTemplate_pschist=NULL;
tblQuickStartTemplate_pschmask=NULL;
tblRegion=NULL;
tblRemoteControl=NULL;
tblRemoteControl_pschist=NULL;
tblRemoteControl_pschmask=NULL;
tblRemoteMapping=NULL;
tblRemoteMapping_pschist=NULL;
tblRemoteMapping_pschmask=NULL;
tblRepositorySource=NULL;
tblRepositorySource_URL=NULL;
tblRepositorySource_URL_pschist=NULL;
tblRepositorySource_URL_pschmask=NULL;
tblRepositorySource_pschist=NULL;
tblRepositorySource_pschmask=NULL;
tblRepositoryType=NULL;
tblRepositoryType_pschist=NULL;
tblRepositoryType_pschmask=NULL;
tblRoom=NULL;
tblRoomType=NULL;
tblRoomType_pschist=NULL;
tblRoomType_pschmask=NULL;
tblRoom_Users=NULL;
tblRoom_Users_pschist=NULL;
tblRoom_Users_pschmask=NULL;
tblRoom_pschist=NULL;
tblRoom_pschmask=NULL;
tblSchema=NULL;
tblSchema_pschist=NULL;
tblSchema_pschmask=NULL;
tblScreen=NULL;
tblScreen_CommandParameter=NULL;
tblScreen_CommandParameter_pschist=NULL;
tblScreen_CommandParameter_pschmask=NULL;
tblScreen_DesignObj=NULL;
tblScreen_DesignObj_pschist=NULL;
tblScreen_DesignObj_pschmask=NULL;
tblScreen_pschist=NULL;
tblScreen_pschmask=NULL;
tblSetupStep=NULL;
tblSetupStep_pschist=NULL;
tblSetupStep_pschmask=NULL;
tblSize=NULL;
tblSize_pschist=NULL;
tblSize_pschmask=NULL;
tblSkin=NULL;
tblSkin_pschist=NULL;
tblSkin_pschmask=NULL;
tblSoftware=NULL;
tblSoftware_Device=NULL;
tblSoftware_Source=NULL;
tblStabilityStatus=NULL;
tblStabilityStatus_pschist=NULL;
tblStabilityStatus_pschmask=NULL;
tblStartupScript=NULL;
tblStartupScript_pschist=NULL;
tblStartupScript_pschmask=NULL;
tblStyle=NULL;
tblStyleVariation=NULL;
tblStyleVariation_pschist=NULL;
tblStyleVariation_pschmask=NULL;
tblStyle_pschist=NULL;
tblStyle_pschmask=NULL;
tblSystem=NULL;
tblSystem_pschist=NULL;
tblSystem_pschmask=NULL;
tblTemplate=NULL;
tblTemplate_pschist=NULL;
tblTemplate_pschmask=NULL;
tblText=NULL;
tblTextCategory=NULL;
tblTextCategory_pschist=NULL;
tblTextCategory_pschmask=NULL;
tblText_LS=NULL;
tblText_LS_AltVersions=NULL;
tblText_LS_AltVersions_pschist=NULL;
tblText_LS_AltVersions_pschmask=NULL;
tblText_LS_pschist=NULL;
tblText_LS_pschmask=NULL;
tblText_pschist=NULL;
tblText_pschmask=NULL;
tblTimeZone=NULL;
tblUI=NULL;
tblUI_pschist=NULL;
tblUI_pschmask=NULL;
tblUnknownDevices=NULL;
tblUnknownDevices_pschist=NULL;
tblUnknownDevices_pschmask=NULL;
tblUserMode=NULL;
tblUserMode_pschist=NULL;
tblUserMode_pschmask=NULL;
tblUsers=NULL;
tblUsers_pschist=NULL;
tblUsers_pschmask=NULL;
tblVariable=NULL;
tblVariable_pschist=NULL;
tblVariable_pschmask=NULL;
tblVersion=NULL;
tblVersion_pschist=NULL;
tblVersion_pschmask=NULL;
tblVertAlignment=NULL;
tblVertAlignment_pschist=NULL;
tblVertAlignment_pschmask=NULL;
tblpsc_constants_batdet=NULL;
tblpsc_constants_bathdr=NULL;
tblpsc_constants_batuser=NULL;
tblpsc_constants_repset=NULL;
tblpsc_constants_schema=NULL;
tblpsc_constants_tables=NULL;
tblpsc_dce_batdet=NULL;
tblpsc_dce_bathdr=NULL;
tblpsc_dce_batuser=NULL;
tblpsc_dce_repset=NULL;
tblpsc_dce_schema=NULL;
tblpsc_dce_tables=NULL;
tblpsc_designer_batdet=NULL;
tblpsc_designer_bathdr=NULL;
tblpsc_designer_batuser=NULL;
tblpsc_designer_repset=NULL;
tblpsc_designer_schema=NULL;
tblpsc_designer_tables=NULL;
tblpsc_document_batdet=NULL;
tblpsc_document_bathdr=NULL;
tblpsc_document_batuser=NULL;
tblpsc_document_repset=NULL;
tblpsc_document_schema=NULL;
tblpsc_document_tables=NULL;
tblpsc_ir_batdet=NULL;
tblpsc_ir_bathdr=NULL;
tblpsc_ir_batuser=NULL;
tblpsc_ir_repset=NULL;
tblpsc_ir_schema=NULL;
tblpsc_ir_tables=NULL;
tblpsc_local_batdet=NULL;
tblpsc_local_bathdr=NULL;
tblpsc_local_batuser=NULL;
tblpsc_local_repset=NULL;
tblpsc_local_schema=NULL;
tblpsc_local_tables=NULL;
tblpsc_website_batdet=NULL;
tblpsc_website_bathdr=NULL;
tblpsc_website_batuser=NULL;
tblpsc_website_repset=NULL;
tblpsc_website_schema=NULL;
tblpsc_website_tables=NULL;
}

Database_pluto_main::~Database_pluto_main()
{
	DeleteAllTables();
}

bool Database_pluto_main::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblArray!=NULL )
	if( !Commit_Array(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblArray_pschist!=NULL )
	if( !Commit_Array_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblArray_pschmask!=NULL )
	if( !Commit_Array_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblBroadcast!=NULL )
	if( !Commit_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblBroadcast_pschist!=NULL )
	if( !Commit_Broadcast_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblBroadcast_pschmask!=NULL )
	if( !Commit_Broadcast_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblButton!=NULL )
	if( !Commit_Button(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblButton_pschist!=NULL )
	if( !Commit_Button_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblButton_pschmask!=NULL )
	if( !Commit_Button_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCachedScreens!=NULL )
	if( !Commit_CachedScreens(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCachedScreens_pschist!=NULL )
	if( !Commit_CachedScreens_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCachedScreens_pschmask!=NULL )
	if( !Commit_CachedScreens_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents!=NULL )
	if( !Commit_CannedEvents(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_CriteriaParmList!=NULL )
	if( !Commit_CannedEvents_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_CriteriaParmList_pschist!=NULL )
	if( !Commit_CannedEvents_CriteriaParmList_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_CriteriaParmList_pschmask!=NULL )
	if( !Commit_CannedEvents_CriteriaParmList_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_pschist!=NULL )
	if( !Commit_CannedEvents_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_pschmask!=NULL )
	if( !Commit_CannedEvents_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCity!=NULL )
	if( !Commit_City(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommMethod!=NULL )
	if( !Commit_CommMethod(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommMethod_pschist!=NULL )
	if( !Commit_CommMethod_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommMethod_pschmask!=NULL )
	if( !Commit_CommMethod_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand!=NULL )
	if( !Commit_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandCategory!=NULL )
	if( !Commit_CommandCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandCategory_pschist!=NULL )
	if( !Commit_CommandCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandCategory_pschmask!=NULL )
	if( !Commit_CommandCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup!=NULL )
	if( !Commit_CommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command!=NULL )
	if( !Commit_CommandGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command_CommandParameter!=NULL )
	if( !Commit_CommandGroup_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command_CommandParameter_pschist!=NULL )
	if( !Commit_CommandGroup_Command_CommandParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command_CommandParameter_pschmask!=NULL )
	if( !Commit_CommandGroup_Command_CommandParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command_pschist!=NULL )
	if( !Commit_CommandGroup_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Command_pschmask!=NULL )
	if( !Commit_CommandGroup_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D!=NULL )
	if( !Commit_CommandGroup_D(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command!=NULL )
	if( !Commit_CommandGroup_D_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_CommandParameter!=NULL )
	if( !Commit_CommandGroup_D_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_CommandParameter_pschist!=NULL )
	if( !Commit_CommandGroup_D_Command_CommandParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_CommandParameter_pschmask!=NULL )
	if( !Commit_CommandGroup_D_Command_CommandParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_pschist!=NULL )
	if( !Commit_CommandGroup_D_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_pschmask!=NULL )
	if( !Commit_CommandGroup_D_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_pschist!=NULL )
	if( !Commit_CommandGroup_D_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_pschmask!=NULL )
	if( !Commit_CommandGroup_D_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_EntertainArea!=NULL )
	if( !Commit_CommandGroup_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_EntertainArea_pschist!=NULL )
	if( !Commit_CommandGroup_EntertainArea_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_EntertainArea_pschmask!=NULL )
	if( !Commit_CommandGroup_EntertainArea_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Room!=NULL )
	if( !Commit_CommandGroup_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Room_pschist!=NULL )
	if( !Commit_CommandGroup_Room_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Room_pschmask!=NULL )
	if( !Commit_CommandGroup_Room_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_pschist!=NULL )
	if( !Commit_CommandGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_pschmask!=NULL )
	if( !Commit_CommandGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandParameter!=NULL )
	if( !Commit_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandParameter_pschist!=NULL )
	if( !Commit_CommandParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandParameter_pschmask!=NULL )
	if( !Commit_CommandParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_CommandParameter!=NULL )
	if( !Commit_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_CommandParameter_pschist!=NULL )
	if( !Commit_Command_CommandParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_CommandParameter_pschmask!=NULL )
	if( !Commit_Command_CommandParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_Pipe!=NULL )
	if( !Commit_Command_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_Pipe_pschist!=NULL )
	if( !Commit_Command_Pipe_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_Pipe_pschmask!=NULL )
	if( !Commit_Command_Pipe_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_pschist!=NULL )
	if( !Commit_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_pschmask!=NULL )
	if( !Commit_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType!=NULL )
	if( !Commit_ConfigType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_File!=NULL )
	if( !Commit_ConfigType_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_File_pschist!=NULL )
	if( !Commit_ConfigType_File_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_File_pschmask!=NULL )
	if( !Commit_ConfigType_File_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Setting!=NULL )
	if( !Commit_ConfigType_Setting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Setting_pschist!=NULL )
	if( !Commit_ConfigType_Setting_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Setting_pschmask!=NULL )
	if( !Commit_ConfigType_Setting_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Token!=NULL )
	if( !Commit_ConfigType_Token(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Token_pschist!=NULL )
	if( !Commit_ConfigType_Token_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Token_pschmask!=NULL )
	if( !Commit_ConfigType_Token_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_pschist!=NULL )
	if( !Commit_ConfigType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_pschmask!=NULL )
	if( !Commit_ConfigType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConnectorType!=NULL )
	if( !Commit_ConnectorType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConnectorType_pschist!=NULL )
	if( !Commit_ConnectorType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConnectorType_pschmask!=NULL )
	if( !Commit_ConnectorType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCountry!=NULL )
	if( !Commit_Country(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteria!=NULL )
	if( !Commit_Criteria(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList!=NULL )
	if( !Commit_CriteriaList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList_CriteriaParmList!=NULL )
	if( !Commit_CriteriaList_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList_CriteriaParmList_pschist!=NULL )
	if( !Commit_CriteriaList_CriteriaParmList_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList_CriteriaParmList_pschmask!=NULL )
	if( !Commit_CriteriaList_CriteriaParmList_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList_pschist!=NULL )
	if( !Commit_CriteriaList_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaList_pschmask!=NULL )
	if( !Commit_CriteriaList_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParm!=NULL )
	if( !Commit_CriteriaParm(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmList!=NULL )
	if( !Commit_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmList_pschist!=NULL )
	if( !Commit_CriteriaParmList_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmList_pschmask!=NULL )
	if( !Commit_CriteriaParmList_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmNesting!=NULL )
	if( !Commit_CriteriaParmNesting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmNesting_pschist!=NULL )
	if( !Commit_CriteriaParmNesting_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmNesting_pschmask!=NULL )
	if( !Commit_CriteriaParmNesting_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParm_pschist!=NULL )
	if( !Commit_CriteriaParm_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParm_pschmask!=NULL )
	if( !Commit_CriteriaParm_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteria_pschist!=NULL )
	if( !Commit_Criteria_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteria_pschmask!=NULL )
	if( !Commit_Criteria_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice!=NULL )
	if( !Commit_DHCPDevice(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_DeviceData!=NULL )
	if( !Commit_DHCPDevice_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_DeviceData_pschist!=NULL )
	if( !Commit_DHCPDevice_DeviceData_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_DeviceData_pschmask!=NULL )
	if( !Commit_DHCPDevice_DeviceData_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_pschist!=NULL )
	if( !Commit_DHCPDevice_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_pschmask!=NULL )
	if( !Commit_DHCPDevice_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDataGrid!=NULL )
	if( !Commit_DataGrid(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDataGrid_pschist!=NULL )
	if( !Commit_DataGrid_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDataGrid_pschmask!=NULL )
	if( !Commit_DataGrid_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObj!=NULL )
	if( !Commit_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjCategory!=NULL )
	if( !Commit_DesignObjCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjCategory_pschist!=NULL )
	if( !Commit_DesignObjCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjCategory_pschmask!=NULL )
	if( !Commit_DesignObjCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjParameter!=NULL )
	if( !Commit_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjParameter_pschist!=NULL )
	if( !Commit_DesignObjParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjParameter_pschmask!=NULL )
	if( !Commit_DesignObjParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType!=NULL )
	if( !Commit_DesignObjType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_DesignObjParameter!=NULL )
	if( !Commit_DesignObjType_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_DesignObjParameter_pschist!=NULL )
	if( !Commit_DesignObjType_DesignObjParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_DesignObjParameter_pschmask!=NULL )
	if( !Commit_DesignObjType_DesignObjParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_pschist!=NULL )
	if( !Commit_DesignObjType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_pschmask!=NULL )
	if( !Commit_DesignObjType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation!=NULL )
	if( !Commit_DesignObjVariation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj!=NULL )
	if( !Commit_DesignObjVariation_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObjParameter!=NULL )
	if( !Commit_DesignObjVariation_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObjParameter_pschist!=NULL )
	if( !Commit_DesignObjVariation_DesignObjParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObjParameter_pschmask!=NULL )
	if( !Commit_DesignObjVariation_DesignObjParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj_Skin_Language!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_Skin_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj_Skin_Language_pschist!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_Skin_Language_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj_Skin_Language_pschmask!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_Skin_Language_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj_pschist!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_DesignObj_pschmask!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text!=NULL )
	if( !Commit_DesignObjVariation_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_Skin_Language!=NULL )
	if( !Commit_DesignObjVariation_Text_Skin_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_Skin_Language_pschist!=NULL )
	if( !Commit_DesignObjVariation_Text_Skin_Language_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_Skin_Language_pschmask!=NULL )
	if( !Commit_DesignObjVariation_Text_Skin_Language_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_pschist!=NULL )
	if( !Commit_DesignObjVariation_Text_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_pschmask!=NULL )
	if( !Commit_DesignObjVariation_Text_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Zone!=NULL )
	if( !Commit_DesignObjVariation_Zone(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Zone_pschist!=NULL )
	if( !Commit_DesignObjVariation_Zone_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Zone_pschmask!=NULL )
	if( !Commit_DesignObjVariation_Zone_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_pschist!=NULL )
	if( !Commit_DesignObjVariation_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_pschmask!=NULL )
	if( !Commit_DesignObjVariation_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObj_pschist!=NULL )
	if( !Commit_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObj_pschmask!=NULL )
	if( !Commit_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice!=NULL )
	if( !Commit_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory!=NULL )
	if( !Commit_DeviceCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_DeviceData!=NULL )
	if( !Commit_DeviceCategory_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_DeviceData_pschist!=NULL )
	if( !Commit_DeviceCategory_DeviceData_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_DeviceData_pschmask!=NULL )
	if( !Commit_DeviceCategory_DeviceData_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_Event!=NULL )
	if( !Commit_DeviceCategory_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_Event_pschist!=NULL )
	if( !Commit_DeviceCategory_Event_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_Event_pschmask!=NULL )
	if( !Commit_DeviceCategory_Event_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_pschist!=NULL )
	if( !Commit_DeviceCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCategory_pschmask!=NULL )
	if( !Commit_DeviceCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup!=NULL )
	if( !Commit_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_Command!=NULL )
	if( !Commit_DeviceCommandGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_Command_pschist!=NULL )
	if( !Commit_DeviceCommandGroup_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_Command_pschmask!=NULL )
	if( !Commit_DeviceCommandGroup_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent!=NULL )
	if( !Commit_DeviceCommandGroup_DeviceCommandGroup_Parent(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent_pschist!=NULL )
	if( !Commit_DeviceCommandGroup_DeviceCommandGroup_Parent_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent_pschmask!=NULL )
	if( !Commit_DeviceCommandGroup_DeviceCommandGroup_Parent_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_pschist!=NULL )
	if( !Commit_DeviceCommandGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_pschmask!=NULL )
	if( !Commit_DeviceCommandGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceData!=NULL )
	if( !Commit_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceData_pschist!=NULL )
	if( !Commit_DeviceData_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceData_pschmask!=NULL )
	if( !Commit_DeviceData_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceGroup!=NULL )
	if( !Commit_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceGroup_pschist!=NULL )
	if( !Commit_DeviceGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceGroup_pschmask!=NULL )
	if( !Commit_DeviceGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate!=NULL )
	if( !Commit_DeviceTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_AV!=NULL )
	if( !Commit_DeviceTemplate_AV(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_AV_pschist!=NULL )
	if( !Commit_DeviceTemplate_AV_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_AV_pschmask!=NULL )
	if( !Commit_DeviceTemplate_AV_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DSPMode!=NULL )
	if( !Commit_DeviceTemplate_DSPMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DSPMode_pschist!=NULL )
	if( !Commit_DeviceTemplate_DSPMode_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DSPMode_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DSPMode_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DesignObj!=NULL )
	if( !Commit_DeviceTemplate_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DesignObj_pschist!=NULL )
	if( !Commit_DeviceTemplate_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DesignObj_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCommandGroup!=NULL )
	if( !Commit_DeviceTemplate_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCommandGroup_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceCommandGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCommandGroup_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceCommandGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceData!=NULL )
	if( !Commit_DeviceTemplate_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceData_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceData_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceData_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceData_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_Related!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_Related_pschist!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_Related_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_Related_pschmask!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_Related_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Event!=NULL )
	if( !Commit_DeviceTemplate_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Event_pschist!=NULL )
	if( !Commit_DeviceTemplate_Event_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Event_pschmask!=NULL )
	if( !Commit_DeviceTemplate_Event_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_InfraredGroup!=NULL )
	if( !Commit_DeviceTemplate_InfraredGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_InfraredGroup_pschist!=NULL )
	if( !Commit_DeviceTemplate_InfraredGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_InfraredGroup_pschmask!=NULL )
	if( !Commit_DeviceTemplate_InfraredGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Input!=NULL )
	if( !Commit_DeviceTemplate_Input(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Input_pschist!=NULL )
	if( !Commit_DeviceTemplate_Input_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Input_pschmask!=NULL )
	if( !Commit_DeviceTemplate_Input_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType!=NULL )
	if( !Commit_DeviceTemplate_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_DesignObj!=NULL )
	if( !Commit_DeviceTemplate_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_DesignObj_pschist!=NULL )
	if( !Commit_DeviceTemplate_MediaType_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_DesignObj_pschmask!=NULL )
	if( !Commit_DeviceTemplate_MediaType_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_pschist!=NULL )
	if( !Commit_DeviceTemplate_MediaType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_pschmask!=NULL )
	if( !Commit_DeviceTemplate_MediaType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Output!=NULL )
	if( !Commit_DeviceTemplate_Output(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Output_pschist!=NULL )
	if( !Commit_DeviceTemplate_Output_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Output_pschmask!=NULL )
	if( !Commit_DeviceTemplate_Output_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_PageSetup!=NULL )
	if( !Commit_DeviceTemplate_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_PageSetup_pschist!=NULL )
	if( !Commit_DeviceTemplate_PageSetup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_PageSetup_pschmask!=NULL )
	if( !Commit_DeviceTemplate_PageSetup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_pschist!=NULL )
	if( !Commit_DeviceTemplate_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_pschmask!=NULL )
	if( !Commit_DeviceTemplate_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Command!=NULL )
	if( !Commit_Device_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_CommandGroup!=NULL )
	if( !Commit_Device_CommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_CommandGroup_pschist!=NULL )
	if( !Commit_Device_CommandGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_CommandGroup_pschmask!=NULL )
	if( !Commit_Device_CommandGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Command_pschist!=NULL )
	if( !Commit_Device_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Command_pschmask!=NULL )
	if( !Commit_Device_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceData!=NULL )
	if( !Commit_Device_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceData_pschist!=NULL )
	if( !Commit_Device_DeviceData_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceData_pschmask!=NULL )
	if( !Commit_Device_DeviceData_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceGroup!=NULL )
	if( !Commit_Device_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceGroup_pschist!=NULL )
	if( !Commit_Device_DeviceGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceGroup_pschmask!=NULL )
	if( !Commit_Device_DeviceGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Pipe!=NULL )
	if( !Commit_Device_Device_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Pipe_pschist!=NULL )
	if( !Commit_Device_Device_Pipe_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Pipe_pschmask!=NULL )
	if( !Commit_Device_Device_Pipe_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Related!=NULL )
	if( !Commit_Device_Device_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Related_pschist!=NULL )
	if( !Commit_Device_Device_Related_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Related_pschmask!=NULL )
	if( !Commit_Device_Device_Related_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_EntertainArea!=NULL )
	if( !Commit_Device_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_EntertainArea_pschist!=NULL )
	if( !Commit_Device_EntertainArea_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_EntertainArea_pschmask!=NULL )
	if( !Commit_Device_EntertainArea_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_HouseMode!=NULL )
	if( !Commit_Device_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_HouseMode_pschist!=NULL )
	if( !Commit_Device_HouseMode_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_HouseMode_pschmask!=NULL )
	if( !Commit_Device_HouseMode_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_MRU!=NULL )
	if( !Commit_Device_MRU(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_MRU_pschist!=NULL )
	if( !Commit_Device_MRU_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_MRU_pschmask!=NULL )
	if( !Commit_Device_MRU_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Orbiter!=NULL )
	if( !Commit_Device_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Orbiter_pschist!=NULL )
	if( !Commit_Device_Orbiter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Orbiter_pschmask!=NULL )
	if( !Commit_Device_Orbiter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_QuickStart!=NULL )
	if( !Commit_Device_QuickStart(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_QuickStart_pschist!=NULL )
	if( !Commit_Device_QuickStart_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_QuickStart_pschmask!=NULL )
	if( !Commit_Device_QuickStart_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_StartupScript!=NULL )
	if( !Commit_Device_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_StartupScript_pschist!=NULL )
	if( !Commit_Device_StartupScript_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_StartupScript_pschmask!=NULL )
	if( !Commit_Device_StartupScript_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Users!=NULL )
	if( !Commit_Device_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Users_pschist!=NULL )
	if( !Commit_Device_Users_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Users_pschmask!=NULL )
	if( !Commit_Device_Users_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_pschist!=NULL )
	if( !Commit_Device_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_pschmask!=NULL )
	if( !Commit_Device_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirection!=NULL )
	if( !Commit_Direction(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirection_pschist!=NULL )
	if( !Commit_Direction_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirection_pschmask!=NULL )
	if( !Commit_Direction_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirectory!=NULL )
	if( !Commit_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirectory_pschist!=NULL )
	if( !Commit_Directory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirectory_pschmask!=NULL )
	if( !Commit_Directory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDistro!=NULL )
	if( !Commit_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDistro_pschist!=NULL )
	if( !Commit_Distro_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDistro_pschmask!=NULL )
	if( !Commit_Distro_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument!=NULL )
	if( !Commit_Document(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_Comment!=NULL )
	if( !Commit_Document_Comment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_Comment_pschist!=NULL )
	if( !Commit_Document_Comment_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_Comment_pschmask!=NULL )
	if( !Commit_Document_Comment_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_pschist!=NULL )
	if( !Commit_Document_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_pschmask!=NULL )
	if( !Commit_Document_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffect!=NULL )
	if( !Commit_Effect(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType!=NULL )
	if( !Commit_EffectType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType_Effect_Skin!=NULL )
	if( !Commit_EffectType_Effect_Skin(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType_Effect_Skin_pschist!=NULL )
	if( !Commit_EffectType_Effect_Skin_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType_Effect_Skin_pschmask!=NULL )
	if( !Commit_EffectType_Effect_Skin_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType_pschist!=NULL )
	if( !Commit_EffectType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffectType_pschmask!=NULL )
	if( !Commit_EffectType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffect_pschist!=NULL )
	if( !Commit_Effect_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEffect_pschmask!=NULL )
	if( !Commit_Effect_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEntertainArea!=NULL )
	if( !Commit_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEntertainArea_pschist!=NULL )
	if( !Commit_EntertainArea_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEntertainArea_pschmask!=NULL )
	if( !Commit_EntertainArea_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent!=NULL )
	if( !Commit_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventCategory!=NULL )
	if( !Commit_EventCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventCategory_pschist!=NULL )
	if( !Commit_EventCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventCategory_pschmask!=NULL )
	if( !Commit_EventCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventHandler!=NULL )
	if( !Commit_EventHandler(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventHandler_pschist!=NULL )
	if( !Commit_EventHandler_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventHandler_pschmask!=NULL )
	if( !Commit_EventHandler_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventParameter!=NULL )
	if( !Commit_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventParameter_pschist!=NULL )
	if( !Commit_EventParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventParameter_pschmask!=NULL )
	if( !Commit_EventParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_EventParameter!=NULL )
	if( !Commit_Event_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_EventParameter_pschist!=NULL )
	if( !Commit_Event_EventParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_EventParameter_pschmask!=NULL )
	if( !Commit_Event_EventParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_pschist!=NULL )
	if( !Commit_Event_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_pschmask!=NULL )
	if( !Commit_Event_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFAQ!=NULL )
	if( !Commit_FAQ(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFAQ_pschist!=NULL )
	if( !Commit_FAQ_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFAQ_pschmask!=NULL )
	if( !Commit_FAQ_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFirewall!=NULL )
	if( !Commit_Firewall(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFirewall_pschist!=NULL )
	if( !Commit_Firewall_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFirewall_pschmask!=NULL )
	if( !Commit_Firewall_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplan!=NULL )
	if( !Commit_Floorplan(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType!=NULL )
	if( !Commit_FloorplanObjectType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType_Color!=NULL )
	if( !Commit_FloorplanObjectType_Color(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType_Color_pschist!=NULL )
	if( !Commit_FloorplanObjectType_Color_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType_Color_pschmask!=NULL )
	if( !Commit_FloorplanObjectType_Color_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType_pschist!=NULL )
	if( !Commit_FloorplanObjectType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanObjectType_pschmask!=NULL )
	if( !Commit_FloorplanObjectType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanType!=NULL )
	if( !Commit_FloorplanType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanType_pschist!=NULL )
	if( !Commit_FloorplanType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplanType_pschmask!=NULL )
	if( !Commit_FloorplanType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplan_pschist!=NULL )
	if( !Commit_Floorplan_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFloorplan_pschmask!=NULL )
	if( !Commit_Floorplan_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHorizAlignment!=NULL )
	if( !Commit_HorizAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHorizAlignment_pschist!=NULL )
	if( !Commit_HorizAlignment_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHorizAlignment_pschmask!=NULL )
	if( !Commit_HorizAlignment_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHouseMode!=NULL )
	if( !Commit_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHouseMode_pschist!=NULL )
	if( !Commit_HouseMode_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHouseMode_pschmask!=NULL )
	if( !Commit_HouseMode_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold!=NULL )
	if( !Commit_Household(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_Installation!=NULL )
	if( !Commit_Household_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_Installation_pschist!=NULL )
	if( !Commit_Household_Installation_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_Installation_pschmask!=NULL )
	if( !Commit_Household_Installation_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_pschist!=NULL )
	if( !Commit_Household_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_pschmask!=NULL )
	if( !Commit_Household_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblIcon!=NULL )
	if( !Commit_Icon(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblIcon_pschist!=NULL )
	if( !Commit_Icon_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblIcon_pschmask!=NULL )
	if( !Commit_Icon_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblImage!=NULL )
	if( !Commit_Image(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblImage_pschist!=NULL )
	if( !Commit_Image_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblImage_pschmask!=NULL )
	if( !Commit_Image_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup!=NULL )
	if( !Commit_InfraredGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command!=NULL )
	if( !Commit_InfraredGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command_Preferred!=NULL )
	if( !Commit_InfraredGroup_Command_Preferred(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command_Preferred_pschist!=NULL )
	if( !Commit_InfraredGroup_Command_Preferred_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command_Preferred_pschmask!=NULL )
	if( !Commit_InfraredGroup_Command_Preferred_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command_pschist!=NULL )
	if( !Commit_InfraredGroup_Command_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_Command_pschmask!=NULL )
	if( !Commit_InfraredGroup_Command_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_pschist!=NULL )
	if( !Commit_InfraredGroup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInfraredGroup_pschmask!=NULL )
	if( !Commit_InfraredGroup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard!=NULL )
	if( !Commit_InstallWizard(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_Distro!=NULL )
	if( !Commit_InstallWizard_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_Distro_pschist!=NULL )
	if( !Commit_InstallWizard_Distro_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_Distro_pschmask!=NULL )
	if( !Commit_InstallWizard_Distro_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_pschist!=NULL )
	if( !Commit_InstallWizard_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_pschmask!=NULL )
	if( !Commit_InstallWizard_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation!=NULL )
	if( !Commit_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_RepositorySource_URL!=NULL )
	if( !Commit_Installation_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_RepositorySource_URL_pschist!=NULL )
	if( !Commit_Installation_RepositorySource_URL_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_RepositorySource_URL_pschmask!=NULL )
	if( !Commit_Installation_RepositorySource_URL_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_Users!=NULL )
	if( !Commit_Installation_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_Users_pschist!=NULL )
	if( !Commit_Installation_Users_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_Users_pschmask!=NULL )
	if( !Commit_Installation_Users_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_pschist!=NULL )
	if( !Commit_Installation_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_pschmask!=NULL )
	if( !Commit_Installation_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLanguage!=NULL )
	if( !Commit_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLanguage_pschist!=NULL )
	if( !Commit_Language_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLanguage_pschmask!=NULL )
	if( !Commit_Language_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicense!=NULL )
	if( !Commit_License(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicense_pschist!=NULL )
	if( !Commit_License_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicense_pschmask!=NULL )
	if( !Commit_License_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicensing!=NULL )
	if( !Commit_Licensing(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicensing_pschist!=NULL )
	if( !Commit_Licensing_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicensing_pschmask!=NULL )
	if( !Commit_Licensing_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblManufacturer!=NULL )
	if( !Commit_Manufacturer(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblManufacturer_pschist!=NULL )
	if( !Commit_Manufacturer_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblManufacturer_pschmask!=NULL )
	if( !Commit_Manufacturer_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType!=NULL )
	if( !Commit_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_Broadcast!=NULL )
	if( !Commit_MediaType_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_Broadcast_pschist!=NULL )
	if( !Commit_MediaType_Broadcast_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_Broadcast_pschmask!=NULL )
	if( !Commit_MediaType_Broadcast_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_DesignObj!=NULL )
	if( !Commit_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_DesignObj_pschist!=NULL )
	if( !Commit_MediaType_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_DesignObj_pschmask!=NULL )
	if( !Commit_MediaType_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_pschist!=NULL )
	if( !Commit_MediaType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_pschmask!=NULL )
	if( !Commit_MediaType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNews!=NULL )
	if( !Commit_News(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNews_pschist!=NULL )
	if( !Commit_News_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNews_pschmask!=NULL )
	if( !Commit_News_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOperatingSystem!=NULL )
	if( !Commit_OperatingSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOperatingSystem_pschist!=NULL )
	if( !Commit_OperatingSystem_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOperatingSystem_pschmask!=NULL )
	if( !Commit_OperatingSystem_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter!=NULL )
	if( !Commit_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Users_PasswordReq!=NULL )
	if( !Commit_Orbiter_Users_PasswordReq(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Users_PasswordReq_pschist!=NULL )
	if( !Commit_Orbiter_Users_PasswordReq_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Users_PasswordReq_pschmask!=NULL )
	if( !Commit_Orbiter_Users_PasswordReq_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Variable!=NULL )
	if( !Commit_Orbiter_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Variable_pschist!=NULL )
	if( !Commit_Orbiter_Variable_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Variable_pschmask!=NULL )
	if( !Commit_Orbiter_Variable_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_pschist!=NULL )
	if( !Commit_Orbiter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_pschmask!=NULL )
	if( !Commit_Orbiter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage!=NULL )
	if( !Commit_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackageType!=NULL )
	if( !Commit_PackageType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackageType_pschist!=NULL )
	if( !Commit_PackageType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackageType_pschmask!=NULL )
	if( !Commit_PackageType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Compat!=NULL )
	if( !Commit_Package_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Compat_pschist!=NULL )
	if( !Commit_Package_Compat_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Compat_pschmask!=NULL )
	if( !Commit_Package_Compat_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Device!=NULL )
	if( !Commit_Package_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Device_pschist!=NULL )
	if( !Commit_Package_Device_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Device_pschmask!=NULL )
	if( !Commit_Package_Device_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory!=NULL )
	if( !Commit_Package_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_File!=NULL )
	if( !Commit_Package_Directory_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_File_pschist!=NULL )
	if( !Commit_Package_Directory_File_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_File_pschmask!=NULL )
	if( !Commit_Package_Directory_File_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_pschist!=NULL )
	if( !Commit_Package_Directory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_pschmask!=NULL )
	if( !Commit_Package_Directory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Package!=NULL )
	if( !Commit_Package_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Package_pschist!=NULL )
	if( !Commit_Package_Package_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Package_pschmask!=NULL )
	if( !Commit_Package_Package_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source!=NULL )
	if( !Commit_Package_Source(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_Compat!=NULL )
	if( !Commit_Package_Source_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_Compat_pschist!=NULL )
	if( !Commit_Package_Source_Compat_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_Compat_pschmask!=NULL )
	if( !Commit_Package_Source_Compat_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_pschist!=NULL )
	if( !Commit_Package_Source_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_pschmask!=NULL )
	if( !Commit_Package_Source_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Users!=NULL )
	if( !Commit_Package_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Users_pschist!=NULL )
	if( !Commit_Package_Users_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Users_pschmask!=NULL )
	if( !Commit_Package_Users_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Version!=NULL )
	if( !Commit_Package_Version(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Version_pschist!=NULL )
	if( !Commit_Package_Version_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Version_pschmask!=NULL )
	if( !Commit_Package_Version_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_pschist!=NULL )
	if( !Commit_Package_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_pschmask!=NULL )
	if( !Commit_Package_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPageSetup!=NULL )
	if( !Commit_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPageSetup_pschist!=NULL )
	if( !Commit_PageSetup_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPageSetup_pschmask!=NULL )
	if( !Commit_PageSetup_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPaidLicense!=NULL )
	if( !Commit_PaidLicense(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPaidLicense_pschist!=NULL )
	if( !Commit_PaidLicense_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPaidLicense_pschmask!=NULL )
	if( !Commit_PaidLicense_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblParameterType!=NULL )
	if( !Commit_ParameterType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblParameterType_pschist!=NULL )
	if( !Commit_ParameterType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblParameterType_pschmask!=NULL )
	if( !Commit_ParameterType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneLineType!=NULL )
	if( !Commit_PhoneLineType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneLineType_pschist!=NULL )
	if( !Commit_PhoneLineType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneLineType_pschmask!=NULL )
	if( !Commit_PhoneLineType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPipe!=NULL )
	if( !Commit_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPipe_pschist!=NULL )
	if( !Commit_Pipe_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPipe_pschmask!=NULL )
	if( !Commit_Pipe_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpLevel!=NULL )
	if( !Commit_PnpLevel(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpLevel_pschist!=NULL )
	if( !Commit_PnpLevel_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpLevel_pschmask!=NULL )
	if( !Commit_PnpLevel_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpProtocol!=NULL )
	if( !Commit_PnpProtocol(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpProtocol_pschist!=NULL )
	if( !Commit_PnpProtocol_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpProtocol_pschmask!=NULL )
	if( !Commit_PnpProtocol_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpQueue!=NULL )
	if( !Commit_PnpQueue(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPostalCode!=NULL )
	if( !Commit_PostalCode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartCategory!=NULL )
	if( !Commit_QuickStartCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartCategory_pschist!=NULL )
	if( !Commit_QuickStartCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartCategory_pschmask!=NULL )
	if( !Commit_QuickStartCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartTemplate!=NULL )
	if( !Commit_QuickStartTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartTemplate_pschist!=NULL )
	if( !Commit_QuickStartTemplate_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblQuickStartTemplate_pschmask!=NULL )
	if( !Commit_QuickStartTemplate_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRegion!=NULL )
	if( !Commit_Region(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteControl!=NULL )
	if( !Commit_RemoteControl(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteControl_pschist!=NULL )
	if( !Commit_RemoteControl_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteControl_pschmask!=NULL )
	if( !Commit_RemoteControl_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteMapping!=NULL )
	if( !Commit_RemoteMapping(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteMapping_pschist!=NULL )
	if( !Commit_RemoteMapping_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteMapping_pschmask!=NULL )
	if( !Commit_RemoteMapping_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource!=NULL )
	if( !Commit_RepositorySource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_URL!=NULL )
	if( !Commit_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_URL_pschist!=NULL )
	if( !Commit_RepositorySource_URL_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_URL_pschmask!=NULL )
	if( !Commit_RepositorySource_URL_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_pschist!=NULL )
	if( !Commit_RepositorySource_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_pschmask!=NULL )
	if( !Commit_RepositorySource_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositoryType!=NULL )
	if( !Commit_RepositoryType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositoryType_pschist!=NULL )
	if( !Commit_RepositoryType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositoryType_pschmask!=NULL )
	if( !Commit_RepositoryType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom!=NULL )
	if( !Commit_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoomType!=NULL )
	if( !Commit_RoomType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoomType_pschist!=NULL )
	if( !Commit_RoomType_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoomType_pschmask!=NULL )
	if( !Commit_RoomType_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_Users!=NULL )
	if( !Commit_Room_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_Users_pschist!=NULL )
	if( !Commit_Room_Users_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_Users_pschmask!=NULL )
	if( !Commit_Room_Users_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_pschist!=NULL )
	if( !Commit_Room_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_pschmask!=NULL )
	if( !Commit_Room_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSchema!=NULL )
	if( !Commit_Schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSchema_pschist!=NULL )
	if( !Commit_Schema_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSchema_pschmask!=NULL )
	if( !Commit_Schema_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen!=NULL )
	if( !Commit_Screen(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_CommandParameter!=NULL )
	if( !Commit_Screen_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_CommandParameter_pschist!=NULL )
	if( !Commit_Screen_CommandParameter_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_CommandParameter_pschmask!=NULL )
	if( !Commit_Screen_CommandParameter_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_DesignObj!=NULL )
	if( !Commit_Screen_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_DesignObj_pschist!=NULL )
	if( !Commit_Screen_DesignObj_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_DesignObj_pschmask!=NULL )
	if( !Commit_Screen_DesignObj_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_pschist!=NULL )
	if( !Commit_Screen_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_pschmask!=NULL )
	if( !Commit_Screen_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSetupStep!=NULL )
	if( !Commit_SetupStep(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSetupStep_pschist!=NULL )
	if( !Commit_SetupStep_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSetupStep_pschmask!=NULL )
	if( !Commit_SetupStep_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSize!=NULL )
	if( !Commit_Size(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSize_pschist!=NULL )
	if( !Commit_Size_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSize_pschmask!=NULL )
	if( !Commit_Size_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSkin!=NULL )
	if( !Commit_Skin(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSkin_pschist!=NULL )
	if( !Commit_Skin_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSkin_pschmask!=NULL )
	if( !Commit_Skin_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSoftware!=NULL )
	if( !Commit_Software(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSoftware_Device!=NULL )
	if( !Commit_Software_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSoftware_Source!=NULL )
	if( !Commit_Software_Source(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStabilityStatus!=NULL )
	if( !Commit_StabilityStatus(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStabilityStatus_pschist!=NULL )
	if( !Commit_StabilityStatus_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStabilityStatus_pschmask!=NULL )
	if( !Commit_StabilityStatus_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStartupScript!=NULL )
	if( !Commit_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStartupScript_pschist!=NULL )
	if( !Commit_StartupScript_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStartupScript_pschmask!=NULL )
	if( !Commit_StartupScript_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyle!=NULL )
	if( !Commit_Style(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyleVariation!=NULL )
	if( !Commit_StyleVariation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyleVariation_pschist!=NULL )
	if( !Commit_StyleVariation_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyleVariation_pschmask!=NULL )
	if( !Commit_StyleVariation_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyle_pschist!=NULL )
	if( !Commit_Style_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyle_pschmask!=NULL )
	if( !Commit_Style_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSystem!=NULL )
	if( !Commit_System(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSystem_pschist!=NULL )
	if( !Commit_System_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSystem_pschmask!=NULL )
	if( !Commit_System_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTemplate!=NULL )
	if( !Commit_Template(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTemplate_pschist!=NULL )
	if( !Commit_Template_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTemplate_pschmask!=NULL )
	if( !Commit_Template_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText!=NULL )
	if( !Commit_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTextCategory!=NULL )
	if( !Commit_TextCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTextCategory_pschist!=NULL )
	if( !Commit_TextCategory_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTextCategory_pschmask!=NULL )
	if( !Commit_TextCategory_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS!=NULL )
	if( !Commit_Text_LS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_AltVersions!=NULL )
	if( !Commit_Text_LS_AltVersions(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_AltVersions_pschist!=NULL )
	if( !Commit_Text_LS_AltVersions_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_AltVersions_pschmask!=NULL )
	if( !Commit_Text_LS_AltVersions_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_pschist!=NULL )
	if( !Commit_Text_LS_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_pschmask!=NULL )
	if( !Commit_Text_LS_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_pschist!=NULL )
	if( !Commit_Text_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_pschmask!=NULL )
	if( !Commit_Text_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTimeZone!=NULL )
	if( !Commit_TimeZone(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUI!=NULL )
	if( !Commit_UI(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUI_pschist!=NULL )
	if( !Commit_UI_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUI_pschmask!=NULL )
	if( !Commit_UI_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUnknownDevices!=NULL )
	if( !Commit_UnknownDevices(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUnknownDevices_pschist!=NULL )
	if( !Commit_UnknownDevices_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUnknownDevices_pschmask!=NULL )
	if( !Commit_UnknownDevices_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUserMode!=NULL )
	if( !Commit_UserMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUserMode_pschist!=NULL )
	if( !Commit_UserMode_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUserMode_pschmask!=NULL )
	if( !Commit_UserMode_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUsers!=NULL )
	if( !Commit_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUsers_pschist!=NULL )
	if( !Commit_Users_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUsers_pschmask!=NULL )
	if( !Commit_Users_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVariable!=NULL )
	if( !Commit_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVariable_pschist!=NULL )
	if( !Commit_Variable_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVariable_pschmask!=NULL )
	if( !Commit_Variable_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVersion!=NULL )
	if( !Commit_Version(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVersion_pschist!=NULL )
	if( !Commit_Version_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVersion_pschmask!=NULL )
	if( !Commit_Version_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVertAlignment!=NULL )
	if( !Commit_VertAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVertAlignment_pschist!=NULL )
	if( !Commit_VertAlignment_pschist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVertAlignment_pschmask!=NULL )
	if( !Commit_VertAlignment_pschmask(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_batdet!=NULL )
	if( !Commit_psc_constants_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_bathdr!=NULL )
	if( !Commit_psc_constants_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_batuser!=NULL )
	if( !Commit_psc_constants_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_repset!=NULL )
	if( !Commit_psc_constants_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_schema!=NULL )
	if( !Commit_psc_constants_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_constants_tables!=NULL )
	if( !Commit_psc_constants_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_batdet!=NULL )
	if( !Commit_psc_dce_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_bathdr!=NULL )
	if( !Commit_psc_dce_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_batuser!=NULL )
	if( !Commit_psc_dce_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_repset!=NULL )
	if( !Commit_psc_dce_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_schema!=NULL )
	if( !Commit_psc_dce_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_dce_tables!=NULL )
	if( !Commit_psc_dce_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_batdet!=NULL )
	if( !Commit_psc_designer_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_bathdr!=NULL )
	if( !Commit_psc_designer_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_batuser!=NULL )
	if( !Commit_psc_designer_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_repset!=NULL )
	if( !Commit_psc_designer_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_schema!=NULL )
	if( !Commit_psc_designer_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_designer_tables!=NULL )
	if( !Commit_psc_designer_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_batdet!=NULL )
	if( !Commit_psc_document_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_bathdr!=NULL )
	if( !Commit_psc_document_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_batuser!=NULL )
	if( !Commit_psc_document_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_repset!=NULL )
	if( !Commit_psc_document_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_schema!=NULL )
	if( !Commit_psc_document_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_document_tables!=NULL )
	if( !Commit_psc_document_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_batdet!=NULL )
	if( !Commit_psc_ir_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_bathdr!=NULL )
	if( !Commit_psc_ir_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_batuser!=NULL )
	if( !Commit_psc_ir_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_repset!=NULL )
	if( !Commit_psc_ir_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_schema!=NULL )
	if( !Commit_psc_ir_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_ir_tables!=NULL )
	if( !Commit_psc_ir_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_batdet!=NULL )
	if( !Commit_psc_local_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_bathdr!=NULL )
	if( !Commit_psc_local_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_batuser!=NULL )
	if( !Commit_psc_local_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_repset!=NULL )
	if( !Commit_psc_local_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_schema!=NULL )
	if( !Commit_psc_local_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_local_tables!=NULL )
	if( !Commit_psc_local_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_batdet!=NULL )
	if( !Commit_psc_website_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_bathdr!=NULL )
	if( !Commit_psc_website_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_batuser!=NULL )
	if( !Commit_psc_website_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_repset!=NULL )
	if( !Commit_psc_website_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_schema!=NULL )
	if( !Commit_psc_website_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_website_tables!=NULL )
	if( !Commit_psc_website_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_main::DeleteAllTables()
{
DeleteTable_Array();
DeleteTable_Array_pschist();
DeleteTable_Array_pschmask();
DeleteTable_Broadcast();
DeleteTable_Broadcast_pschist();
DeleteTable_Broadcast_pschmask();
DeleteTable_Button();
DeleteTable_Button_pschist();
DeleteTable_Button_pschmask();
DeleteTable_CachedScreens();
DeleteTable_CachedScreens_pschist();
DeleteTable_CachedScreens_pschmask();
DeleteTable_CannedEvents();
DeleteTable_CannedEvents_CriteriaParmList();
DeleteTable_CannedEvents_CriteriaParmList_pschist();
DeleteTable_CannedEvents_CriteriaParmList_pschmask();
DeleteTable_CannedEvents_pschist();
DeleteTable_CannedEvents_pschmask();
DeleteTable_City();
DeleteTable_CommMethod();
DeleteTable_CommMethod_pschist();
DeleteTable_CommMethod_pschmask();
DeleteTable_Command();
DeleteTable_CommandCategory();
DeleteTable_CommandCategory_pschist();
DeleteTable_CommandCategory_pschmask();
DeleteTable_CommandGroup();
DeleteTable_CommandGroup_Command();
DeleteTable_CommandGroup_Command_CommandParameter();
DeleteTable_CommandGroup_Command_CommandParameter_pschist();
DeleteTable_CommandGroup_Command_CommandParameter_pschmask();
DeleteTable_CommandGroup_Command_pschist();
DeleteTable_CommandGroup_Command_pschmask();
DeleteTable_CommandGroup_D();
DeleteTable_CommandGroup_D_Command();
DeleteTable_CommandGroup_D_Command_CommandParameter();
DeleteTable_CommandGroup_D_Command_CommandParameter_pschist();
DeleteTable_CommandGroup_D_Command_CommandParameter_pschmask();
DeleteTable_CommandGroup_D_Command_pschist();
DeleteTable_CommandGroup_D_Command_pschmask();
DeleteTable_CommandGroup_D_pschist();
DeleteTable_CommandGroup_D_pschmask();
DeleteTable_CommandGroup_EntertainArea();
DeleteTable_CommandGroup_EntertainArea_pschist();
DeleteTable_CommandGroup_EntertainArea_pschmask();
DeleteTable_CommandGroup_Room();
DeleteTable_CommandGroup_Room_pschist();
DeleteTable_CommandGroup_Room_pschmask();
DeleteTable_CommandGroup_pschist();
DeleteTable_CommandGroup_pschmask();
DeleteTable_CommandParameter();
DeleteTable_CommandParameter_pschist();
DeleteTable_CommandParameter_pschmask();
DeleteTable_Command_CommandParameter();
DeleteTable_Command_CommandParameter_pschist();
DeleteTable_Command_CommandParameter_pschmask();
DeleteTable_Command_Pipe();
DeleteTable_Command_Pipe_pschist();
DeleteTable_Command_Pipe_pschmask();
DeleteTable_Command_pschist();
DeleteTable_Command_pschmask();
DeleteTable_ConfigType();
DeleteTable_ConfigType_File();
DeleteTable_ConfigType_File_pschist();
DeleteTable_ConfigType_File_pschmask();
DeleteTable_ConfigType_Setting();
DeleteTable_ConfigType_Setting_pschist();
DeleteTable_ConfigType_Setting_pschmask();
DeleteTable_ConfigType_Token();
DeleteTable_ConfigType_Token_pschist();
DeleteTable_ConfigType_Token_pschmask();
DeleteTable_ConfigType_pschist();
DeleteTable_ConfigType_pschmask();
DeleteTable_ConnectorType();
DeleteTable_ConnectorType_pschist();
DeleteTable_ConnectorType_pschmask();
DeleteTable_Country();
DeleteTable_Criteria();
DeleteTable_CriteriaList();
DeleteTable_CriteriaList_CriteriaParmList();
DeleteTable_CriteriaList_CriteriaParmList_pschist();
DeleteTable_CriteriaList_CriteriaParmList_pschmask();
DeleteTable_CriteriaList_pschist();
DeleteTable_CriteriaList_pschmask();
DeleteTable_CriteriaParm();
DeleteTable_CriteriaParmList();
DeleteTable_CriteriaParmList_pschist();
DeleteTable_CriteriaParmList_pschmask();
DeleteTable_CriteriaParmNesting();
DeleteTable_CriteriaParmNesting_pschist();
DeleteTable_CriteriaParmNesting_pschmask();
DeleteTable_CriteriaParm_pschist();
DeleteTable_CriteriaParm_pschmask();
DeleteTable_Criteria_pschist();
DeleteTable_Criteria_pschmask();
DeleteTable_DHCPDevice();
DeleteTable_DHCPDevice_DeviceData();
DeleteTable_DHCPDevice_DeviceData_pschist();
DeleteTable_DHCPDevice_DeviceData_pschmask();
DeleteTable_DHCPDevice_pschist();
DeleteTable_DHCPDevice_pschmask();
DeleteTable_DataGrid();
DeleteTable_DataGrid_pschist();
DeleteTable_DataGrid_pschmask();
DeleteTable_DesignObj();
DeleteTable_DesignObjCategory();
DeleteTable_DesignObjCategory_pschist();
DeleteTable_DesignObjCategory_pschmask();
DeleteTable_DesignObjParameter();
DeleteTable_DesignObjParameter_pschist();
DeleteTable_DesignObjParameter_pschmask();
DeleteTable_DesignObjType();
DeleteTable_DesignObjType_DesignObjParameter();
DeleteTable_DesignObjType_DesignObjParameter_pschist();
DeleteTable_DesignObjType_DesignObjParameter_pschmask();
DeleteTable_DesignObjType_pschist();
DeleteTable_DesignObjType_pschmask();
DeleteTable_DesignObjVariation();
DeleteTable_DesignObjVariation_DesignObj();
DeleteTable_DesignObjVariation_DesignObjParameter();
DeleteTable_DesignObjVariation_DesignObjParameter_pschist();
DeleteTable_DesignObjVariation_DesignObjParameter_pschmask();
DeleteTable_DesignObjVariation_DesignObj_Skin_Language();
DeleteTable_DesignObjVariation_DesignObj_Skin_Language_pschist();
DeleteTable_DesignObjVariation_DesignObj_Skin_Language_pschmask();
DeleteTable_DesignObjVariation_DesignObj_pschist();
DeleteTable_DesignObjVariation_DesignObj_pschmask();
DeleteTable_DesignObjVariation_Text();
DeleteTable_DesignObjVariation_Text_Skin_Language();
DeleteTable_DesignObjVariation_Text_Skin_Language_pschist();
DeleteTable_DesignObjVariation_Text_Skin_Language_pschmask();
DeleteTable_DesignObjVariation_Text_pschist();
DeleteTable_DesignObjVariation_Text_pschmask();
DeleteTable_DesignObjVariation_Zone();
DeleteTable_DesignObjVariation_Zone_pschist();
DeleteTable_DesignObjVariation_Zone_pschmask();
DeleteTable_DesignObjVariation_pschist();
DeleteTable_DesignObjVariation_pschmask();
DeleteTable_DesignObj_pschist();
DeleteTable_DesignObj_pschmask();
DeleteTable_Device();
DeleteTable_DeviceCategory();
DeleteTable_DeviceCategory_DeviceData();
DeleteTable_DeviceCategory_DeviceData_pschist();
DeleteTable_DeviceCategory_DeviceData_pschmask();
DeleteTable_DeviceCategory_Event();
DeleteTable_DeviceCategory_Event_pschist();
DeleteTable_DeviceCategory_Event_pschmask();
DeleteTable_DeviceCategory_pschist();
DeleteTable_DeviceCategory_pschmask();
DeleteTable_DeviceCommandGroup();
DeleteTable_DeviceCommandGroup_Command();
DeleteTable_DeviceCommandGroup_Command_pschist();
DeleteTable_DeviceCommandGroup_Command_pschmask();
DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent_pschist();
DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent_pschmask();
DeleteTable_DeviceCommandGroup_pschist();
DeleteTable_DeviceCommandGroup_pschmask();
DeleteTable_DeviceData();
DeleteTable_DeviceData_pschist();
DeleteTable_DeviceData_pschmask();
DeleteTable_DeviceGroup();
DeleteTable_DeviceGroup_pschist();
DeleteTable_DeviceGroup_pschmask();
DeleteTable_DeviceTemplate();
DeleteTable_DeviceTemplate_AV();
DeleteTable_DeviceTemplate_AV_pschist();
DeleteTable_DeviceTemplate_AV_pschmask();
DeleteTable_DeviceTemplate_DSPMode();
DeleteTable_DeviceTemplate_DSPMode_pschist();
DeleteTable_DeviceTemplate_DSPMode_pschmask();
DeleteTable_DeviceTemplate_DesignObj();
DeleteTable_DeviceTemplate_DesignObj_pschist();
DeleteTable_DeviceTemplate_DesignObj_pschmask();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschist();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe_pschmask();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_pschist();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_pschmask();
DeleteTable_DeviceTemplate_DeviceCommandGroup();
DeleteTable_DeviceTemplate_DeviceCommandGroup_pschist();
DeleteTable_DeviceTemplate_DeviceCommandGroup_pschmask();
DeleteTable_DeviceTemplate_DeviceData();
DeleteTable_DeviceTemplate_DeviceData_pschist();
DeleteTable_DeviceTemplate_DeviceData_pschmask();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschist();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_pschmask();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_pschist();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_pschmask();
DeleteTable_DeviceTemplate_DeviceTemplate_Related();
DeleteTable_DeviceTemplate_DeviceTemplate_Related_pschist();
DeleteTable_DeviceTemplate_DeviceTemplate_Related_pschmask();
DeleteTable_DeviceTemplate_Event();
DeleteTable_DeviceTemplate_Event_pschist();
DeleteTable_DeviceTemplate_Event_pschmask();
DeleteTable_DeviceTemplate_InfraredGroup();
DeleteTable_DeviceTemplate_InfraredGroup_pschist();
DeleteTable_DeviceTemplate_InfraredGroup_pschmask();
DeleteTable_DeviceTemplate_Input();
DeleteTable_DeviceTemplate_Input_pschist();
DeleteTable_DeviceTemplate_Input_pschmask();
DeleteTable_DeviceTemplate_MediaType();
DeleteTable_DeviceTemplate_MediaType_DesignObj();
DeleteTable_DeviceTemplate_MediaType_DesignObj_pschist();
DeleteTable_DeviceTemplate_MediaType_DesignObj_pschmask();
DeleteTable_DeviceTemplate_MediaType_pschist();
DeleteTable_DeviceTemplate_MediaType_pschmask();
DeleteTable_DeviceTemplate_Output();
DeleteTable_DeviceTemplate_Output_pschist();
DeleteTable_DeviceTemplate_Output_pschmask();
DeleteTable_DeviceTemplate_PageSetup();
DeleteTable_DeviceTemplate_PageSetup_pschist();
DeleteTable_DeviceTemplate_PageSetup_pschmask();
DeleteTable_DeviceTemplate_pschist();
DeleteTable_DeviceTemplate_pschmask();
DeleteTable_Device_Command();
DeleteTable_Device_CommandGroup();
DeleteTable_Device_CommandGroup_pschist();
DeleteTable_Device_CommandGroup_pschmask();
DeleteTable_Device_Command_pschist();
DeleteTable_Device_Command_pschmask();
DeleteTable_Device_DeviceData();
DeleteTable_Device_DeviceData_pschist();
DeleteTable_Device_DeviceData_pschmask();
DeleteTable_Device_DeviceGroup();
DeleteTable_Device_DeviceGroup_pschist();
DeleteTable_Device_DeviceGroup_pschmask();
DeleteTable_Device_Device_Pipe();
DeleteTable_Device_Device_Pipe_pschist();
DeleteTable_Device_Device_Pipe_pschmask();
DeleteTable_Device_Device_Related();
DeleteTable_Device_Device_Related_pschist();
DeleteTable_Device_Device_Related_pschmask();
DeleteTable_Device_EntertainArea();
DeleteTable_Device_EntertainArea_pschist();
DeleteTable_Device_EntertainArea_pschmask();
DeleteTable_Device_HouseMode();
DeleteTable_Device_HouseMode_pschist();
DeleteTable_Device_HouseMode_pschmask();
DeleteTable_Device_MRU();
DeleteTable_Device_MRU_pschist();
DeleteTable_Device_MRU_pschmask();
DeleteTable_Device_Orbiter();
DeleteTable_Device_Orbiter_pschist();
DeleteTable_Device_Orbiter_pschmask();
DeleteTable_Device_QuickStart();
DeleteTable_Device_QuickStart_pschist();
DeleteTable_Device_QuickStart_pschmask();
DeleteTable_Device_StartupScript();
DeleteTable_Device_StartupScript_pschist();
DeleteTable_Device_StartupScript_pschmask();
DeleteTable_Device_Users();
DeleteTable_Device_Users_pschist();
DeleteTable_Device_Users_pschmask();
DeleteTable_Device_pschist();
DeleteTable_Device_pschmask();
DeleteTable_Direction();
DeleteTable_Direction_pschist();
DeleteTable_Direction_pschmask();
DeleteTable_Directory();
DeleteTable_Directory_pschist();
DeleteTable_Directory_pschmask();
DeleteTable_Distro();
DeleteTable_Distro_pschist();
DeleteTable_Distro_pschmask();
DeleteTable_Document();
DeleteTable_Document_Comment();
DeleteTable_Document_Comment_pschist();
DeleteTable_Document_Comment_pschmask();
DeleteTable_Document_pschist();
DeleteTable_Document_pschmask();
DeleteTable_Effect();
DeleteTable_EffectType();
DeleteTable_EffectType_Effect_Skin();
DeleteTable_EffectType_Effect_Skin_pschist();
DeleteTable_EffectType_Effect_Skin_pschmask();
DeleteTable_EffectType_pschist();
DeleteTable_EffectType_pschmask();
DeleteTable_Effect_pschist();
DeleteTable_Effect_pschmask();
DeleteTable_EntertainArea();
DeleteTable_EntertainArea_pschist();
DeleteTable_EntertainArea_pschmask();
DeleteTable_Event();
DeleteTable_EventCategory();
DeleteTable_EventCategory_pschist();
DeleteTable_EventCategory_pschmask();
DeleteTable_EventHandler();
DeleteTable_EventHandler_pschist();
DeleteTable_EventHandler_pschmask();
DeleteTable_EventParameter();
DeleteTable_EventParameter_pschist();
DeleteTable_EventParameter_pschmask();
DeleteTable_Event_EventParameter();
DeleteTable_Event_EventParameter_pschist();
DeleteTable_Event_EventParameter_pschmask();
DeleteTable_Event_pschist();
DeleteTable_Event_pschmask();
DeleteTable_FAQ();
DeleteTable_FAQ_pschist();
DeleteTable_FAQ_pschmask();
DeleteTable_Firewall();
DeleteTable_Firewall_pschist();
DeleteTable_Firewall_pschmask();
DeleteTable_Floorplan();
DeleteTable_FloorplanObjectType();
DeleteTable_FloorplanObjectType_Color();
DeleteTable_FloorplanObjectType_Color_pschist();
DeleteTable_FloorplanObjectType_Color_pschmask();
DeleteTable_FloorplanObjectType_pschist();
DeleteTable_FloorplanObjectType_pschmask();
DeleteTable_FloorplanType();
DeleteTable_FloorplanType_pschist();
DeleteTable_FloorplanType_pschmask();
DeleteTable_Floorplan_pschist();
DeleteTable_Floorplan_pschmask();
DeleteTable_HorizAlignment();
DeleteTable_HorizAlignment_pschist();
DeleteTable_HorizAlignment_pschmask();
DeleteTable_HouseMode();
DeleteTable_HouseMode_pschist();
DeleteTable_HouseMode_pschmask();
DeleteTable_Household();
DeleteTable_Household_Installation();
DeleteTable_Household_Installation_pschist();
DeleteTable_Household_Installation_pschmask();
DeleteTable_Household_pschist();
DeleteTable_Household_pschmask();
DeleteTable_Icon();
DeleteTable_Icon_pschist();
DeleteTable_Icon_pschmask();
DeleteTable_Image();
DeleteTable_Image_pschist();
DeleteTable_Image_pschmask();
DeleteTable_InfraredGroup();
DeleteTable_InfraredGroup_Command();
DeleteTable_InfraredGroup_Command_Preferred();
DeleteTable_InfraredGroup_Command_Preferred_pschist();
DeleteTable_InfraredGroup_Command_Preferred_pschmask();
DeleteTable_InfraredGroup_Command_pschist();
DeleteTable_InfraredGroup_Command_pschmask();
DeleteTable_InfraredGroup_pschist();
DeleteTable_InfraredGroup_pschmask();
DeleteTable_InstallWizard();
DeleteTable_InstallWizard_Distro();
DeleteTable_InstallWizard_Distro_pschist();
DeleteTable_InstallWizard_Distro_pschmask();
DeleteTable_InstallWizard_pschist();
DeleteTable_InstallWizard_pschmask();
DeleteTable_Installation();
DeleteTable_Installation_RepositorySource_URL();
DeleteTable_Installation_RepositorySource_URL_pschist();
DeleteTable_Installation_RepositorySource_URL_pschmask();
DeleteTable_Installation_Users();
DeleteTable_Installation_Users_pschist();
DeleteTable_Installation_Users_pschmask();
DeleteTable_Installation_pschist();
DeleteTable_Installation_pschmask();
DeleteTable_Language();
DeleteTable_Language_pschist();
DeleteTable_Language_pschmask();
DeleteTable_License();
DeleteTable_License_pschist();
DeleteTable_License_pschmask();
DeleteTable_Licensing();
DeleteTable_Licensing_pschist();
DeleteTable_Licensing_pschmask();
DeleteTable_Manufacturer();
DeleteTable_Manufacturer_pschist();
DeleteTable_Manufacturer_pschmask();
DeleteTable_MediaType();
DeleteTable_MediaType_Broadcast();
DeleteTable_MediaType_Broadcast_pschist();
DeleteTable_MediaType_Broadcast_pschmask();
DeleteTable_MediaType_DesignObj();
DeleteTable_MediaType_DesignObj_pschist();
DeleteTable_MediaType_DesignObj_pschmask();
DeleteTable_MediaType_pschist();
DeleteTable_MediaType_pschmask();
DeleteTable_News();
DeleteTable_News_pschist();
DeleteTable_News_pschmask();
DeleteTable_OperatingSystem();
DeleteTable_OperatingSystem_pschist();
DeleteTable_OperatingSystem_pschmask();
DeleteTable_Orbiter();
DeleteTable_Orbiter_Users_PasswordReq();
DeleteTable_Orbiter_Users_PasswordReq_pschist();
DeleteTable_Orbiter_Users_PasswordReq_pschmask();
DeleteTable_Orbiter_Variable();
DeleteTable_Orbiter_Variable_pschist();
DeleteTable_Orbiter_Variable_pschmask();
DeleteTable_Orbiter_pschist();
DeleteTable_Orbiter_pschmask();
DeleteTable_Package();
DeleteTable_PackageType();
DeleteTable_PackageType_pschist();
DeleteTable_PackageType_pschmask();
DeleteTable_Package_Compat();
DeleteTable_Package_Compat_pschist();
DeleteTable_Package_Compat_pschmask();
DeleteTable_Package_Device();
DeleteTable_Package_Device_pschist();
DeleteTable_Package_Device_pschmask();
DeleteTable_Package_Directory();
DeleteTable_Package_Directory_File();
DeleteTable_Package_Directory_File_pschist();
DeleteTable_Package_Directory_File_pschmask();
DeleteTable_Package_Directory_pschist();
DeleteTable_Package_Directory_pschmask();
DeleteTable_Package_Package();
DeleteTable_Package_Package_pschist();
DeleteTable_Package_Package_pschmask();
DeleteTable_Package_Source();
DeleteTable_Package_Source_Compat();
DeleteTable_Package_Source_Compat_pschist();
DeleteTable_Package_Source_Compat_pschmask();
DeleteTable_Package_Source_pschist();
DeleteTable_Package_Source_pschmask();
DeleteTable_Package_Users();
DeleteTable_Package_Users_pschist();
DeleteTable_Package_Users_pschmask();
DeleteTable_Package_Version();
DeleteTable_Package_Version_pschist();
DeleteTable_Package_Version_pschmask();
DeleteTable_Package_pschist();
DeleteTable_Package_pschmask();
DeleteTable_PageSetup();
DeleteTable_PageSetup_pschist();
DeleteTable_PageSetup_pschmask();
DeleteTable_PaidLicense();
DeleteTable_PaidLicense_pschist();
DeleteTable_PaidLicense_pschmask();
DeleteTable_ParameterType();
DeleteTable_ParameterType_pschist();
DeleteTable_ParameterType_pschmask();
DeleteTable_PhoneLineType();
DeleteTable_PhoneLineType_pschist();
DeleteTable_PhoneLineType_pschmask();
DeleteTable_Pipe();
DeleteTable_Pipe_pschist();
DeleteTable_Pipe_pschmask();
DeleteTable_PnpLevel();
DeleteTable_PnpLevel_pschist();
DeleteTable_PnpLevel_pschmask();
DeleteTable_PnpProtocol();
DeleteTable_PnpProtocol_pschist();
DeleteTable_PnpProtocol_pschmask();
DeleteTable_PnpQueue();
DeleteTable_PostalCode();
DeleteTable_QuickStartCategory();
DeleteTable_QuickStartCategory_pschist();
DeleteTable_QuickStartCategory_pschmask();
DeleteTable_QuickStartTemplate();
DeleteTable_QuickStartTemplate_pschist();
DeleteTable_QuickStartTemplate_pschmask();
DeleteTable_Region();
DeleteTable_RemoteControl();
DeleteTable_RemoteControl_pschist();
DeleteTable_RemoteControl_pschmask();
DeleteTable_RemoteMapping();
DeleteTable_RemoteMapping_pschist();
DeleteTable_RemoteMapping_pschmask();
DeleteTable_RepositorySource();
DeleteTable_RepositorySource_URL();
DeleteTable_RepositorySource_URL_pschist();
DeleteTable_RepositorySource_URL_pschmask();
DeleteTable_RepositorySource_pschist();
DeleteTable_RepositorySource_pschmask();
DeleteTable_RepositoryType();
DeleteTable_RepositoryType_pschist();
DeleteTable_RepositoryType_pschmask();
DeleteTable_Room();
DeleteTable_RoomType();
DeleteTable_RoomType_pschist();
DeleteTable_RoomType_pschmask();
DeleteTable_Room_Users();
DeleteTable_Room_Users_pschist();
DeleteTable_Room_Users_pschmask();
DeleteTable_Room_pschist();
DeleteTable_Room_pschmask();
DeleteTable_Schema();
DeleteTable_Schema_pschist();
DeleteTable_Schema_pschmask();
DeleteTable_Screen();
DeleteTable_Screen_CommandParameter();
DeleteTable_Screen_CommandParameter_pschist();
DeleteTable_Screen_CommandParameter_pschmask();
DeleteTable_Screen_DesignObj();
DeleteTable_Screen_DesignObj_pschist();
DeleteTable_Screen_DesignObj_pschmask();
DeleteTable_Screen_pschist();
DeleteTable_Screen_pschmask();
DeleteTable_SetupStep();
DeleteTable_SetupStep_pschist();
DeleteTable_SetupStep_pschmask();
DeleteTable_Size();
DeleteTable_Size_pschist();
DeleteTable_Size_pschmask();
DeleteTable_Skin();
DeleteTable_Skin_pschist();
DeleteTable_Skin_pschmask();
DeleteTable_Software();
DeleteTable_Software_Device();
DeleteTable_Software_Source();
DeleteTable_StabilityStatus();
DeleteTable_StabilityStatus_pschist();
DeleteTable_StabilityStatus_pschmask();
DeleteTable_StartupScript();
DeleteTable_StartupScript_pschist();
DeleteTable_StartupScript_pschmask();
DeleteTable_Style();
DeleteTable_StyleVariation();
DeleteTable_StyleVariation_pschist();
DeleteTable_StyleVariation_pschmask();
DeleteTable_Style_pschist();
DeleteTable_Style_pschmask();
DeleteTable_System();
DeleteTable_System_pschist();
DeleteTable_System_pschmask();
DeleteTable_Template();
DeleteTable_Template_pschist();
DeleteTable_Template_pschmask();
DeleteTable_Text();
DeleteTable_TextCategory();
DeleteTable_TextCategory_pschist();
DeleteTable_TextCategory_pschmask();
DeleteTable_Text_LS();
DeleteTable_Text_LS_AltVersions();
DeleteTable_Text_LS_AltVersions_pschist();
DeleteTable_Text_LS_AltVersions_pschmask();
DeleteTable_Text_LS_pschist();
DeleteTable_Text_LS_pschmask();
DeleteTable_Text_pschist();
DeleteTable_Text_pschmask();
DeleteTable_TimeZone();
DeleteTable_UI();
DeleteTable_UI_pschist();
DeleteTable_UI_pschmask();
DeleteTable_UnknownDevices();
DeleteTable_UnknownDevices_pschist();
DeleteTable_UnknownDevices_pschmask();
DeleteTable_UserMode();
DeleteTable_UserMode_pschist();
DeleteTable_UserMode_pschmask();
DeleteTable_Users();
DeleteTable_Users_pschist();
DeleteTable_Users_pschmask();
DeleteTable_Variable();
DeleteTable_Variable_pschist();
DeleteTable_Variable_pschmask();
DeleteTable_Version();
DeleteTable_Version_pschist();
DeleteTable_Version_pschmask();
DeleteTable_VertAlignment();
DeleteTable_VertAlignment_pschist();
DeleteTable_VertAlignment_pschmask();
DeleteTable_psc_constants_batdet();
DeleteTable_psc_constants_bathdr();
DeleteTable_psc_constants_batuser();
DeleteTable_psc_constants_repset();
DeleteTable_psc_constants_schema();
DeleteTable_psc_constants_tables();
DeleteTable_psc_dce_batdet();
DeleteTable_psc_dce_bathdr();
DeleteTable_psc_dce_batuser();
DeleteTable_psc_dce_repset();
DeleteTable_psc_dce_schema();
DeleteTable_psc_dce_tables();
DeleteTable_psc_designer_batdet();
DeleteTable_psc_designer_bathdr();
DeleteTable_psc_designer_batuser();
DeleteTable_psc_designer_repset();
DeleteTable_psc_designer_schema();
DeleteTable_psc_designer_tables();
DeleteTable_psc_document_batdet();
DeleteTable_psc_document_bathdr();
DeleteTable_psc_document_batuser();
DeleteTable_psc_document_repset();
DeleteTable_psc_document_schema();
DeleteTable_psc_document_tables();
DeleteTable_psc_ir_batdet();
DeleteTable_psc_ir_bathdr();
DeleteTable_psc_ir_batuser();
DeleteTable_psc_ir_repset();
DeleteTable_psc_ir_schema();
DeleteTable_psc_ir_tables();
DeleteTable_psc_local_batdet();
DeleteTable_psc_local_bathdr();
DeleteTable_psc_local_batuser();
DeleteTable_psc_local_repset();
DeleteTable_psc_local_schema();
DeleteTable_psc_local_tables();
DeleteTable_psc_website_batdet();
DeleteTable_psc_website_bathdr();
DeleteTable_psc_website_batuser();
DeleteTable_psc_website_repset();
DeleteTable_psc_website_schema();
DeleteTable_psc_website_tables();
}

bool Database_pluto_main::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_main::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
