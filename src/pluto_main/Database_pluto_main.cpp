#ifdef WIN32
	#include <WinSock2.h>
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
tblBroadcast=NULL;
tblButton=NULL;
tblCachedScreens=NULL;
tblCannedEvents=NULL;
tblCannedEvents_CriteriaParmList=NULL;
tblCity=NULL;
tblCommMethod=NULL;
tblCommand=NULL;
tblCommandCategory=NULL;
tblCommandGroup=NULL;
tblCommandGroup_Command=NULL;
tblCommandGroup_Command_CommandParameter=NULL;
tblCommandGroup_D=NULL;
tblCommandGroup_D_Command=NULL;
tblCommandGroup_D_Command_CommandParameter=NULL;
tblCommandGroup_EntertainArea=NULL;
tblCommandGroup_Room=NULL;
tblCommandParameter=NULL;
tblCommand_CommandParameter=NULL;
tblCommand_Pipe=NULL;
tblConfigType=NULL;
tblConfigType_File=NULL;
tblConfigType_Setting=NULL;
tblConfigType_Token=NULL;
tblConnectorType=NULL;
tblCountry=NULL;
tblCriteria=NULL;
tblCriteriaList=NULL;
tblCriteriaList_CriteriaParmList=NULL;
tblCriteriaParm=NULL;
tblCriteriaParmList=NULL;
tblCriteriaParmNesting=NULL;
tblDHCPDevice=NULL;
tblDHCPDevice_DeviceData=NULL;
tblDataGrid=NULL;
tblDesignObj=NULL;
tblDesignObjCategory=NULL;
tblDesignObjParameter=NULL;
tblDesignObjType=NULL;
tblDesignObjType_DesignObjParameter=NULL;
tblDesignObjVariation=NULL;
tblDesignObjVariation_DesignObj=NULL;
tblDesignObjVariation_DesignObjParameter=NULL;
tblDesignObjVariation_DesignObj_Skin_Language=NULL;
tblDesignObjVariation_Text=NULL;
tblDesignObjVariation_Text_Skin_Language=NULL;
tblDesignObjVariation_Zone=NULL;
tblDevice=NULL;
tblDeviceCategory=NULL;
tblDeviceCategory_DeviceData=NULL;
tblDeviceCategory_Event=NULL;
tblDeviceCommandGroup=NULL;
tblDeviceCommandGroup_Command=NULL;
tblDeviceCommandGroup_DeviceCommandGroup_Parent=NULL;
tblDeviceData=NULL;
tblDeviceGroup=NULL;
tblDeviceTemplate=NULL;
tblDeviceTemplate_AV=NULL;
tblDeviceTemplate_DSPMode=NULL;
tblDeviceTemplate_DesignObj=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia=NULL;
tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceCommandGroup=NULL;
tblDeviceTemplate_DeviceData=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia=NULL;
tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe=NULL;
tblDeviceTemplate_DeviceTemplate_Related=NULL;
tblDeviceTemplate_Event=NULL;
tblDeviceTemplate_InfraredGroup=NULL;
tblDeviceTemplate_Input=NULL;
tblDeviceTemplate_MediaType=NULL;
tblDeviceTemplate_MediaType_DesignObj=NULL;
tblDeviceTemplate_Output=NULL;
tblDeviceTemplate_PageSetup=NULL;
tblDevice_Command=NULL;
tblDevice_CommandGroup=NULL;
tblDevice_DeviceData=NULL;
tblDevice_DeviceGroup=NULL;
tblDevice_Device_Pipe=NULL;
tblDevice_Device_Related=NULL;
tblDevice_EntertainArea=NULL;
tblDevice_HouseMode=NULL;
tblDevice_MRU=NULL;
tblDevice_Orbiter=NULL;
tblDevice_QuickStart=NULL;
tblDevice_StartupScript=NULL;
tblDevice_Users=NULL;
tblDirection=NULL;
tblDirectory=NULL;
tblDistro=NULL;
tblDocument=NULL;
tblDocument_Comment=NULL;
tblEffect=NULL;
tblEffectType=NULL;
tblEffectType_Effect_Skin=NULL;
tblEntertainArea=NULL;
tblEvent=NULL;
tblEventCategory=NULL;
tblEventHandler=NULL;
tblEventParameter=NULL;
tblEvent_EventParameter=NULL;
tblFAQ=NULL;
tblFirewall=NULL;
tblFloorplan=NULL;
tblFloorplanObjectType=NULL;
tblFloorplanObjectType_Color=NULL;
tblFloorplanType=NULL;
tblHorizAlignment=NULL;
tblHouseMode=NULL;
tblHousehold=NULL;
tblHousehold_Installation=NULL;
tblIcon=NULL;
tblImage=NULL;
tblInfraredGroup=NULL;
tblInfraredGroup_Command=NULL;
tblInfraredGroup_Command_Preferred=NULL;
tblInstallWizard=NULL;
tblInstallWizard_Distro=NULL;
tblInstallation=NULL;
tblInstallation_RepositorySource_URL=NULL;
tblInstallation_Users=NULL;
tblLanguage=NULL;
tblLicense=NULL;
tblLicensing=NULL;
tblManufacturer=NULL;
tblMediaType=NULL;
tblMediaType_Broadcast=NULL;
tblMediaType_DesignObj=NULL;
tblNews=NULL;
tblOperatingSystem=NULL;
tblOrbiter=NULL;
tblOrbiter_Users_PasswordReq=NULL;
tblOrbiter_Variable=NULL;
tblPackage=NULL;
tblPackageType=NULL;
tblPackage_Compat=NULL;
tblPackage_Device=NULL;
tblPackage_Directory=NULL;
tblPackage_Directory_File=NULL;
tblPackage_Package=NULL;
tblPackage_Source=NULL;
tblPackage_Source_Compat=NULL;
tblPackage_Users=NULL;
tblPageSetup=NULL;
tblPaidLicense=NULL;
tblParameterType=NULL;
tblPhoneLineType=NULL;
tblPipe=NULL;
tblPnpLevel=NULL;
tblPnpProtocol=NULL;
tblPnpQueue=NULL;
tblPostalCode=NULL;
tblQuickStartCategory=NULL;
tblQuickStartTemplate=NULL;
tblRegion=NULL;
tblRemoteControl=NULL;
tblRemoteMapping=NULL;
tblRepositorySource=NULL;
tblRepositorySource_URL=NULL;
tblRepositoryType=NULL;
tblRoom=NULL;
tblRoomType=NULL;
tblRoom_Users=NULL;
tblSchema=NULL;
tblScreen=NULL;
tblScreen_CommandParameter=NULL;
tblScreen_DesignObj=NULL;
tblSetupStep=NULL;
tblSize=NULL;
tblSkin=NULL;
tblSoftware=NULL;
tblSoftware_Device=NULL;
tblSoftware_Source=NULL;
tblStabilityStatus=NULL;
tblStartupScript=NULL;
tblStyle=NULL;
tblStyleVariation=NULL;
tblSystem=NULL;
tblTemplate=NULL;
tblText=NULL;
tblTextCategory=NULL;
tblText_LS=NULL;
tblText_LS_AltVersions=NULL;
tblTimeZone=NULL;
tblUI=NULL;
tblUnknownDevices=NULL;
tblUserMode=NULL;
tblUsers=NULL;
tblVariable=NULL;
tblVersion=NULL;
tblVertAlignment=NULL;
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
if( tblBroadcast!=NULL )
	if( !Commit_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblButton!=NULL )
	if( !Commit_Button(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCachedScreens!=NULL )
	if( !Commit_CachedScreens(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents!=NULL )
	if( !Commit_CannedEvents(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCannedEvents_CriteriaParmList!=NULL )
	if( !Commit_CannedEvents_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCity!=NULL )
	if( !Commit_City(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommMethod!=NULL )
	if( !Commit_CommMethod(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand!=NULL )
	if( !Commit_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandCategory!=NULL )
	if( !Commit_CommandCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblCommandGroup_D!=NULL )
	if( !Commit_CommandGroup_D(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command!=NULL )
	if( !Commit_CommandGroup_D_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_D_Command_CommandParameter!=NULL )
	if( !Commit_CommandGroup_D_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_EntertainArea!=NULL )
	if( !Commit_CommandGroup_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandGroup_Room!=NULL )
	if( !Commit_CommandGroup_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommandParameter!=NULL )
	if( !Commit_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_CommandParameter!=NULL )
	if( !Commit_Command_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCommand_Pipe!=NULL )
	if( !Commit_Command_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType!=NULL )
	if( !Commit_ConfigType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_File!=NULL )
	if( !Commit_ConfigType_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Setting!=NULL )
	if( !Commit_ConfigType_Setting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConfigType_Token!=NULL )
	if( !Commit_ConfigType_Token(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblConnectorType!=NULL )
	if( !Commit_ConnectorType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblCriteriaParm!=NULL )
	if( !Commit_CriteriaParm(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmList!=NULL )
	if( !Commit_CriteriaParmList(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCriteriaParmNesting!=NULL )
	if( !Commit_CriteriaParmNesting(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice!=NULL )
	if( !Commit_DHCPDevice(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDHCPDevice_DeviceData!=NULL )
	if( !Commit_DHCPDevice_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDataGrid!=NULL )
	if( !Commit_DataGrid(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObj!=NULL )
	if( !Commit_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjCategory!=NULL )
	if( !Commit_DesignObjCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjParameter!=NULL )
	if( !Commit_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType!=NULL )
	if( !Commit_DesignObjType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjType_DesignObjParameter!=NULL )
	if( !Commit_DesignObjType_DesignObjParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblDesignObjVariation_DesignObj_Skin_Language!=NULL )
	if( !Commit_DesignObjVariation_DesignObj_Skin_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text!=NULL )
	if( !Commit_DesignObjVariation_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Text_Skin_Language!=NULL )
	if( !Commit_DesignObjVariation_Text_Skin_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDesignObjVariation_Zone!=NULL )
	if( !Commit_DesignObjVariation_Zone(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblDeviceCategory_Event!=NULL )
	if( !Commit_DeviceCategory_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup!=NULL )
	if( !Commit_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_Command!=NULL )
	if( !Commit_DeviceCommandGroup_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceCommandGroup_DeviceCommandGroup_Parent!=NULL )
	if( !Commit_DeviceCommandGroup_DeviceCommandGroup_Parent(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceData!=NULL )
	if( !Commit_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceGroup!=NULL )
	if( !Commit_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate!=NULL )
	if( !Commit_DeviceTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_AV!=NULL )
	if( !Commit_DeviceTemplate_AV(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DSPMode!=NULL )
	if( !Commit_DeviceTemplate_DSPMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DesignObj!=NULL )
	if( !Commit_DeviceTemplate_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe!=NULL )
	if( !Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceCommandGroup!=NULL )
	if( !Commit_DeviceTemplate_DeviceCommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceData!=NULL )
	if( !Commit_DeviceTemplate_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_DeviceTemplate_Related!=NULL )
	if( !Commit_DeviceTemplate_DeviceTemplate_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Event!=NULL )
	if( !Commit_DeviceTemplate_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_InfraredGroup!=NULL )
	if( !Commit_DeviceTemplate_InfraredGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Input!=NULL )
	if( !Commit_DeviceTemplate_Input(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType!=NULL )
	if( !Commit_DeviceTemplate_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_MediaType_DesignObj!=NULL )
	if( !Commit_DeviceTemplate_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_Output!=NULL )
	if( !Commit_DeviceTemplate_Output(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDeviceTemplate_PageSetup!=NULL )
	if( !Commit_DeviceTemplate_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Command!=NULL )
	if( !Commit_Device_Command(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_CommandGroup!=NULL )
	if( !Commit_Device_CommandGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceData!=NULL )
	if( !Commit_Device_DeviceData(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_DeviceGroup!=NULL )
	if( !Commit_Device_DeviceGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Pipe!=NULL )
	if( !Commit_Device_Device_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Device_Related!=NULL )
	if( !Commit_Device_Device_Related(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_EntertainArea!=NULL )
	if( !Commit_Device_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_HouseMode!=NULL )
	if( !Commit_Device_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_MRU!=NULL )
	if( !Commit_Device_MRU(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Orbiter!=NULL )
	if( !Commit_Device_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_QuickStart!=NULL )
	if( !Commit_Device_QuickStart(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_StartupScript!=NULL )
	if( !Commit_Device_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDevice_Users!=NULL )
	if( !Commit_Device_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirection!=NULL )
	if( !Commit_Direction(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDirectory!=NULL )
	if( !Commit_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDistro!=NULL )
	if( !Commit_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument!=NULL )
	if( !Commit_Document(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDocument_Comment!=NULL )
	if( !Commit_Document_Comment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblEntertainArea!=NULL )
	if( !Commit_EntertainArea(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent!=NULL )
	if( !Commit_Event(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventCategory!=NULL )
	if( !Commit_EventCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventHandler!=NULL )
	if( !Commit_EventHandler(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEventParameter!=NULL )
	if( !Commit_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblEvent_EventParameter!=NULL )
	if( !Commit_Event_EventParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFAQ!=NULL )
	if( !Commit_FAQ(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFirewall!=NULL )
	if( !Commit_Firewall(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblFloorplanType!=NULL )
	if( !Commit_FloorplanType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHorizAlignment!=NULL )
	if( !Commit_HorizAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHouseMode!=NULL )
	if( !Commit_HouseMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold!=NULL )
	if( !Commit_Household(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblHousehold_Installation!=NULL )
	if( !Commit_Household_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblIcon!=NULL )
	if( !Commit_Icon(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblImage!=NULL )
	if( !Commit_Image(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblInstallWizard!=NULL )
	if( !Commit_InstallWizard(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallWizard_Distro!=NULL )
	if( !Commit_InstallWizard_Distro(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation!=NULL )
	if( !Commit_Installation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_RepositorySource_URL!=NULL )
	if( !Commit_Installation_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblInstallation_Users!=NULL )
	if( !Commit_Installation_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLanguage!=NULL )
	if( !Commit_Language(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicense!=NULL )
	if( !Commit_License(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLicensing!=NULL )
	if( !Commit_Licensing(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblManufacturer!=NULL )
	if( !Commit_Manufacturer(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType!=NULL )
	if( !Commit_MediaType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_Broadcast!=NULL )
	if( !Commit_MediaType_Broadcast(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_DesignObj!=NULL )
	if( !Commit_MediaType_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblNews!=NULL )
	if( !Commit_News(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOperatingSystem!=NULL )
	if( !Commit_OperatingSystem(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter!=NULL )
	if( !Commit_Orbiter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Users_PasswordReq!=NULL )
	if( !Commit_Orbiter_Users_PasswordReq(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblOrbiter_Variable!=NULL )
	if( !Commit_Orbiter_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage!=NULL )
	if( !Commit_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackageType!=NULL )
	if( !Commit_PackageType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Compat!=NULL )
	if( !Commit_Package_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Device!=NULL )
	if( !Commit_Package_Device(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory!=NULL )
	if( !Commit_Package_Directory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Directory_File!=NULL )
	if( !Commit_Package_Directory_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Package!=NULL )
	if( !Commit_Package_Package(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source!=NULL )
	if( !Commit_Package_Source(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Source_Compat!=NULL )
	if( !Commit_Package_Source_Compat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPackage_Users!=NULL )
	if( !Commit_Package_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPageSetup!=NULL )
	if( !Commit_PageSetup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPaidLicense!=NULL )
	if( !Commit_PaidLicense(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblParameterType!=NULL )
	if( !Commit_ParameterType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPhoneLineType!=NULL )
	if( !Commit_PhoneLineType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPipe!=NULL )
	if( !Commit_Pipe(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpLevel!=NULL )
	if( !Commit_PnpLevel(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPnpProtocol!=NULL )
	if( !Commit_PnpProtocol(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblQuickStartTemplate!=NULL )
	if( !Commit_QuickStartTemplate(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRegion!=NULL )
	if( !Commit_Region(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteControl!=NULL )
	if( !Commit_RemoteControl(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRemoteMapping!=NULL )
	if( !Commit_RemoteMapping(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource!=NULL )
	if( !Commit_RepositorySource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositorySource_URL!=NULL )
	if( !Commit_RepositorySource_URL(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRepositoryType!=NULL )
	if( !Commit_RepositoryType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom!=NULL )
	if( !Commit_Room(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoomType!=NULL )
	if( !Commit_RoomType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRoom_Users!=NULL )
	if( !Commit_Room_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSchema!=NULL )
	if( !Commit_Schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen!=NULL )
	if( !Commit_Screen(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_CommandParameter!=NULL )
	if( !Commit_Screen_CommandParameter(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblScreen_DesignObj!=NULL )
	if( !Commit_Screen_DesignObj(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSetupStep!=NULL )
	if( !Commit_SetupStep(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSize!=NULL )
	if( !Commit_Size(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSkin!=NULL )
	if( !Commit_Skin(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
if( tblStartupScript!=NULL )
	if( !Commit_StartupScript(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyle!=NULL )
	if( !Commit_Style(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblStyleVariation!=NULL )
	if( !Commit_StyleVariation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSystem!=NULL )
	if( !Commit_System(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTemplate!=NULL )
	if( !Commit_Template(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText!=NULL )
	if( !Commit_Text(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTextCategory!=NULL )
	if( !Commit_TextCategory(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS!=NULL )
	if( !Commit_Text_LS(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblText_LS_AltVersions!=NULL )
	if( !Commit_Text_LS_AltVersions(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblTimeZone!=NULL )
	if( !Commit_TimeZone(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUI!=NULL )
	if( !Commit_UI(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUnknownDevices!=NULL )
	if( !Commit_UnknownDevices(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUserMode!=NULL )
	if( !Commit_UserMode(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblUsers!=NULL )
	if( !Commit_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVariable!=NULL )
	if( !Commit_Variable(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVersion!=NULL )
	if( !Commit_Version(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblVertAlignment!=NULL )
	if( !Commit_VertAlignment(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
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
DeleteTable_Broadcast();
DeleteTable_Button();
DeleteTable_CachedScreens();
DeleteTable_CannedEvents();
DeleteTable_CannedEvents_CriteriaParmList();
DeleteTable_City();
DeleteTable_CommMethod();
DeleteTable_Command();
DeleteTable_CommandCategory();
DeleteTable_CommandGroup();
DeleteTable_CommandGroup_Command();
DeleteTable_CommandGroup_Command_CommandParameter();
DeleteTable_CommandGroup_D();
DeleteTable_CommandGroup_D_Command();
DeleteTable_CommandGroup_D_Command_CommandParameter();
DeleteTable_CommandGroup_EntertainArea();
DeleteTable_CommandGroup_Room();
DeleteTable_CommandParameter();
DeleteTable_Command_CommandParameter();
DeleteTable_Command_Pipe();
DeleteTable_ConfigType();
DeleteTable_ConfigType_File();
DeleteTable_ConfigType_Setting();
DeleteTable_ConfigType_Token();
DeleteTable_ConnectorType();
DeleteTable_Country();
DeleteTable_Criteria();
DeleteTable_CriteriaList();
DeleteTable_CriteriaList_CriteriaParmList();
DeleteTable_CriteriaParm();
DeleteTable_CriteriaParmList();
DeleteTable_CriteriaParmNesting();
DeleteTable_DHCPDevice();
DeleteTable_DHCPDevice_DeviceData();
DeleteTable_DataGrid();
DeleteTable_DesignObj();
DeleteTable_DesignObjCategory();
DeleteTable_DesignObjParameter();
DeleteTable_DesignObjType();
DeleteTable_DesignObjType_DesignObjParameter();
DeleteTable_DesignObjVariation();
DeleteTable_DesignObjVariation_DesignObj();
DeleteTable_DesignObjVariation_DesignObjParameter();
DeleteTable_DesignObjVariation_DesignObj_Skin_Language();
DeleteTable_DesignObjVariation_Text();
DeleteTable_DesignObjVariation_Text_Skin_Language();
DeleteTable_DesignObjVariation_Zone();
DeleteTable_Device();
DeleteTable_DeviceCategory();
DeleteTable_DeviceCategory_DeviceData();
DeleteTable_DeviceCategory_Event();
DeleteTable_DeviceCommandGroup();
DeleteTable_DeviceCommandGroup_Command();
DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
DeleteTable_DeviceData();
DeleteTable_DeviceGroup();
DeleteTable_DeviceTemplate();
DeleteTable_DeviceTemplate_AV();
DeleteTable_DeviceTemplate_DSPMode();
DeleteTable_DeviceTemplate_DesignObj();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia();
DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceCommandGroup();
DeleteTable_DeviceTemplate_DeviceData();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia();
DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
DeleteTable_DeviceTemplate_DeviceTemplate_Related();
DeleteTable_DeviceTemplate_Event();
DeleteTable_DeviceTemplate_InfraredGroup();
DeleteTable_DeviceTemplate_Input();
DeleteTable_DeviceTemplate_MediaType();
DeleteTable_DeviceTemplate_MediaType_DesignObj();
DeleteTable_DeviceTemplate_Output();
DeleteTable_DeviceTemplate_PageSetup();
DeleteTable_Device_Command();
DeleteTable_Device_CommandGroup();
DeleteTable_Device_DeviceData();
DeleteTable_Device_DeviceGroup();
DeleteTable_Device_Device_Pipe();
DeleteTable_Device_Device_Related();
DeleteTable_Device_EntertainArea();
DeleteTable_Device_HouseMode();
DeleteTable_Device_MRU();
DeleteTable_Device_Orbiter();
DeleteTable_Device_QuickStart();
DeleteTable_Device_StartupScript();
DeleteTable_Device_Users();
DeleteTable_Direction();
DeleteTable_Directory();
DeleteTable_Distro();
DeleteTable_Document();
DeleteTable_Document_Comment();
DeleteTable_Effect();
DeleteTable_EffectType();
DeleteTable_EffectType_Effect_Skin();
DeleteTable_EntertainArea();
DeleteTable_Event();
DeleteTable_EventCategory();
DeleteTable_EventHandler();
DeleteTable_EventParameter();
DeleteTable_Event_EventParameter();
DeleteTable_FAQ();
DeleteTable_Firewall();
DeleteTable_Floorplan();
DeleteTable_FloorplanObjectType();
DeleteTable_FloorplanObjectType_Color();
DeleteTable_FloorplanType();
DeleteTable_HorizAlignment();
DeleteTable_HouseMode();
DeleteTable_Household();
DeleteTable_Household_Installation();
DeleteTable_Icon();
DeleteTable_Image();
DeleteTable_InfraredGroup();
DeleteTable_InfraredGroup_Command();
DeleteTable_InfraredGroup_Command_Preferred();
DeleteTable_InstallWizard();
DeleteTable_InstallWizard_Distro();
DeleteTable_Installation();
DeleteTable_Installation_RepositorySource_URL();
DeleteTable_Installation_Users();
DeleteTable_Language();
DeleteTable_License();
DeleteTable_Licensing();
DeleteTable_Manufacturer();
DeleteTable_MediaType();
DeleteTable_MediaType_Broadcast();
DeleteTable_MediaType_DesignObj();
DeleteTable_News();
DeleteTable_OperatingSystem();
DeleteTable_Orbiter();
DeleteTable_Orbiter_Users_PasswordReq();
DeleteTable_Orbiter_Variable();
DeleteTable_Package();
DeleteTable_PackageType();
DeleteTable_Package_Compat();
DeleteTable_Package_Device();
DeleteTable_Package_Directory();
DeleteTable_Package_Directory_File();
DeleteTable_Package_Package();
DeleteTable_Package_Source();
DeleteTable_Package_Source_Compat();
DeleteTable_Package_Users();
DeleteTable_PageSetup();
DeleteTable_PaidLicense();
DeleteTable_ParameterType();
DeleteTable_PhoneLineType();
DeleteTable_Pipe();
DeleteTable_PnpLevel();
DeleteTable_PnpProtocol();
DeleteTable_PnpQueue();
DeleteTable_PostalCode();
DeleteTable_QuickStartCategory();
DeleteTable_QuickStartTemplate();
DeleteTable_Region();
DeleteTable_RemoteControl();
DeleteTable_RemoteMapping();
DeleteTable_RepositorySource();
DeleteTable_RepositorySource_URL();
DeleteTable_RepositoryType();
DeleteTable_Room();
DeleteTable_RoomType();
DeleteTable_Room_Users();
DeleteTable_Schema();
DeleteTable_Screen();
DeleteTable_Screen_CommandParameter();
DeleteTable_Screen_DesignObj();
DeleteTable_SetupStep();
DeleteTable_Size();
DeleteTable_Skin();
DeleteTable_Software();
DeleteTable_Software_Device();
DeleteTable_Software_Source();
DeleteTable_StabilityStatus();
DeleteTable_StartupScript();
DeleteTable_Style();
DeleteTable_StyleVariation();
DeleteTable_System();
DeleteTable_Template();
DeleteTable_Text();
DeleteTable_TextCategory();
DeleteTable_Text_LS();
DeleteTable_Text_LS_AltVersions();
DeleteTable_TimeZone();
DeleteTable_UI();
DeleteTable_UnknownDevices();
DeleteTable_UserMode();
DeleteTable_Users();
DeleteTable_Variable();
DeleteTable_Version();
DeleteTable_VertAlignment();
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
