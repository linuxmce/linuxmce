/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __SCREEN_HANDLER_H__
#define __SCREEN_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "Gen_Devices/AllScreens.h"
#include "Orbiter.h"
#include "CallBackData.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_media/Define_MediaSource.h"
#include "DesignObj_DataGrid.h"
#include "SerializeClass/ShapesColors.h"

//-----------------------------------------------------------------------------------------------------
typedef bool (ScreenHandler::*ScreenHandlerCallBack)(CallBackData *pData);
//-----------------------------------------------------------------------------------------------------
typedef class ScreenHandler * (* RAOP_FType) (class Orbiter *,  map<int,int> *p_MapDesignObj, Logger *);
//-----------------------------------------------------------------------------------------------------
#define SETUP_SCREEN_HANDLER_CALLBACK(SCREEN_HANDLER, CALLBACK_TYPE, CALLBACK_CLASS, PARAMETERS) \
{ \
	if(NULL != SCREEN_HANDLER) \
	{ \
		CALLBACK_CLASS *pCallBackData = dynamic_cast<CALLBACK_CLASS*>(SCREEN_HANDLER->m_mapCallBackData_Find(CALLBACK_TYPE)); \
		if(NULL != pCallBackData) \
			pCallBackData->Setup PARAMETERS; \
\
		CALLBACK_CLASS *pPersistentCallBackData = dynamic_cast<CALLBACK_CLASS*>(SCREEN_HANDLER->m_mapPersistentCallBackData_Find(CALLBACK_TYPE)); \
		if(NULL != pPersistentCallBackData) \
			pPersistentCallBackData->Setup PARAMETERS; \
	} \
}
//-----------------------------------------------------------------------------------------------------
// For keeping track of the tree when we're drilling down through attributes
class MediaFileAttributeDrillDown
{
public:
	int m_PK_Attribute;
	string m_sDescription;
	int m_iLastRow_ListGrid;
	string m_sExternalDirectory;

	MediaFileAttributeDrillDown(int PK_Attribute, string sDescription,int iLastRow_ListGrid)
	{
		m_PK_Attribute=PK_Attribute;
		m_sDescription=sDescription;
		m_iLastRow_ListGrid=iLastRow_ListGrid;
	}
	MediaFileAttributeDrillDown(string sExternalDirectory, string sDescription,int iLastRow_ListGrid)
	{
		m_PK_Attribute=0;
		m_sDescription=sDescription;
		m_sExternalDirectory=sExternalDirectory;
		m_iLastRow_ListGrid=iLastRow_ListGrid;
	}
};
//-----------------------------------------------------------------------------------------------------
// For keeping track of media browsing options
class MediaFileBrowserOptions
{
public:
	int m_PK_MediaType,m_PK_AttributeType_Sort,m_PK_Users,m_iPK_Screen;
	int m_iLastViewed; // 0=no, 1=yes, 2=either
	int m_iPK_DesignObj_Browser; // The Design obj for the current browse screen
	int m_iLastRowBeforeMediaFileSelect_ListGrid; // If non-zero, when the user chooses a media file we will return to this row in the grid

	// If the user chooses close or delete the prior row will be preserved because PersistsXY is true for the list/pic grid.  But if he drills
	// down attributes and wants to go back we need to restore the row position.  Set this flag to true to resetore the position in m_iLastRowBeforeMediaFileSelect_ListGrid
	// on the next render
	bool m_bRestoreListGridRow;  
	string m_sPK_MediaSubType,m_sPK_FileFormat,m_sPK_Attribute_Genres,m_sSources,m_sPK_Users_Private;
	string m_sSelectedFile;  // Used to hold the selected value
	list<int> m_listPK_AttributeType_Sort_Prior;
	list< MediaFileAttributeDrillDown * > m_listMediaFileAttributeDrillDown;  // For drilling down on attributes
	map< pair<int,string>, DesignObj_Orbiter * > m_mapObjectsValues;
	Orbiter *m_pOrbiter;
	DesignObj_DataGrid *m_pObj_ListGrid,*m_pObj_PicGrid;
	DesignObj_Orbiter *m_pObj_Users_EnteringPin;
	int m_iPK_Users_EnteringPin;

