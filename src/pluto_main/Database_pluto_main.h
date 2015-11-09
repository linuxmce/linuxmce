#ifndef __Database_pluto_main_H_
#define __Database_pluto_main_H_
#include "PlutoUtils/DBHelper.h"
#include "DCE/Logger.h"
#ifdef WIN32
#ifdef EXPORT_DLL
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllexport)
	#endif
#else
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllimport)
	#endif
#endif
#else
#define DECLSPECIFIER
#endif
class DECLSPECIFIER DBHelper;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Database_pluto_main: public DBHelper
{
public:
Database_pluto_main(Logger *pLogger=NULL);
~Database_pluto_main();
void DeleteAllTables();
private:
class Table_Array* tblArray;
bool Commit_Array(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Broadcast* tblBroadcast;
bool Commit_Broadcast(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Button* tblButton;
bool Commit_Button(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CachedScreens* tblCachedScreens;
bool Commit_CachedScreens(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CannedEvents* tblCannedEvents;
bool Commit_CannedEvents(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CannedEvents_CriteriaParmList* tblCannedEvents_CriteriaParmList;
bool Commit_CannedEvents_CriteriaParmList(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_City* tblCity;
bool Commit_City(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommMethod* tblCommMethod;
bool Commit_CommMethod(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Command* tblCommand;
bool Commit_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandCategory* tblCommandCategory;
bool Commit_CommandCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup* tblCommandGroup;
bool Commit_CommandGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_Command* tblCommandGroup_Command;
bool Commit_CommandGroup_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_Command_CommandParameter* tblCommandGroup_Command_CommandParameter;
bool Commit_CommandGroup_Command_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_D* tblCommandGroup_D;
bool Commit_CommandGroup_D(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_D_Command* tblCommandGroup_D_Command;
bool Commit_CommandGroup_D_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_D_Command_CommandParameter* tblCommandGroup_D_Command_CommandParameter;
bool Commit_CommandGroup_D_Command_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_EntertainArea* tblCommandGroup_EntertainArea;
bool Commit_CommandGroup_EntertainArea(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandGroup_Room* tblCommandGroup_Room;
bool Commit_CommandGroup_Room(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CommandParameter* tblCommandParameter;
bool Commit_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Command_CommandParameter* tblCommand_CommandParameter;
bool Commit_Command_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Command_Pipe* tblCommand_Pipe;
bool Commit_Command_Pipe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ConfigType* tblConfigType;
bool Commit_ConfigType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ConfigType_File* tblConfigType_File;
bool Commit_ConfigType_File(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ConfigType_Setting* tblConfigType_Setting;
bool Commit_ConfigType_Setting(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ConfigType_Token* tblConfigType_Token;
bool Commit_ConfigType_Token(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ConnectorType* tblConnectorType;
bool Commit_ConnectorType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Country* tblCountry;
bool Commit_Country(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Criteria* tblCriteria;
bool Commit_Criteria(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CriteriaList* tblCriteriaList;
bool Commit_CriteriaList(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CriteriaList_CriteriaParmList* tblCriteriaList_CriteriaParmList;
bool Commit_CriteriaList_CriteriaParmList(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CriteriaParm* tblCriteriaParm;
bool Commit_CriteriaParm(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CriteriaParmList* tblCriteriaParmList;
bool Commit_CriteriaParmList(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_CriteriaParmNesting* tblCriteriaParmNesting;
bool Commit_CriteriaParmNesting(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DHCPDevice* tblDHCPDevice;
bool Commit_DHCPDevice(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DHCPDevice_DeviceData* tblDHCPDevice_DeviceData;
bool Commit_DHCPDevice_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DataGrid* tblDataGrid;
bool Commit_DataGrid(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObj* tblDesignObj;
bool Commit_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjCategory* tblDesignObjCategory;
bool Commit_DesignObjCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjParameter* tblDesignObjParameter;
bool Commit_DesignObjParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjType* tblDesignObjType;
bool Commit_DesignObjType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjType_DesignObjParameter* tblDesignObjType_DesignObjParameter;
bool Commit_DesignObjType_DesignObjParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation* tblDesignObjVariation;
bool Commit_DesignObjVariation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_DesignObj* tblDesignObjVariation_DesignObj;
bool Commit_DesignObjVariation_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_DesignObjParameter* tblDesignObjVariation_DesignObjParameter;
bool Commit_DesignObjVariation_DesignObjParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_DesignObj_Skin_Language* tblDesignObjVariation_DesignObj_Skin_Language;
bool Commit_DesignObjVariation_DesignObj_Skin_Language(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_Text* tblDesignObjVariation_Text;
bool Commit_DesignObjVariation_Text(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_Text_Skin_Language* tblDesignObjVariation_Text_Skin_Language;
bool Commit_DesignObjVariation_Text_Skin_Language(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DesignObjVariation_Zone* tblDesignObjVariation_Zone;
bool Commit_DesignObjVariation_Zone(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device* tblDevice;
bool Commit_Device(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCategory* tblDeviceCategory;
bool Commit_DeviceCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCategory_DeviceData* tblDeviceCategory_DeviceData;
bool Commit_DeviceCategory_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCategory_Event* tblDeviceCategory_Event;
bool Commit_DeviceCategory_Event(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCommandGroup* tblDeviceCommandGroup;
bool Commit_DeviceCommandGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCommandGroup_Command* tblDeviceCommandGroup_Command;
bool Commit_DeviceCommandGroup_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceCommandGroup_DeviceCommandGroup_Parent* tblDeviceCommandGroup_DeviceCommandGroup_Parent;
bool Commit_DeviceCommandGroup_DeviceCommandGroup_Parent(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceData* tblDeviceData;
bool Commit_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceGroup* tblDeviceGroup;
bool Commit_DeviceGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate* tblDeviceTemplate;
bool Commit_DeviceTemplate(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_AV* tblDeviceTemplate_AV;
bool Commit_DeviceTemplate_AV(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DSPMode* tblDeviceTemplate_DSPMode;
bool Commit_DeviceTemplate_DSPMode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DesignObj* tblDeviceTemplate_DesignObj;
bool Commit_DeviceTemplate_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceCategory_ControlledVia* tblDeviceTemplate_DeviceCategory_ControlledVia;
bool Commit_DeviceTemplate_DeviceCategory_ControlledVia(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe;
bool Commit_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceCommandGroup* tblDeviceTemplate_DeviceCommandGroup;
bool Commit_DeviceTemplate_DeviceCommandGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceData* tblDeviceTemplate_DeviceData;
bool Commit_DeviceTemplate_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceTemplate_ControlledVia* tblDeviceTemplate_DeviceTemplate_ControlledVia;
bool Commit_DeviceTemplate_DeviceTemplate_ControlledVia(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe* tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe;
bool Commit_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_DeviceTemplate_Related* tblDeviceTemplate_DeviceTemplate_Related;
bool Commit_DeviceTemplate_DeviceTemplate_Related(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_Event* tblDeviceTemplate_Event;
bool Commit_DeviceTemplate_Event(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_InfraredGroup* tblDeviceTemplate_InfraredGroup;
bool Commit_DeviceTemplate_InfraredGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_Input* tblDeviceTemplate_Input;
bool Commit_DeviceTemplate_Input(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_MediaType* tblDeviceTemplate_MediaType;
bool Commit_DeviceTemplate_MediaType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_MediaType_DesignObj* tblDeviceTemplate_MediaType_DesignObj;
bool Commit_DeviceTemplate_MediaType_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_Output* tblDeviceTemplate_Output;
bool Commit_DeviceTemplate_Output(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_DeviceTemplate_PageSetup* tblDeviceTemplate_PageSetup;
bool Commit_DeviceTemplate_PageSetup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_Command* tblDevice_Command;
bool Commit_Device_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_CommandGroup* tblDevice_CommandGroup;
bool Commit_Device_CommandGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_DeviceData* tblDevice_DeviceData;
bool Commit_Device_DeviceData(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_DeviceGroup* tblDevice_DeviceGroup;
bool Commit_Device_DeviceGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_Device_Pipe* tblDevice_Device_Pipe;
bool Commit_Device_Device_Pipe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_Device_Related* tblDevice_Device_Related;
bool Commit_Device_Device_Related(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_EntertainArea* tblDevice_EntertainArea;
bool Commit_Device_EntertainArea(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_HouseMode* tblDevice_HouseMode;
bool Commit_Device_HouseMode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_MRU* tblDevice_MRU;
bool Commit_Device_MRU(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_Orbiter* tblDevice_Orbiter;
bool Commit_Device_Orbiter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_QuickStart* tblDevice_QuickStart;
bool Commit_Device_QuickStart(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_StartupScript* tblDevice_StartupScript;
bool Commit_Device_StartupScript(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Device_Users* tblDevice_Users;
bool Commit_Device_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Direction* tblDirection;
bool Commit_Direction(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Directory* tblDirectory;
bool Commit_Directory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Distro* tblDistro;
bool Commit_Distro(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Document* tblDocument;
bool Commit_Document(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Document_Comment* tblDocument_Comment;
bool Commit_Document_Comment(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Effect* tblEffect;
bool Commit_Effect(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EffectType* tblEffectType;
bool Commit_EffectType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EffectType_Effect_Skin* tblEffectType_Effect_Skin;
bool Commit_EffectType_Effect_Skin(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EntertainArea* tblEntertainArea;
bool Commit_EntertainArea(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Event* tblEvent;
bool Commit_Event(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EventCategory* tblEventCategory;
bool Commit_EventCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EventHandler* tblEventHandler;
bool Commit_EventHandler(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_EventParameter* tblEventParameter;
bool Commit_EventParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Event_EventParameter* tblEvent_EventParameter;
bool Commit_Event_EventParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_FAQ* tblFAQ;
bool Commit_FAQ(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Firewall* tblFirewall;
bool Commit_Firewall(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Floorplan* tblFloorplan;
bool Commit_Floorplan(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_FloorplanObjectType* tblFloorplanObjectType;
bool Commit_FloorplanObjectType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_FloorplanObjectType_Color* tblFloorplanObjectType_Color;
bool Commit_FloorplanObjectType_Color(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_FloorplanType* tblFloorplanType;
bool Commit_FloorplanType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_HorizAlignment* tblHorizAlignment;
bool Commit_HorizAlignment(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_HouseMode* tblHouseMode;
bool Commit_HouseMode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Household* tblHousehold;
bool Commit_Household(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Household_Installation* tblHousehold_Installation;
bool Commit_Household_Installation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Icon* tblIcon;
bool Commit_Icon(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Image* tblImage;
bool Commit_Image(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_InfraredGroup* tblInfraredGroup;
bool Commit_InfraredGroup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_InfraredGroup_Command* tblInfraredGroup_Command;
bool Commit_InfraredGroup_Command(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_InfraredGroup_Command_Preferred* tblInfraredGroup_Command_Preferred;
bool Commit_InfraredGroup_Command_Preferred(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_InstallWizard* tblInstallWizard;
bool Commit_InstallWizard(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_InstallWizard_Distro* tblInstallWizard_Distro;
bool Commit_InstallWizard_Distro(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Installation* tblInstallation;
bool Commit_Installation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Installation_RepositorySource_URL* tblInstallation_RepositorySource_URL;
bool Commit_Installation_RepositorySource_URL(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Installation_Users* tblInstallation_Users;
bool Commit_Installation_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Language* tblLanguage;
bool Commit_Language(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_License* tblLicense;
bool Commit_License(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Licensing* tblLicensing;
bool Commit_Licensing(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Manufacturer* tblManufacturer;
bool Commit_Manufacturer(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType* tblMediaType;
bool Commit_MediaType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType_Broadcast* tblMediaType_Broadcast;
bool Commit_MediaType_Broadcast(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType_DesignObj* tblMediaType_DesignObj;
bool Commit_MediaType_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_News* tblNews;
bool Commit_News(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_OperatingSystem* tblOperatingSystem;
bool Commit_OperatingSystem(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Orbiter* tblOrbiter;
bool Commit_Orbiter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Orbiter_Users_PasswordReq* tblOrbiter_Users_PasswordReq;
bool Commit_Orbiter_Users_PasswordReq(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Orbiter_Variable* tblOrbiter_Variable;
bool Commit_Orbiter_Variable(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package* tblPackage;
bool Commit_Package(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PackageType* tblPackageType;
bool Commit_PackageType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Compat* tblPackage_Compat;
bool Commit_Package_Compat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Device* tblPackage_Device;
bool Commit_Package_Device(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Directory* tblPackage_Directory;
bool Commit_Package_Directory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Directory_File* tblPackage_Directory_File;
bool Commit_Package_Directory_File(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Package* tblPackage_Package;
bool Commit_Package_Package(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Source* tblPackage_Source;
bool Commit_Package_Source(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Source_Compat* tblPackage_Source_Compat;
bool Commit_Package_Source_Compat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Users* tblPackage_Users;
bool Commit_Package_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Package_Version* tblPackage_Version;
bool Commit_Package_Version(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PageSetup* tblPageSetup;
bool Commit_PageSetup(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PaidLicense* tblPaidLicense;
bool Commit_PaidLicense(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_ParameterType* tblParameterType;
bool Commit_ParameterType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PhoneLineType* tblPhoneLineType;
bool Commit_PhoneLineType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Pipe* tblPipe;
bool Commit_Pipe(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PnpLevel* tblPnpLevel;
bool Commit_PnpLevel(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PnpProtocol* tblPnpProtocol;
bool Commit_PnpProtocol(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PnpQueue* tblPnpQueue;
bool Commit_PnpQueue(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PostalCode* tblPostalCode;
bool Commit_PostalCode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_QuickStartCategory* tblQuickStartCategory;
bool Commit_QuickStartCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_QuickStartTemplate* tblQuickStartTemplate;
bool Commit_QuickStartTemplate(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Region* tblRegion;
bool Commit_Region(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RemoteControl* tblRemoteControl;
bool Commit_RemoteControl(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RemoteMapping* tblRemoteMapping;
bool Commit_RemoteMapping(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RepositorySource* tblRepositorySource;
bool Commit_RepositorySource(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RepositorySource_URL* tblRepositorySource_URL;
bool Commit_RepositorySource_URL(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RepositoryType* tblRepositoryType;
bool Commit_RepositoryType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Room* tblRoom;
bool Commit_Room(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_RoomType* tblRoomType;
bool Commit_RoomType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Room_Users* tblRoom_Users;
bool Commit_Room_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Schema* tblSchema;
bool Commit_Schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Screen* tblScreen;
bool Commit_Screen(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Screen_CommandParameter* tblScreen_CommandParameter;
bool Commit_Screen_CommandParameter(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Screen_DesignObj* tblScreen_DesignObj;
bool Commit_Screen_DesignObj(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_SetupStep* tblSetupStep;
bool Commit_SetupStep(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Size* tblSize;
bool Commit_Size(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Skin* tblSkin;
bool Commit_Skin(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Software* tblSoftware;
bool Commit_Software(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Software_Device* tblSoftware_Device;
bool Commit_Software_Device(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Software_Source* tblSoftware_Source;
bool Commit_Software_Source(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_StabilityStatus* tblStabilityStatus;
bool Commit_StabilityStatus(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_StartupScript* tblStartupScript;
bool Commit_StartupScript(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Style* tblStyle;
bool Commit_Style(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_StyleVariation* tblStyleVariation;
bool Commit_StyleVariation(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_System* tblSystem;
bool Commit_System(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Template* tblTemplate;
bool Commit_Template(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Text* tblText;
bool Commit_Text(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_TextCategory* tblTextCategory;
bool Commit_TextCategory(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Text_LS* tblText_LS;
bool Commit_Text_LS(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Text_LS_AltVersions* tblText_LS_AltVersions;
bool Commit_Text_LS_AltVersions(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_TimeZone* tblTimeZone;
bool Commit_TimeZone(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_UI* tblUI;
bool Commit_UI(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_UnknownDevices* tblUnknownDevices;
bool Commit_UnknownDevices(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_UserMode* tblUserMode;
bool Commit_UserMode(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Users* tblUsers;
bool Commit_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Variable* tblVariable;
bool Commit_Variable(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Version* tblVersion;
bool Commit_Version(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_VertAlignment* tblVertAlignment;
bool Commit_VertAlignment(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_batdet* tblpsc_constants_batdet;
bool Commit_psc_constants_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_bathdr* tblpsc_constants_bathdr;
bool Commit_psc_constants_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_batuser* tblpsc_constants_batuser;
bool Commit_psc_constants_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_repset* tblpsc_constants_repset;
bool Commit_psc_constants_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_schema* tblpsc_constants_schema;
bool Commit_psc_constants_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_constants_tables* tblpsc_constants_tables;
bool Commit_psc_constants_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_batdet* tblpsc_dce_batdet;
bool Commit_psc_dce_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_bathdr* tblpsc_dce_bathdr;
bool Commit_psc_dce_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_batuser* tblpsc_dce_batuser;
bool Commit_psc_dce_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_repset* tblpsc_dce_repset;
bool Commit_psc_dce_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_schema* tblpsc_dce_schema;
bool Commit_psc_dce_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_dce_tables* tblpsc_dce_tables;
bool Commit_psc_dce_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_batdet* tblpsc_designer_batdet;
bool Commit_psc_designer_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_bathdr* tblpsc_designer_bathdr;
bool Commit_psc_designer_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_batuser* tblpsc_designer_batuser;
bool Commit_psc_designer_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_repset* tblpsc_designer_repset;
bool Commit_psc_designer_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_schema* tblpsc_designer_schema;
bool Commit_psc_designer_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_designer_tables* tblpsc_designer_tables;
bool Commit_psc_designer_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_batdet* tblpsc_document_batdet;
bool Commit_psc_document_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_bathdr* tblpsc_document_bathdr;
bool Commit_psc_document_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_batuser* tblpsc_document_batuser;
bool Commit_psc_document_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_repset* tblpsc_document_repset;
bool Commit_psc_document_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_schema* tblpsc_document_schema;
bool Commit_psc_document_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_document_tables* tblpsc_document_tables;
bool Commit_psc_document_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_batdet* tblpsc_ir_batdet;
bool Commit_psc_ir_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_bathdr* tblpsc_ir_bathdr;
bool Commit_psc_ir_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_batuser* tblpsc_ir_batuser;
bool Commit_psc_ir_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_repset* tblpsc_ir_repset;
bool Commit_psc_ir_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_schema* tblpsc_ir_schema;
bool Commit_psc_ir_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_ir_tables* tblpsc_ir_tables;
bool Commit_psc_ir_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_batdet* tblpsc_local_batdet;
bool Commit_psc_local_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_bathdr* tblpsc_local_bathdr;
bool Commit_psc_local_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_batuser* tblpsc_local_batuser;
bool Commit_psc_local_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_repset* tblpsc_local_repset;
bool Commit_psc_local_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_schema* tblpsc_local_schema;
bool Commit_psc_local_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_local_tables* tblpsc_local_tables;
bool Commit_psc_local_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_batdet* tblpsc_website_batdet;
bool Commit_psc_website_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_bathdr* tblpsc_website_bathdr;
bool Commit_psc_website_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_batuser* tblpsc_website_batuser;
bool Commit_psc_website_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_repset* tblpsc_website_repset;
bool Commit_psc_website_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_schema* tblpsc_website_schema;
bool Commit_psc_website_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_website_tables* tblpsc_website_tables;
bool Commit_psc_website_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Array* Array_get() { if( !tblArray ) CreateTable_Array(); return tblArray; }
class Table_Broadcast* Broadcast_get() { if( !tblBroadcast ) CreateTable_Broadcast(); return tblBroadcast; }
class Table_Button* Button_get() { if( !tblButton ) CreateTable_Button(); return tblButton; }
class Table_CachedScreens* CachedScreens_get() { if( !tblCachedScreens ) CreateTable_CachedScreens(); return tblCachedScreens; }
class Table_CannedEvents* CannedEvents_get() { if( !tblCannedEvents ) CreateTable_CannedEvents(); return tblCannedEvents; }
class Table_CannedEvents_CriteriaParmList* CannedEvents_CriteriaParmList_get() { if( !tblCannedEvents_CriteriaParmList ) CreateTable_CannedEvents_CriteriaParmList(); return tblCannedEvents_CriteriaParmList; }
class Table_City* City_get() { if( !tblCity ) CreateTable_City(); return tblCity; }
class Table_CommMethod* CommMethod_get() { if( !tblCommMethod ) CreateTable_CommMethod(); return tblCommMethod; }
class Table_Command* Command_get() { if( !tblCommand ) CreateTable_Command(); return tblCommand; }
class Table_CommandCategory* CommandCategory_get() { if( !tblCommandCategory ) CreateTable_CommandCategory(); return tblCommandCategory; }
class Table_CommandGroup* CommandGroup_get() { if( !tblCommandGroup ) CreateTable_CommandGroup(); return tblCommandGroup; }
class Table_CommandGroup_Command* CommandGroup_Command_get() { if( !tblCommandGroup_Command ) CreateTable_CommandGroup_Command(); return tblCommandGroup_Command; }
class Table_CommandGroup_Command_CommandParameter* CommandGroup_Command_CommandParameter_get() { if( !tblCommandGroup_Command_CommandParameter ) CreateTable_CommandGroup_Command_CommandParameter(); return tblCommandGroup_Command_CommandParameter; }
class Table_CommandGroup_D* CommandGroup_D_get() { if( !tblCommandGroup_D ) CreateTable_CommandGroup_D(); return tblCommandGroup_D; }
class Table_CommandGroup_D_Command* CommandGroup_D_Command_get() { if( !tblCommandGroup_D_Command ) CreateTable_CommandGroup_D_Command(); return tblCommandGroup_D_Command; }
class Table_CommandGroup_D_Command_CommandParameter* CommandGroup_D_Command_CommandParameter_get() { if( !tblCommandGroup_D_Command_CommandParameter ) CreateTable_CommandGroup_D_Command_CommandParameter(); return tblCommandGroup_D_Command_CommandParameter; }
class Table_CommandGroup_EntertainArea* CommandGroup_EntertainArea_get() { if( !tblCommandGroup_EntertainArea ) CreateTable_CommandGroup_EntertainArea(); return tblCommandGroup_EntertainArea; }
class Table_CommandGroup_Room* CommandGroup_Room_get() { if( !tblCommandGroup_Room ) CreateTable_CommandGroup_Room(); return tblCommandGroup_Room; }
class Table_CommandParameter* CommandParameter_get() { if( !tblCommandParameter ) CreateTable_CommandParameter(); return tblCommandParameter; }
class Table_Command_CommandParameter* Command_CommandParameter_get() { if( !tblCommand_CommandParameter ) CreateTable_Command_CommandParameter(); return tblCommand_CommandParameter; }
class Table_Command_Pipe* Command_Pipe_get() { if( !tblCommand_Pipe ) CreateTable_Command_Pipe(); return tblCommand_Pipe; }
class Table_ConfigType* ConfigType_get() { if( !tblConfigType ) CreateTable_ConfigType(); return tblConfigType; }
class Table_ConfigType_File* ConfigType_File_get() { if( !tblConfigType_File ) CreateTable_ConfigType_File(); return tblConfigType_File; }
class Table_ConfigType_Setting* ConfigType_Setting_get() { if( !tblConfigType_Setting ) CreateTable_ConfigType_Setting(); return tblConfigType_Setting; }
class Table_ConfigType_Token* ConfigType_Token_get() { if( !tblConfigType_Token ) CreateTable_ConfigType_Token(); return tblConfigType_Token; }
class Table_ConnectorType* ConnectorType_get() { if( !tblConnectorType ) CreateTable_ConnectorType(); return tblConnectorType; }
class Table_Country* Country_get() { if( !tblCountry ) CreateTable_Country(); return tblCountry; }
class Table_Criteria* Criteria_get() { if( !tblCriteria ) CreateTable_Criteria(); return tblCriteria; }
class Table_CriteriaList* CriteriaList_get() { if( !tblCriteriaList ) CreateTable_CriteriaList(); return tblCriteriaList; }
class Table_CriteriaList_CriteriaParmList* CriteriaList_CriteriaParmList_get() { if( !tblCriteriaList_CriteriaParmList ) CreateTable_CriteriaList_CriteriaParmList(); return tblCriteriaList_CriteriaParmList; }
class Table_CriteriaParm* CriteriaParm_get() { if( !tblCriteriaParm ) CreateTable_CriteriaParm(); return tblCriteriaParm; }
class Table_CriteriaParmList* CriteriaParmList_get() { if( !tblCriteriaParmList ) CreateTable_CriteriaParmList(); return tblCriteriaParmList; }
class Table_CriteriaParmNesting* CriteriaParmNesting_get() { if( !tblCriteriaParmNesting ) CreateTable_CriteriaParmNesting(); return tblCriteriaParmNesting; }
class Table_DHCPDevice* DHCPDevice_get() { if( !tblDHCPDevice ) CreateTable_DHCPDevice(); return tblDHCPDevice; }
class Table_DHCPDevice_DeviceData* DHCPDevice_DeviceData_get() { if( !tblDHCPDevice_DeviceData ) CreateTable_DHCPDevice_DeviceData(); return tblDHCPDevice_DeviceData; }
class Table_DataGrid* DataGrid_get() { if( !tblDataGrid ) CreateTable_DataGrid(); return tblDataGrid; }
class Table_DesignObj* DesignObj_get() { if( !tblDesignObj ) CreateTable_DesignObj(); return tblDesignObj; }
class Table_DesignObjCategory* DesignObjCategory_get() { if( !tblDesignObjCategory ) CreateTable_DesignObjCategory(); return tblDesignObjCategory; }
class Table_DesignObjParameter* DesignObjParameter_get() { if( !tblDesignObjParameter ) CreateTable_DesignObjParameter(); return tblDesignObjParameter; }
class Table_DesignObjType* DesignObjType_get() { if( !tblDesignObjType ) CreateTable_DesignObjType(); return tblDesignObjType; }
class Table_DesignObjType_DesignObjParameter* DesignObjType_DesignObjParameter_get() { if( !tblDesignObjType_DesignObjParameter ) CreateTable_DesignObjType_DesignObjParameter(); return tblDesignObjType_DesignObjParameter; }
class Table_DesignObjVariation* DesignObjVariation_get() { if( !tblDesignObjVariation ) CreateTable_DesignObjVariation(); return tblDesignObjVariation; }
class Table_DesignObjVariation_DesignObj* DesignObjVariation_DesignObj_get() { if( !tblDesignObjVariation_DesignObj ) CreateTable_DesignObjVariation_DesignObj(); return tblDesignObjVariation_DesignObj; }
class Table_DesignObjVariation_DesignObjParameter* DesignObjVariation_DesignObjParameter_get() { if( !tblDesignObjVariation_DesignObjParameter ) CreateTable_DesignObjVariation_DesignObjParameter(); return tblDesignObjVariation_DesignObjParameter; }
class Table_DesignObjVariation_DesignObj_Skin_Language* DesignObjVariation_DesignObj_Skin_Language_get() { if( !tblDesignObjVariation_DesignObj_Skin_Language ) CreateTable_DesignObjVariation_DesignObj_Skin_Language(); return tblDesignObjVariation_DesignObj_Skin_Language; }
class Table_DesignObjVariation_Text* DesignObjVariation_Text_get() { if( !tblDesignObjVariation_Text ) CreateTable_DesignObjVariation_Text(); return tblDesignObjVariation_Text; }
class Table_DesignObjVariation_Text_Skin_Language* DesignObjVariation_Text_Skin_Language_get() { if( !tblDesignObjVariation_Text_Skin_Language ) CreateTable_DesignObjVariation_Text_Skin_Language(); return tblDesignObjVariation_Text_Skin_Language; }
class Table_DesignObjVariation_Zone* DesignObjVariation_Zone_get() { if( !tblDesignObjVariation_Zone ) CreateTable_DesignObjVariation_Zone(); return tblDesignObjVariation_Zone; }
class Table_Device* Device_get() { if( !tblDevice ) CreateTable_Device(); return tblDevice; }
class Table_DeviceCategory* DeviceCategory_get() { if( !tblDeviceCategory ) CreateTable_DeviceCategory(); return tblDeviceCategory; }
class Table_DeviceCategory_DeviceData* DeviceCategory_DeviceData_get() { if( !tblDeviceCategory_DeviceData ) CreateTable_DeviceCategory_DeviceData(); return tblDeviceCategory_DeviceData; }
class Table_DeviceCategory_Event* DeviceCategory_Event_get() { if( !tblDeviceCategory_Event ) CreateTable_DeviceCategory_Event(); return tblDeviceCategory_Event; }
class Table_DeviceCommandGroup* DeviceCommandGroup_get() { if( !tblDeviceCommandGroup ) CreateTable_DeviceCommandGroup(); return tblDeviceCommandGroup; }
class Table_DeviceCommandGroup_Command* DeviceCommandGroup_Command_get() { if( !tblDeviceCommandGroup_Command ) CreateTable_DeviceCommandGroup_Command(); return tblDeviceCommandGroup_Command; }
class Table_DeviceCommandGroup_DeviceCommandGroup_Parent* DeviceCommandGroup_DeviceCommandGroup_Parent_get() { if( !tblDeviceCommandGroup_DeviceCommandGroup_Parent ) CreateTable_DeviceCommandGroup_DeviceCommandGroup_Parent(); return tblDeviceCommandGroup_DeviceCommandGroup_Parent; }
class Table_DeviceData* DeviceData_get() { if( !tblDeviceData ) CreateTable_DeviceData(); return tblDeviceData; }
class Table_DeviceGroup* DeviceGroup_get() { if( !tblDeviceGroup ) CreateTable_DeviceGroup(); return tblDeviceGroup; }
class Table_DeviceTemplate* DeviceTemplate_get() { if( !tblDeviceTemplate ) CreateTable_DeviceTemplate(); return tblDeviceTemplate; }
class Table_DeviceTemplate_AV* DeviceTemplate_AV_get() { if( !tblDeviceTemplate_AV ) CreateTable_DeviceTemplate_AV(); return tblDeviceTemplate_AV; }
class Table_DeviceTemplate_DSPMode* DeviceTemplate_DSPMode_get() { if( !tblDeviceTemplate_DSPMode ) CreateTable_DeviceTemplate_DSPMode(); return tblDeviceTemplate_DSPMode; }
class Table_DeviceTemplate_DesignObj* DeviceTemplate_DesignObj_get() { if( !tblDeviceTemplate_DesignObj ) CreateTable_DeviceTemplate_DesignObj(); return tblDeviceTemplate_DesignObj; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia* DeviceTemplate_DeviceCategory_ControlledVia_get() { if( !tblDeviceTemplate_DeviceCategory_ControlledVia ) CreateTable_DeviceTemplate_DeviceCategory_ControlledVia(); return tblDeviceTemplate_DeviceCategory_ControlledVia; }
class Table_DeviceTemplate_DeviceCategory_ControlledVia_Pipe* DeviceTemplate_DeviceCategory_ControlledVia_Pipe_get() { if( !tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe ) CreateTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe(); return tblDeviceTemplate_DeviceCategory_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceCommandGroup* DeviceTemplate_DeviceCommandGroup_get() { if( !tblDeviceTemplate_DeviceCommandGroup ) CreateTable_DeviceTemplate_DeviceCommandGroup(); return tblDeviceTemplate_DeviceCommandGroup; }
class Table_DeviceTemplate_DeviceData* DeviceTemplate_DeviceData_get() { if( !tblDeviceTemplate_DeviceData ) CreateTable_DeviceTemplate_DeviceData(); return tblDeviceTemplate_DeviceData; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia* DeviceTemplate_DeviceTemplate_ControlledVia_get() { if( !tblDeviceTemplate_DeviceTemplate_ControlledVia ) CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia(); return tblDeviceTemplate_DeviceTemplate_ControlledVia; }
class Table_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe* DeviceTemplate_DeviceTemplate_ControlledVia_Pipe_get() { if( !tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe ) CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe(); return tblDeviceTemplate_DeviceTemplate_ControlledVia_Pipe; }
class Table_DeviceTemplate_DeviceTemplate_Related* DeviceTemplate_DeviceTemplate_Related_get() { if( !tblDeviceTemplate_DeviceTemplate_Related ) CreateTable_DeviceTemplate_DeviceTemplate_Related(); return tblDeviceTemplate_DeviceTemplate_Related; }
class Table_DeviceTemplate_Event* DeviceTemplate_Event_get() { if( !tblDeviceTemplate_Event ) CreateTable_DeviceTemplate_Event(); return tblDeviceTemplate_Event; }
class Table_DeviceTemplate_InfraredGroup* DeviceTemplate_InfraredGroup_get() { if( !tblDeviceTemplate_InfraredGroup ) CreateTable_DeviceTemplate_InfraredGroup(); return tblDeviceTemplate_InfraredGroup; }
class Table_DeviceTemplate_Input* DeviceTemplate_Input_get() { if( !tblDeviceTemplate_Input ) CreateTable_DeviceTemplate_Input(); return tblDeviceTemplate_Input; }
class Table_DeviceTemplate_MediaType* DeviceTemplate_MediaType_get() { if( !tblDeviceTemplate_MediaType ) CreateTable_DeviceTemplate_MediaType(); return tblDeviceTemplate_MediaType; }
class Table_DeviceTemplate_MediaType_DesignObj* DeviceTemplate_MediaType_DesignObj_get() { if( !tblDeviceTemplate_MediaType_DesignObj ) CreateTable_DeviceTemplate_MediaType_DesignObj(); return tblDeviceTemplate_MediaType_DesignObj; }
class Table_DeviceTemplate_Output* DeviceTemplate_Output_get() { if( !tblDeviceTemplate_Output ) CreateTable_DeviceTemplate_Output(); return tblDeviceTemplate_Output; }
class Table_DeviceTemplate_PageSetup* DeviceTemplate_PageSetup_get() { if( !tblDeviceTemplate_PageSetup ) CreateTable_DeviceTemplate_PageSetup(); return tblDeviceTemplate_PageSetup; }
class Table_Device_Command* Device_Command_get() { if( !tblDevice_Command ) CreateTable_Device_Command(); return tblDevice_Command; }
class Table_Device_CommandGroup* Device_CommandGroup_get() { if( !tblDevice_CommandGroup ) CreateTable_Device_CommandGroup(); return tblDevice_CommandGroup; }
class Table_Device_DeviceData* Device_DeviceData_get() { if( !tblDevice_DeviceData ) CreateTable_Device_DeviceData(); return tblDevice_DeviceData; }
class Table_Device_DeviceGroup* Device_DeviceGroup_get() { if( !tblDevice_DeviceGroup ) CreateTable_Device_DeviceGroup(); return tblDevice_DeviceGroup; }
class Table_Device_Device_Pipe* Device_Device_Pipe_get() { if( !tblDevice_Device_Pipe ) CreateTable_Device_Device_Pipe(); return tblDevice_Device_Pipe; }
class Table_Device_Device_Related* Device_Device_Related_get() { if( !tblDevice_Device_Related ) CreateTable_Device_Device_Related(); return tblDevice_Device_Related; }
class Table_Device_EntertainArea* Device_EntertainArea_get() { if( !tblDevice_EntertainArea ) CreateTable_Device_EntertainArea(); return tblDevice_EntertainArea; }
class Table_Device_HouseMode* Device_HouseMode_get() { if( !tblDevice_HouseMode ) CreateTable_Device_HouseMode(); return tblDevice_HouseMode; }
class Table_Device_MRU* Device_MRU_get() { if( !tblDevice_MRU ) CreateTable_Device_MRU(); return tblDevice_MRU; }
class Table_Device_Orbiter* Device_Orbiter_get() { if( !tblDevice_Orbiter ) CreateTable_Device_Orbiter(); return tblDevice_Orbiter; }
class Table_Device_QuickStart* Device_QuickStart_get() { if( !tblDevice_QuickStart ) CreateTable_Device_QuickStart(); return tblDevice_QuickStart; }
class Table_Device_StartupScript* Device_StartupScript_get() { if( !tblDevice_StartupScript ) CreateTable_Device_StartupScript(); return tblDevice_StartupScript; }
class Table_Device_Users* Device_Users_get() { if( !tblDevice_Users ) CreateTable_Device_Users(); return tblDevice_Users; }
class Table_Direction* Direction_get() { if( !tblDirection ) CreateTable_Direction(); return tblDirection; }
class Table_Directory* Directory_get() { if( !tblDirectory ) CreateTable_Directory(); return tblDirectory; }
class Table_Distro* Distro_get() { if( !tblDistro ) CreateTable_Distro(); return tblDistro; }
class Table_Document* Document_get() { if( !tblDocument ) CreateTable_Document(); return tblDocument; }
class Table_Document_Comment* Document_Comment_get() { if( !tblDocument_Comment ) CreateTable_Document_Comment(); return tblDocument_Comment; }
class Table_Effect* Effect_get() { if( !tblEffect ) CreateTable_Effect(); return tblEffect; }
class Table_EffectType* EffectType_get() { if( !tblEffectType ) CreateTable_EffectType(); return tblEffectType; }
class Table_EffectType_Effect_Skin* EffectType_Effect_Skin_get() { if( !tblEffectType_Effect_Skin ) CreateTable_EffectType_Effect_Skin(); return tblEffectType_Effect_Skin; }
class Table_EntertainArea* EntertainArea_get() { if( !tblEntertainArea ) CreateTable_EntertainArea(); return tblEntertainArea; }
class Table_Event* Event_get() { if( !tblEvent ) CreateTable_Event(); return tblEvent; }
class Table_EventCategory* EventCategory_get() { if( !tblEventCategory ) CreateTable_EventCategory(); return tblEventCategory; }
class Table_EventHandler* EventHandler_get() { if( !tblEventHandler ) CreateTable_EventHandler(); return tblEventHandler; }
class Table_EventParameter* EventParameter_get() { if( !tblEventParameter ) CreateTable_EventParameter(); return tblEventParameter; }
class Table_Event_EventParameter* Event_EventParameter_get() { if( !tblEvent_EventParameter ) CreateTable_Event_EventParameter(); return tblEvent_EventParameter; }
class Table_FAQ* FAQ_get() { if( !tblFAQ ) CreateTable_FAQ(); return tblFAQ; }
class Table_Firewall* Firewall_get() { if( !tblFirewall ) CreateTable_Firewall(); return tblFirewall; }
class Table_Floorplan* Floorplan_get() { if( !tblFloorplan ) CreateTable_Floorplan(); return tblFloorplan; }
class Table_FloorplanObjectType* FloorplanObjectType_get() { if( !tblFloorplanObjectType ) CreateTable_FloorplanObjectType(); return tblFloorplanObjectType; }
class Table_FloorplanObjectType_Color* FloorplanObjectType_Color_get() { if( !tblFloorplanObjectType_Color ) CreateTable_FloorplanObjectType_Color(); return tblFloorplanObjectType_Color; }
class Table_FloorplanType* FloorplanType_get() { if( !tblFloorplanType ) CreateTable_FloorplanType(); return tblFloorplanType; }
class Table_HorizAlignment* HorizAlignment_get() { if( !tblHorizAlignment ) CreateTable_HorizAlignment(); return tblHorizAlignment; }
class Table_HouseMode* HouseMode_get() { if( !tblHouseMode ) CreateTable_HouseMode(); return tblHouseMode; }
class Table_Household* Household_get() { if( !tblHousehold ) CreateTable_Household(); return tblHousehold; }
class Table_Household_Installation* Household_Installation_get() { if( !tblHousehold_Installation ) CreateTable_Household_Installation(); return tblHousehold_Installation; }
class Table_Icon* Icon_get() { if( !tblIcon ) CreateTable_Icon(); return tblIcon; }
class Table_Image* Image_get() { if( !tblImage ) CreateTable_Image(); return tblImage; }
class Table_InfraredGroup* InfraredGroup_get() { if( !tblInfraredGroup ) CreateTable_InfraredGroup(); return tblInfraredGroup; }
class Table_InfraredGroup_Command* InfraredGroup_Command_get() { if( !tblInfraredGroup_Command ) CreateTable_InfraredGroup_Command(); return tblInfraredGroup_Command; }
class Table_InfraredGroup_Command_Preferred* InfraredGroup_Command_Preferred_get() { if( !tblInfraredGroup_Command_Preferred ) CreateTable_InfraredGroup_Command_Preferred(); return tblInfraredGroup_Command_Preferred; }
class Table_InstallWizard* InstallWizard_get() { if( !tblInstallWizard ) CreateTable_InstallWizard(); return tblInstallWizard; }
class Table_InstallWizard_Distro* InstallWizard_Distro_get() { if( !tblInstallWizard_Distro ) CreateTable_InstallWizard_Distro(); return tblInstallWizard_Distro; }
class Table_Installation* Installation_get() { if( !tblInstallation ) CreateTable_Installation(); return tblInstallation; }
class Table_Installation_RepositorySource_URL* Installation_RepositorySource_URL_get() { if( !tblInstallation_RepositorySource_URL ) CreateTable_Installation_RepositorySource_URL(); return tblInstallation_RepositorySource_URL; }
class Table_Installation_Users* Installation_Users_get() { if( !tblInstallation_Users ) CreateTable_Installation_Users(); return tblInstallation_Users; }
class Table_Language* Language_get() { if( !tblLanguage ) CreateTable_Language(); return tblLanguage; }
class Table_License* License_get() { if( !tblLicense ) CreateTable_License(); return tblLicense; }
class Table_Licensing* Licensing_get() { if( !tblLicensing ) CreateTable_Licensing(); return tblLicensing; }
class Table_Manufacturer* Manufacturer_get() { if( !tblManufacturer ) CreateTable_Manufacturer(); return tblManufacturer; }
class Table_MediaType* MediaType_get() { if( !tblMediaType ) CreateTable_MediaType(); return tblMediaType; }
class Table_MediaType_Broadcast* MediaType_Broadcast_get() { if( !tblMediaType_Broadcast ) CreateTable_MediaType_Broadcast(); return tblMediaType_Broadcast; }
class Table_MediaType_DesignObj* MediaType_DesignObj_get() { if( !tblMediaType_DesignObj ) CreateTable_MediaType_DesignObj(); return tblMediaType_DesignObj; }
class Table_News* News_get() { if( !tblNews ) CreateTable_News(); return tblNews; }
class Table_OperatingSystem* OperatingSystem_get() { if( !tblOperatingSystem ) CreateTable_OperatingSystem(); return tblOperatingSystem; }
class Table_Orbiter* Orbiter_get() { if( !tblOrbiter ) CreateTable_Orbiter(); return tblOrbiter; }
class Table_Orbiter_Users_PasswordReq* Orbiter_Users_PasswordReq_get() { if( !tblOrbiter_Users_PasswordReq ) CreateTable_Orbiter_Users_PasswordReq(); return tblOrbiter_Users_PasswordReq; }
class Table_Orbiter_Variable* Orbiter_Variable_get() { if( !tblOrbiter_Variable ) CreateTable_Orbiter_Variable(); return tblOrbiter_Variable; }
class Table_Package* Package_get() { if( !tblPackage ) CreateTable_Package(); return tblPackage; }
class Table_PackageType* PackageType_get() { if( !tblPackageType ) CreateTable_PackageType(); return tblPackageType; }
class Table_Package_Compat* Package_Compat_get() { if( !tblPackage_Compat ) CreateTable_Package_Compat(); return tblPackage_Compat; }
class Table_Package_Device* Package_Device_get() { if( !tblPackage_Device ) CreateTable_Package_Device(); return tblPackage_Device; }
class Table_Package_Directory* Package_Directory_get() { if( !tblPackage_Directory ) CreateTable_Package_Directory(); return tblPackage_Directory; }
class Table_Package_Directory_File* Package_Directory_File_get() { if( !tblPackage_Directory_File ) CreateTable_Package_Directory_File(); return tblPackage_Directory_File; }
class Table_Package_Package* Package_Package_get() { if( !tblPackage_Package ) CreateTable_Package_Package(); return tblPackage_Package; }
class Table_Package_Source* Package_Source_get() { if( !tblPackage_Source ) CreateTable_Package_Source(); return tblPackage_Source; }
class Table_Package_Source_Compat* Package_Source_Compat_get() { if( !tblPackage_Source_Compat ) CreateTable_Package_Source_Compat(); return tblPackage_Source_Compat; }
class Table_Package_Users* Package_Users_get() { if( !tblPackage_Users ) CreateTable_Package_Users(); return tblPackage_Users; }
class Table_Package_Version* Package_Version_get() { if( !tblPackage_Version ) CreateTable_Package_Version(); return tblPackage_Version; }
class Table_PageSetup* PageSetup_get() { if( !tblPageSetup ) CreateTable_PageSetup(); return tblPageSetup; }
class Table_PaidLicense* PaidLicense_get() { if( !tblPaidLicense ) CreateTable_PaidLicense(); return tblPaidLicense; }
class Table_ParameterType* ParameterType_get() { if( !tblParameterType ) CreateTable_ParameterType(); return tblParameterType; }
class Table_PhoneLineType* PhoneLineType_get() { if( !tblPhoneLineType ) CreateTable_PhoneLineType(); return tblPhoneLineType; }
class Table_Pipe* Pipe_get() { if( !tblPipe ) CreateTable_Pipe(); return tblPipe; }
class Table_PnpLevel* PnpLevel_get() { if( !tblPnpLevel ) CreateTable_PnpLevel(); return tblPnpLevel; }
class Table_PnpProtocol* PnpProtocol_get() { if( !tblPnpProtocol ) CreateTable_PnpProtocol(); return tblPnpProtocol; }
class Table_PnpQueue* PnpQueue_get() { if( !tblPnpQueue ) CreateTable_PnpQueue(); return tblPnpQueue; }
class Table_PostalCode* PostalCode_get() { if( !tblPostalCode ) CreateTable_PostalCode(); return tblPostalCode; }
class Table_QuickStartCategory* QuickStartCategory_get() { if( !tblQuickStartCategory ) CreateTable_QuickStartCategory(); return tblQuickStartCategory; }
class Table_QuickStartTemplate* QuickStartTemplate_get() { if( !tblQuickStartTemplate ) CreateTable_QuickStartTemplate(); return tblQuickStartTemplate; }
class Table_Region* Region_get() { if( !tblRegion ) CreateTable_Region(); return tblRegion; }
class Table_RemoteControl* RemoteControl_get() { if( !tblRemoteControl ) CreateTable_RemoteControl(); return tblRemoteControl; }
class Table_RemoteMapping* RemoteMapping_get() { if( !tblRemoteMapping ) CreateTable_RemoteMapping(); return tblRemoteMapping; }
class Table_RepositorySource* RepositorySource_get() { if( !tblRepositorySource ) CreateTable_RepositorySource(); return tblRepositorySource; }
class Table_RepositorySource_URL* RepositorySource_URL_get() { if( !tblRepositorySource_URL ) CreateTable_RepositorySource_URL(); return tblRepositorySource_URL; }
class Table_RepositoryType* RepositoryType_get() { if( !tblRepositoryType ) CreateTable_RepositoryType(); return tblRepositoryType; }
class Table_Room* Room_get() { if( !tblRoom ) CreateTable_Room(); return tblRoom; }
class Table_RoomType* RoomType_get() { if( !tblRoomType ) CreateTable_RoomType(); return tblRoomType; }
class Table_Room_Users* Room_Users_get() { if( !tblRoom_Users ) CreateTable_Room_Users(); return tblRoom_Users; }
class Table_Schema* Schema_get() { if( !tblSchema ) CreateTable_Schema(); return tblSchema; }
class Table_Screen* Screen_get() { if( !tblScreen ) CreateTable_Screen(); return tblScreen; }
class Table_Screen_CommandParameter* Screen_CommandParameter_get() { if( !tblScreen_CommandParameter ) CreateTable_Screen_CommandParameter(); return tblScreen_CommandParameter; }
class Table_Screen_DesignObj* Screen_DesignObj_get() { if( !tblScreen_DesignObj ) CreateTable_Screen_DesignObj(); return tblScreen_DesignObj; }
class Table_SetupStep* SetupStep_get() { if( !tblSetupStep ) CreateTable_SetupStep(); return tblSetupStep; }
class Table_Size* Size_get() { if( !tblSize ) CreateTable_Size(); return tblSize; }
class Table_Skin* Skin_get() { if( !tblSkin ) CreateTable_Skin(); return tblSkin; }
class Table_Software* Software_get() { if( !tblSoftware ) CreateTable_Software(); return tblSoftware; }
class Table_Software_Device* Software_Device_get() { if( !tblSoftware_Device ) CreateTable_Software_Device(); return tblSoftware_Device; }
class Table_Software_Source* Software_Source_get() { if( !tblSoftware_Source ) CreateTable_Software_Source(); return tblSoftware_Source; }
class Table_StabilityStatus* StabilityStatus_get() { if( !tblStabilityStatus ) CreateTable_StabilityStatus(); return tblStabilityStatus; }
class Table_StartupScript* StartupScript_get() { if( !tblStartupScript ) CreateTable_StartupScript(); return tblStartupScript; }
class Table_Style* Style_get() { if( !tblStyle ) CreateTable_Style(); return tblStyle; }
class Table_StyleVariation* StyleVariation_get() { if( !tblStyleVariation ) CreateTable_StyleVariation(); return tblStyleVariation; }
class Table_System* System_get() { if( !tblSystem ) CreateTable_System(); return tblSystem; }
class Table_Template* Template_get() { if( !tblTemplate ) CreateTable_Template(); return tblTemplate; }
class Table_Text* Text_get() { if( !tblText ) CreateTable_Text(); return tblText; }
class Table_TextCategory* TextCategory_get() { if( !tblTextCategory ) CreateTable_TextCategory(); return tblTextCategory; }
class Table_Text_LS* Text_LS_get() { if( !tblText_LS ) CreateTable_Text_LS(); return tblText_LS; }
class Table_Text_LS_AltVersions* Text_LS_AltVersions_get() { if( !tblText_LS_AltVersions ) CreateTable_Text_LS_AltVersions(); return tblText_LS_AltVersions; }
class Table_TimeZone* TimeZone_get() { if( !tblTimeZone ) CreateTable_TimeZone(); return tblTimeZone; }
class Table_UI* UI_get() { if( !tblUI ) CreateTable_UI(); return tblUI; }
class Table_UnknownDevices* UnknownDevices_get() { if( !tblUnknownDevices ) CreateTable_UnknownDevices(); return tblUnknownDevices; }
class Table_UserMode* UserMode_get() { if( !tblUserMode ) CreateTable_UserMode(); return tblUserMode; }
class Table_Users* Users_get() { if( !tblUsers ) CreateTable_Users(); return tblUsers; }
class Table_Variable* Variable_get() { if( !tblVariable ) CreateTable_Variable(); return tblVariable; }
class Table_Version* Version_get() { if( !tblVersion ) CreateTable_Version(); return tblVersion; }
class Table_VertAlignment* VertAlignment_get() { if( !tblVertAlignment ) CreateTable_VertAlignment(); return tblVertAlignment; }
class Table_psc_constants_batdet* psc_constants_batdet_get() { if( !tblpsc_constants_batdet ) CreateTable_psc_constants_batdet(); return tblpsc_constants_batdet; }
class Table_psc_constants_bathdr* psc_constants_bathdr_get() { if( !tblpsc_constants_bathdr ) CreateTable_psc_constants_bathdr(); return tblpsc_constants_bathdr; }
class Table_psc_constants_batuser* psc_constants_batuser_get() { if( !tblpsc_constants_batuser ) CreateTable_psc_constants_batuser(); return tblpsc_constants_batuser; }
class Table_psc_constants_repset* psc_constants_repset_get() { if( !tblpsc_constants_repset ) CreateTable_psc_constants_repset(); return tblpsc_constants_repset; }
class Table_psc_constants_schema* psc_constants_schema_get() { if( !tblpsc_constants_schema ) CreateTable_psc_constants_schema(); return tblpsc_constants_schema; }
class Table_psc_constants_tables* psc_constants_tables_get() { if( !tblpsc_constants_tables ) CreateTable_psc_constants_tables(); return tblpsc_constants_tables; }
class Table_psc_dce_batdet* psc_dce_batdet_get() { if( !tblpsc_dce_batdet ) CreateTable_psc_dce_batdet(); return tblpsc_dce_batdet; }
class Table_psc_dce_bathdr* psc_dce_bathdr_get() { if( !tblpsc_dce_bathdr ) CreateTable_psc_dce_bathdr(); return tblpsc_dce_bathdr; }
class Table_psc_dce_batuser* psc_dce_batuser_get() { if( !tblpsc_dce_batuser ) CreateTable_psc_dce_batuser(); return tblpsc_dce_batuser; }
class Table_psc_dce_repset* psc_dce_repset_get() { if( !tblpsc_dce_repset ) CreateTable_psc_dce_repset(); return tblpsc_dce_repset; }
class Table_psc_dce_schema* psc_dce_schema_get() { if( !tblpsc_dce_schema ) CreateTable_psc_dce_schema(); return tblpsc_dce_schema; }
class Table_psc_dce_tables* psc_dce_tables_get() { if( !tblpsc_dce_tables ) CreateTable_psc_dce_tables(); return tblpsc_dce_tables; }
class Table_psc_designer_batdet* psc_designer_batdet_get() { if( !tblpsc_designer_batdet ) CreateTable_psc_designer_batdet(); return tblpsc_designer_batdet; }
class Table_psc_designer_bathdr* psc_designer_bathdr_get() { if( !tblpsc_designer_bathdr ) CreateTable_psc_designer_bathdr(); return tblpsc_designer_bathdr; }
class Table_psc_designer_batuser* psc_designer_batuser_get() { if( !tblpsc_designer_batuser ) CreateTable_psc_designer_batuser(); return tblpsc_designer_batuser; }
class Table_psc_designer_repset* psc_designer_repset_get() { if( !tblpsc_designer_repset ) CreateTable_psc_designer_repset(); return tblpsc_designer_repset; }
class Table_psc_designer_schema* psc_designer_schema_get() { if( !tblpsc_designer_schema ) CreateTable_psc_designer_schema(); return tblpsc_designer_schema; }
class Table_psc_designer_tables* psc_designer_tables_get() { if( !tblpsc_designer_tables ) CreateTable_psc_designer_tables(); return tblpsc_designer_tables; }
class Table_psc_document_batdet* psc_document_batdet_get() { if( !tblpsc_document_batdet ) CreateTable_psc_document_batdet(); return tblpsc_document_batdet; }
class Table_psc_document_bathdr* psc_document_bathdr_get() { if( !tblpsc_document_bathdr ) CreateTable_psc_document_bathdr(); return tblpsc_document_bathdr; }
class Table_psc_document_batuser* psc_document_batuser_get() { if( !tblpsc_document_batuser ) CreateTable_psc_document_batuser(); return tblpsc_document_batuser; }
class Table_psc_document_repset* psc_document_repset_get() { if( !tblpsc_document_repset ) CreateTable_psc_document_repset(); return tblpsc_document_repset; }
class Table_psc_document_schema* psc_document_schema_get() { if( !tblpsc_document_schema ) CreateTable_psc_document_schema(); return tblpsc_document_schema; }
class Table_psc_document_tables* psc_document_tables_get() { if( !tblpsc_document_tables ) CreateTable_psc_document_tables(); return tblpsc_document_tables; }
class Table_psc_ir_batdet* psc_ir_batdet_get() { if( !tblpsc_ir_batdet ) CreateTable_psc_ir_batdet(); return tblpsc_ir_batdet; }
class Table_psc_ir_bathdr* psc_ir_bathdr_get() { if( !tblpsc_ir_bathdr ) CreateTable_psc_ir_bathdr(); return tblpsc_ir_bathdr; }
class Table_psc_ir_batuser* psc_ir_batuser_get() { if( !tblpsc_ir_batuser ) CreateTable_psc_ir_batuser(); return tblpsc_ir_batuser; }
class Table_psc_ir_repset* psc_ir_repset_get() { if( !tblpsc_ir_repset ) CreateTable_psc_ir_repset(); return tblpsc_ir_repset; }
class Table_psc_ir_schema* psc_ir_schema_get() { if( !tblpsc_ir_schema ) CreateTable_psc_ir_schema(); return tblpsc_ir_schema; }
class Table_psc_ir_tables* psc_ir_tables_get() { if( !tblpsc_ir_tables ) CreateTable_psc_ir_tables(); return tblpsc_ir_tables; }
class Table_psc_local_batdet* psc_local_batdet_get() { if( !tblpsc_local_batdet ) CreateTable_psc_local_batdet(); return tblpsc_local_batdet; }
class Table_psc_local_bathdr* psc_local_bathdr_get() { if( !tblpsc_local_bathdr ) CreateTable_psc_local_bathdr(); return tblpsc_local_bathdr; }
class Table_psc_local_batuser* psc_local_batuser_get() { if( !tblpsc_local_batuser ) CreateTable_psc_local_batuser(); return tblpsc_local_batuser; }
class Table_psc_local_repset* psc_local_repset_get() { if( !tblpsc_local_repset ) CreateTable_psc_local_repset(); return tblpsc_local_repset; }
class Table_psc_local_schema* psc_local_schema_get() { if( !tblpsc_local_schema ) CreateTable_psc_local_schema(); return tblpsc_local_schema; }
class Table_psc_local_tables* psc_local_tables_get() { if( !tblpsc_local_tables ) CreateTable_psc_local_tables(); return tblpsc_local_tables; }
class Table_psc_website_batdet* psc_website_batdet_get() { if( !tblpsc_website_batdet ) CreateTable_psc_website_batdet(); return tblpsc_website_batdet; }
class Table_psc_website_bathdr* psc_website_bathdr_get() { if( !tblpsc_website_bathdr ) CreateTable_psc_website_bathdr(); return tblpsc_website_bathdr; }
class Table_psc_website_batuser* psc_website_batuser_get() { if( !tblpsc_website_batuser ) CreateTable_psc_website_batuser(); return tblpsc_website_batuser; }
class Table_psc_website_repset* psc_website_repset_get() { if( !tblpsc_website_repset ) CreateTable_psc_website_repset(); return tblpsc_website_repset; }
class Table_psc_website_schema* psc_website_schema_get() { if( !tblpsc_website_schema ) CreateTable_psc_website_schema(); return tblpsc_website_schema; }
class Table_psc_website_tables* psc_website_tables_get() { if( !tblpsc_website_tables ) CreateTable_psc_website_tables(); return tblpsc_website_tables; }
string m_sLastDBError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Array();
void CreateTable_Broadcast();
void CreateTable_Button();
void CreateTable_CachedScreens();
void CreateTable_CannedEvents();
void CreateTable_CannedEvents_CriteriaParmList();
void CreateTable_City();
void CreateTable_CommMethod();
void CreateTable_Command();
void CreateTable_CommandCategory();
void CreateTable_CommandGroup();
void CreateTable_CommandGroup_Command();
void CreateTable_CommandGroup_Command_CommandParameter();
void CreateTable_CommandGroup_D();
void CreateTable_CommandGroup_D_Command();
void CreateTable_CommandGroup_D_Command_CommandParameter();
void CreateTable_CommandGroup_EntertainArea();
void CreateTable_CommandGroup_Room();
void CreateTable_CommandParameter();
void CreateTable_Command_CommandParameter();
void CreateTable_Command_Pipe();
void CreateTable_ConfigType();
void CreateTable_ConfigType_File();
void CreateTable_ConfigType_Setting();
void CreateTable_ConfigType_Token();
void CreateTable_ConnectorType();
void CreateTable_Country();
void CreateTable_Criteria();
void CreateTable_CriteriaList();
void CreateTable_CriteriaList_CriteriaParmList();
void CreateTable_CriteriaParm();
void CreateTable_CriteriaParmList();
void CreateTable_CriteriaParmNesting();
void CreateTable_DHCPDevice();
void CreateTable_DHCPDevice_DeviceData();
void CreateTable_DataGrid();
void CreateTable_DesignObj();
void CreateTable_DesignObjCategory();
void CreateTable_DesignObjParameter();
void CreateTable_DesignObjType();
void CreateTable_DesignObjType_DesignObjParameter();
void CreateTable_DesignObjVariation();
void CreateTable_DesignObjVariation_DesignObj();
void CreateTable_DesignObjVariation_DesignObjParameter();
void CreateTable_DesignObjVariation_DesignObj_Skin_Language();
void CreateTable_DesignObjVariation_Text();
void CreateTable_DesignObjVariation_Text_Skin_Language();
void CreateTable_DesignObjVariation_Zone();
void CreateTable_Device();
void CreateTable_DeviceCategory();
void CreateTable_DeviceCategory_DeviceData();
void CreateTable_DeviceCategory_Event();
void CreateTable_DeviceCommandGroup();
void CreateTable_DeviceCommandGroup_Command();
void CreateTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
void CreateTable_DeviceData();
void CreateTable_DeviceGroup();
void CreateTable_DeviceTemplate();
void CreateTable_DeviceTemplate_AV();
void CreateTable_DeviceTemplate_DSPMode();
void CreateTable_DeviceTemplate_DesignObj();
void CreateTable_DeviceTemplate_DeviceCategory_ControlledVia();
void CreateTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
void CreateTable_DeviceTemplate_DeviceCommandGroup();
void CreateTable_DeviceTemplate_DeviceData();
void CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia();
void CreateTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
void CreateTable_DeviceTemplate_DeviceTemplate_Related();
void CreateTable_DeviceTemplate_Event();
void CreateTable_DeviceTemplate_InfraredGroup();
void CreateTable_DeviceTemplate_Input();
void CreateTable_DeviceTemplate_MediaType();
void CreateTable_DeviceTemplate_MediaType_DesignObj();
void CreateTable_DeviceTemplate_Output();
void CreateTable_DeviceTemplate_PageSetup();
void CreateTable_Device_Command();
void CreateTable_Device_CommandGroup();
void CreateTable_Device_DeviceData();
void CreateTable_Device_DeviceGroup();
void CreateTable_Device_Device_Pipe();
void CreateTable_Device_Device_Related();
void CreateTable_Device_EntertainArea();
void CreateTable_Device_HouseMode();
void CreateTable_Device_MRU();
void CreateTable_Device_Orbiter();
void CreateTable_Device_QuickStart();
void CreateTable_Device_StartupScript();
void CreateTable_Device_Users();
void CreateTable_Direction();
void CreateTable_Directory();
void CreateTable_Distro();
void CreateTable_Document();
void CreateTable_Document_Comment();
void CreateTable_Effect();
void CreateTable_EffectType();
void CreateTable_EffectType_Effect_Skin();
void CreateTable_EntertainArea();
void CreateTable_Event();
void CreateTable_EventCategory();
void CreateTable_EventHandler();
void CreateTable_EventParameter();
void CreateTable_Event_EventParameter();
void CreateTable_FAQ();
void CreateTable_Firewall();
void CreateTable_Floorplan();
void CreateTable_FloorplanObjectType();
void CreateTable_FloorplanObjectType_Color();
void CreateTable_FloorplanType();
void CreateTable_HorizAlignment();
void CreateTable_HouseMode();
void CreateTable_Household();
void CreateTable_Household_Installation();
void CreateTable_Icon();
void CreateTable_Image();
void CreateTable_InfraredGroup();
void CreateTable_InfraredGroup_Command();
void CreateTable_InfraredGroup_Command_Preferred();
void CreateTable_InstallWizard();
void CreateTable_InstallWizard_Distro();
void CreateTable_Installation();
void CreateTable_Installation_RepositorySource_URL();
void CreateTable_Installation_Users();
void CreateTable_Language();
void CreateTable_License();
void CreateTable_Licensing();
void CreateTable_Manufacturer();
void CreateTable_MediaType();
void CreateTable_MediaType_Broadcast();
void CreateTable_MediaType_DesignObj();
void CreateTable_News();
void CreateTable_OperatingSystem();
void CreateTable_Orbiter();
void CreateTable_Orbiter_Users_PasswordReq();
void CreateTable_Orbiter_Variable();
void CreateTable_Package();
void CreateTable_PackageType();
void CreateTable_Package_Compat();
void CreateTable_Package_Device();
void CreateTable_Package_Directory();
void CreateTable_Package_Directory_File();
void CreateTable_Package_Package();
void CreateTable_Package_Source();
void CreateTable_Package_Source_Compat();
void CreateTable_Package_Users();
void CreateTable_Package_Version();
void CreateTable_PageSetup();
void CreateTable_PaidLicense();
void CreateTable_ParameterType();
void CreateTable_PhoneLineType();
void CreateTable_Pipe();
void CreateTable_PnpLevel();
void CreateTable_PnpProtocol();
void CreateTable_PnpQueue();
void CreateTable_PostalCode();
void CreateTable_QuickStartCategory();
void CreateTable_QuickStartTemplate();
void CreateTable_Region();
void CreateTable_RemoteControl();
void CreateTable_RemoteMapping();
void CreateTable_RepositorySource();
void CreateTable_RepositorySource_URL();
void CreateTable_RepositoryType();
void CreateTable_Room();
void CreateTable_RoomType();
void CreateTable_Room_Users();
void CreateTable_Schema();
void CreateTable_Screen();
void CreateTable_Screen_CommandParameter();
void CreateTable_Screen_DesignObj();
void CreateTable_SetupStep();
void CreateTable_Size();
void CreateTable_Skin();
void CreateTable_Software();
void CreateTable_Software_Device();
void CreateTable_Software_Source();
void CreateTable_StabilityStatus();
void CreateTable_StartupScript();
void CreateTable_Style();
void CreateTable_StyleVariation();
void CreateTable_System();
void CreateTable_Template();
void CreateTable_Text();
void CreateTable_TextCategory();
void CreateTable_Text_LS();
void CreateTable_Text_LS_AltVersions();
void CreateTable_TimeZone();
void CreateTable_UI();
void CreateTable_UnknownDevices();
void CreateTable_UserMode();
void CreateTable_Users();
void CreateTable_Variable();
void CreateTable_Version();
void CreateTable_VertAlignment();
void CreateTable_psc_constants_batdet();
void CreateTable_psc_constants_bathdr();
void CreateTable_psc_constants_batuser();
void CreateTable_psc_constants_repset();
void CreateTable_psc_constants_schema();
void CreateTable_psc_constants_tables();
void CreateTable_psc_dce_batdet();
void CreateTable_psc_dce_bathdr();
void CreateTable_psc_dce_batuser();
void CreateTable_psc_dce_repset();
void CreateTable_psc_dce_schema();
void CreateTable_psc_dce_tables();
void CreateTable_psc_designer_batdet();
void CreateTable_psc_designer_bathdr();
void CreateTable_psc_designer_batuser();
void CreateTable_psc_designer_repset();
void CreateTable_psc_designer_schema();
void CreateTable_psc_designer_tables();
void CreateTable_psc_document_batdet();
void CreateTable_psc_document_bathdr();
void CreateTable_psc_document_batuser();
void CreateTable_psc_document_repset();
void CreateTable_psc_document_schema();
void CreateTable_psc_document_tables();
void CreateTable_psc_ir_batdet();
void CreateTable_psc_ir_bathdr();
void CreateTable_psc_ir_batuser();
void CreateTable_psc_ir_repset();
void CreateTable_psc_ir_schema();
void CreateTable_psc_ir_tables();
void CreateTable_psc_local_batdet();
void CreateTable_psc_local_bathdr();
void CreateTable_psc_local_batuser();
void CreateTable_psc_local_repset();
void CreateTable_psc_local_schema();
void CreateTable_psc_local_tables();
void CreateTable_psc_website_batdet();
void CreateTable_psc_website_bathdr();
void CreateTable_psc_website_batuser();
void CreateTable_psc_website_repset();
void CreateTable_psc_website_schema();
void CreateTable_psc_website_tables();
void DeleteTable_Array();
void DeleteTable_Broadcast();
void DeleteTable_Button();
void DeleteTable_CachedScreens();
void DeleteTable_CannedEvents();
void DeleteTable_CannedEvents_CriteriaParmList();
void DeleteTable_City();
void DeleteTable_CommMethod();
void DeleteTable_Command();
void DeleteTable_CommandCategory();
void DeleteTable_CommandGroup();
void DeleteTable_CommandGroup_Command();
void DeleteTable_CommandGroup_Command_CommandParameter();
void DeleteTable_CommandGroup_D();
void DeleteTable_CommandGroup_D_Command();
void DeleteTable_CommandGroup_D_Command_CommandParameter();
void DeleteTable_CommandGroup_EntertainArea();
void DeleteTable_CommandGroup_Room();
void DeleteTable_CommandParameter();
void DeleteTable_Command_CommandParameter();
void DeleteTable_Command_Pipe();
void DeleteTable_ConfigType();
void DeleteTable_ConfigType_File();
void DeleteTable_ConfigType_Setting();
void DeleteTable_ConfigType_Token();
void DeleteTable_ConnectorType();
void DeleteTable_Country();
void DeleteTable_Criteria();
void DeleteTable_CriteriaList();
void DeleteTable_CriteriaList_CriteriaParmList();
void DeleteTable_CriteriaParm();
void DeleteTable_CriteriaParmList();
void DeleteTable_CriteriaParmNesting();
void DeleteTable_DHCPDevice();
void DeleteTable_DHCPDevice_DeviceData();
void DeleteTable_DataGrid();
void DeleteTable_DesignObj();
void DeleteTable_DesignObjCategory();
void DeleteTable_DesignObjParameter();
void DeleteTable_DesignObjType();
void DeleteTable_DesignObjType_DesignObjParameter();
void DeleteTable_DesignObjVariation();
void DeleteTable_DesignObjVariation_DesignObj();
void DeleteTable_DesignObjVariation_DesignObjParameter();
void DeleteTable_DesignObjVariation_DesignObj_Skin_Language();
void DeleteTable_DesignObjVariation_Text();
void DeleteTable_DesignObjVariation_Text_Skin_Language();
void DeleteTable_DesignObjVariation_Zone();
void DeleteTable_Device();
void DeleteTable_DeviceCategory();
void DeleteTable_DeviceCategory_DeviceData();
void DeleteTable_DeviceCategory_Event();
void DeleteTable_DeviceCommandGroup();
void DeleteTable_DeviceCommandGroup_Command();
void DeleteTable_DeviceCommandGroup_DeviceCommandGroup_Parent();
void DeleteTable_DeviceData();
void DeleteTable_DeviceGroup();
void DeleteTable_DeviceTemplate();
void DeleteTable_DeviceTemplate_AV();
void DeleteTable_DeviceTemplate_DSPMode();
void DeleteTable_DeviceTemplate_DesignObj();
void DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia();
void DeleteTable_DeviceTemplate_DeviceCategory_ControlledVia_Pipe();
void DeleteTable_DeviceTemplate_DeviceCommandGroup();
void DeleteTable_DeviceTemplate_DeviceData();
void DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia();
void DeleteTable_DeviceTemplate_DeviceTemplate_ControlledVia_Pipe();
void DeleteTable_DeviceTemplate_DeviceTemplate_Related();
void DeleteTable_DeviceTemplate_Event();
void DeleteTable_DeviceTemplate_InfraredGroup();
void DeleteTable_DeviceTemplate_Input();
void DeleteTable_DeviceTemplate_MediaType();
void DeleteTable_DeviceTemplate_MediaType_DesignObj();
void DeleteTable_DeviceTemplate_Output();
void DeleteTable_DeviceTemplate_PageSetup();
void DeleteTable_Device_Command();
void DeleteTable_Device_CommandGroup();
void DeleteTable_Device_DeviceData();
void DeleteTable_Device_DeviceGroup();
void DeleteTable_Device_Device_Pipe();
void DeleteTable_Device_Device_Related();
void DeleteTable_Device_EntertainArea();
void DeleteTable_Device_HouseMode();
void DeleteTable_Device_MRU();
void DeleteTable_Device_Orbiter();
void DeleteTable_Device_QuickStart();
void DeleteTable_Device_StartupScript();
void DeleteTable_Device_Users();
void DeleteTable_Direction();
void DeleteTable_Directory();
void DeleteTable_Distro();
void DeleteTable_Document();
void DeleteTable_Document_Comment();
void DeleteTable_Effect();
void DeleteTable_EffectType();
void DeleteTable_EffectType_Effect_Skin();
void DeleteTable_EntertainArea();
void DeleteTable_Event();
void DeleteTable_EventCategory();
void DeleteTable_EventHandler();
void DeleteTable_EventParameter();
void DeleteTable_Event_EventParameter();
void DeleteTable_FAQ();
void DeleteTable_Firewall();
void DeleteTable_Floorplan();
void DeleteTable_FloorplanObjectType();
void DeleteTable_FloorplanObjectType_Color();
void DeleteTable_FloorplanType();
void DeleteTable_HorizAlignment();
void DeleteTable_HouseMode();
void DeleteTable_Household();
void DeleteTable_Household_Installation();
void DeleteTable_Icon();
void DeleteTable_Image();
void DeleteTable_InfraredGroup();
void DeleteTable_InfraredGroup_Command();
void DeleteTable_InfraredGroup_Command_Preferred();
void DeleteTable_InstallWizard();
void DeleteTable_InstallWizard_Distro();
void DeleteTable_Installation();
void DeleteTable_Installation_RepositorySource_URL();
void DeleteTable_Installation_Users();
void DeleteTable_Language();
void DeleteTable_License();
void DeleteTable_Licensing();
void DeleteTable_Manufacturer();
void DeleteTable_MediaType();
void DeleteTable_MediaType_Broadcast();
void DeleteTable_MediaType_DesignObj();
void DeleteTable_News();
void DeleteTable_OperatingSystem();
void DeleteTable_Orbiter();
void DeleteTable_Orbiter_Users_PasswordReq();
void DeleteTable_Orbiter_Variable();
void DeleteTable_Package();
void DeleteTable_PackageType();
void DeleteTable_Package_Compat();
void DeleteTable_Package_Device();
void DeleteTable_Package_Directory();
void DeleteTable_Package_Directory_File();
void DeleteTable_Package_Package();
void DeleteTable_Package_Source();
void DeleteTable_Package_Source_Compat();
void DeleteTable_Package_Users();
void DeleteTable_Package_Version();
void DeleteTable_PageSetup();
void DeleteTable_PaidLicense();
void DeleteTable_ParameterType();
void DeleteTable_PhoneLineType();
void DeleteTable_Pipe();
void DeleteTable_PnpLevel();
void DeleteTable_PnpProtocol();
void DeleteTable_PnpQueue();
void DeleteTable_PostalCode();
void DeleteTable_QuickStartCategory();
void DeleteTable_QuickStartTemplate();
void DeleteTable_Region();
void DeleteTable_RemoteControl();
void DeleteTable_RemoteMapping();
void DeleteTable_RepositorySource();
void DeleteTable_RepositorySource_URL();
void DeleteTable_RepositoryType();
void DeleteTable_Room();
void DeleteTable_RoomType();
void DeleteTable_Room_Users();
void DeleteTable_Schema();
void DeleteTable_Screen();
void DeleteTable_Screen_CommandParameter();
void DeleteTable_Screen_DesignObj();
void DeleteTable_SetupStep();
void DeleteTable_Size();
void DeleteTable_Skin();
void DeleteTable_Software();
void DeleteTable_Software_Device();
void DeleteTable_Software_Source();
void DeleteTable_StabilityStatus();
void DeleteTable_StartupScript();
void DeleteTable_Style();
void DeleteTable_StyleVariation();
void DeleteTable_System();
void DeleteTable_Template();
void DeleteTable_Text();
void DeleteTable_TextCategory();
void DeleteTable_Text_LS();
void DeleteTable_Text_LS_AltVersions();
void DeleteTable_TimeZone();
void DeleteTable_UI();
void DeleteTable_UnknownDevices();
void DeleteTable_UserMode();
void DeleteTable_Users();
void DeleteTable_Variable();
void DeleteTable_Version();
void DeleteTable_VertAlignment();
void DeleteTable_psc_constants_batdet();
void DeleteTable_psc_constants_bathdr();
void DeleteTable_psc_constants_batuser();
void DeleteTable_psc_constants_repset();
void DeleteTable_psc_constants_schema();
void DeleteTable_psc_constants_tables();
void DeleteTable_psc_dce_batdet();
void DeleteTable_psc_dce_bathdr();
void DeleteTable_psc_dce_batuser();
void DeleteTable_psc_dce_repset();
void DeleteTable_psc_dce_schema();
void DeleteTable_psc_dce_tables();
void DeleteTable_psc_designer_batdet();
void DeleteTable_psc_designer_bathdr();
void DeleteTable_psc_designer_batuser();
void DeleteTable_psc_designer_repset();
void DeleteTable_psc_designer_schema();
void DeleteTable_psc_designer_tables();
void DeleteTable_psc_document_batdet();
void DeleteTable_psc_document_bathdr();
void DeleteTable_psc_document_batuser();
void DeleteTable_psc_document_repset();
void DeleteTable_psc_document_schema();
void DeleteTable_psc_document_tables();
void DeleteTable_psc_ir_batdet();
void DeleteTable_psc_ir_bathdr();
void DeleteTable_psc_ir_batuser();
void DeleteTable_psc_ir_repset();
void DeleteTable_psc_ir_schema();
void DeleteTable_psc_ir_tables();
void DeleteTable_psc_local_batdet();
void DeleteTable_psc_local_bathdr();
void DeleteTable_psc_local_batuser();
void DeleteTable_psc_local_repset();
void DeleteTable_psc_local_schema();
void DeleteTable_psc_local_tables();
void DeleteTable_psc_website_batdet();
void DeleteTable_psc_website_bathdr();
void DeleteTable_psc_website_batuser();
void DeleteTable_psc_website_repset();
void DeleteTable_psc_website_schema();
void DeleteTable_psc_website_tables();
};
#endif