	MediaFileBrowserOptions(Orbiter *pOrbiter) 
	{ 
		m_pOrbiter=pOrbiter; 
		m_pObj_ListGrid=m_pObj_PicGrid=NULL;
		m_iLastViewed=2;
		m_iPK_DesignObj_Browser = 0;
		m_bRestoreListGridRow = false;
		ClearAll(0,0,0); 
	}

	string ToString()
	{
		string sResult = StringUtils::itos(m_PK_MediaType) + "|" + m_sPK_MediaSubType + "|" + m_sPK_FileFormat + "|" + m_sPK_Attribute_Genres + "|" + m_sSources +
			"|" + m_sPK_Users_Private + "|" + StringUtils::itos(m_PK_AttributeType_Sort) + "|" + StringUtils::itos(m_PK_Users) + " | "
			+ StringUtils::itos(m_iLastViewed) + " |"; 
		if( m_listMediaFileAttributeDrillDown.size() )
		{
			MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = m_listMediaFileAttributeDrillDown.front();
			if( pMediaFileAttributeDrillDown->m_PK_Attribute )
				sResult += StringUtils::itos(pMediaFileAttributeDrillDown->m_PK_Attribute);
			else
				sResult += pMediaFileAttributeDrillDown->m_sExternalDirectory;
		}
		return sResult;
	}

	string HumanReadable();

	void ClearAll(int PK_MediaType,int PK_Screen,int PK_AttributeType_Sort)
	{
		m_iPK_Screen=PK_Screen;
		m_PK_MediaType=PK_MediaType;
		m_PK_Users=0;
		m_PK_AttributeType_Sort = PK_AttributeType_Sort;
		m_listPK_AttributeType_Sort_Prior.clear();
		Clear_m_listMediaFileAttributeDrillDown();
		m_iLastRowBeforeMediaFileSelect_ListGrid=0;
		m_pObj_Users_EnteringPin = NULL;
		m_iPK_Users_EnteringPin = 0;

		m_sPK_MediaSubType=""; m_sPK_FileFormat=""; m_sPK_Attribute_Genres=""; m_sPK_Users_Private="0";
		m_sSources=TOSTRING(MEDIASOURCE_Hard_Drives_CONST) "," TOSTRING(MEDIASOURCE_Discs__Jukeboxes_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_MediaType_CONST, StringUtils::itos(m_PK_MediaType)); 
		m_mapObjectsValues.clear();
		if( PK_Screen )
			m_iPK_DesignObj_Browser = m_pOrbiter->m_mapDesignObj[PK_Screen];

		if( !m_pObj_ListGrid && m_iPK_DesignObj_Browser )
			m_pObj_ListGrid=(DesignObj_DataGrid *) m_pOrbiter->FindObject(StringUtils::itos(m_iPK_DesignObj_Browser) + ".0.0." TOSTRING(DESIGNOBJ_dgFileList2_CONST));
		if( !m_pObj_PicGrid && m_iPK_DesignObj_Browser )
			m_pObj_PicGrid=(DesignObj_DataGrid *) m_pOrbiter->FindObject(StringUtils::itos(m_iPK_DesignObj_Browser) + ".0.0." TOSTRING(DESIGNOBJ_dgFileList2_Pics_CONST));
		ReacquireGrids();
	}

	void Clear_m_listMediaFileAttributeDrillDown()
	{
		for(list< MediaFileAttributeDrillDown * >::iterator it=m_listMediaFileAttributeDrillDown.begin();
			it!=m_listMediaFileAttributeDrillDown.end();++it)
		{
			delete *it;
		}
		m_listMediaFileAttributeDrillDown.clear();
	}

	void ReacquireGrids();
	void SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,string &sValues);
	void SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,int &iValue);
	void SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,string &sValues,bool bTreatZeroAsAll);
	void SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,int &iValue);
};

class ScreenHandler : public ScreenHandlerBase
{
protected:
	Orbiter *m_pOrbiter;
	
	map<CallBackType, ScreenHandlerCallBack> m_mapCallBack;
	map<CallBackType, CallBackData *> m_mapCallBackData;

	map<CallBackType, ScreenHandlerCallBack> m_mapPersistentCallBack;
	map<CallBackType, CallBackData *> m_mapPersistentCallBackData;

	string m_sActiveApplication_Description,m_sActiveApplication_Window;
	int m_PK_Screen_ActiveApp_OSD,m_PK_Screen_ActiveApp_Remote;
	int m_iStage; // Some screens go through stages and need a variable to track that
	time_t m_tLastDeviceAdded;

	int m_PK_Software;  // For add software grid
	string m_sInstallationStatus;  // For add software grid

	pluto_pthread_mutex_t m_MapMutex; /** < Protected the access to our maps */

	char *m_pData_LastThumbnail;
	int m_iData_Size_LastThumbnail;

	virtual void DisplayMessageOnOrbiter(int PK_Screen,
		string sMessage, bool bPromptToResetRouter = false,
		string sTimeout = "0", bool bCantGoBack = false, 
		string sOption1 = "", string sMessage1 = "",
		string sOption2 = "", string sMessage2 = "", 
		string sOption3 = "", string sMessage3 = "",
		string sOption4 = "", string sMessage4 = "",
		string sID="");


public: 
	ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	virtual ~ScreenHandler();

	//used to register as plugin
	virtual bool Register() { return 0; }

	//callback related stuff
	void RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack, 
		CallBackData *pCallBackData);

	ScreenHandlerCallBack m_mapCallBack_Find(CallBackType aCallBackType);
	CallBackData *m_mapCallBackData_Find(CallBackType aCallBackType);

	void RegisterPersistentCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack, 
		CallBackData *pCallBackData);

	ScreenHandlerCallBack m_mapPersistentCallBack_Find(CallBackType aCallBackType);
	CallBackData *m_mapPersistentCallBackData_Find(CallBackType aCallBackType);

	virtual void ResetCallBacks();


	map<string,string> m_mapKeywords;  // Used for storing keywords to be substituded into text
	string m_mapKeywords_Find(string sKeyword) { PLUTO_SAFETY_LOCK( vm, m_pOrbiter->m_VariableMutex ); map<string,string>::iterator it = m_mapKeywords.find(sKeyword); return it==m_mapKeywords.end() ? "" : (*it).second; }

	//helper functions
	int GetCurrentScreen_PK_DesignObj();
	void RefreshDatagrid(long PK_DesignObj_Datagrid);
	void BadGotoScreen(int PK_Screen);

	//screens functions
	virtual void GotoDesignObj(int PK_DesignObj,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false);
	virtual void SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue);
	virtual void SCREEN_New_Pnp_Device_One_Possibility(long PK_Screen, int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue);
	bool Pnp_ObjectSelected(CallBackData *pData);
	virtual void SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID);
	MediaFileBrowserOptions mediaFileBrowserOptions;  // Current sort/filter options for the media browser

//	virtual void SCREEN_FileList_PlayLists(long PK_Screen);
	virtual void SCREEN_FileList_Music_Movies_Video(long PK_Screen);
	bool MediaBrowsre_Intercepted(CallBackData *pData);
	bool MediaBrowser_ObjectSelected(CallBackData *pData);
	string GetFileBrowserPopup(DesignObj_Orbiter *pObj_MenuPad);
	bool MediaBrowser_DatagridSelected(CallBackData *pData);
	void SelectedMediaFile(string sFile,char cActionItem);
	bool FileList_GridRendering(CallBackData *pData);
	void SelectedExternalDirectory(DataGridCell *pCell); // The user selected a drill-down directory provided by an external source of directory info (like YouTube)
	void SelectedAttributeCell(DataGridCell *pCell);
	bool MediaBrowser_Render(CallBackData *pData);
	void SetMediaSortFilterSelectedObjects();
	void GetAttributesForMediaFile(const char *pFilename);
	bool FileList_KeyDown(CallBackData *pData);
	bool MediaBrowser_OnTimer(CallBackData *pData);

	virtual void SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription, int iPK_PnpQueue);
	virtual void SCREEN_WhatModelMobileOrbiter(long PK_Screen, int iPK_Users, string sMac_address);
	virtual void SCREEN_Pick_Room_For_Device(long PK_Screen, int iPK_Device, string sDescription, string ssComments);
	virtual void SCREEN_SensorsNotReady(long PK_Screen, string sDescription);
	virtual void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, 
		string sExitDelay, string sAlerts);
	virtual void SCREEN_CurrentlyActiveRemote(long PK_Screen);
	virtual void SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack);
	virtual void SCREEN_GenericKeyboard(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sCannotGoBack){ SCREEN_PopupMessage(PK_Screen, sText, sCommand_Line, sDescription, "0", "0", sCannotGoBack); }  // Treat this like the popup message
	virtual void SCREEN_Power(long PK_Screen);
	bool Power_ObjectSelected(CallBackData *pData);

	virtual void SCREEN_GenericAppController(long PK_Screen);
	virtual void SCREEN_GenericAppFullScreen(long PK_Screen);
	virtual void SCREEN_Computing(long PK_Screen);
	bool Computing_ObjectSelected(CallBackData *pData);
	bool Computing_DatagridSelected(CallBackData *pData);

	virtual void SCREEN_Remote_Assistance(long PK_Screen);
	bool SCREEN_Remote_Assistance_ObjectSelected(CallBackData *pData);
	bool SCREEN_Remote_Assistance_OnTimer(CallBackData *pData);

	//dialogs
	virtual void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors);
	virtual void SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_DeviceFrom, string sPK_RippingDevice);
	virtual void SCREEN_DialogCheckingDrive(long PK_Screen);
	virtual void SCREEN_DialogUnableToSavePlaylist(long PK_Screen);
	virtual void SCREEN_DialogPlaylistSaved(long PK_Screen);
	virtual void SCREEN_DialogUnableToLoadPlaylist(long PK_Screen);
	virtual void SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout	);
	virtual void SCREEN_Cannot_Reload_Router(long PK_Screen, string sDescription);
	virtual void SCREEN_DialogRippingInstructions(long PK_Screen);
	virtual void SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, 
		string sTimeout, string sCannotGoBack);
	virtual void SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors);
	virtual void SCREEN_DialogAskToResume(long PK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, 
		string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo);
	virtual void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack);
	virtual void SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName);
	virtual void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine, 
		string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer);
	virtual void SCREEN_Floorplan(long PK_Screen, string sPK_DesignObj);

	//Screens with locations
	virtual void SCREEN_Main(long PK_Screen, string sLocation);
	void SetupAudioServer(); //the main screen for Audio Server
	void AudioServer_PopulateDatagrid();

	virtual void SCREEN_Lights(long PK_Screen, string sLocation);
	virtual void SCREEN_Media(long PK_Screen, string sLocation);
	virtual void SCREEN_Climate(long PK_Screen, string sLocation);
	virtual void SCREEN_Security(long PK_Screen, string sLocation);
	virtual void SCREEN_Telephony(long PK_Screen, string sLocation);
	virtual void SCREEN_Misc(long PK_Screen, string sLocation);

	virtual void SCREEN_popLights(long PK_Screen, string sLocation);
	virtual void SCREEN_popMedia(long PK_Screen, string sLocation);
	virtual void SCREEN_popClimate(long PK_Screen, string sLocation);
	virtual void SCREEN_popTelecom(long PK_Screen, string sLocation);
	virtual void SCREEN_popSecurity(long PK_Screen, string sLocation);

	virtual void SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue);
	virtual void SCREEN_Get_Username_Password_For_Devices(long PK_Screen, bool bAlready_processed, string sDescription, int iPK_PnpQueue);

	virtual void SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device, string sPK_Device_AppServer);
	virtual void SCREEN_PNP_Generic_Options(long PK_Screen, string sOptions, int iPK_PnpQueue);
	bool PNP_Generic_Options_DatagridSelected(CallBackData *pData);

	//cameras
	virtual void SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device);
	virtual void SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device);
	virtual void SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sOptions, int iPK_PnpQueue);
	bool Sensors_ObjectSelected(CallBackData *pData);

	virtual bool New_Phone_Enter_Number_DeviceConfigured(CallBackData *pData);
	virtual void SCREEN_PVR_Remote(long PK_Screen);
	virtual void SCREEN_PVR_Recordings(long PK_Screen);
	virtual void SCREEN_TV_Channels(long PK_Screen);
	bool TV_Channels_ObjectSelected(CallBackData *pData);
	bool TV_Channels_GridRendering(CallBackData *pData);

	virtual void SCREEN_Add_Software(long PK_Screen);
	bool AddSoftware_ObjectSelected(CallBackData *pData);
	bool AddSoftware_ObjectHighlighted(CallBackData *pData);
	bool AddSoftware_GridRendering(CallBackData *pData);
	bool AddSoftware_GridSelected(CallBackData *pData);

	virtual void SCREEN_Halt_System(long PK_Screen);

	void SCREEN_CreateViewBookmarksTV(long PK_Screen);
	bool CreateViewBookmarksTV_ObjectSelected(CallBackData *pData);

	void SCREEN_Thumbnail(long PK_Screen);
	bool Thumbnail_DatagridSelected(CallBackData *pData);

	void SCREEN_Manage_Drives(long PK_Screen);
	void SCREEN_Jukebox_Manager(long PK_Screen, int iPK_Device);
	void SCREEN_NAS_Manager(long PK_Screen, int iPK_Device);
	bool DriveOverview_ObjectSelected(CallBackData *pData);
	bool DriveOverview_GridRendering(CallBackData *pData);
	bool JukeboxManager_ObjectSelected(CallBackData *pData);
	bool JukeboxManager_GridRendering(CallBackData *pData);
	bool NASManager_ObjectSelected(CallBackData *pData);

	virtual void SCREEN_PendingTasks(long PK_Screen);
	bool PendingTasks_GridRendering(CallBackData *pData);
	bool PendingTasks_ObjectSelected(CallBackData *pData);

	virtual void SCREEN_AutoConfigure_TV(long PK_Screen, int iPK_PnpQueue);

	virtual void SCREEN_AdvancedOptions(long PK_Screen);
	bool AdvancedOptions_ObjectSelected(CallBackData *pData);

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    SCREEN_AdjustScreenSettings
//

	virtual void SCREEN_AdjustScreenSettings(long PK_Screen);
	void AdjustScreenSettings_DrawArrows();
	bool AdjustScreenSettings_KeyDown(CallBackData *pData);
	bool AdjustScreenSettings_OnTimer(CallBackData *pData);
	bool AdjustScreenSettings_RemoteKeyCodeIntercepted(CallBackData *pData);
	bool AdjustScreenSettings_ObjectSelected(CallBackData *pData);

	PlutoPoint AdjustScreenSettings_LoadOffset();
	void AdjustScreenSettings_SaveOffset(PlutoPoint offset);
	int AdjustScreenSettings_LoadSpacing();
	void AdjustScreenSettings_SaveSpacing(int nValue);

	PlutoPoint m_ScreenOffset;
	int m_nScreenSpacing;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    TELECOM SCREENS
//

	void SCREEN_MakeCallPhonebook(long PK_Screen);
	void SCREEN_MakeCallFavorites(long PK_Screen);
	void SCREEN_MakeCallDialNumber(long PK_Screen);
	void SCREEN_MakeCallPlutoUser(long PK_Screen);
	void SCREEN_MakeCallIntercom(long PK_Screen);
	void SCREEN_MakeCallDevice(long PK_Screen);
	void SCREEN_Active_Calls(long PK_Screen);
	void SCREEN_DevCallInProgress(long PK_Screen, string sPhoneCallerID, string sPhoneCallID, 
		string sChannel);
	void SCREEN_DevIncomingCall(long PK_Screen, string sPhoneNumber, string sSource_Channel, 
		string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID);
	void SCREEN_Call_Dropped(long PK_Screen, string sReason);
	void SCREEN_Assisted_Transfer_In_Progress(long PK_Screen, bool bTrueFalse, string sTask);

	//interceptors
	bool Telecom_ObjectSelected(CallBackData *pData);
	bool Telecom_DataGridRendering(CallBackData *pData);
	bool Telecom_OnTimer(CallBackData *pData);
	bool Telecom_CapturedKeyboardBufferChanged(CallBackData *pData);
	bool Telecom_CustomRender(CallBackData *pData);
	bool Telecom_DatagridSelected(CallBackData *pData);

	//events
	bool ActiveCalls_Join();
	bool ActiveCalls_AddToActiveCall();
	bool ActiveCalls_RemoveFromActiveCall();
	bool ActiveCalls_CallInProgress();

	bool CallInProgress_TransferConference();
	bool CallInProgress_VoiceMail();

	//helpers
	void HandleAssistedMakeCall(int iPK_Users, string sPhoneExtension, int iPK_Device_From,
		int iPK_Device_To, string sSecondPhoneCall);
	void RefreshActiveCallsButtons();

	//data
	enum TelecomCommandStatus
	{
		tcsDirectDial = 0,
		tcsTransferConference, 
		tcsJoin,
		tcsBlindTransfer
	};

	TelecomCommandStatus m_TelecomCommandStatus;
	string m_sMyCallID;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    SCREEN_FileSave
//
	virtual void SCREEN_FileSave(long PK_Screen, int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options);
	bool FileSave_ObjectSelected(CallBackData *pData);
	bool FileSave_GridSelected(CallBackData *pData);

	//helper methods
	void SaveFile_GotoChooseFolderDesignObj();
	void SaveFile_SendCommand(int PK_Users);

	//savefile data
	string m_sSaveFile_MountedFolder;
	string m_sSaveFile_RelativeFolder;
	string m_sSaveFile_Drive;
	string m_sSaveFile_FullBasePath;
	string m_sSaveFile_FileName;
	string m_sSaveFile_Command;
	bool m_bSaveFile_IsMove;
	bool m_bSaveFile_CreatingFolder;
	bool m_bSaveFile_Advanced_options;
	bool m_bUseDirectoryStructure;  // The drive we're saving to should use our public/private directory structure
	int m_nSaveFile_PK_DeviceDrive;
	int m_nPK_Users_SaveFile;

	void Reset_SaveFile_Info();

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//    SCREEN_Network_Setup
//
	void SCREEN_Static_IP_settings(long PK_Screen);

	void SCREEN_Network_Settings(long PK_Screen);
	bool SCREEN_Network_Settings_ObjectSelected(CallBackData *pData);
	bool SCREEN_Network_Settings_VariableChanged(CallBackData *pData);
	bool SCREEN_Network_Settings_OnTimer(CallBackData *pData);

	//data
	map<string,string> m_mapNetworkSettings;
///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// UserStatus stuff
//

	void SCREEN_UserStatus(long PK_Screen);
	bool UserStatus_Voicemail_DataGridSelected(CallBackData *pData);

};
//-----------------------------------------------------------------------------------------------------
#endif
