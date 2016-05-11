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
#include "ScreenHandler.h"
#include "ScreenHistory.h"
#include "CallBackTypes.h"
#include "OrbiterRenderer.h"
#ifdef ENABLE_MOUSE_BEHAVIOR
#include "MouseBehavior.h"
#endif
#include "MediaBrowserMouseHandler.h"
#include "DataGrid.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Message.h"
#include "DCE/DCEConfig.h"
#include "ObjectRenderer.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Array.h"
#include "pluto_media/Define_AttributeType.h"
#include "pluto_media/Define_FileFormat.h"

#include "OrbiterFileBrowser.h"

using namespace DCE;

//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : 
	ScreenHandlerBase(p_MapDesignObj)
	, m_MapMutex("maps")
	, mediaFileBrowserOptions(pOrbiter)
{
	m_pOrbiter = pOrbiter;
	m_tLastDeviceAdded = 0;
	m_MapMutex.Init(NULL);

	m_pData_LastThumbnail=NULL;
	m_iData_Size_LastThumbnail=0;

	m_TelecomCommandStatus = tcsDirectDial;

	Reset_SaveFile_Info();
}
//-----------------------------------------------------------------------------------------------------
ScreenHandler::~ScreenHandler()
{
	pthread_mutex_destroy(&m_MapMutex.mutex);
	delete m_pData_LastThumbnail;
	m_pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack,
	CallBackData *pCallBackData)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	m_mapCallBack[aCallBackType] = aScreenHandlerCallBack;
	m_mapCallBackData[aCallBackType] = pCallBackData;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RegisterPersistentCallBack(CallBackType aCallBackType, 
	ScreenHandlerCallBack aScreenHandlerCallBack, CallBackData *pCallBackData)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	m_mapPersistentCallBack[aCallBackType] = aScreenHandlerCallBack;
	m_mapPersistentCallBackData[aCallBackType] = pCallBackData;
}
//-----------------------------------------------------------------------------------------------------
ScreenHandlerCallBack ScreenHandler::m_mapPersistentCallBack_Find(CallBackType aCallBackType)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, ScreenHandlerCallBack>::iterator it = m_mapPersistentCallBack.find(aCallBackType);
	return it == m_mapPersistentCallBack.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
CallBackData *ScreenHandler::m_mapPersistentCallBackData_Find(CallBackType aCallBackType)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, CallBackData *>::iterator it = m_mapPersistentCallBackData.find(aCallBackType);
	return it == m_mapPersistentCallBackData.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::ResetCallBacks()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::ResetCallBacks");
#endif
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	m_mapCallBack.clear();

	for(map<CallBackType, CallBackData *>::iterator it = m_mapCallBackData.begin(); it != m_mapCallBackData.end(); it++)
	{
		CallBackData *pCallBackData = it->second;
		delete pCallBackData;
	}
	m_mapCallBackData.clear();
}
//-----------------------------------------------------------------------------------------------------
ScreenHandlerCallBack ScreenHandler::m_mapCallBack_Find(CallBackType aCallBackType)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, ScreenHandlerCallBack>::iterator it = m_mapCallBack.find(aCallBackType);
	return it == m_mapCallBack.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
CallBackData *ScreenHandler::m_mapCallBackData_Find(CallBackType aCallBackType)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, CallBackData *>::iterator it = m_mapCallBackData.find(aCallBackType);
	return it == m_mapCallBackData.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RefreshDatagrid(long PK_DesignObj_Datagrid)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(StringUtils::ltos(PK_DesignObj_Datagrid));
	if(pObj)
	{
		m_pOrbiter->Renderer()->RenderObjectAsync(pObj);
		m_pOrbiter->InitializeGrid((DesignObj_DataGrid *)pObj);
		m_pOrbiter->Renderer()->RedrawObjects();
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GotoDesignObj(int PK_DesignObj,string sID,bool bStore_Variables,bool bCant_Go_Back)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(PK_DesignObj), sID, "", bStore_Variables, bCant_Go_Back);
}
//-----------------------------------------------------------------------------------------------------
int ScreenHandler::GetCurrentScreen_PK_DesignObj()
{
	return m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
{
	string::size_type pos=0;
	string sTitle = StringUtils::Tokenize(sText,"|",pos);
	string sOption1 = StringUtils::Tokenize(sText,"|",pos);
	string sOption2 = StringUtils::Tokenize(sText,"|",pos);
	string sOption3 = StringUtils::Tokenize(sText,"|",pos);
	string sOption4 = StringUtils::Tokenize(sText,"|",pos);

	pos=0;
	string sMessage1 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage2 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage3 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage4 = StringUtils::Tokenize(sCommand_Line,"|",pos);

	DisplayMessageOnOrbiter(PK_Screen,sTitle, sPromptToResetRouter == "1", sTimeout, sCannotGoBack == "1",
		sOption1,sMessage1,sOption2,sMessage2,sOption3,sMessage3,sOption4,sMessage4);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue)
{
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Pnp_ObjectSelected,	new ObjectInfoBackData());
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);

	if(NULL != m_pOrbiter->m_pScreenHistory_Pivot)
		m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(StringUtils::itos(iPK_PnpQueue));

	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), StringUtils::itos(iPK_PnpQueue), "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_New_Pnp_Device_One_Possibility(long PK_Screen, int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue)
{
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Pnp_ObjectSelected,	new ObjectInfoBackData());
	string sTitle = m_pOrbiter->m_mapTextString[TEXT_new_pnp_device_one_possibility_CONST];
	string sRoom;
	if( iPK_Room )
	{
		sRoom = m_pOrbiter->m_mapTextString[TEXT_I_think_it_is_in_the_room_CONST];
		StringUtils::Replace(&sRoom,"<%=ROOM%>",ssComments);
	}
	StringUtils::Replace(&sTitle,"<%=ROOM%>",sRoom);
	StringUtils::Replace(&sTitle,"<%=DEVICE%>",sDescription);

	string sMessageYesUseItWrongRoom = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " 1 " + 
		TOSTRING(COMMAND_Choose_Pnp_Device_Template_CONST) + " " + TOSTRING(COMMANDPARAMETER_PK_DHCPDevice_CONST) + " " + StringUtils::itos(iPK_DHCPDevice) +
		" " + TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue);

	string sMessageYesUseIt = sMessageYesUseItWrongRoom +
		" " + TOSTRING(COMMANDPARAMETER_PK_Room_CONST) + " " + StringUtils::itos(iPK_Room);

	string sMessageNoIgnoreIt = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " 1 " + 
		TOSTRING(COMMAND_Ignore_PNP_Device_CONST) + " " + " " + TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue);

	string sMessageNoIgnoreItEveryTime = sMessageNoIgnoreIt + " " + TOSTRING(COMMANDPARAMETER_Always_CONST) + " 1";

	if( iPK_Room )
		DisplayMessageOnOrbiter(PK_Screen,sTitle, false, "0", true,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sMessageYesUseIt,m_pOrbiter->m_mapTextString[TEXT_Yes_but_wrong_room_CONST],sMessageYesUseItWrongRoom,
			m_pOrbiter->m_mapTextString[TEXT_No_ignore_it_CONST],sMessageNoIgnoreIt,m_pOrbiter->m_mapTextString[TEXT_Ignore_everytime_CONST],sMessageNoIgnoreItEveryTime,StringUtils::itos(iPK_PnpQueue));
	else
		DisplayMessageOnOrbiter(PK_Screen,sTitle, false, "0", true,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sMessageYesUseIt,
			m_pOrbiter->m_mapTextString[TEXT_No_ignore_it_CONST],sMessageNoIgnoreIt,m_pOrbiter->m_mapTextString[TEXT_Ignore_everytime_CONST],sMessageNoIgnoreItEveryTime,"","",StringUtils::itos(iPK_PnpQueue));
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Pnp_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST || 
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse3_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse4_CONST )
	{
		m_tLastDeviceAdded = time(NULL);
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sFormat, string sName, int iEK_Disc, int iSlot_Number, int iDriveID)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::ltos(iPK_Users));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sFormat);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sName);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, StringUtils::itos(iEK_Disc));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, StringUtils::itos(iSlot_Number));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(iDriveID));
	ScreenHandlerBase::SCREEN_CDTrackCopy(PK_Screen, iPK_Users, sFormat, sName, iEK_Disc, iSlot_Number, iDriveID);
}
//-----------------------------------------------------------------------------------------------------
string MediaFileBrowserOptions::HumanReadable()
{
	string sResult = m_pOrbiter->m_mapPK_MediaType_Description[ m_PK_MediaType ] + " : " 
		+ m_pOrbiter->m_mapPK_AttributeType_Description[ m_PK_AttributeType_Sort ];

	string sFilter;
	if( m_listMediaFileAttributeDrillDown.size() )
	{
		MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = m_listMediaFileAttributeDrillDown.front();
		sFilter = pMediaFileAttributeDrillDown->m_sDescription;
	}

	bool bHide = m_listMediaFileAttributeDrillDown.empty();
	DesignObj_Orbiter *pObj_Back = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFileBrowserBack_CONST));
	if( pObj_Back && pObj_Back->m_bHidden != bHide )
	{
		NeedToRender render2( m_pOrbiter, "ScreenHandler::HumanReadable" );
		m_pOrbiter->CMD_Show_Object( pObj_Back->m_ObjectID,0,"","", bHide ? "0" : "1" );
	}
	return sResult + " " + sFilter;
}
/*
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileList_PlayLists(long PK_Screen)
{
	// This uses all the same actions as the music/movie browser
	SCREEN_FileList_Music_Movies_Video(PK_Screen);
}
*/
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileList_Music_Movies_Video(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_FileList_Music_Movies_Video(PK_Screen);

	SetMediaSortFilterSelectedObjects();
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_Render, new RenderScreenCallBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::FileList_GridRendering,	new DatagridAcquiredBackData());
	RegisterCallBack(cbOnKeyDown, (ScreenHandlerCallBack) &ScreenHandler::FileList_KeyDown, new KeyCallBackData());
	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowsre_Intercepted, new MsgInterceptorCellBackData());

	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_ViewedOnly_CONST) );
	if( pObj )
		pObj->m_GraphicToDisplay_set("fmv1",GRAPHIC_NORMAL,false,true);

	pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_UnviewedOnly_CONST) );
	if( pObj )
		pObj->m_GraphicToDisplay_set("fmv2",GRAPHIC_NORMAL,false,true);

	return;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowsre_Intercepted(CallBackData *pData)
{
	MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;
	// If the user hits play while browsing this screen, immediately play whatever is highlighted
	if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMsgInterceptorCellBackData->m_pMessage->m_dwID==COMMAND_Play_Media_CONST && mediaFileBrowserOptions.m_pObj_ListGrid )
	{
		DataGridTable *pDataGridTable = mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get();
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowsre_Intercepted got the play button %p",pDataGridTable);
#endif
		if( pDataGridTable )
		{
			DataGridCell *pCell = pDataGridTable->GetData( 0, mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow + mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow );
			if( pCell && pCell->m_Value && strstr(pCell->m_Value,"\t!D")==NULL )
			{
				// User is not going into a sub directory, and be sure also not selecting an attribute
				if( pCell->m_Value[0]!=0 && (pCell->m_Value[0]!='!' || pCell->m_Value[1]!='A') )
				{
					mediaFileBrowserOptions.m_sSelectedFile = pCell->m_Value;
					bool bQueue = m_pOrbiter->GetData()->Get_Queue_Instead_of_Instant_Play();
					DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						0,mediaFileBrowserOptions.m_sSelectedFile,0,0,StringUtils::itos( m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),false,0,bQueue, 0 /* bBypass_Event */, 0 /* bDont_Setup_AV */ );
					m_pOrbiter->SendCommand(CMD_MH_Play_Media);
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileList_KeyDown(CallBackData *pData)
{
	KeyCallBackData *pKeyCallBackData = (KeyCallBackData *) pData;

	if( pKeyCallBackData->m_nPlutoKey==BUTTON_Scroll_Down_CONST || pKeyCallBackData->m_nPlutoKey==BUTTON_Scroll_Up_CONST )
	{
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_dgFileList2_CONST);
		if( pObj && pObj->m_bOnScreen )
		{
			m_pOrbiter->Scroll_Grid("",pObj->m_ObjectID,pKeyCallBackData->m_nPlutoKey==BUTTON_Scroll_Down_CONST ? DIRECTION_Down_CONST : DIRECTION_Up_CONST);
			return true;
		}
	}
	else if( (pKeyCallBackData->m_nPlutoKey>=BUTTON_1_CONST && pKeyCallBackData->m_nPlutoKey<=BUTTON_0_CONST) || 
		(pKeyCallBackData->m_nPlutoKey>=BUTTON_a_CONST && pKeyCallBackData->m_nPlutoKey<=BUTTON_Z_CONST) )
	{
		DesignObj_DataGrid *pObj = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgFileList2_CONST);
		if( pObj )
		{
			string sSeek = " ";
			if( pKeyCallBackData->m_nPlutoKey==BUTTON_0_CONST )
				sSeek[0] = '0';
			else if( pKeyCallBackData->m_nPlutoKey>=BUTTON_1_CONST && pKeyCallBackData->m_nPlutoKey<=BUTTON_0_CONST )
				sSeek[0] = '1' + pKeyCallBackData->m_nPlutoKey-BUTTON_1_CONST;
			else if( pKeyCallBackData->m_nPlutoKey>=BUTTON_a_CONST && pKeyCallBackData->m_nPlutoKey<=BUTTON_z_CONST )
				sSeek[0] = 'A' + pKeyCallBackData->m_nPlutoKey-BUTTON_a_CONST;
			else if( pKeyCallBackData->m_nPlutoKey>=BUTTON_A_CONST && pKeyCallBackData->m_nPlutoKey<=BUTTON_Z_CONST )
				sSeek[0] = 'A' + pKeyCallBackData->m_nPlutoKey-BUTTON_A_CONST;

			m_pOrbiter->CMD_Seek_Data_Grid(sSeek,0,0,pObj->m_sGridID);
		}
	}
	else if( pKeyCallBackData->m_nPlutoKey==BUTTON_Back_CONST )  // Treat the backspace key like hitting the back button
	{
		ObjectInfoBackData objectInfoData;
		objectInfoData.m_PK_DesignObj_SelectedObject = DESIGNOBJ_butFileBrowserBack_CONST;
		objectInfoData.m_pObj = NULL;
		MediaBrowser_ObjectSelected(&objectInfoData);
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SetMediaSortFilterSelectedObjects()
{
	string sMediaType = StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType);
	if( m_pOrbiter->UsesUIVersion2() )
	{
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) "." + sMediaType + ".0"), ARRAY_Media_Filter_Subtype_CONST, mediaFileBrowserOptions.m_sPK_MediaSubType );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST)), ARRAY_Media_Filter_File_Format_CONST, mediaFileBrowserOptions.m_sPK_FileFormat );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) "." + sMediaType + ".0"), ARRAY_Media_Filter_Genres_CONST, mediaFileBrowserOptions.m_sPK_Attribute_Genres );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) "." + sMediaType + ".0"), ARRAY_Media_Sort_Options_CONST, mediaFileBrowserOptions.m_PK_AttributeType_Sort );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) "." + sMediaType + ".0"), ARRAY_Media_Filter_Source_CONST, mediaFileBrowserOptions.m_sSources );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_grpPrivateMedia_CONST)), ARRAY_All_Users_CONST, mediaFileBrowserOptions.m_sPK_Users_Private );
	}
	else
	{
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST)), ARRAY_Media_Filter_Subtype_CONST, mediaFileBrowserOptions.m_sPK_MediaSubType );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST)), ARRAY_Media_Filter_File_Format_CONST, mediaFileBrowserOptions.m_sPK_FileFormat );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST)), ARRAY_Media_Filter_Genres_CONST, mediaFileBrowserOptions.m_sPK_Attribute_Genres );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST)), ARRAY_Media_Sort_Options_CONST, mediaFileBrowserOptions.m_PK_AttributeType_Sort );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST)), ARRAY_Media_Filter_Source_CONST, mediaFileBrowserOptions.m_sSources );
		mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuFileList_SortOptions_CONST) "." + sMediaType + ".0." TOSTRING(DESIGNOBJ_grpPrivateMedia_CONST)), ARRAY_All_Users_CONST, mediaFileBrowserOptions.m_sPK_Users_Private );
	}
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_Render(CallBackData *pData)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_objFileBrowserHeader_CONST) );
	if( pObj && pObj->m_vectDesignObjText.size() )
	{
		DesignObjText *pText = pObj->m_vectDesignObjText[0];
		pText->m_sText = mediaFileBrowserOptions.HumanReadable();
		pObj->m_bHidden = pText->m_sText.empty();
	}

	if( mediaFileBrowserOptions.m_bRestoreListGridRow )
	{
		mediaFileBrowserOptions.m_bRestoreListGridRow=false;
		if( mediaFileBrowserOptions.m_pObj_ListGrid && mediaFileBrowserOptions.m_pObj_PicGrid )
		{
			mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow = mediaFileBrowserOptions.m_iLastRowBeforeMediaFileSelect_ListGrid;
			mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow = mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow / mediaFileBrowserOptions.m_pObj_PicGrid->m_MaxCol;
		}
	}

	if( mediaFileBrowserOptions.m_pObj_ListGrid && mediaFileBrowserOptions.m_pObj_PicGrid && mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow >= mediaFileBrowserOptions.m_pObj_ListGrid->m_iPopulatedHeight )
	{
		// If we have deleted some files, the list may have shrunk and we are going back to a point that's past the end of the list
		mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow = ((mediaFileBrowserOptions.m_pObj_ListGrid->m_iPopulatedHeight-1) / mediaFileBrowserOptions.m_pObj_ListGrid->m_MaxRow) * mediaFileBrowserOptions.m_pObj_ListGrid->m_MaxRow;
		mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow = mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow / mediaFileBrowserOptions.m_pObj_PicGrid->m_MaxCol;
	}
	if( mediaFileBrowserOptions.m_pObj_ListGrid && mediaFileBrowserOptions.m_pObj_PicGrid && mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow<0 )
	{
		// Be sure we're not at a point above the top of the list.  This can happen if we delete all the records in a grid and the 
		// above blocks scrolls us up past the top.  Put the check here just so we catch anything else that may move us past the top
		mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow = mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow = 0;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_ScreenMutex);
	bool bChangedSortFilter=false;

	if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_icoVertAlpha_CONST && mediaFileBrowserOptions.m_pObj_ListGrid && mediaFileBrowserOptions.m_pObj_PicGrid )
	{
		// Find the position where 0=the top, 1=A, 26=7
		int Letter = (pObjectInfoData->m_Y-pObjectInfoData->m_pObj->m_rPosition.Y) * 27 / pObjectInfoData->m_pObj->m_rPosition.Height;
		if( Letter==0 )
		{
			mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow = mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow = 0;
			m_pOrbiter->CMD_Refresh("");
		}
		else
		{
			string s = " ";
			s[0] = (char) Letter + 64;
			m_pOrbiter->CMD_Remove_Popup("","coverart");
			m_pOrbiter->CMD_Seek_Data_Grid(s,0,0,mediaFileBrowserOptions.m_pObj_ListGrid->m_sGridID);
		}
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_objCDCover_CONST && atoi(pObjectInfoData->m_pObj->m_ObjectID.c_str())==DESIGNOBJ_popCoverArt_CONST && mediaFileBrowserOptions.m_pObj_ListGrid )
	{
		DataGridTable *pDataGridTable = mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get();
		if( pDataGridTable )
		{
			DataGridCell *pCell = pDataGridTable->GetData( 0, mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow + mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow );
			if( pCell )
			{
				// Special case.  The user clicked on the cover art popup.  Since it's a popup MediaBrowser_DatagridSelected won't get called;
				DatagridCellBackData datagridCellBackData;
				datagridCellBackData.m_pDesignObj_DataGrid = mediaFileBrowserOptions.m_pObj_ListGrid;
				datagridCellBackData.m_nPK_Datagrid = mediaFileBrowserOptions.m_pObj_ListGrid->m_iPK_Datagrid;
				datagridCellBackData.m_Column = mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedColumn;
				datagridCellBackData.m_Row = mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow + mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow;
				datagridCellBackData.m_pDataGridCell = pCell;
				if( pCell->m_Text )
					datagridCellBackData.m_sText = pCell->m_Text;
				if( pCell->m_Value )
					datagridCellBackData.m_sValue = pCell->m_Value;
				return MediaBrowser_DatagridSelected(&datagridCellBackData);
			}
		}
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_More_ViewedOnly_CONST )
	{
		mediaFileBrowserOptions.m_iLastViewed = mediaFileBrowserOptions.m_iLastViewed!=1 ? 1 : 2;
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_ViewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmvs1",mediaFileBrowserOptions.m_iLastViewed==1 ? GRAPHIC_SELECTED : GRAPHIC_NORMAL,false,true);
		pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_UnviewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmvs3",GRAPHIC_NORMAL,false,true);

		mediaFileBrowserOptions.ReacquireGrids();
		MediaBrowser_Render(NULL);
		m_pOrbiter->CMD_Refresh("*");
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_More_UnviewedOnly_CONST )
	{
		mediaFileBrowserOptions.m_iLastViewed = mediaFileBrowserOptions.m_iLastViewed!=0 ? 0 : 2;
		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_UnviewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmvs2",mediaFileBrowserOptions.m_iLastViewed==0 ? GRAPHIC_SELECTED : GRAPHIC_NORMAL,false,true);
		pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_ViewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmvs4",GRAPHIC_NORMAL,false,true);
		mediaFileBrowserOptions.ReacquireGrids();
		MediaBrowser_Render(NULL);
		m_pOrbiter->CMD_Refresh("*");
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Close_CONST )
	{
		m_pOrbiter->m_pObj_Highlighted_set(mediaFileBrowserOptions.m_pObj_ListGrid);

#ifdef ENABLE_MOUSE_BEHAVIOR
		if( m_pOrbiter->m_pMouseBehavior )
			m_pOrbiter->m_pMouseBehavior->SetMousePosition(mediaFileBrowserOptions.m_pObj_ListGrid);  // Move back to the list so we don't select a filter option
#endif

		DesignObj_Orbiter *pObj_CoverArt;

		if( m_pOrbiter->m_pScreenHistory_Current && m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==DESIGNOBJ_mnuFilelist_Video_Music_Small_CONST )
			pObj_CoverArt = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_objCDCover_CONST) );
		else
			pObj_CoverArt = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_objCDCover_CONST) );

		if(NULL != pObj_CoverArt)
			m_pOrbiter->Renderer()->RemoveGraphic(pObj_CoverArt->GenerateObjectHash(pObj_CoverArt->m_pPopupPoint, false));
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Play_CONST || pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Purchase_CONST  )
	{
//		if( !mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get() )
//			return false; //shouldn't happen

LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowser_ObjectSelected Play Media higlighted %s",mediaFileBrowserOptions.m_sSelectedFile.c_str());

//		m_pOrbiter->CMD_Remove_Popup("","filedetails");
/*
#ifdef ENABLE_MOUSE_BEHAVIOR
		if( m_pOrbiter->m_pMouseBehavior )
			m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
			*/
		bool bQueue = m_pOrbiter->GetData()->Get_Queue_Instead_of_Instant_Play();
		DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			0,mediaFileBrowserOptions.m_sSelectedFile,0,0,StringUtils::itos( m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),false,0, bQueue , 0 /* bBypass_Event */, 0 /* bDont_Setup_AV */);
		m_pOrbiter->SendCommand(CMD_MH_Play_Media);
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Move_CONST )
	  {
	    // Move File button selected.
	    Reset_SaveFile_Info();

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XXXXXXXXXXX Path is %s, Filename is %s .",m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_5_CONST).c_str(),m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST).c_str());

	    string sCommand = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
	      "10" + " "
	      " 1 " TOSTRING (COMMAND_Move_File_CONST) " " + 
	      TOSTRING(COMMANDPARAMETER_Filename_CONST) " " +
	      mediaFileBrowserOptions.m_sSelectedFile + " " +
	      TOSTRING(COMMANDPARAMETER_Path_CONST) " " +
	      "<%=" TOSTRING(VARIABLE_Misc_Data_5_CONST) "%>/" +
	      "<%=" TOSTRING(VARIABLE_Seek_Value_CONST) "%>";
	     
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sending command %s to Media Plugin",sCommand.c_str());		
 
	    string sTitle = "Choose a location to move the file. You can also change the file name here.";
	    m_bSaveFile_IsMove=true;
	    DCE::SCREEN_FileSave screen_FileSave(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,mediaFileBrowserOptions.m_PK_MediaType,-999,sTitle,sCommand,true); // true allows us to see the destination disk
	    m_pOrbiter->SendCommand(screen_FileSave);

	  }
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Delete_CONST )
	{
		string sMessage = m_pOrbiter->m_mapTextString[TEXT_confirm_file_delete_CONST];
		StringUtils::Replace(&sMessage,"<%=FILE%>",m_mapKeywords["FILE"]);
		StringUtils::Replace(&sMessage,"<%=FILENAME%>",mediaFileBrowserOptions.m_sSelectedFile);

		if(m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN)
		{
			mediaFileBrowserOptions.ReacquireGrids();
		}
		else
		{
			int ListCol = mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurCol;
			int ListRow = mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow;
			int PicCol = mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurCol;
			int PicRow = mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow;
			mediaFileBrowserOptions.ReacquireGrids();  // Since we'll be deleting a file, re-acquire the grid next time
			mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurCol = ListCol;
			mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow = ListRow;
			mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurCol = PicCol;
			mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow = PicRow;
		}

		DisplayMessageOnOrbiter(0, sMessage,
			false, "0", true,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST],
			//"Delete file"
			StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_MediaPlugIn) + " 1 " TOSTRING(COMMAND_Delete_File_CONST)
				" " TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"" + mediaFileBrowserOptions.m_sSelectedFile + "\"",
			m_pOrbiter->m_mapTextString[TEXT_NO_CONST]
		);
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFileBrowserBack_CONST )
	{
		if( mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.size() )
		{
			mediaFileBrowserOptions.ReacquireGrids();

			MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.front();
			mediaFileBrowserOptions.m_bRestoreListGridRow = true;
			mediaFileBrowserOptions.m_iLastRowBeforeMediaFileSelect_ListGrid = pMediaFileAttributeDrillDown->m_iLastRow_ListGrid;
			delete pMediaFileAttributeDrillDown;

			mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.pop_front();
			if( mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.size() )
			{
				mediaFileBrowserOptions.m_PK_AttributeType_Sort = mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.front();
				mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.pop_front();
			}
		}
		else
			mediaFileBrowserOptions.ClearAll(mediaFileBrowserOptions.m_PK_MediaType,m_pOrbiter->m_pScreenHistory_Current->PK_Screen(),
				m_pOrbiter->m_mapPK_MediaType_PK_Attribute_Sort[mediaFileBrowserOptions.m_PK_MediaType]);

		AudioServer_PopulateDatagrid();
		mediaFileBrowserOptions.ReacquireGrids();
		m_pOrbiter->CMD_Refresh("*");
		MediaBrowser_Render(NULL);
		return false;
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Go_CONST )  // UI 1's Go
	{
		mediaFileBrowserOptions.ReacquireGrids();
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butPINGo_CONST && mediaFileBrowserOptions.m_pObj_Users_EnteringPin && mediaFileBrowserOptions.m_iPK_Users_EnteringPin )  // user entered a PIN
	{
		string sPin = m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);
		bool bValid=false;
		DCE::CMD_Verify_PIN CMD_Verify_PIN(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_SecurityPlugIn,
			mediaFileBrowserOptions.m_iPK_Users_EnteringPin,sPin,&bValid);
		if( m_pOrbiter->SendCommand(CMD_Verify_PIN) && bValid )
		{
			mediaFileBrowserOptions.m_pObj_Users_EnteringPin->m_GraphicToDisplay_set("sh19",GRAPHIC_SELECTED,false,true);
			mediaFileBrowserOptions.m_sPK_Users_Private += "," + StringUtils::itos(mediaFileBrowserOptions.m_iPK_Users_EnteringPin);
			bChangedSortFilter=true;
			m_pOrbiter->CMD_Go_back("","");
		}
		else
		{
			DisplayMessageOnOrbiter(0, "<%=T" + StringUtils::itos(TEXT_Invalid_PIN_CONST) + "%>");
			mediaFileBrowserOptions.m_iPK_Users_EnteringPin = 0;
			mediaFileBrowserOptions.m_pObj_Users_EnteringPin = NULL;
		}
		return true;
	}
	else if( pObjectInfoData->m_pObj && pObjectInfoData->m_pObj->m_pParentObject )
	{
		switch( pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID )
		{
		case DESIGNOBJ_popFBSF_Genres_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_Media_Filter_Genres_CONST,mediaFileBrowserOptions.m_sPK_Attribute_Genres,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_PK_FileFormat_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_Media_Filter_File_Format_CONST,mediaFileBrowserOptions.m_sPK_FileFormat,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_MediaType_CONST:
			{
				mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_PK_MediaType_CONST,mediaFileBrowserOptions.m_PK_MediaType);
				m_pOrbiter->CMD_Show_File_List(mediaFileBrowserOptions.m_PK_MediaType);
			}
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_grpPrivateMedia_CONST:
			{
				// If the user is choosing private media we need to ask for a PIN code.  Be sure the user is turning 'on' the private media
				int iPK_Array = atoi(pObjectInfoData->m_pObj->GetVariableAssignment(VARIABLE_PK_Array_CONST).c_str());
				string sPK_Users = pObjectInfoData->m_pObj->GetVariableAssignment(VARIABLE_Array_ID_CONST);
				string::size_type p1 = mediaFileBrowserOptions.m_sPK_Users_Private.find("," + sPK_Users + ",");
				string::size_type p2 = mediaFileBrowserOptions.m_sPK_Users_Private.find("," + sPK_Users);
				// If we're turning on a user and it's not already in the middle or end of the active list (ie we're turning it on not off)
				if( iPK_Array==ARRAY_All_Users_CONST && atoi(sPK_Users.c_str())>0 && p1==string::npos && (p2==string::npos || p2!=mediaFileBrowserOptions.m_sPK_Users_Private.size()-sPK_Users.size()-1) )
				{
					mediaFileBrowserOptions.m_iPK_Users_EnteringPin = atoi(sPK_Users.c_str());
					mediaFileBrowserOptions.m_pObj_Users_EnteringPin = pObjectInfoData->m_pObj;
					m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_mnuUserPinCode_CONST),"","",false,true);
					return true;  // We don't want the framework to mark the object as selected
				}
			}

			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_All_Users_CONST,mediaFileBrowserOptions.m_sPK_Users_Private,false);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_RatingsByUser_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_All_Users_CONST,mediaFileBrowserOptions.m_PK_Users);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_Sort_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_Media_Sort_Options_CONST,mediaFileBrowserOptions.m_PK_AttributeType_Sort);
			m_pOrbiter->m_mapPK_MediaType_PK_Attribute_Sort[mediaFileBrowserOptions.m_PK_MediaType]=mediaFileBrowserOptions.m_PK_AttributeType_Sort;
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_MediaSource_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_Media_Filter_Source_CONST,mediaFileBrowserOptions.m_sSources,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_PK_MediaSubType_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,ARRAY_Media_Filter_Subtype_CONST,mediaFileBrowserOptions.m_sPK_MediaSubType,true);
			bChangedSortFilter=true;
			break;
		};
	}
	if( bChangedSortFilter )
	{
		mediaFileBrowserOptions.ReacquireGrids();
		SetMediaSortFilterSelectedObjects();
		m_pOrbiter->Renderer()->RenderObjectAsync(pObjectInfoData->m_pObj);  // We will be changing the selected state
		m_pOrbiter->CMD_Refresh("*");
		return true;  // We don't want the framework to mark the object as selected
	}

	if(m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN)
	{
		bool bChangedSortFilter = false;

		if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSortByAlbum_CONST)
		{
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Album_CONST;
			bChangedSortFilter = true;
		}
		else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSortByPerformer_CONST)
		{
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Performer_CONST;
			bChangedSortFilter = true;
		}
		else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSortByComposer_CONST)
		{
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_ComposerWriter_CONST;
			bChangedSortFilter = true;
		}
		else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == 5564)
		{
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Genre_CONST;
			bChangedSortFilter = true;
		}

		if(bChangedSortFilter)
		{
			mediaFileBrowserOptions.Clear_m_listMediaFileAttributeDrillDown();
			mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.clear();
			AudioServer_PopulateDatagrid();
			mediaFileBrowserOptions.ReacquireGrids();
			SetMediaSortFilterSelectedObjects();
			m_pOrbiter->Renderer()->RenderObjectAsync(pObjectInfoData->m_pObj);  // We will be changing the selected state
			m_pOrbiter->CMD_Refresh("*");
			return true;  // We don't want the framework to mark the object as selected
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
string ScreenHandler::GetFileBrowserPopup(DesignObj_Orbiter *pObj_MenuPad)
{
	switch( pObj_MenuPad->m_iBaseObjectID )
	{
	case DESIGNOBJ_butFBSF_Show_MediaType_CONST: // what
		return TOSTRING(DESIGNOBJ_popFBSF_MediaType_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaPrivate_CONST:  // private
		return TOSTRING(DESIGNOBJ_popFBSF_PrivateMedia_CONST);
	case DESIGNOBJ_butFBSF_Show_MediaRating_CONST:  // rating
		return TOSTRING(DESIGNOBJ_popFBSF_RatingsByUser_CONST);
	case DESIGNOBJ_butFBSF_Show_Sort_CONST: // sort
		return TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaGenre_CONST: // gemre
		return TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSubType_CONST: // sub type
		return TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaFormat_CONST: // format
		return TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_MediaSource_CONST: // source
		return TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0";
	case DESIGNOBJ_butFBSF_Show_Media_Go_CONST: // go
		return TOSTRING(DESIGNOBJ_popFBSF_Go_CONST);
	}

	return "";
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowser_DatagridSelected sel value %s text %s row %d col %d cell value %s",
			pCellInfoData->m_sValue.c_str(),pCellInfoData->m_sText.c_str(),
			pCellInfoData->m_Row,pCellInfoData->m_Column,
			pCellInfoData->m_pDataGridCell && pCellInfoData->m_pDataGridCell->m_Value ? pCellInfoData->m_pDataGridCell->m_Value : "*NONE*");

#endif

	if(	pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgFileList2_CONST || pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgFileList2_Pics_CONST || pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgFileList_SmallUI_CONST )
	{
		DataGridCell *pCell_Pic=NULL,*pCell_List=NULL;
		if( pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgFileList2_CONST || pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgFileList_SmallUI_CONST )
		{
			pCell_List=pCellInfoData->m_pDataGridCell;
			if( mediaFileBrowserOptions.m_pObj_PicGrid && mediaFileBrowserOptions.m_pObj_ListGrid )
			{
				DataGridTable *pDataGridTable = mediaFileBrowserOptions.m_pObj_PicGrid->DataGridTable_Get();
				if( pDataGridTable )
				{
					int Row = pCellInfoData->m_Row / mediaFileBrowserOptions.m_pObj_PicGrid->m_MaxCol;
					pCell_Pic = pDataGridTable->GetData( pCellInfoData->m_Row % mediaFileBrowserOptions.m_pObj_PicGrid->m_MaxCol, Row );
				}
			}
		}
		else
		{
			pCell_Pic=pCellInfoData->m_pDataGridCell;
			if( mediaFileBrowserOptions.m_pObj_PicGrid && mediaFileBrowserOptions.m_pObj_ListGrid )
			{
				DataGridTable *pDataGridTable = mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get();
				if( pDataGridTable )
				{
					int Row = pCellInfoData->m_Row * mediaFileBrowserOptions.m_pObj_PicGrid->m_MaxCol + pCellInfoData->m_Column;
					pCell_List = pDataGridTable->GetData( 0, Row );
				}
			}
		}

		if(m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN)
		{
			pCell_List = pCell_Pic;
		}

#ifdef DEBUG
		if(NULL != mediaFileBrowserOptions.m_pObj_ListGrid)
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowser_ObjectSelected sel grid pic %p list %p pich %d,%d  listh %d,%d",
				pCell_Pic,pCell_List,mediaFileBrowserOptions.m_pObj_PicGrid ? mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedColumn : -999,
				mediaFileBrowserOptions.m_pObj_PicGrid ? mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedRow : -999,
				0,mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow);
#endif

		if( !pCell_List )
			return true; // Shouldn't happen

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowser_ObjectSelected sel grid value %s text %s",
			pCell_List->m_Value ? pCell_List->m_Value : "*NULL*",pCell_List->m_Text ? pCell_List->m_Text : "*NULL*");
#endif

		if( !pCell_List->m_Value || strstr(pCell_List->m_Value,"\t!D")!=NULL )  // If it contains this, the user is going into a subdirectory
		{
			// It's a sub directory.  Update the source and refresh the page
			mediaFileBrowserOptions.m_sSources = pCell_List->m_Value ? pCell_List->m_Value : "";
			mediaFileBrowserOptions.ReacquireGrids();
			MediaBrowser_Render(NULL);
			m_pOrbiter->CMD_Refresh("*");
			return true;
		}

		m_pOrbiter->CMD_Remove_Popup("","coverart");

		char cActionItem = pCell_List->m_Value[0]=='!' ? pCell_List->m_Value[1] : 0;

		if( cActionItem=='e' )
		{
			SelectedExternalDirectory(pCell_List);
			return true;
		}
		
		// Previously, this was getting called each and every time a cell was being selected, causing severe slow-down on large
		// data grids! This should only be called for !Fxxxx items! -tschak
		if (cActionItem!='A') // Actually, this should only be called for everything NOT an attribute. -tschak
			GetAttributesForMediaFile(pCell_List->m_Value);

		if( cActionItem=='A' )
		{
			SelectedAttributeCell(pCell_List);
			return true;
		}

		if( mediaFileBrowserOptions.m_pObj_ListGrid )
			mediaFileBrowserOptions.m_iLastRowBeforeMediaFileSelect_ListGrid = mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow;
		SelectedMediaFile(pCell_List->m_Value,cActionItem);

		return true;  // Otherwise there may be a crash in selected grid since we may have already removed the grid and the cells
	}
	else if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Media_Attributes_For_File_CONST )
	{
		SelectedAttributeCell(pCellInfoData->m_pDataGridCell);
		m_pOrbiter->CMD_Go_back("","");
		return true;
	}
	else if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Media_Search_Auto_Compl_CONST )
	{
		if( StringUtils::StartsWith(pCellInfoData->m_sValue,"!F") )
		{
			GetAttributesForMediaFile(pCellInfoData->m_sValue.c_str());
			SelectedMediaFile(pCellInfoData->m_sValue,0);
			return true;
		}
		else if( StringUtils::StartsWith(pCellInfoData->m_sValue,"!A") )
			pCellInfoData->m_sValue = pCellInfoData->m_sValue.substr(2);

		MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = new MediaFileAttributeDrillDown(atoi(pCellInfoData->m_sValue.c_str()),pCellInfoData->m_sText,0);
		mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.push_front(mediaFileBrowserOptions.m_PK_AttributeType_Sort);
		mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.push_front( pMediaFileAttributeDrillDown );
		// Reset the sort type back to the title when the user is doing a keyword search
		mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Title_CONST;
		mediaFileBrowserOptions.ReacquireGrids();
		m_pOrbiter->CMD_Go_back("","");
		return true;
	}
	return true;  // Always return true since we're handing everything datagrid related here and may destroy the grids causing SelectedGrid to crash if we return false
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SelectedMediaFile(string sFile,char cActionItem)
{
	DesignObj_Orbiter *pObj_Play = NULL,*pObj_Close = NULL, *pObj_Delete = NULL, *pObj_Move = NULL, *pObj_Download = NULL;
	string sTerms = m_mapKeywords_Find("TERMS");
	if( sTerms.empty() )
	{
		if( m_pOrbiter->m_pScreenHistory_Current && m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==DESIGNOBJ_mnuFilelist_Video_Music_Small_CONST )
		{
			pObj_Play = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Play_CONST) );
			pObj_Close = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Close_CONST) );
			pObj_Delete = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Delete_CONST) );
			pObj_Move = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Move_CONST) );
			pObj_Download = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuFileDetails_SmallUI_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Download_CONST) );
		}
		else
		{
			pObj_Play = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Play_CONST) );
			pObj_Close = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Close_CONST) );
			pObj_Delete = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Delete_CONST) );
			pObj_Move = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Move_CONST) );
			pObj_Download = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Download_CONST) );
		}
	}
	else
		pObj_Play = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFilePurchaseDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Purchase_CONST) );

	if( !pObj_Play || !pObj_Play->m_pParentObject )
		return; // Shouldn't happen

	mediaFileBrowserOptions.m_sSelectedFile = sFile;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,mediaFileBrowserOptions.m_sSelectedFile);
	NeedToRender render2( m_pOrbiter, "ScreenHandler::MediaBrowser_DatagridSelected" );
	m_pOrbiter->CMD_Goto_DesignObj(0,pObj_Play->m_pParentObject->m_ObjectID,"","",false,true);

	string sPicture = m_mapKeywords_Find("PICTURE");
	pObj_Play->m_bHidden = cActionItem=='o';  // No play if it's downloadable

	// If this is external content there's no delete or move, and download is only for downloadable
        if( pObj_Move )
                pObj_Move->m_bHidden = cActionItem=='o' || cActionItem=='E' ||
                        mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_lmce_StreamedAudio_CONST ||
                        mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_lmce_StreamedVideo_CONST;
        if( pObj_Delete )
                pObj_Delete->m_bHidden = cActionItem=='o' || cActionItem=='E';
	if( pObj_Download )
		pObj_Download->m_bHidden = cActionItem!='o';

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SelectedMediaFile file %s pic %s", sFile.c_str(), sPicture.c_str());
	if( sPicture.empty()==false )
	{
		size_t size;
		char *pGraphicData = m_pOrbiter->ReadFileIntoBuffer(sPicture,size);
		m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",
			pGraphicData,
			(int) size,"0");

		delete [] pGraphicData;
		pGraphicData = NULL;
	}
	else
		m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",
			NULL,
			0,"0");

	DesignObj_Orbiter *pParentObject = dynamic_cast<DesignObj_Orbiter *>(pObj_Play->m_pParentObject);
	if(NULL != pParentObject)
		m_pOrbiter->Renderer()->RenderObjectAsync(pParentObject);

	m_pOrbiter->m_pObj_Highlighted_set(pObj_Play);
#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
		m_pOrbiter->m_pMouseBehavior->SetMousePosition(pObj_Play);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileList_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the 'virus free' check and hide it if it's virus free,
	// and also find the child object for the icon and assign it the picture associated with the cell.
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid )
	{
		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
			colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;

			int PK_FileFormat = atoi(pCell->m_mapAttributes_Find("PK_FileFormat").c_str());
			char cMediaSource = pCell->m_mapAttributes_Find("Source").c_str()[0];

			// See if there is an object assigned for this column/row
			map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
			if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
			{
				DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
				DesignObj_DataList::iterator iHao;

				// Iterate through all the object's children
				for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_fbico_HighDef_CONST )
						pDesignObj_Orbiter->m_bHidden = (PK_FileFormat!=FILEFORMAT_HD_720_CONST && PK_FileFormat!=FILEFORMAT_HD_1080_CONST && PK_FileFormat!=FILEFORMAT_Highdef_audio_CONST);
					if( pDesignObj_Orbiter->m_iBaseObjectID==FILEFORMAT_Standard_Def_CONST )
						pDesignObj_Orbiter->m_bHidden = (PK_FileFormat!=FILEFORMAT_DVD_CONST || PK_FileFormat!=FILEFORMAT_MP3_CONST || PK_FileFormat!=FILEFORMAT_CD_Quality_CONST);
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_fbico_LocalFile_CONST )
						pDesignObj_Orbiter->m_bHidden = cMediaSource!='F';
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_fbico_Download_CONST )
						pDesignObj_Orbiter->m_bHidden = cMediaSource!='L';
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_fbico_Disc_CONST )
						pDesignObj_Orbiter->m_bHidden = cMediaSource!='D';
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SelectedExternalDirectory(DataGridCell *pCell)
{
	mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.push_front(mediaFileBrowserOptions.m_PK_AttributeType_Sort);
	string sName = pCell->m_mapAttributes["Name"];
	if( sName.empty() && pCell->m_Text )
		sName = pCell->m_Text;

	MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = new MediaFileAttributeDrillDown(
		pCell->m_Value,sName,
		mediaFileBrowserOptions.m_pObj_ListGrid ? mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow : 0);
	mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.push_front(pMediaFileAttributeDrillDown );

	mediaFileBrowserOptions.ReacquireGrids();
	MediaBrowser_Render(NULL);
	m_pOrbiter->CMD_Refresh("*");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SelectedAttributeCell(DataGridCell *pCell)
{
	mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.push_front(mediaFileBrowserOptions.m_PK_AttributeType_Sort);
	string sName = pCell->m_mapAttributes["Name"];
	if( sName.empty() && pCell->m_Text )
		sName = pCell->m_Text;

	MediaFileAttributeDrillDown *pMediaFileAttributeDrillDown = new MediaFileAttributeDrillDown(
		atoi(&pCell->m_Value[2]),sName,
		mediaFileBrowserOptions.m_pObj_ListGrid ? mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow : 0);
	mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.push_front(pMediaFileAttributeDrillDown );

	// If we're browing a collection of attributes, say actors, when we choose
	// one we no longer want to browsing (or sorting by) that same attribute (actors)
	// we now want to browse by the titles with that attribute.  So we will switch
	// the sort.  Normally it goes to 'Title'.  However for Artists it goes to Albums
	// and for media without titles, like pictures, it goes to filename (which is 0)
	if( (mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_pluto_CD_CONST || mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST)
		&& (mediaFileBrowserOptions.m_PK_AttributeType_Sort==ATTRIBUTETYPE_Performer_CONST || mediaFileBrowserOptions.m_PK_AttributeType_Sort==ATTRIBUTETYPE_Album_Artist_CONST) )
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Album_CONST;
        else if (mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_lmce_StreamedAudio_CONST &&
                 mediaFileBrowserOptions.m_PK_AttributeType_Sort==ATTRIBUTETYPE_Genre_CONST)
	{
		mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Channel_CONST;
	}
	else
	{
		switch( mediaFileBrowserOptions.m_PK_AttributeType_Sort )
		{
		case ATTRIBUTETYPE_Director_CONST:
		case ATTRIBUTETYPE_Performer_CONST:
		case ATTRIBUTETYPE_Album_CONST:
		case ATTRIBUTETYPE_Title_CONST:
		case ATTRIBUTETYPE_System_CONST:
		case ATTRIBUTETYPE_Genre_CONST:
		case ATTRIBUTETYPE_Year_CONST:
		case ATTRIBUTETYPE_Manufacturer_CONST:
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Title_CONST;
			break;

		default:
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = 0;  // Show files
			break;
		}

	}

	AudioServer_PopulateDatagrid();
	mediaFileBrowserOptions.ReacquireGrids();
	MediaBrowser_Render(NULL);
	m_pOrbiter->CMD_Refresh("*");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GetAttributesForMediaFile(const char *pFilename)
{
	m_mapKeywords.clear();
	string sResult;
	DCE::CMD_Get_Attributes_For_Media CMD_Get_Attributes_For_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
		pFilename,"",&sResult);
	if( m_pOrbiter->SendCommand(CMD_Get_Attributes_For_Media) )
	{
		string::size_type pos=0;
		while(pos<sResult.size())
		{
			string s = StringUtils::Tokenize(sResult,"\t",pos);
			m_mapKeywords[s] = StringUtils::Tokenize(sResult,"\t",pos);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaFileBrowserOptions::SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,string &sValues)
{
	if( !pObj )
		return; // Shouldn't happen

	for( DesignObj_DataList::iterator iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;

		pDesignObj_Orbiter->m_bDontResetState=true;
		string sArrayValue = pDesignObj_Orbiter->GetVariableAssignment(VARIABLE_Array_ID_CONST);
		string sPK_Array = pDesignObj_Orbiter->GetVariableAssignment(VARIABLE_PK_Array_CONST);
		if( PK_Array!=atoi(sPK_Array.c_str()) )
			continue; // It's not this array
#ifdef WIN32
		m_mapObjectsValues[make_pair (pDesignObj_Orbiter->m_iBaseObjectID, sArrayValue)] = pDesignObj_Orbiter;
#else
		m_mapObjectsValues[ make_pair<int,string> (pDesignObj_Orbiter->m_iBaseObjectID,sArrayValue) ] = pDesignObj_Orbiter;
#endif
		if( sArrayValue.size() && sArrayValue==sValues || StringUtils::StartsWith(sValues,sArrayValue + ",") )  // If it's the only one, or the first one
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh0",GRAPHIC_SELECTED,false,true);
		else if( sArrayValue.size()==0 || atoi(sArrayValue.c_str())==0 )
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh1",sValues.size()==0 ? GRAPHIC_SELECTED : GRAPHIC_NORMAL,false,true);
		else if( ("," + sValues + ",").find( "," + sArrayValue + "," )!=string::npos )
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh2",GRAPHIC_SELECTED,false,true);
		else
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh3",GRAPHIC_NORMAL,false,true);
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaFileBrowserOptions::SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,int &iValue)
{
	if( !pObj )
		return; // Shouldn't happen

	for( DesignObj_DataList::iterator iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		pDesignObj_Orbiter->m_bDontResetState=true;
		string sArrayValue = pDesignObj_Orbiter->GetVariableAssignment(VARIABLE_Array_ID_CONST);
		string sPK_Array = pDesignObj_Orbiter->GetVariableAssignment(VARIABLE_PK_Array_CONST);
		if( PK_Array!=atoi(sPK_Array.c_str()) )
			continue; // It's not this array
#ifdef WIN32
		m_mapObjectsValues[make_pair (pDesignObj_Orbiter->m_iBaseObjectID, sArrayValue)] = pDesignObj_Orbiter;
#else
		m_mapObjectsValues[ make_pair<int,string> (pDesignObj_Orbiter->m_iBaseObjectID,sArrayValue) ] = pDesignObj_Orbiter;
#endif // WIN32
if( atoi(sArrayValue.c_str())==iValue )
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh4",GRAPHIC_SELECTED,false,true);
		else
			pDesignObj_Orbiter->m_GraphicToDisplay_set("sh5",GRAPHIC_NORMAL,false,true);
	}
}

void MediaFileBrowserOptions::SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,string &sValues,bool bTreatZeroAsAll)
{
	if( !pObj )
		return; // Shouldn't happen
	string sPK_Array = pObj->GetVariableAssignment(VARIABLE_PK_Array_CONST);
	if( PK_Array!=atoi(sPK_Array.c_str()) )
		return; // It's not this array
	string sArrayValue = pObj->GetVariableAssignment(VARIABLE_Array_ID_CONST);
	string::size_type pos=0,prior=0;
	if( sArrayValue!="0" || !bTreatZeroAsAll )  // Don't bother if the user selected the 'all'
	{
		while(pos<sValues.size())
		{
			string sToken = StringUtils::Tokenize(sValues,",",pos);
			if( sToken==sArrayValue )
			{
				string sNewValue;
				if( prior==0 && pos>=sValues.size() )
					sNewValue = "";
				else if( prior==0 )
					sNewValue = sValues.substr(pos);
				else if( pos>=sValues.size() )
					sNewValue = sValues.substr(0,prior-1);
				else
					sNewValue = sValues.substr(0,prior) + sValues.substr(pos);
				sValues = sNewValue;
				return;
			}
			prior=pos;
		}
	}

	if( bTreatZeroAsAll )
	{
		// Clear the 'all' unless the user selected all, in which case clear everything else
#ifdef WIN32
		map< pair<int,string>, DesignObj_Orbiter * >::iterator it = m_mapObjectsValues.find( make_pair ( pObj->m_iBaseObjectID, "0" ) );
#else
		map< pair<int,string>, DesignObj_Orbiter * >::iterator it = m_mapObjectsValues.find( make_pair<int,string> ( pObj->m_iBaseObjectID, "0" ) );
#endif
		if( it != m_mapObjectsValues.end() )
		{
			DesignObj_Orbiter *pObj_All = it->second;
			if( pObj_All==pObj )  // user selected all, clear everything else
			{
				pObj->m_GraphicToDisplay_set("sh6",GRAPHIC_SELECTED,false,true);
				for( DesignObj_DataList::iterator iHao=pObj->m_pParentObject->m_ChildObjects.begin(  ); iHao != pObj->m_pParentObject->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pObj_Child=( DesignObj_Orbiter * )*iHao;
					if( pObj_Child!=pObj )
					{
						pObj_Child->m_GraphicToDisplay_set("sh7",GRAPHIC_NORMAL,false,true);
						m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Child);
					}
				}
				sValues="";
				return;
			}
			else  // User selected something besides 'all'
			{
				pObj_All->m_GraphicToDisplay_set("sh8",GRAPHIC_NORMAL,false,true);
				m_pOrbiter->Renderer()->RenderObjectAsync(pObj_All);
			}
		}
	}

	pObj->m_GraphicToDisplay_set("sh9",GRAPHIC_SELECTED,false,true);

	if( sValues.size() )
		sValues += ",";
	sValues += sArrayValue;
//int k=2;
}

void MediaFileBrowserOptions::SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,int &iValue)
{
	if( !pObj )
		return; // Shouldn't happen

	string sPK_Array = pObj->GetVariableAssignment(VARIABLE_PK_Array_CONST);
	if( PK_Array!=atoi(sPK_Array.c_str()) )
		return; // It's not this array
	string sValue = pObj->GetVariableAssignment(VARIABLE_Array_ID_CONST);
	int iValue_New = atoi(sValue.c_str());
	if( iValue==iValue_New )
		return; // User just chose the same thing

	// Find the old one if it's changed
#ifdef WIN32
	map< pair<int, string>, DesignObj_Orbiter * >::iterator it = m_mapObjectsValues.find(make_pair (pObj->m_iBaseObjectID, StringUtils::itos(iValue)));
#else
	map< pair<int,string>, DesignObj_Orbiter * >::iterator it= m_mapObjectsValues.find( make_pair<int,string> (pObj->m_iBaseObjectID,StringUtils::itos(iValue)) );
#endif // WIN32
	if( it!=m_mapObjectsValues.end() )
	{
		DesignObj_Orbiter *pObj_PriorSelected = it->second;
		pObj_PriorSelected->m_GraphicToDisplay_set("mfb1",GRAPHIC_NORMAL,false,true);
		m_pOrbiter->Renderer()->RenderObjectAsync(pObj_PriorSelected);  // We will be changing the selected state
	}

	pObj->m_GraphicToDisplay_set("mfb2",GRAPHIC_SELECTED,false,true);
	iValue = iValue_New;
}

//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription, int iPK_PnpQueue)
{
	m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sDescription);
	ScreenHandlerBase::SCREEN_NewPhoneDetected(PK_Screen, sMac_address, sDescription, iPK_PnpQueue);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_WhatModelMobileOrbiter(long PK_Screen, int iPK_Users, string sMac_address)
{
	m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(iPK_Users));
	ScreenHandlerBase::SCREEN_WhatModelMobileOrbiter(PK_Screen, iPK_Users, sMac_address);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Pick_Room_For_Device(long PK_Screen, int iPK_Device,
												   string sDescription, string sComments)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sComments);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, StringUtils::ltos(iPK_Device));
	GotoScreen(SCREEN_Pick_Room_For_Device_CONST,StringUtils::ltos(iPK_Device),true,false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_SensorsNotReady(long PK_Screen, string sDescription)
{
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sDescription,
		TEXT_TRIPPED_SENSOR_CONST);
	ScreenHandlerBase::SCREEN_SensorsNotReady(PK_Screen, sDescription);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime,
						   string sExitDelay, string sAlerts)
{
	string sPK_DesignObj = StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen));

	m_pOrbiter->CMD_Set_Graphic_To_Display(sPK_DesignObj + ".0.0." +
		StringUtils::itos(DESIGNOBJ_icoHouseStatusIndicator_CONST), sPK_HouseMode);
	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sHouseModeTime, TEXT_House_Mode_Time_CONST);


	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sAlerts, TEXT_Alerts_Placeholder_CONST);
	ScreenHandlerBase::SCREEN_ModeChanged(PK_Screen, sPK_HouseMode, sHouseModeTime, sExitDelay, sAlerts);
	if(sExitDelay != "")
		m_pOrbiter->CMD_Select_Object(sPK_DesignObj, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), sExitDelay);  // Do this last since the above resets the selected object
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CurrentlyActiveRemote(long PK_Screen)
{
	if( !m_pOrbiter->UsesUIVersion2() &&  // TODO -- Is this right?????  When we say go to remote, we always want the chapters, right??
		m_pOrbiter->m_bIsOSD && m_pOrbiter->m_iPK_Screen_RemoteOSD && m_pOrbiter->m_iLocation_Initial==m_pOrbiter->m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SCREEN_CurrentlyActiveRemote osd ui2 going to %d",m_pOrbiter->m_iPK_Screen_RemoteOSD);
		m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_RemoteOSD);
	}
	else if( m_pOrbiter->m_iPK_Screen_Remote )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SCREEN_CurrentlyActiveRemote going to %d",m_pOrbiter->m_iPK_Screen_Remote);
		m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_Remote);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::SCREEN_CurrentlyActiveRemote no remote");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Computing(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	ScreenHandlerBase::SCREEN_Computing(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_DatagridSelected, new DatagridCellBackData());

	string sActiveApplication_Description;
	if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
	{
		int PK_Screen_ActiveApp_OSD,PK_Screen_ActiveApp_Remote;

		DCE::CMD_Get_Active_Application CMD_Get_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
			&m_sActiveApplication_Description,&PK_Screen_ActiveApp_OSD,&m_sActiveApplication_Window,&PK_Screen_ActiveApp_Remote);
		m_pOrbiter->SendCommand(CMD_Get_Active_Application);
		m_PK_Screen_ActiveApp_OSD = PK_Screen_ActiveApp_OSD;
		m_PK_Screen_ActiveApp_Remote = PK_Screen_ActiveApp_Remote;
	}
	else
	{
		m_sActiveApplication_Description = m_pOrbiter->m_sActiveApplication_Description;
		m_sActiveApplication_Window = m_pOrbiter->m_sActiveApplication_Window;
		m_PK_Screen_ActiveApp_OSD = m_pOrbiter->m_PK_Screen_ActiveApp_OSD;
		m_PK_Screen_ActiveApp_Remote = m_pOrbiter->m_PK_Screen_ActiveApp_Remote;
	}

	string sOnOff = m_sActiveApplication_Description.size() ? "1" : "0";
	m_pOrbiter->CMD_Show_Object( NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID + "." TOSTRING(DESIGNOBJ_butResumeControl_CONST),0,"","",sOnOff);
	m_pOrbiter->CMD_Show_Object(NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID + "." TOSTRING(DESIGNOBJ_objExitAppOnOSD_CONST),0,"","",sOnOff);

	m_pOrbiter->CMD_Set_Text(NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID,m_sActiveApplication_Description,TEXT_STATUS_CONST);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_GenericAppController(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_GenericAppController(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_GenericAppFullScreen(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_GenericAppFullScreen(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Power(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Power(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Power_ObjectSelected, new ObjectInfoBackData());
	int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
	string sPK_DesignObj = StringUtils::itos(PK_DesignObj) + ".0.0.";
	
	long dwPK_Device_MD;
	if( m_pOrbiter->m_bIsOSD )
	  {
	    if ( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
	      {
		// Are we in a different room? if so, grab the device from Location
		dwPK_Device_MD = m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector;
	      }
	    else
	      {
		dwPK_Device_MD = m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia;
	      }
	  }
	else if( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
	  dwPK_Device_MD=m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector;
	
	// At this point, we have the target MD to get device data from.
	string sOperating_System = m_pOrbiter->m_pEvent->GetDeviceDataFromDatabase(dwPK_Device_MD, DEVICEDATA_Operating_System_CONST);

	if( !m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector || m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector==DEVICEID_NULL )
	{
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayON_OtherControlling_CONST), 0, "", "",  "0" );
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
		m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
	}
	else
	{
		// We know we've got an m/d and it's not us.  Find out the state and status
		string sState = m_pOrbiter->GetState(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector);
		string sStatus = m_pOrbiter->GetStatus(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector);

		if( sStatus.length()>1 && sStatus.substr(0,2)=="MD" )
		{
			if( sOperating_System.empty() )
			{
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
			}
			else
			{
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "1" );
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
			}
			// See if we've got media playing
			if( m_pOrbiter->m_sNowPlaying.length() )
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "1" );
			else
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
		}
		else
		{
			if( sOperating_System.empty() )
			{
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "0" );
			}
			else
			{
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootHD_CONST), 0, "", "",  "0" );
				m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_mnuBootNet_CONST), 0, "", "",  "1" );
			}
			m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_CurrentMedia_CONST), 0, "", "",  "0" );
		}

		// See if it's on.  If so, enable the halt and displayoff buttons
		if( sStatus.length()>3 && (sStatus.substr(3) == "ON" || sStatus.substr(3) == "BLACK") )
		{
			m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "1" );
			m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "1" );
		}
		else
		{
			m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_halt_CONST), 0, "", "",  "0" );
			m_pOrbiter->CMD_Show_Object( sPK_DesignObj + StringUtils::itos(DESIGNOBJ_DisplayOFF_Display_CONST), 0, "", "",  "0" );
		}
	}
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Power_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	// If we're an OSD and controlling ourselves, this should send us to the screen saver
	if( m_pOrbiter->m_bIsOSD && m_pOrbiter->m_iLocation_Initial==m_pOrbiter->m_pLocationInfo->iLocation &&
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_DisplayOFF_Display_CONST )
	{
		m_pOrbiter->StartScreenSaver(true);
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Computing_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_objExitAppOnOSD_CONST )
	{
		DCE::CMD_Kill_Application CMD_Kill_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_AppServer,"generic_app",true);
		m_pOrbiter->SendCommand(CMD_Kill_Application);

		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
		{
			DCE::CMD_Set_Active_Application CMD_Set_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				"",0,"",0);
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,"",SCREEN_Main_CONST,interuptAlways,false,false);
			CMD_Set_Active_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
			m_pOrbiter->SendCommand(CMD_Set_Active_Application);
		}
		else
		{
			m_pOrbiter->m_sActiveApplication_Description = "";
			m_pOrbiter->m_sActiveApplication_Window = "";
			m_PK_Screen_ActiveApp_OSD=m_PK_Screen_ActiveApp_Remote=0;
		}
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResumeControl_CONST )
	{
		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )
		{
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				"",m_PK_Screen_ActiveApp_OSD,interuptAlways,false,false);
			m_pOrbiter->SendCommand(CMD_Goto_Screen);
			m_pOrbiter->CMD_Goto_Screen("",m_PK_Screen_ActiveApp_Remote);

			if(m_sActiveApplication_Window != "")
			{
				DCE::CMD_Activate_Window cmd_Activate_Window(
					m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
					m_sActiveApplication_Window);
				m_pOrbiter->SendCommand(cmd_Activate_Window);
			}
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",m_PK_Screen_ActiveApp_OSD);
			m_pOrbiter->CMD_Activate_Window(m_sActiveApplication_Window);
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Computing_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_sValue.size() && m_pOrbiter->m_pLocationInfo )
	{
		string::size_type pos=0;
		string sActiveApplication_Window = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		int PK_Screen_Remote = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos).c_str());
		int PK_Screen_OSD = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos).c_str());
		if( !PK_Screen_Remote )
			PK_Screen_Remote = SCREEN_GenericAppController_CONST;
		if( !PK_Screen_OSD )
			PK_Screen_OSD=SCREEN_GenericAppFullScreen_CONST;
		string sActiveApplication_Description = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		string sBinary = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		string sArguments = pos<pCellInfoData->m_sValue.size() ? pCellInfoData->m_sValue.substr(pos) : "";

		string sMessageOnComplete = "0 " + StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter) + " 1 " + TOSTRING(COMMAND_Set_Active_Application_CONST) +
			" " + TOSTRING(COMMANDPARAMETER_Name_CONST) + " \"\" " + TOSTRING(COMMANDPARAMETER_Identifier_CONST) + " \"\" " +
			TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) + " 0 " + TOSTRING(COMMANDPARAMETER_PK_DesignObj_CurrentScreen_CONST) + " 0"
			" & 0 " + StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter) + 
			" 1 8 159 " + StringUtils::itos(PK_Screen_OSD);

		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
			sMessageOnComplete += " & 0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " 1 " + TOSTRING(COMMAND_Set_Active_Application_CONST) +
			" " + TOSTRING(COMMANDPARAMETER_Name_CONST) + " \"\" " + TOSTRING(COMMANDPARAMETER_Identifier_CONST) + " \"\" " +
			TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) + " 0 " + TOSTRING(COMMANDPARAMETER_PK_DesignObj_CurrentScreen_CONST) + " 0"
			" & 0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + 
			" 1 8 159 " + StringUtils::itos(PK_Screen_Remote);

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_AppServer,
			sBinary,"generic_app",sArguments,sMessageOnComplete,sMessageOnComplete,false,false,true,false);
		
		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
		{
			//DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,0,
			//	StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
			//CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

			DCE::SCREEN_GenericAppFullScreen screen_GenericAppFullScreen(m_pOrbiter->m_dwPK_Device,
				m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(screen_GenericAppFullScreen.m_pMessage);

			DCE::CMD_Set_Active_Application CMD_Set_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				sActiveApplication_Description,PK_Screen_OSD,sActiveApplication_Window,PK_Screen_Remote);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Active_Application.m_pMessage);

			m_pOrbiter->CMD_Goto_Screen("",PK_Screen_Remote);

			DCE::CMD_Activate_Window cmd_Activate_Window(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				sActiveApplication_Window);
			m_pOrbiter->SendCommand(cmd_Activate_Window);
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",PK_Screen_OSD);
			m_pOrbiter->CMD_Set_Active_Application(sActiveApplication_Description,PK_Screen_OSD,sActiveApplication_Window,PK_Screen_Remote);
		}
		m_pOrbiter->SendCommand(CMD_Spawn_Application);
		return true; // We did a goto screen
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>" + sErrors);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_DeviceFrom, string sPK_RippingDevice)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_ripping_cant_play_disc_CONST) + "%>",
		false, "10", true,
		//"Monitor progress text"
		"<%=T" + StringUtils::itos(TEXT_Monitor_progress_CONST) + "%>",
		//"Monitor progress message"
		"0 " + StringUtils::itos(DEVICETEMPLATE_This_Orbiter_CONST) +
		" 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::itos(SCREEN_PendingTasks_CONST)
	);
	return;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCheckingDrive(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Checking_drive_CONST) + "%>", false, "20",true,
		"Eject","0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_MediaPlugIn) + " 1 " + StringUtils::itos(COMMAND_Eject_Disk_CONST));
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogUnableToSavePlaylist(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "Unable to save playlist", false, "10", true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogPlaylistSaved(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Playlist_Saved_CONST) + "%>");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogUnableToLoadPlaylist(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "Unable to load playlist", false, "10", true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, sTimeout, false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Cannot_Reload_Router(long PK_Screen, string sDescription)
{
	DisplayMessageOnOrbiter(PK_Screen, m_pOrbiter->m_mapTextString[TEXT_Cannot_Reload_CONST] + "\n" + sDescription, false, "0", true, 
		//OK
		m_pOrbiter->m_mapTextString[TEXT_Ok_CONST],"",
		//Force text
		m_pOrbiter->m_mapTextString[TEXT_Force_Reload_CONST],
		//the command
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " TOSTRING(DEVICEID_DCEROUTER) 
		" 7 6"
	);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingInstructions(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen,
		"<%=T" + StringUtils::itos(TEXT_Ripping_Instructions_CONST) + "%>",
        false, "40", true,
		//Monitor progress text
		"<%=T" + StringUtils::itos(TEXT_Monitor_progress_CONST) + "%>",
		//the command
		"0 " + StringUtils::itos(DEVICETEMPLATE_This_Orbiter_CONST) +
		" 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " +
		StringUtils::itos(SCREEN_PendingTasks_CONST)
	);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter,
							   string sTimeout, string sCannotGoBack)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, sPromptToResetRouter == "1", sTimeout, sCannotGoBack == "1");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Cannot_bookmark_CONST) +
		"%> " + sErrors);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogAskToResume(long PK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, 
					string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo)
{
	string sMessageToResume = sPK_Device_From + " " + sPK_Device_MediaSource
		+ " 1 " + StringUtils::itos(COMMAND_Set_Media_Position_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_StreamID_CONST) + " " + sStreamID_String + " "
		+ StringUtils::itos(COMMANDPARAMETER_MediaPosition_CONST) + " \"" + sPosition + "\"";

	string sMessageToGoToRemote = sPK_Device_From + " " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device)
		+ " 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::itos(iPK_Screen_GoTo);

	string sMessageToSetPreference = StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + sPK_Device_From
		+ " 1 " + StringUtils::itos(COMMAND_Set_Auto_Resume_Options_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_Users_CONST) + " " + sUsers + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_MediaType_CONST) + " " + sPK_MediaType_String + " "
		+ StringUtils::itos(COMMANDPARAMETER_Value_To_Assign_CONST);

	DisplayMessageOnOrbiter(PK_Screen,
		"<%=T" + StringUtils::itos(TEXT_Ask_to_resume_CONST) + "%>",
		false, "0", false,
		"<%=T" + StringUtils::itos(TEXT_YES_CONST) + "%>",sMessageToResume + "\n" + sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_NO_CONST) + "%>",sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_Always_Resume_CONST) + "%>",sMessageToResume + "\n" +
			sMessageToGoToRemote + "\n" + sMessageToSetPreference + " A",
		"<%=T" + StringUtils::itos(TEXT_Never_Resume_CONST) + "%>",sMessageToGoToRemote + "\n" +
			sMessageToSetPreference + " N");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, "0", sCannotGoBack == "1");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::DisplayMessageOnOrbiter(int PK_Screen,
	string sMessage, bool bPromptToResetRouter,string sTimeout, bool bCantGoBack,
	string sOption1, string sMessage1, string sOption2, string sMessage2,
	string sOption3, string sMessage3, string sOption4, string sMessage4,string sID)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::DisplayMessageOnOrbiter screen %d message %s option 1 %s / %s 2: %s / %s",
		PK_Screen,sMessage.c_str(),sOption1.c_str(),sMessage1.c_str(),sOption2.c_str(),sMessage2.c_str());

	NeedToRender render2( m_pOrbiter, "ScreenHandler::DisplayMessageOnOrbiter" );  // Redraw anything that was changed by this command
	int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
	if( !PK_DesignObj )
		PK_DesignObj = PK_Screen ? PK_Screen : DESIGNOBJ_mnuPopupMessage_CONST;  // Allow the user to pass in a design obj to use instead
	string sPK_DesignObj = StringUtils::ltos(PK_DesignObj);

	m_pOrbiter->CMD_Set_Timeout(sPK_DesignObj, sTimeout);

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Text_CONST, sMessage);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_1_CONST, sOption1);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_2_CONST, sOption2);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_3_CONST, sOption3);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_4_CONST, sOption4);

	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, sID, "", false, bCantGoBack );
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_objControllerBack_CONST), 0, "", "", bCantGoBack ? "0" : "1" );

	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, "<%=" + StringUtils::ltos(VARIABLE_Display_Message_Text_CONST) + "%>", TEXT_STATUS_CONST);

	if(bPromptToResetRouter)
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );

	if( sOption1.size()==0 && sOption2.size()==0 && sOption3.size()==0 && sOption4.size()==0 )
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, "0 -300 1 " TOSTRING(COMMAND_Go_back_CONST) " " TOSTRING(COMMANDPARAMETER_Force_CONST) " 1");
		return;
	}

	if(sOption1.size() )
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_1_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, sMessage1);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", sOption1.size() && sOption1!="NO_BUTTON" ? "1" : "0" );

	if(sOption2.size())
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_2_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_2_CONST, sMessage2);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 0, "", "", sOption2.size() ? "1" : "0" );

	if(sOption3.size())
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_3_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_3_CONST, sMessage3);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 0, "", "", sOption3.size() ? "1" : "0" );

	if(sOption4.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_4_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_4_CONST, sMessage4);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", sOption4.size() ? "1" : "0" );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPhoneName);
	DisplayMessageOnOrbiter(PK_Screen, sInstructions, true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine,
	string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer)
{
	string sYesMessage =
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + sPK_DeviceFrom  + " " +
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Send_File_To_Phone_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_Mac_address_CONST) + " \"" + sMacAddress + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Command_Line_CONST) + " \"" + sCommandLine + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_App_Server_Device_ID_CONST) + " " + sPK_Device_AppServer;

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPhoneName);
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_FAILED_TO_UPLOAD_SIS_FILE_CONST) + "%>",
		false, "0", false,
		//Yes button
		"Yes",
		//Yes command
		sYesMessage,
		//No button
		"No",
		//No command
		"" //do nothing
	);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Main(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, m_pOrbiter->m_sMainMenu, sLocation, "", false, false );

	Reset_SaveFile_Info();

	DesignObj_Orbiter *pObjMainMenu = m_pOrbiter->FindObject(m_pOrbiter->m_sMainMenu);
	if(NULL != pObjMainMenu && pObjMainMenu->m_iBaseObjectID == DESIGNOBJ_mnuMenuAudioServer_CONST)
		SetupAudioServer();
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SetupAudioServer()
{
	int iPK_MediaType = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_MediaType_CONST).c_str());

	if(iPK_MediaType == 0)
		iPK_MediaType = MEDIATYPE_pluto_StoredAudio_CONST;

	OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry =
		m_pOrbiter->m_pOrbiterFileBrowser_Collection->m_mapOrbiterFileBrowser[iPK_MediaType];

	pOrbiterFileBrowser_Entry->m_PK_Screen = SCREEN_Main_CONST;

	if( !pOrbiterFileBrowser_Entry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No way to show file list for %d",iPK_MediaType);
	}
	else
	{
		mediaFileBrowserOptions.ClearAll(iPK_MediaType,pOrbiterFileBrowser_Entry->m_PK_Screen,m_pOrbiter->m_mapPK_MediaType_PK_Attribute_Sort[iPK_MediaType]);
		mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Album_CONST;
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Filename_CONST, pOrbiterFileBrowser_Entry->m_sFilename);

		SetMediaSortFilterSelectedObjects();
		RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_ObjectSelected,	new ObjectInfoBackData());
		RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_Render, new RenderScreenCallBackData());
		RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_DatagridSelected, new DatagridCellBackData());
		RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::FileList_GridRendering,	new DatagridAcquiredBackData());
		RegisterCallBack(cbOnKeyDown, (ScreenHandlerCallBack) &ScreenHandler::FileList_KeyDown, new KeyCallBackData());
		RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowsre_Intercepted, new MsgInterceptorCellBackData());
		RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_OnTimer, new CallBackData());

		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_ViewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmv1",GRAPHIC_NORMAL,false,true);

		pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFBSF_More_CONST) "." + StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType) + ".0." TOSTRING(DESIGNOBJ_butFBSF_More_UnviewedOnly_CONST) );
		if( pObj )
			pObj->m_GraphicToDisplay_set("fmv2",GRAPHIC_NORMAL,false,true);

		m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." TOSTRING(DESIGNOBJ_butGotoRemote_CONST), 0, "", "", 
			m_pOrbiter->m_sNowPlaying.empty() ? "0" : "1"); 

		AudioServer_PopulateDatagrid();

		m_pOrbiter->StartScreenHandlerTimer(5000);
	}
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_OnTimer(CallBackData *pData)
{
	DesignObj_Orbiter *pClockObj = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." + StringUtils::itos(DESIGNOBJ_butCurrentTime_CONST));
	DesignObj_Orbiter *pRippingStatusObj = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." + StringUtils::itos(DESIGNOBJ_butRipStatus_CONST));
	if( NULL != pRippingStatusObj && NULL != pClockObj)
	{
		NeedToRender render(m_pOrbiter, "MediaBrowser_OnTimer");
		m_pOrbiter->Renderer()->RenderObjectAsync(pRippingStatusObj);
		m_pOrbiter->Renderer()->RenderObjectAsync(pClockObj);
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::AudioServer_PopulateDatagrid()
{
	DesignObj_DataGrid *pObj = (DesignObj_DataGrid *)m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgFileList2_Pics_CONST));

	if(m_pOrbiter->m_sSkin == AUDIO_STATION_SKIN && NULL != pObj)
	{
		bool bResponse;
		int iPK_Variable=0;
		string sValue_To_Assign;
		string sParams = m_pOrbiter->SubstituteVariables("<%=FBO%>", pObj, 0, 0 );
		string sGridID = m_pOrbiter->SubstituteVariables("MediaFile_<%=!%>", pObj, 0, 0 );

		int PK_DeviceTemplate = pObj->m_iPK_DeviceTemplate;
		DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid(
			m_pOrbiter->m_dwPK_Device,  m_pOrbiter->m_dwPK_Device_DatagridPlugIn,  
			StringUtils::itos(m_pOrbiter->m_dwIDataGridRequestCounter), 
			sGridID,
			DATAGRID_Media_Browser_CONST, sParams, PK_DeviceTemplate, 
			&iPK_Variable, &sValue_To_Assign, &bResponse, 
			&pObj->m_iPopulatedWidth, &pObj->m_iPopulatedHeight);

		if(!m_pOrbiter->SendCommand( CMD_Populate_Datagrid) || !bResponse) // wait for a response
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Populate datagrid: %d failed", pObj->m_iPK_Datagrid);

		m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFileBrowserBack_CONST), 0, "", "", 
			mediaFileBrowserOptions.m_listMediaFileAttributeDrillDown.empty() ? "0" : "1"); 
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Lights(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Media(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Climate(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Security(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Telephony(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Misc(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popLights(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popMedia(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popClimate(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popTelecom(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popSecurity(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0",
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_CONST, StringUtils::ltos(iPK_Device));
	ScreenHandlerBase::SCREEN_SingleCameraViewOnly(PK_Screen, iPK_Device);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device)
{
	vector<string> vectDevices;
	StringUtils::Tokenize(sList_PK_Device, ",", vectDevices);

	size_t NumCameras = vectDevices.size();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, NumCameras>=1 ? vectDevices[0] : "0");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, NumCameras>=2 ? vectDevices[1] : "0");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_3_CONST, NumCameras>=3 ? vectDevices[2] : "0");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_4_CONST, NumCameras>=4 ? vectDevices[3] : "0");
	ScreenHandlerBase::SCREEN_QuadViewCameras(PK_Screen, sList_PK_Device);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue)
{
	m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), StringUtils::itos(iPK_PnpQueue), "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Get_Username_Password_For_Devices(long PK_Screen, bool bAlready_processed, string sDescription, int iPK_PnpQueue)
{
	string sText;
	int iPK_DeviceData;

	if( bAlready_processed )
	{
		sText = m_pOrbiter->m_mapTextString[TEXT_password_for_device_CONST];
		iPK_DeviceData = DEVICEDATA_Password_CONST;
	}
	else
	{
		sText = m_pOrbiter->m_mapTextString[TEXT_username_for_device_CONST];
		iPK_DeviceData = DEVICEDATA_Username_CONST;
	}

	StringUtils::Replace( &sText, "<%=device%>", sDescription );

	m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(StringUtils::itos(iPK_PnpQueue));
	SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
		sText + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], 
		StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " " +
		"1 " + StringUtils::ltos(COMMAND_Set_Pnp_Options_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_DeviceData_CONST) + " " + StringUtils::itos(iPK_DeviceData) + " " +
		StringUtils::itos(COMMANDPARAMETER_Value_To_Assign_CONST) + " \"<%=17%>\"", 
		"PNP Device username/password", "0");

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::New_Phone_Enter_Number_DeviceConfigured(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if(
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSaveMobileNumber_CONST	||
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objControllerHome_CONST		||
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objControllerBack_CONST
	)
	{
		string sName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
		m_pOrbiter->m_pEvent->SendMessage(
			new Message(
				m_pOrbiter->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL,
				MESSAGETYPE_EVENT, EVENT_Device_Configured_CONST, 1, EVENTPARAMETER_Name_CONST, sName.c_str()
			)
		);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sOptions, int iPK_PnpQueue)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");  // Camera sensors
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sOptions);
	int iOptions = atoi(sOptions.c_str());
	if( iOptions & 1 ) // Need to do lights
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST));
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_lights_near_camera_CONST], TEXT_STATUS_CONST);
	}
	else
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Security_Device_CONST));
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_Sensors_Viewed_CONST], TEXT_STATUS_CONST);
	}

	if(NULL != m_pOrbiter->m_pScreenHistory_Pivot)
		m_pOrbiter->m_pScreenHistory_Pivot->ScreenID(StringUtils::itos(iPK_PnpQueue));

	ScreenHandlerBase::SCREEN_Sensors_Viewed_By_Camera(PK_Screen, sOptions, iPK_PnpQueue);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Sensors_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Sensors_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butSetVisibleSensors_CONST )
	{	
		string sExisting=m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
		if( sExisting.empty()==false && sExisting[ sExisting.size()-1 ]!='|' )
			sExisting += "|";
		sExisting += m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, sExisting); 
		if( atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST).c_str())==DEVICECATEGORY_Lighting_Device_CONST && atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_3_CONST).c_str()) & 2 )
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Security_Device_CONST));
			m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_Sensors_Viewed_CONST], TEXT_STATUS_CONST);
			GotoScreen(SCREEN_Sensors_Viewed_By_Camera_CONST,m_pOrbiter->m_pScreenHistory_Current->ScreenID(),true,true);
		}
		else
		{
			StringUtils::Replace(&sExisting,"|","\t");  // Per CreateDevice::DoIt relations are tab separated
			DCE::CMD_Set_Pnp_Options CMD_Set_Pnp_Options(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn,
				sExisting,DEVICEDATA_sPK_Device_Relations_For_Creat_CONST,atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str()));
			m_pOrbiter->SendCommand(CMD_Set_Pnp_Options);
			m_pOrbiter->CMD_Remove_Screen_From_History(m_pOrbiter->m_pScreenHistory_Current->ScreenID(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
		}
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::BadGotoScreen(int PK_Screen)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ScreenHandler::BadGotoScreen %d",PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device,
	string sPK_Device_AppServer)
{
	string sYesMessage =
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + sPK_Device_AppServer  + " " +
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Halt_Device_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " \"" + StringUtils::ltos(iPK_Device) + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Force_CONST) + " \"R\"";

	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Download_finnished_CONST) + "%>",
		false, "0", false,
		//Yes button
		"Yes",
		//Yes command
		sYesMessage,
		//No button
		"No",
		//No command
		"" //do nothing
		);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_PVR_Recordings(long PK_Screen)
{
        m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAddSoftware_CONST));
	ScreenHandlerBase::SCREEN_PVR_Remote(PK_Screen);
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_GridRendering,        new DatagridAcquiredBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_ObjectSelected,  new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_PVR_Remote(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAddSoftware_CONST));
	ScreenHandlerBase::SCREEN_PVR_Remote(PK_Screen);
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_GridRendering,	new DatagridAcquiredBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_TV_Channels(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAddSoftware_CONST));
	ScreenHandlerBase::SCREEN_TV_Channels(PK_Screen);
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_GridRendering,	new DatagridAcquiredBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::TV_Channels_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::TV_Channels_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Bookmark_Show_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgUpcomingShows_CONST);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected DESIGNOBJ_dgUpcomingShows_CONST %p",pDesignObj_DataGrid);

		if( pDesignObj_DataGrid )
		{
			DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
			if( pDataGridTable )
			{
				DataGridCell *pCell = pDataGridTable->GetData( 0, pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow );
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected cell %p",pCell);
				if( !pCell )
					return false;

				string sBookmark,sDescription;
				if( pCell->m_Text )
					sDescription = pCell->m_Text;
				string sSeries = pCell->m_mapAttributes_Find("seriesid");
				if( sSeries.empty()==false )
					sBookmark = " SERIES:" + sSeries;
				else
				{
					string sProgram = pCell->m_mapAttributes_Find("programid");
					if( sProgram.empty()==false )
						sBookmark = " PROG:" + sProgram;
				}
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected series %s bookmark %s/%d",sSeries.c_str(),sBookmark.c_str(),(int) sBookmark.empty());
				if( sBookmark.empty()==false )
				{
					DCE::CMD_Save_Bookmark CMD_Save_Bookmark(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						0,NULL,0,MEDIATYPE_pluto_LiveTV_CONST,"",sDescription,sBookmark,false);
					m_pOrbiter->SendCommand(CMD_Save_Bookmark);
					pCell->m_mapAttributes["PK_Bookmark"]="0";
				}
			}
			m_pOrbiter->Renderer()->RenderObjectAsync(pDesignObj_DataGrid);
		}
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Bookmark_Chan_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgTvChannels_UI2_CONST);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected DESIGNOBJ_dgTvChannels_UI2_CONST %p",pDesignObj_DataGrid);

		if( pDesignObj_DataGrid )
		{
			DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
			if( pDataGridTable )
			{
				DataGridCell *pCell = pDataGridTable->GetData( 0, pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow );
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected cell %p",pCell);
				if( !pCell )
					return false;

				string sBookmark,sDescription;
				string sChannel = pCell->GetValue();
				if( sChannel.empty()==false )
				{
					sBookmark = " CHAN:" + sChannel;
					DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgTvChannels_UI2_CONST);
					if( pDesignObj_DataGrid )
					{
						DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
						if( pDataGridTable )
						{
							DataGridCell *pCell = pDataGridTable->GetData( 0, pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow );
							if( pCell->m_Text )
								sDescription = pCell->m_Text;
						}
					}
				}
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::TV_Channels_ObjectSelected sChannel %s bookmark %s/%d",sChannel.c_str(),sBookmark.c_str(),(int) sBookmark.empty());
				if( sBookmark.empty()==false )
				{
					DCE::CMD_Save_Bookmark CMD_Save_Bookmark(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						0,NULL,0,MEDIATYPE_pluto_LiveTV_CONST,"",sDescription,sBookmark,false);
					m_pOrbiter->SendCommand(CMD_Save_Bookmark);
					pCell->m_mapAttributes["PK_Bookmark"]="0";
				}
			}
			m_pOrbiter->Renderer()->RenderObjectAsync(pDesignObj_DataGrid);
		}
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Record_Once_CONST ||
		pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Record_All_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) m_pOrbiter->FindObject(DESIGNOBJ_dgUpcomingShows_CONST);
		if( pDesignObj_DataGrid )
		{
			DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
			if( pDataGridTable )
			{
				DataGridCell *pCell = pDataGridTable->GetData( 0, pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow );
				if( !pCell )
					return false;

				m_pOrbiter->Renderer()->RenderObjectAsync(pDesignObj_DataGrid);  // re-render the grid with the record icon
				string sRecordID = pCell->m_mapAttributes_Find("recordid");

				if( sRecordID.empty()==false )
				{
					DCE::CMD_Remove_Scheduled_Recording CMD_Remove_Scheduled_Recording(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						sRecordID,"");
					m_pOrbiter->SendCommand(CMD_Remove_Scheduled_Recording);
					pCell->m_mapAttributes.erase("recording");  // temporarily set it so we don't have to re-acquire the grid
					pCell->m_mapAttributes.erase("recordid");  // temporarily set it so we don't have to re-acquire the grid
				}
				else
				{
					string sID;
					DCE::CMD_Schedule_Recording CMD_Schedule_Recording(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Record_Once_CONST ? "O" : "C",
						   "", pCell->m_mapAttributes["chanid"] + "," + pCell->m_mapAttributes["starttime"] + "," + pCell->m_mapAttributes["endtime"], &sID);
					m_pOrbiter->SendCommand(CMD_Schedule_Recording);
					pCell->m_mapAttributes["recording"] = pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_butUI2_Ch_Prev_Record_Once_CONST ? "O" : "C";
					pCell->m_mapAttributes["recordid"] = sID;
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::TV_Channels_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the 'virus free' check and hide it if it's virus free,
	// and also find the child object for the icon and assign it the picture associated with the cell.
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_EPG_All_Shows_CONST )
	{
		string sSelected = m_pOrbiter->m_mapVariable_Find( pDatagridAcquiredBackData->m_pObj->m_iPK_Variable );

		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			bool bSelected = pCell->m_Value && pCell->m_Value == sSelected;

			pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
			if(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size() != 0)
				colRow.second = colRow.second % int(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size());

			// See if there is an object assigned for this column/row
			map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
			if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
			{
				DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
				DesignObj_DataList::iterator iHao;

				pObj->m_GraphicToDisplay_set("tvcharend", bSelected ? GRAPHIC_SELECTED : GRAPHIC_NORMAL);

				// Iterate through all the object's children
				for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconTVChannels_CONST )
					{
						m_pOrbiter->m_pOrbiterRenderer->UpdateObjectImage(pDesignObj_Orbiter->m_ObjectID, "JPG",
							pCell->m_pGraphicData, pCell->m_GraphicLength, "0");  // Store the icon, which is cell's picture
					}
					else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoFavorite_CONST )
					{
						pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes.find("PK_Bookmark")==pCell->m_mapAttributes.end();
					}
				}
			}
		}
	}
	else if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_EPG_Current_Shows_CONST )
	{
		string sSelected = m_pOrbiter->m_mapVariable_Find( pDatagridAcquiredBackData->m_pObj->m_iPK_Variable );
		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			bool bSelected = pCell->m_Value && pCell->m_Value == sSelected;

			pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
			if(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size() != 0)
				colRow.second = colRow.second % int(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size());

			// See if there is an object assigned for this column/row
			map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
			if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
			{
				DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
				DesignObj_DataList::iterator iHao;

				pObj->m_GraphicToDisplay_set("tvprogrend", bSelected ? GRAPHIC_SELECTED : GRAPHIC_NORMAL);

				// Iterate through all the object's children
				for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
					if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconTVChannels_CONST )
					{
						m_pOrbiter->m_pOrbiterRenderer->UpdateObjectImage(pDesignObj_Orbiter->m_ObjectID, "JPG",
							pCell->m_pGraphicData, pCell->m_GraphicLength, "0");  // Store the icon, which is cell's picture
					}
					else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoFavorite_CONST )
					{
						pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes.find("PK_Bookmark")==pCell->m_mapAttributes.end();
					}
					else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoScheduledRecording_CONST )
					{
						pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes.find("recording")==pCell->m_mapAttributes.end();
					}
				}
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_Add_Software(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAddSoftware_CONST));
	ScreenHandlerBase::SCREEN_Add_Software(PK_Screen);
	RegisterCallBack(cbObjectHighlighted, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_ObjectHighlighted,	new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_GridSelected, new DatagridCellBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_GridRendering,	new DatagridAcquiredBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST )
	{
		if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST && m_pOrbiter->m_pScreenHistory_Current )
		{
			string::size_type pos=0;
			string sID = m_pOrbiter->m_pScreenHistory_Current->ScreenID();
			int PK_Software = atoi( StringUtils::Tokenize( sID, ",", pos ).c_str() );
			string sInstallation_Status = StringUtils::Tokenize( sID, ",", pos );
LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Fix this, the id is always the first one, then remove the variables below %s",sID.c_str());
		 PK_Software=m_PK_Software;
		sInstallation_Status=m_sInstallationStatus;

		int PK_Device = 
			m_pOrbiter->m_bIsOSD ? m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia :
			NULL != m_pOrbiter->m_pLocationInfo ? 
				m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector : 0;

			// Valid Installation status are: [I]nstalled, [i]nstalling, [R]emoved, [r]emoving
			DCE::CMD_Add_Software CMD_Add_Software(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn, 
				PK_Device, sInstallation_Status!="I" ,PK_Software);
			string sResponse;
			m_pOrbiter->SendCommand(CMD_Add_Software,&sResponse); // Send with delivery confirmation so the command updates the installation status before the grid refreshes

		}

		m_pOrbiter->CMD_Go_back("","");
		return true;
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_ObjectHighlighted(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_dgInstallableApps_CONST )
	{
		NeedToRender render2( m_pOrbiter, "ScreenHandler::AddSoftware_ObjectHighlighted" );  // Redraw anything that was changed by this command
		m_pOrbiter->Renderer()->RenderTextAsync( m_pOrbiter->FindText( (DesignObj_Orbiter *) pObjectInfoData->m_pObj->m_pParentObject,TEXT_selected_add_software_CONST ) );

//		DataGridCell *pCell = m_pOrbiter->GetDataGridHighlightCell( (DesignObj_DataGrid *) pObjectInfoData->m_pObj);
//		m_pOrbiter->CMD_Refresh("");
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the 'virus free' check and hide it if it's virus free,
	// and also find the child object for the icon and assign it the picture associated with the cell.
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	// Iterate through all the cells
	for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
	{
		DataGridCell *pCell = it->second;
		string sStatus = pCell->m_mapAttributes_Find("Installation_status");
#ifdef DEBUG
		string s = (pCell->GetText() ? pCell->GetText() : "*NO TEXT") + string("/") + (pCell->GetValue() ? pCell->GetValue() : "*NO VALUE");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::AddSoftware_GridRendering %s=%s",s.c_str(),sStatus.c_str());
#endif

		pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
		if( colRow.second == pDatagridAcquiredBackData->m_pDataGridTable->m_iDownRow || colRow.second == pDatagridAcquiredBackData->m_pDataGridTable->m_iUpRow )
			continue;

		colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;
		colRow.first -= pDatagridAcquiredBackData->m_pObj->m_GridCurCol;

		// See if there is an object assigned for this column/row
		map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
		if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
		{
			DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
			DesignObj_DataList::iterator iHao;

			// Iterate through all the object's children
			for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
			{
				DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_Installed_Apps_Not_Virus_Free_CONST )
					pDesignObj_Orbiter->m_bHidden = true; // pCell->m_mapAttributes_Find("Virus_Free")=="Yes";  // Hide this if it's virus fee
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconCheckMark_CONST )
					pDesignObj_Orbiter->m_bHidden = sStatus!="I";  // Hide this if it's not installed
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconHour_Glass_CONST )
					pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes_Find("Installation_status")!="i" && pCell->m_mapAttributes_Find("Installation_status")!="r";  // Hide this if it's not being installed/removed
					
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_Installed_Apps_Icon_CONST )
					m_pOrbiter->m_pOrbiterRenderer->UpdateObjectImage(pDesignObj_Orbiter->m_ObjectID, "PNG",
						pCell->m_pGraphicData, pCell->m_GraphicLength, "0");  // Store the icon, which is cell's picture
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_pDataGridCell )
	{
		int PK_Software = atoi(pCellInfoData->m_pDataGridCell->GetValue());
		string sInstallation_Status = pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("Installation_status");
		string sText,sCommand;
		if( !PK_Software )
			return false;

		m_PK_Software = PK_Software;
		m_sInstallationStatus = sInstallation_Status;
		if( PK_Software && sInstallation_Status!="I" )
			sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_Add_Software_CONST];
		else if( PK_Software )
			sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_delete_software_CONST];

		DataGridCell *pCell = pCellInfoData->m_pDesignObj_DataGrid->DataGridTable_Get() ? pCellInfoData->m_pDesignObj_DataGrid->DataGridTable_Get()->GetData( 0, pCellInfoData->m_Row ) : NULL;
		if( pCell )
			sText+="\n" + pCell->m_mapAttributes["Title"];
		DisplayMessageOnOrbiter(0,sText,false,"30",false,m_pOrbiter->m_mapTextString[TEXT_YES_CONST],"",m_pOrbiter->m_mapTextString[TEXT_NO_CONST],"","","","","",StringUtils::itos(PK_Software) + "," + sInstallation_Status);
		return true; // We did a goto screen
	}

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileSave(long PK_Screen, int iPK_MediaType, int iEK_Disc, string sCaption, string sCommand, bool bAdvanced_options)
{
	//the command to execute with the selected file
	m_sSaveFile_Command = sCommand;
	m_bSaveFile_Advanced_options = bAdvanced_options;
	string sFolder;
	if(!m_bSaveFile_CreatingFolder)
	{
		//get default ripping info
		string sMounterFolder;
		bool bUseDirectoryStructure;
		CMD_Get_Default_Ripping_Info cmd_CMD_Get_Default_Ripping_Info(m_pOrbiter->m_dwPK_Device,
			m_pOrbiter->m_dwPK_Device_MediaPlugIn, iEK_Disc, &m_pOrbiter->m_sDefaultRippingName,
			&bUseDirectoryStructure, &sMounterFolder, &m_pOrbiter->m_nDefaultStorageDeviceForRipping, 
			&sFolder,
			&m_pOrbiter->m_sDefaultStorageDeviceForRippingName);
		m_pOrbiter->SendCommand(cmd_CMD_Get_Default_Ripping_Info);

		if(m_nSaveFile_PK_DeviceDrive == 0)
			m_nSaveFile_PK_DeviceDrive = m_pOrbiter->m_nDefaultStorageDeviceForRipping;

		if(m_sSaveFile_Drive == "")
			m_sSaveFile_Drive = m_pOrbiter->m_sDefaultStorageDeviceForRippingName; 

		if(m_sSaveFile_FileName == "")
			m_sSaveFile_FileName = m_pOrbiter->m_sDefaultRippingName;

		if(m_sSaveFile_MountedFolder == "")
		{
			m_sSaveFile_MountedFolder = sMounterFolder;
			m_bUseDirectoryStructure = bUseDirectoryStructure;
		}

		if( sFolder.empty() )  // It's possible a valid disk wasn't passed in, but we at least know the type
		{
			if( iPK_MediaType==MEDIATYPE_pluto_CD_CONST || 
			    iPK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
				sFolder = "audio";
			else if( iPK_MediaType==MEDIATYPE_pluto_DVD_CONST ||
				 iPK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
				sFolder = "videos";
		}
	}
	
	if(m_bSaveFile_CreatingFolder)
	{
		//wait for app_server to create the folder
		//maybe it would be better instead to send "spawn_application" command to app_server with confirmation?
		Sleep(200); 
	}

	if ( iEK_Disc==-998 )  // Special to fill filename with selected MediaBrowser file.
	  {
	    string sSelectedFile, sSelectedPath;
	    m_sSaveFile_FileName = m_mapKeywords["FILENAME"];
	  }

	//setup variables
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, m_sSaveFile_Drive);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, m_sSaveFile_RelativeFolder);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(m_nSaveFile_PK_DeviceDrive));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, FileUtils::ExcludeTrailingSlash(m_sSaveFile_MountedFolder) + "\nMT-1\nP");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, m_sSaveFile_FileName);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Status_CONST, sFolder);
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sCaption, TEXT_STATUS_CONST);
	if( iEK_Disc==-999 )  // Special things means go to the bulk ripping screen
	{
		if (m_bSaveFile_IsMove)
		  {
		    // Move File uses its own screen, but hides the Folder button, as it is superfluous.
		    m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_mnuMoveFile_CONST),"","",false,false);
		    m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuMoveFile_CONST), m_pOrbiter->m_mapTextString[2174], TEXT_STATUS_CONST); // FIXME: use CONST
		    string sPicture = m_mapKeywords_Find("PICTURE");
		    if (sPicture.empty() == false)
		      {
			size_t size;
			char *pGraphicData = m_pOrbiter->ReadFileIntoBuffer(sPicture,size);
			m_pOrbiter->CMD_Update_Object_Image("6254.0.0.2355","jpg",pGraphicData,(int)size,"0");
			delete [] pGraphicData;
			pGraphicData=NULL;
		      }
		  }
		else
		  {
	            m_pOrbiter->CMD_Goto_DesignObj(0,TOSTRING(DESIGNOBJ_mnuBulkRipping_CONST),"","",false,false);
		    m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuBulkRipping_CONST), m_pOrbiter->m_mapTextString[TEXT_Bulk_rip_choose_name_CONST], TEXT_STATUS_CONST);
		  }
	}
	else
		ScreenHandlerBase::SCREEN_FileSave(PK_Screen, iPK_MediaType, iEK_Disc, sCaption, sCommand, bAdvanced_options);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::FileSave_ObjectSelected,	
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::FileSave_GridSelected, 
		new DatagridCellBackData());

	if(m_bSaveFile_CreatingFolder)
	{
		SaveFile_GotoChooseFolderDesignObj();
		m_bSaveFile_CreatingFolder = false;
	}
	else
	{
		m_pOrbiter->CMD_Show_Object(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + ".0.0." + StringUtils::ltos(DESIGNOBJ_butChooseDrive_CONST), 0, "", "", 
			m_bSaveFile_Advanced_options ? "1" : "0");
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::Reset_SaveFile_Info()
{
	m_sSaveFile_MountedFolder = "";
	m_sSaveFile_RelativeFolder = "";
	m_sSaveFile_Drive = "";
	m_sSaveFile_FullBasePath = "";
	m_sSaveFile_FileName = "";
	m_sSaveFile_Command = "";
	m_bSaveFile_CreatingFolder = false;
	m_bSaveFile_Advanced_options = true;
	m_bUseDirectoryStructure = false;
	m_nSaveFile_PK_DeviceDrive = 0;
	m_nPK_Users_SaveFile = 0;
	m_bSaveFile_IsMove=false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	
	if(NULL != pObjectInfoData)
	{
		if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseDrive_CONST)
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChoose_CONST)
			{
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, m_sSaveFile_Drive);
				m_pOrbiter->CMD_Go_back("", "");
				return true;
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseFolder_CONST)
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChoose_CONST)
			{
				SaveFile_SendCommand(m_nPK_Users_SaveFile);
				return true;
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butCreateDir_CONST)
			{
				string sParentFolder = m_pOrbiter->m_mapVariable_Find(VARIABLE_Path_CONST) + "/";
				string sNewFolder = "<%=" + StringUtils::ltos(VARIABLE_Seek_Value_CONST) + "%>";
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

				long nAppServer = 0;
				m_pOrbiter->TranslateVirtualDevice(DEVICETEMPLATE_VirtDev_AppServer_CONST,nAppServer);
				m_bSaveFile_CreatingFolder = true;

				SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
					m_pOrbiter->m_mapTextString[TEXT_type_dir_name_CONST] + "|" + 
						m_pOrbiter->m_mapTextString[TEXT_Create_Directory_CONST] + "|" + 
						m_pOrbiter->m_mapTextString[TEXT_CANCEL_CONST],
					StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::ltos(nAppServer) + " " +
					"1 " + StringUtils::ltos(COMMAND_Spawn_Application_CONST) + " " +
					StringUtils::ltos(COMMANDPARAMETER_Filename_CONST) + " \"mkdir\" " + 
					StringUtils::ltos(COMMANDPARAMETER_Name_CONST) + " \"create_folder\" " +
					StringUtils::ltos(COMMANDPARAMETER_Arguments_CONST) + " \"" + sParentFolder + sNewFolder + "\"|", 
					"Folder creation", "0");

				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, sParentFolder + sNewFolder + "/");
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuFileSave_CONST || GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuBulkRipping_CONST || 
			GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuMoveFile_CONST)
		{
			if(
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePrivate_CONST ||
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePublic_CONST
			)
			{
				m_sSaveFile_FileName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"m_sSaveFile_FileName == %s",m_sSaveFile_FileName.c_str());
				bool bUnknownSubdirectory=false;
				string sSubDir = m_pOrbiter->m_mapVariable_Find(VARIABLE_Status_CONST);
				if( m_bUseDirectoryStructure==false )
					m_sSaveFile_FullBasePath = m_sSaveFile_MountedFolder;
				else
				{
					if( sSubDir.empty() )
					{
						bUnknownSubdirectory=true;
						sSubDir = "___audio___or___video___";
					}
					if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePublic_CONST)
						m_sSaveFile_FullBasePath = m_sSaveFile_MountedFolder + "public/data/" + sSubDir + "/";
					else
						m_sSaveFile_FullBasePath = m_sSaveFile_MountedFolder + "user_" + StringUtils::itos(m_pOrbiter->m_dwPK_Users) + "/data/" + sSubDir + "/";
				}
				
				m_nPK_Users_SaveFile = pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePrivate_CONST ? m_pOrbiter->m_dwPK_Users : 0;
				if(m_bSaveFile_Advanced_options && bUnknownSubdirectory==false && GetCurrentScreen_PK_DesignObj() != DESIGNOBJ_mnuBulkRipping_CONST)  // We can't prompt for the sub-directory if we're using pluto's directory structure and don't know if it's audio or video because it's an unidentified disk in a jukebox
					SaveFile_GotoChooseFolderDesignObj();
				else
				{
					if (m_bSaveFile_IsMove)
					{
						SaveFile_GotoChooseFolderDesignObj();
					}
					else
					{
						if( bUnknownSubdirectory || GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuBulkRipping_CONST )
							m_pOrbiter->CMD_Set_Variable(VARIABLE_Path_CONST,m_sSaveFile_FullBasePath);
						SaveFile_SendCommand(m_nPK_Users_SaveFile);
					}
				}

				return true;
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChooseDrive_CONST ||
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objCurrentUser_CONST)
			{
                		m_sSaveFile_FileName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Once again, m_sSaveFile_FileName is %s set from <%=17%>",m_sSaveFile_FileName.c_str());				
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileSave_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);

	if(NULL != pCellInfoData && NULL != pCellInfoData->m_pDataGridCell)
	{
		if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseDrive_CONST)
		{
			string sValue = pCellInfoData->m_pDataGridCell->GetValue();
			vector<string> vectStrings;
			StringUtils::Tokenize(sValue, "\t", vectStrings);

			if(vectStrings.size() >= 2)
			{	
				m_nSaveFile_PK_DeviceDrive = atoi(vectStrings[0].c_str());
				m_sSaveFile_MountedFolder = vectStrings[1];

				m_sSaveFile_Drive = pCellInfoData->m_pDataGridCell->GetText();
				m_sSaveFile_RelativeFolder = "";

				m_bUseDirectoryStructure = vectStrings.size()>=3 && vectStrings[2]=="1";
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(m_nSaveFile_PK_DeviceDrive));
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseFolder_CONST)
		{
			m_sSaveFile_RelativeFolder = FileUtils::IncludeTrailingSlash(pCellInfoData->m_pDataGridCell->GetValue());

			if(m_sSaveFile_FullBasePath.length() < m_sSaveFile_RelativeFolder.length())
				m_sSaveFile_RelativeFolder = m_sSaveFile_RelativeFolder.substr(m_sSaveFile_FullBasePath.length());
			else
			{
				m_sSaveFile_RelativeFolder = FileUtils::BasePath(m_sSaveFile_RelativeFolder);

				if(m_sSaveFile_RelativeFolder.length() > 0)
				{
					if(m_sSaveFile_RelativeFolder[m_sSaveFile_RelativeFolder.length() - 1] != '/')
						m_sSaveFile_RelativeFolder += "/";
				}
			}

			m_pOrbiter->CMD_Set_Text(StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), 
				"Folder : " + m_sSaveFile_FullBasePath + m_sSaveFile_RelativeFolder, TEXT_STATUS_CONST);

			m_pOrbiter->Renderer()->RenderObjectAsync(m_pOrbiter->FindObject(DESIGNOBJ_mnuChooseFolder_CONST));
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SaveFile_GotoChooseFolderDesignObj()
{
	string sNewPath = FileUtils::IncludeTrailingSlash(m_sSaveFile_FullBasePath) + m_sSaveFile_RelativeFolder;

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, FileUtils::ExcludeTrailingSlash(sNewPath) + "\nMT-1\nP");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, m_sSaveFile_FileName);
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), "", "", 
		false, true);

	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), 
		"Folder : " + sNewPath, TEXT_STATUS_CONST);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SaveFile_SendCommand(int PK_Users)
{
	string sFilename = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
	sFilename = FileUtils::ValidFileName(sFilename,true);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,sFilename);

	string sPath = FileUtils::IncludeTrailingSlash(m_pOrbiter->m_mapVariable_Find(VARIABLE_Path_CONST));
	sPath = FileUtils::ValidFileName(sPath,true);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Path_CONST, sPath);

	m_pOrbiter->CMD_Send_Message(m_sSaveFile_Command + (PK_Users ? " 17 " + StringUtils::itos(PK_Users) : ""), false);
	m_pOrbiter->GotoMainMenu();

	//reset file save info
	m_sSaveFile_MountedFolder = "";
	m_sSaveFile_RelativeFolder = "";
	m_sSaveFile_Drive = "";
	m_sSaveFile_FullBasePath = "";
	m_sSaveFile_FileName = "";
	m_sSaveFile_Command = "";
	m_bSaveFile_CreatingFolder = false;
	m_nSaveFile_PK_DeviceDrive = 0;
	m_bSaveFile_Advanced_options = true;
	m_bUseDirectoryStructure = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_Floorplan(long PK_Screen, string sPK_DesignObj)
{
	m_pOrbiter->GotoDesignObj(sPK_DesignObj);

	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(sPK_DesignObj);
	if(NULL != pObj)
	{
		if(pObj->m_iBaseObjectID == DESIGNOBJ_mapTelecom_CONST)
		{
			bool bCallInProgress = !m_sMyCallID.empty();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SCREEN_Floorplan my call %s", m_sMyCallID.c_str());

			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mapTelecom_CONST) ".0.0." TOSTRING(DESIGNOBJ_butCallInProgress_CONST), 0, "", "", 
				bCallInProgress ? "1" : "0");

			RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
		}
	}
}

/*virtual*/ void ScreenHandler::SCREEN_Halt_System(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Halt_System(PK_Screen);

	long dwPK_Device_MD;
	if( m_pOrbiter->m_bIsOSD )
	  {
	    if ( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
	      {
		// Are we in a different room? if so, grab the device from Location
		dwPK_Device_MD = m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector;
	      }
	    else
	      {
		dwPK_Device_MD = m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia;
	      }
	  }
	else if( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
	  dwPK_Device_MD=m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector;
	
	// At this point, we have the target MD to get device data from.
	string sOperating_System = m_pOrbiter->m_pEvent->GetDeviceDataFromDatabase(dwPK_Device_MD, DEVICEDATA_Operating_System_CONST);

	DesignObj_Orbiter *pDesignObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_mnuSystemHalt_CONST) ".0.0." TOSTRING(DESIGNOBJ_butReset_AsOtherOS_CONST) );
	if( pDesignObj )
		pDesignObj->m_bHidden = sOperating_System.empty();
}

/*virtual*/ void ScreenHandler::SCREEN_CreateViewBookmarksTV(long PK_Screen)
{
	// Grab the position and a snapshot of the current frame
	string sText,sPosition;
	DCE::CMD_Report_Playback_Position CMD_Report_Playback_Position(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,
		m_pOrbiter->m_iStreamID,&sText,&sPosition);

	if( !m_pOrbiter->SendCommand(CMD_Report_Playback_Position) || sPosition.size()==0 )
	{
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_DialogCannotBookmark_CONST);
		return;
	}

	string::size_type pos=sText.find('\t');  // Text will be the channel in human readable form, a tab, and the show in human readable form
	string sChannelDesc = pos!=string::npos ? sText.substr(0,pos) : sText;
	string sShowDesc = pos!=string::npos ? sText.substr(pos+1) : "";

	string sChannel,sShow;
	string::size_type end;
	pos=sPosition.find("CHAN:");
	if( pos!=string::npos )
	{
		end = sPosition.find(" ",pos);
		sChannel = end==string::npos ? sPosition.substr(pos+5) : sPosition.substr(pos+5,end-pos-5);
		if( sChannel.empty()==false )
			sChannel = " CHAN:" + sChannel;
	}

	// For the show, first try to find a series
	pos=sPosition.find("SERIES:");
	if( pos!=string::npos )
	{
		end = sPosition.find(" ",pos);
		sShow = end==string::npos ? sPosition.substr(pos+7) : sPosition.substr(pos+7,end-pos-7);
		if( sShow.empty()==false )
			sShow = " SERIES:" + sShow;
	}

	if( sShow.empty() )
	{
		pos=sPosition.find("PROG:");
		if( pos!=string::npos )
		{
			end = sPosition.find(" ",pos);
			sShow = end==string::npos ? sPosition.substr(pos+5) : sPosition.substr(pos+5,end-pos-5);
			if( sShow.empty()==false )
				sShow = " PROG:" + sShow;
		}
	}

	// We should at least have a channel, although if there's no guide data, there might not be a show
	if( sChannel.empty() || sChannelDesc.empty() )
	{
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_DialogCannotBookmark_CONST);
		return;
	}

	ScreenHandlerBase::SCREEN_CreateViewBookmarksTV(PK_Screen);

	// There's no show for us to bookmark
	if( sShow.empty() || sShowDesc.empty() )
	{
		m_pOrbiter->CMD_Show_Object(StringUtils::itos(m_p_MapDesignObj_Find(PK_Screen)) + ".0.0." TOSTRING(DESIGNOBJ_butBookmarkProgram_Private_CONST),0,"","","0");
		m_pOrbiter->CMD_Show_Object(StringUtils::itos(m_p_MapDesignObj_Find(PK_Screen)) + ".0.0." TOSTRING(DESIGNOBJ_butBookmarkProgram_CONST),0,"","","0");
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,sChannelDesc);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,sShowDesc);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,sChannel);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,sShow);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::CreateViewBookmarksTV_ObjectSelected,	new ObjectInfoBackData());
}

bool ScreenHandler::CreateViewBookmarksTV_ObjectSelected(CallBackData *pData)
{
	//ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	return false;
}

void ScreenHandler::SCREEN_Thumbnail(long PK_Screen)
{
	string sFormat="jpg";
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	delete m_pData_LastThumbnail;
	m_iData_Size_LastThumbnail=0;

	DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_NowPlaying,
		"0",m_pOrbiter->m_iStreamID,800,800,&m_pData_LastThumbnail,&m_iData_Size_LastThumbnail,&sFormat);
	m_pOrbiter->SendCommand(CMD_Get_Video_Frame);

	int PK_DesignObj = m_pOrbiter->m_mapDesignObj[PK_Screen];
	m_pOrbiter->CMD_Update_Object_Image(StringUtils::itos(PK_DesignObj) + ".0.0." TOSTRING(DESIGNOBJ_objCDCover_CONST),sFormat,m_pData_LastThumbnail,m_iData_Size_LastThumbnail,"0");
	vm.Release();
	ScreenHandlerBase::SCREEN_Thumbnail(PK_Screen);

	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::Thumbnail_DatagridSelected, new DatagridCellBackData());
}

bool ScreenHandler::Thumbnail_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);

	if( m_pData_LastThumbnail )
	{
		// Should always be the case
		DCE::CMD_Make_Thumbnail CMD_Make_Thumbnail(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			pCellInfoData->m_sValue,m_pData_LastThumbnail,m_iData_Size_LastThumbnail);
		m_pOrbiter->SendCommand(CMD_Make_Thumbnail);
		delete m_pData_LastThumbnail;
		m_pData_LastThumbnail=NULL;
		m_iData_Size_LastThumbnail=0;
	}
	vm.Release();
	m_pOrbiter->CMD_Go_back("","");
	return true; // We did a goto screen
}

void ScreenHandler::SCREEN_Manage_Drives(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Manage_Drives(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::DriveOverview_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::DriveOverview_GridRendering,	new DatagridAcquiredBackData());
}

bool ScreenHandler::DriveOverview_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( (pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST || 
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoID_CONST || 
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoEject_CONST || 
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoPlay_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butManage_CONST) && 
		pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject &&
		pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		int Row=-1,Col=-1;
		DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject;
		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator it=pObj_Grid->m_mapChildDgObjects.begin();it!=pObj_Grid->m_mapChildDgObjects.end();++it)
		{
			if( it->second == pObjectInfoData->m_pObj->m_pParentObject )
			{
				Col=it->first.first;
				Row=it->first.second;
				break;
			}
		}

		if( Row!=-1 && Col!=-1 )
		{
			DataGridTable *pDataGridTable = pObj_Grid->m_pDataGridTable_Current_get();
			DataGridCell *pCell = pDataGridTable->GetData(Col,Row);
			if( pCell && pCell->GetValue() )
			{
				if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoEject_CONST )
				{
					DCE::CMD_Eject_Disk CMD_Eject_Disk(m_pOrbiter->m_dwPK_Device,atoi(pCell->GetValue()),0);
					m_pOrbiter->SendCommand(CMD_Eject_Disk);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST )
				{
					string sEK_Device_Ripping = pCell->m_mapAttributes_Find("EK_Device_Ripping");
					string sPK_File = pCell->m_mapAttributes_Find("PK_File");
					if( sEK_Device_Ripping.empty()==false || sPK_File.empty()==false )
					{
						DCE::CMD_Abort_Ripping CMD_Abort_Ripping(m_pOrbiter->m_dwPK_Device,atoi(sEK_Device_Ripping.c_str()));
						m_pOrbiter->SendCommand(CMD_Abort_Ripping);
					}
					else
					{
						string sRipMessage = 
							StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
							StringUtils::itos(m_pOrbiter->m_dwPK_Device_MediaPlugIn) + " 1 "
							TOSTRING(COMMAND_Rip_Disk_CONST) " "
							TOSTRING(COMMANDPARAMETER_PK_Users_CONST) " <%=U%> "
							TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"<%=17%>\" "
							TOSTRING(COMMANDPARAMETER_DriveID_CONST) " \"<%=45%>\" "
							TOSTRING(COMMANDPARAMETER_Directory_CONST) " \"<%=9%>\" "
							TOSTRING(COMMANDPARAMETER_EK_Disc_CONST) " \"" + pCell->m_mapAttributes["PK_Disc"] + "\" ";
						if( pCell->GetValue() )
							sRipMessage +=
								TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " \"" + string(pCell->GetValue()) + "\" ";  // This will be either a drive or jukebox depending on which cell was chosen

						string sTitle = m_pOrbiter->m_mapTextString[TEXT_Choose_Filename_CONST];
						Reset_SaveFile_Info();
						DCE::SCREEN_FileSave SCREEN_FileSave(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,atoi(pCell->m_mapAttributes_Find("PK_MediaType").c_str()),atoi(pCell->m_mapAttributes_Find("PK_Disc").c_str()),sTitle,sRipMessage,true);
						m_pOrbiter->SendCommand(SCREEN_FileSave);
					}
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoID_CONST )
				{
					DCE::CMD_Mass_identify_media CMD_Mass_identify_media(m_pOrbiter->m_dwPK_Device,atoi(pCell->GetValue()),"*");
					m_pOrbiter->SendCommand(CMD_Mass_identify_media);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoPlay_CONST && m_pOrbiter->m_pLocationInfo && pCell->m_Value ) // play requires a location to play at
				{
					int PK_Disc = atoi( pCell->m_mapAttributes_Find("PK_Disc").c_str() );
					DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
						0,"!r" + StringUtils::itos(PK_Disc) + ":" + pCell->m_Value, // see MediaAttributes_LowLevel::TransformFilenameToDeque
						0,0,StringUtils::itos(m_pOrbiter->m_pLocationInfo->PK_EntertainArea),false,0,0 /* bQueue */, 0 /* bBypass_Event */, 0 /* bDont_Setup_AV */);
					m_pOrbiter->SendCommand(CMD_MH_Play_Media);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butManage_CONST )
				{
					DCE::SCREEN_Jukebox_Manager SCREEN_Jukebox_Manager(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,atoi(pCell->GetValue()));
					m_pOrbiter->SendCommand(SCREEN_Jukebox_Manager);
				}
			}
		}
	}
	return false; // Keep processing it
}

//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::DriveOverview_GridRendering(CallBackData *pData)
{
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Discs_CONST )
	{
		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			string sEK_Device_Ripping = pCell->m_mapAttributes_Find("EK_Device_Ripping");
			string sPK_File = pCell->m_mapAttributes_Find("PK_File");
			if( sEK_Device_Ripping.empty()==false || sPK_File.empty()==false )
			{
				pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
				if(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size() != 0)
					colRow.second = colRow.second % int(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size());

				// See if there is an object assigned for this column/row
				map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
				if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
				{
					DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
					DesignObj_DataList::iterator iHao;

					// Iterate through all the object's children
					for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
					{
						DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
						if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST )
							pDesignObj_Orbiter->m_GraphicToDisplay_set("dogr", sEK_Device_Ripping.empty()==false ? 2 : 1);
					}
				}
			}
		}
	}
	return false;
}

void ScreenHandler::SCREEN_Jukebox_Manager(long PK_Screen, int iPK_Device)
{
	ScreenHandlerBase::SCREEN_Jukebox_Manager(PK_Screen,iPK_Device);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST,StringUtils::itos(iPK_Device));
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::JukeboxManager_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::JukeboxManager_GridRendering,	new DatagridAcquiredBackData());
}

bool ScreenHandler::JukeboxManager_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	int PK_Device_JukeBox = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());
	if( (pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoEject_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoPlay_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoID_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butUnload_CONST ||
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butLoad_CONST 
		) && 
		pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject &&
		pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		int Row=-1,Col=-1;
		DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeboxManager_ObjectSelected ===============================================");

		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator itc = pObj_Grid->m_mapChildDgObjects.begin();
			itc != pObj_Grid->m_mapChildDgObjects.end(); ++itc)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeboxManager_ObjectSelected --- Cell <%d, %d>",
				itc->first.first, itc->first.second);
		}

		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator it=pObj_Grid->m_mapChildDgObjects.begin();it!=pObj_Grid->m_mapChildDgObjects.end();++it)
		{
			if( it->second == pObjectInfoData->m_pObj->m_pParentObject )
			{
				Col=it->first.first;
				Row=it->first.second;
				break;
			}
		}

		if( Row!=-1 && Col!=-1 )
		{
			DataGridTable *pDataGridTable = pObj_Grid->m_pDataGridTable_Current_get();

			int DGRow = ( ( Row == 0 && pDataGridTable->m_bKeepRowHeader ) ? 0 : Row + pDataGridTable->m_StartingRow );
			int DGColumn = ( Col == 0 && pDataGridTable->m_bKeepColumnHeader ) ? 0 : Col + pDataGridTable->m_StartingColumn;
			DataGridCell *pCell = pDataGridTable->GetData(DGColumn,DGRow);

			if( pCell && pCell->GetValue() )
			{
				if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoEject_CONST )
				{
					int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
					int Slot = atoi( pCell->m_mapAttributes_Find("Slot").c_str() );
					DCE::CMD_Eject_Disk CMD_Eject_Disk(m_pOrbiter->m_dwPK_Device,PK_Device,Slot);
					m_pOrbiter->SendCommand(CMD_Eject_Disk);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST )
				{
					string sEK_Device_Ripping = pCell->m_mapAttributes_Find("EK_Device_Ripping");
					if( sEK_Device_Ripping.empty()==false )
					{
						int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
						string sRipJob = pCell->m_mapAttributes["RipJob"];
						DCE::CMD_Abort_Task CMD_Abort_Task(m_pOrbiter->m_dwPK_Device,PK_Device,atoi(sRipJob.c_str()));
						m_pOrbiter->SendCommand(CMD_Abort_Task);
					}
					else
					{
						string sRipMessage = 
							StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
							StringUtils::itos(m_pOrbiter->m_dwPK_Device_MediaPlugIn) + " 1 "
							TOSTRING(COMMAND_Rip_Disk_CONST) " "
							TOSTRING(COMMANDPARAMETER_PK_Users_CONST) " <%=U%> "
							TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"<%=17%>\" "
							TOSTRING(COMMANDPARAMETER_DriveID_CONST) " \"<%=45%>\" "
							TOSTRING(COMMANDPARAMETER_Directory_CONST) " \"<%=9%>\" "
							TOSTRING(COMMANDPARAMETER_EK_Disc_CONST) " \"" + pCell->m_mapAttributes["PK_Disc"] + "\" "
							TOSTRING(COMMANDPARAMETER_Slot_Number_CONST) " \"" + pCell->m_mapAttributes["Slot"] + "\" "
							TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " \"" + pCell->m_mapAttributes["PK_Device"] + "\" ";  // This will be either a drive or jukebox depending on which cell was chosen

						string sTitle = m_pOrbiter->m_mapTextString[TEXT_Choose_Filename_CONST];
						Reset_SaveFile_Info();
						DCE::SCREEN_FileSave SCREEN_FileSave(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,atoi(pCell->m_mapAttributes_Find("PK_MediaType").c_str()),atoi(pCell->m_mapAttributes_Find("PK_Disc").c_str()),sTitle,sRipMessage,true);
						m_pOrbiter->SendCommand(SCREEN_FileSave);
					}
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoID_CONST )
				{
					int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
					DCE::CMD_Mass_identify_media CMD_Mass_identify_media(
						m_pOrbiter->m_dwPK_Device,PK_Device,pCell->m_mapAttributes_Find("Slot"));
					m_pOrbiter->SendCommand(CMD_Mass_identify_media);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butLoad_CONST )
				{
					int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
					int Slot = atoi( pCell->m_mapAttributes_Find("Slot").c_str() );
					int i=-1;
					DCE::CMD_Load_from_Slot_into_Drive CMD_Load_from_Slot_into_Drive(
						m_pOrbiter->m_dwPK_Device,PK_Device,Slot,&i);
					m_pOrbiter->SendCommand(CMD_Load_from_Slot_into_Drive);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butUnload_CONST )
				{
					int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
					int i=-1;
					DCE::CMD_Unload_from_Drive_into_Slot CMD_Unload_from_Drive_into_Slot(
						m_pOrbiter->m_dwPK_Device,PK_Device,PK_Device,&i);
					m_pOrbiter->SendCommand(CMD_Unload_from_Drive_into_Slot);
				}
				else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoPlay_CONST && m_pOrbiter->m_pLocationInfo ) // play requires a location to play at
				{
					if( pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject->m_iBaseObjectID==DESIGNOBJ_dgJukeboxDrives_CONST )
					{
						int PK_Disc = atoi( pCell->m_mapAttributes_Find("PK_Disc").c_str() );
						int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
						DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
							0,"!r" + StringUtils::itos(PK_Disc) + ":" + StringUtils::itos(PK_Device), // see MediaAttributes_LowLevel::TransformFilenameToDeque
							0,0,StringUtils::itos(m_pOrbiter->m_pLocationInfo->PK_EntertainArea),false,0,0 /* bQueue */, 0 /* bBypass_Event */, 0 /* bDont_Setup_AV */);
						m_pOrbiter->SendCommand(CMD_MH_Play_Media);
					}
					else
					{
						int PK_Disc = atoi( pCell->m_mapAttributes_Find("PK_Disc").c_str() );
						int PK_Device = atoi( pCell->m_mapAttributes_Find("PK_Device").c_str() );
						int Slot = atoi( pCell->m_mapAttributes_Find("Slot").c_str() );
						DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
							0,"!r" + StringUtils::itos(PK_Disc) + ":" + StringUtils::itos(PK_Device) + ":" + StringUtils::itos(Slot), // see MediaAttributes_LowLevel::TransformFilenameToDeque
							0,0,StringUtils::itos(m_pOrbiter->m_pLocationInfo->PK_EntertainArea),false,0,0 /* bQueue */, 0 /* bBypass_Event */, 0 /* bDont_Setup_AV */);
						m_pOrbiter->SendCommand(CMD_MH_Play_Media);
					}
				}
			}
		}
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butCancelRipping_CONST )
	{
		DCE::CMD_Abort_Ripping CMD_Abort_Ripping(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox);
		m_pOrbiter->SendCommand(CMD_Abort_Ripping);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butRipAll_CONST )
	{
		string sRipMessage = 
			StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
			StringUtils::itos(PK_Device_JukeBox) + " 1 "
			TOSTRING(COMMAND_Bulk_Rip_CONST) " "
			TOSTRING(COMMANDPARAMETER_PK_Users_CONST) " <%=U%> "
			TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"<%=9%>\" ";
		string sTitle = m_pOrbiter->m_mapTextString[TEXT_Choose_Filename_CONST];
		
		Reset_SaveFile_Info();
		DCE::SCREEN_FileSave SCREEN_FileSave(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,0,-999,sTitle,sRipMessage,true);
		m_pOrbiter->SendCommand(SCREEN_FileSave);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butJukeboxRefresh_CONST )
	{
		DCE::CMD_Refresh CMD_Refresh(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox,"*");
		m_pOrbiter->SendCommand(CMD_Refresh);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butEjectAll_CONST )
	{
		DCE::CMD_Eject_Disk CMD_Eject_Disk(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox,0);
		m_pOrbiter->SendCommand(CMD_Eject_Disk);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butLoad1_CONST )
	{
		DCE::CMD_Load_Disk CMD_Load_Disk(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox,false);
		m_pOrbiter->SendCommand(CMD_Load_Disk);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butLoadBulk_CONST )
	{
		DCE::CMD_Load_Disk CMD_Load_Disk(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox,true);
		m_pOrbiter->SendCommand(CMD_Load_Disk);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butIdAll_CONST )
	{
		DCE::CMD_Mass_identify_media CMD_Mass_identify_media(m_pOrbiter->m_dwPK_Device,PK_Device_JukeBox,"*");
		m_pOrbiter->SendCommand(CMD_Mass_identify_media);
	}

	return false; // Keep processing it
}

//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::JukeboxManager_GridRendering(CallBackData *pData)
{
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	if( pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Jukebox_Drives_CONST || pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Jukebox_Slots_CONST )
	{
		// Iterate through all the cells
		for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
		{
			DataGridCell *pCell = it->second;
			string sEK_Device_Ripping = pCell->m_mapAttributes_Find("EK_Device_Ripping");
			string sPK_File = pCell->m_mapAttributes_Find("PK_File");
			if( sEK_Device_Ripping.empty()==false || sPK_File.empty()==false )
			{
				pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
				if(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size() != 0)
					colRow.second = colRow.second % int(pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.size());

				// See if there is an object assigned for this column/row
				map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
				if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
				{
					DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
					DesignObj_DataList::iterator iHao;

					// Iterate through all the object's children
					for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
					{
						DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
						if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoRip_CONST )
							pDesignObj_Orbiter->m_GraphicToDisplay_set("dogr", sEK_Device_Ripping.empty()==false ? 2 : 1);
					}
				}
			}
		}
	}
	return false;
}

void ScreenHandler::SCREEN_NAS_Manager(long PK_Screen, int iPK_Device)
{
	ScreenHandlerBase::SCREEN_NAS_Manager(PK_Screen,iPK_Device);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::NASManager_ObjectSelected,	new ObjectInfoBackData());
}

bool ScreenHandler::NASManager_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_icoEject_CONST && 
		pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject &&
		pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		int Row=-1,Col=-1;
		DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject;
		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator it=pObj_Grid->m_mapChildDgObjects.begin();it!=pObj_Grid->m_mapChildDgObjects.end();++it)
		{
			if( it->second == pObjectInfoData->m_pObj->m_pParentObject )
			{
				Col=it->first.first;
				Row=it->first.second;
				break;
			}
		}

		if( Row!=-1 && Col!=-1 )
		{
			DataGridTable *pDataGridTable = pObj_Grid->m_pDataGridTable_Current_get();
			DataGridCell *pCell = pDataGridTable->GetData(Col,Row);
			if( pCell && pCell->GetValue() )
			{
				// toodo
			}
		}
	}
	return false; // Keep processing it
}

void ScreenHandler::SCREEN_PendingTasks(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_PendingTasks(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::PendingTasks_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::PendingTasks_GridRendering,	new DatagridAcquiredBackData());
}

bool ScreenHandler::PendingTasks_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the 'abort' icon and remove it if we can't abort
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	// Iterate through all the cells
	for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
	{
		DataGridCell *pCell = it->second;
		string sAbort = pCell->m_mapAttributes_Find("Abort");

		pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell
		if( colRow.second == pDatagridAcquiredBackData->m_pDataGridTable->m_iDownRow || colRow.second == pDatagridAcquiredBackData->m_pDataGridTable->m_iUpRow )
			continue;

		colRow.second -= pDatagridAcquiredBackData->m_pObj->m_GridCurRow;
		colRow.first -= pDatagridAcquiredBackData->m_pObj->m_GridCurCol;

		// See if there is an object assigned for this column/row
		map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
		if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
		{
			DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
			DesignObj_DataList::iterator iHao;

			// Iterate through all the object's children
			for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
			{
				DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_iconAbort_CONST )
					pDesignObj_Orbiter->m_bHidden = sAbort!="1";
			}
		}
	}
	return false;
}

bool ScreenHandler::PendingTasks_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_iconAbort_CONST && 
		pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject &&
		pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		int Row=-1,Col=-1;
		DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObjectInfoData->m_pObj->m_pParentObject->m_pParentObject;
		for(map< pair<int,int>, DesignObj_Orbiter *>::iterator it=pObj_Grid->m_mapChildDgObjects.begin();it!=pObj_Grid->m_mapChildDgObjects.end();++it)
		{
			if( it->second == pObjectInfoData->m_pObj->m_pParentObject )
			{
				Col=it->first.first;
				Row=it->first.second;
				break;
			}
		}

		if( Row!=-1 && Col!=-1 )
		{
			DataGridTable *pDataGridTable = pObj_Grid->m_pDataGridTable_Current_get();
			if( pDataGridTable )
			{
				DataGridCell *pCell = pDataGridTable->GetData(Col,Row);
				if( pCell && pCell->GetValue() )
				{
					int PK_Device = atoi(pCell->m_mapAttributes["PK_Device"].c_str());
					DCE::CMD_Abort_Task CMD_Abort_Task(m_pOrbiter->m_dwPK_Device,PK_Device,atoi(pCell->GetValue()));
					m_pOrbiter->SendCommand(CMD_Abort_Task);
				}
			}
		}
	}
	return false; // Keep processing it
}

void ScreenHandler::SCREEN_AutoConfigure_TV(long PK_Screen, int iPK_PnpQueue)
{
	// Temporarily remove the message from displaying on the orbiters so we can go directly to the providers screen!
	DeviceData_Base *pMythTV_Plugin = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate( DEVICETEMPLATE_MythTV_PlugIn_CONST );

	string sUseAutomatically;
	string sDontAutoConfigure;

	int iPk_MythTV_Plugin = pMythTV_Plugin ? pMythTV_Plugin->m_dwPK_Device : 0;

	DCE::CMD_Get_Device_Data CMD_Get_Device_Data(
		0,
		DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST,
		iPk_MythTV_Plugin,
		DEVICEDATA_Dont_Auto_Configure_CONST,
		true,
		&sDontAutoConfigure
		);
	m_pOrbiter->SendCommand(CMD_Get_Device_Data);

	if (sDontAutoConfigure=="0") {
		sUseAutomatically="1";
	} else {
		sUseAutomatically="0";
	}

	DCE::CMD_Set_Pnp_Options CMD_Set_Pnp_Options(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn, sUseAutomatically, DEVICEDATA_Use_Automatically_CONST, iPK_PnpQueue);
	m_pOrbiter->SendCommand(CMD_Set_Pnp_Options);
	return;

	/* OLD CODE WHICH DISPLAYED THE CONFUSING METHOD
	DeviceData_Base *pDevice = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate( DEVICETEMPLATE_MythTV_PlugIn_CONST );
	string sSetPnpOptions = 
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " 1 " 
		TOSTRING(COMMAND_Set_Pnp_Options_CONST)  " " 
		TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST)  " " + StringUtils::itos(iPK_PnpQueue) + " " 
		TOSTRING(COMMANDPARAMETER_PK_DeviceData_CONST) " " TOSTRING(DEVICEDATA_Use_Automatically_CONST) " "
		TOSTRING(COMMANDPARAMETER_Value_To_Assign_CONST) " ";

	string sManualConfig = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn) + " 1 " 
		TOSTRING(COMMAND_Set_Device_Data_CONST) " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " " + StringUtils::itos(pDevice ? pDevice->m_dwPK_Device : 0) + " "
		TOSTRING(COMMANDPARAMETER_PK_DeviceData_CONST) " " TOSTRING(DEVICEDATA_Dont_Auto_Configure_CONST) " "
		TOSTRING(COMMANDPARAMETER_Value_To_Assign_CONST) " ";

	string sMessage = m_pOrbiter->m_bNewOrbiter ? m_pOrbiter->m_mapTextString[TEXT_Confirm_PNP_TV_Tuner_1stRegen_CONST] : m_pOrbiter->m_mapTextString[TEXT_Confirm_PNP_TV_Tuner_CONST];

	DisplayMessageOnOrbiter(PK_Screen, sMessage, false, "", false,
		m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sSetPnpOptions + "1 & " + sManualConfig + "0",
		m_pOrbiter->m_mapTextString[TEXT_NO_CONST],sSetPnpOptions + "0 & " + sManualConfig + "1");
	*/
}

void ScreenHandler::SCREEN_Remote_Assistance(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,"");
	char *pData=NULL;
	int size=0;
	DCE::CMD_Request_File CMD_Request_File(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,"/etc/pluto.conf",
		&pData,&size);
	m_pOrbiter->SendCommand(CMD_Request_File);
	if( pData==NULL )
	{
		DisplayMessageOnOrbiter(PK_Screen, "Unknown error getting conf file");
		return;
	}

	pData[size]=0;
	DCEConfig config(pData,size);
	string sRA = config.m_mapParameters_Find("remote");

	ScreenHandlerBase::SCREEN_Remote_Assistance(PK_Screen);  // Before the show object
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0." TOSTRING(DESIGNOBJ_butRAEnable_CONST),0,"","",sRA.empty() ? "1" : "0");
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0." TOSTRING(DESIGNOBJ_butRADisable_CONST),0,"","",sRA.empty() ? "0" : "1");

	if( sRA.empty()==false )
	{
		string sInstallation = config.m_mapParameters_Find("PK_Installation");
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0","Remote assistance enabled.  Give the support rep the code: \n" + sInstallation + "-" + sRA,TEXT_STATUS_CONST);
	}
	else
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0","Remote assistance disabled.",TEXT_STATUS_CONST);

	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::SCREEN_Remote_Assistance_OnTimer, new CallBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::SCREEN_Remote_Assistance_ObjectSelected, new ObjectInfoBackData());
}

bool ScreenHandler::SCREEN_Remote_Assistance_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butRAEnable_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butRADisable_CONST )
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,StringUtils::itos(pObjectInfoData->m_pObj->m_iBaseObjectID));
		bool bSent=false;
		DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
		if( pDevice_Core )
		{
			DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfCategory(DEVICECATEGORY_App_Server_CONST);
			if( pDevice_AppServer )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/RA-handler.sh","remote assist",
					pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butRAEnable_CONST ? "--enable" : "--disable",
					"","",false,false,false,true);
				bSent = m_pOrbiter->SendCommand(CMD_Spawn_Application);
			}
		}

		if( !bSent )
		{
			DisplayMessageOnOrbiter(SCREEN_Remote_Assistance_CONST, "Unknown error getting contacting app server");
			return false;
		}
		else
		{
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0." TOSTRING(DESIGNOBJ_butRAEnable_CONST),0,"","","0");
			m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0." TOSTRING(DESIGNOBJ_butRADisable_CONST),0,"","","0");
			m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuRemoteAssistance_CONST) ".0.0","One moment...",TEXT_STATUS_CONST);
			m_pOrbiter->StartScreenHandlerTimer(2000);
		}
	}
	return false; // Keep processing it
}

bool ScreenHandler::SCREEN_Remote_Assistance_OnTimer(CallBackData *pData)
{
	string sName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);

	char *pBuffer=NULL;
	int size=0;
	DCE::CMD_Request_File CMD_Request_File(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,"/etc/pluto.conf",
		&pBuffer,&size);
	m_pOrbiter->SendCommand(CMD_Request_File);
	if( pBuffer==NULL )
	{
		DisplayMessageOnOrbiter(SCREEN_Remote_Assistance_CONST, "Unknown error getting conf file");
		return false;
	}
	pBuffer[size]=0;

	DCEConfig config(pBuffer,size);
	string sRA = config.m_mapParameters_Find("remote");

	if( (atoi(sName.c_str())==DESIGNOBJ_butRAEnable_CONST && sRA.empty()==false)
		|| (atoi(sName.c_str())==DESIGNOBJ_butRADisable_CONST && sRA.empty()==true) )
	{
		// We're good 
		NeedToRender render2( m_pOrbiter, "ScreenHandler::SCREEN_Remote_Assistance_OnTimer" );
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Remote_Assistance_CONST,0,false,false);
		return false;
	}
	return true;
}

void ScreenHandler::SCREEN_AdjustScreenSettings(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_AdjustScreenSettings(PK_Screen);  

	m_ScreenOffset = AdjustScreenSettings_LoadOffset();
	m_nScreenSpacing = AdjustScreenSettings_LoadSpacing();

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::AdjustScreenSettings_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::AdjustScreenSettings_OnTimer, new CallBackData());
	RegisterCallBack(cbOnKeyDown, (ScreenHandlerCallBack) &ScreenHandler::AdjustScreenSettings_KeyDown, new KeyCallBackData());
	RegisterCallBack(cbOnRemoteKeyCodeIntercepted, (ScreenHandlerCallBack) &ScreenHandler::AdjustScreenSettings_RemoteKeyCodeIntercepted, new RemoteKeyCodeCallBackData());

	m_pOrbiter->StartScreenHandlerTimer(500);
}

void ScreenHandler::AdjustScreenSettings_DrawArrows()
{
	PlutoPoint spacing_offset(
		m_nScreenSpacing * m_pOrbiter->m_iImageWidth / (2 * 100),
		m_nScreenSpacing * m_pOrbiter->m_iImageHeight / (2 * 100)
	);

	PlutoPoint center_point(
		(100 - m_nScreenSpacing) * m_pOrbiter->m_iImageWidth / (2 * 100), 
		(100 - m_nScreenSpacing) * m_pOrbiter->m_iImageHeight / (2 * 100)
	);

	PlutoSize arrow_size(24, 40);
	PlutoPoint total_offset = PlutoPoint(m_ScreenOffset.X + spacing_offset.X, m_ScreenOffset.Y + spacing_offset.Y);
		
	m_pOrbiter->Renderer()->DrawArrow(center_point.operator +(total_offset), total_offset, arrow_size, PlutoColor::Red(), "", "arrow1");
	m_pOrbiter->Renderer()->DrawArrow(center_point.operator +(total_offset), total_offset.operator +(PlutoPoint(m_pOrbiter->m_iImageWidth * (100 - m_nScreenSpacing) / 100, m_pOrbiter->m_iImageHeight * (100 - m_nScreenSpacing) / 100)), arrow_size, PlutoColor::Red(), "", "arrow2");
	m_pOrbiter->Renderer()->DrawArrow(center_point.operator +(total_offset), total_offset.operator +(PlutoPoint(0, m_pOrbiter->m_iImageHeight * (100 - m_nScreenSpacing) / 100)), arrow_size, PlutoColor::Red(), "", "arrow3");
	m_pOrbiter->Renderer()->DrawArrow(center_point.operator +(total_offset), total_offset.operator +(PlutoPoint(m_pOrbiter->m_iImageWidth * (100 - m_nScreenSpacing) / 100, 0)), arrow_size, PlutoColor::Red(), "", "arrow4");
}

bool ScreenHandler::AdjustScreenSettings_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	if(NULL != pObjectInfoData && pObjectInfoData->m_pObj->m_iBaseObjectID == DESIGNOBJ_butConfirm_CONST)
	{
		AdjustScreenSettings_SaveOffset(m_ScreenOffset);
		AdjustScreenSettings_SaveSpacing(m_nScreenSpacing);

		CMD_Regen_Orbiter cmd_Regen_Orbiter(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_OrbiterPlugIn,
			m_pOrbiter->m_dwPK_Device, "", "");
		m_pOrbiter->SendCommand(cmd_Regen_Orbiter);

		m_pOrbiter->GotoMainMenu();
	}
	return false; // Keep processing it
}

bool ScreenHandler::AdjustScreenSettings_KeyDown(CallBackData *pData)
{
	KeyCallBackData *pKeyCallBackData = dynamic_cast<KeyCallBackData *>(pData);

	if(NULL == pKeyCallBackData)
		return false;

	if(pKeyCallBackData->m_nPlutoKey == BUTTON_Up_Arrow_CONST)
	{
		m_ScreenOffset.Y -= 5;
	}
	else if(pKeyCallBackData->m_nPlutoKey == BUTTON_Down_Arrow_CONST)
	{
		m_ScreenOffset.Y += 5;
	}
	else if(pKeyCallBackData->m_nPlutoKey == BUTTON_Left_Arrow_CONST)
	{
		m_ScreenOffset.X -= 5;
	}
	else if(pKeyCallBackData->m_nPlutoKey == BUTTON_Right_Arrow_CONST)
	{
		m_ScreenOffset.X += 5;
	}
	else if(pKeyCallBackData->m_nPlutoKey == BUTTON_plus_CONST)
	{
		m_nScreenSpacing--;
	}
	else if(pKeyCallBackData->m_nPlutoKey == BUTTON_dash_CONST)
	{
		m_nScreenSpacing++;
	}
	else
	{
		return false;
	}

	if(m_ScreenOffset.X < 0)
		m_ScreenOffset.X = 0;

	if(m_ScreenOffset.Y < 0)
		m_ScreenOffset.Y = 0;

	if(m_nScreenSpacing < 0)
		m_nScreenSpacing = 0;

	if(m_nScreenSpacing > 80)
		m_nScreenSpacing = 80;

	AdjustScreenSettings_DrawArrows();
	return true;
}

bool ScreenHandler::AdjustScreenSettings_OnTimer(CallBackData *pData)
{
	AdjustScreenSettings_DrawArrows();
	return false;
}

PlutoPoint ScreenHandler::AdjustScreenSettings_LoadOffset()
{
	string sOffset = m_pOrbiter->m_pEvent->GetDeviceDataFromDatabase(m_pOrbiter->m_dwPK_Device, DEVICEDATA_Offset_CONST);

	vector<string> vectStrings;
	StringUtils::Tokenize(sOffset, ",", vectStrings);

	PlutoPoint offset;
	if(vectStrings.size() >= 2)
	{
		offset.X = atoi(vectStrings[0].c_str());
		offset.Y = atoi(vectStrings[1].c_str());
	}

	return offset;
}

void ScreenHandler::AdjustScreenSettings_SaveOffset(PlutoPoint offset)
{
	m_pOrbiter->DATA_Set_Offset(StringUtils::ltos(offset.X) + "," + StringUtils::ltos(offset.Y), true);
}

int ScreenHandler::AdjustScreenSettings_LoadSpacing()
{
	string sSpacing = m_pOrbiter->m_pEvent->GetDeviceDataFromDatabase(m_pOrbiter->m_dwPK_Device, DEVICEDATA_Spacing_CONST);
	return atoi(sSpacing.c_str());
}

void ScreenHandler::AdjustScreenSettings_SaveSpacing(int nValue)
{
	m_pOrbiter->DATA_Set_Spacing(StringUtils::ltos(nValue), true);
}

bool ScreenHandler::AdjustScreenSettings_RemoteKeyCodeIntercepted(CallBackData *pData)
{
	RemoteKeyCodeCallBackData *pRemoteKeyCodeCallBackData = dynamic_cast<RemoteKeyCodeCallBackData *>(pData);

	if(NULL == pRemoteKeyCodeCallBackData)
		return false;

	string sKeyCode = pRemoteKeyCodeCallBackData->m_sKeyCode;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "TEST AdjustScreenSettings_RemoteKeyCodeIntercepted %s", sKeyCode.c_str());

	sKeyCode = StringUtils::ToLower(sKeyCode);

	if(sKeyCode == "volup" || sKeyCode == "chup")
		m_pOrbiter->ButtonDown(BUTTON_plus_CONST);
	else if(sKeyCode == "voldn" || sKeyCode == "chdown")
		m_pOrbiter->ButtonDown(BUTTON_dash_CONST);
	else 
		return false;
		
	return true;
}

/*virtual */ void ScreenHandler::SCREEN_PNP_Generic_Options(long PK_Screen, string sOptions, int iPK_PnpQueue)
{ 
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgGenericDataGrid_CONST));
	if( !pObj )
		return; // shouldn't happen
	
	DesignObj_DataGrid *pObj_Grid = dynamic_cast<DesignObj_DataGrid *> (pObj);
	pObj_Grid->m_bFlushOnScreen=false;
	pObj_Grid->m_GridCurRow = pObj_Grid->m_GridCurCol = 0;
	DataGridTable *pDataGridTable = new DataGridTable();
	pObj_Grid->DataGridTable_Set(pDataGridTable,0,0);
	DataGridCell *pCell;

	vector<string> vectOptions;
	StringUtils::Tokenize(sOptions,"\t",vectOptions);
	if( vectOptions.size()<4 )
		return; // Shouldn't happen

	int iRow=0;
	vector<string>::iterator it=vectOptions.begin();
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,*it);  // The PK_DeviceData
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,StringUtils::itos(iPK_PnpQueue));  // The PK_PnpQueu
	it++;
	string sTitle = m_pOrbiter->m_mapTextString[atoi(it->c_str())];
	it++;
	for(;it!=vectOptions.end();++it)
	{
		string sText = m_pOrbiter->m_mapTextString[atoi(it->c_str())];
		++it;
		string sValue = *it;

		pCell = new DataGridCell(sText,sValue);
		pObj_Grid->DataGridTable_Get()->SetData(0,iRow++,pCell);
	}
	
	pDataGridTable->m_RowCount = pDataGridTable->GetRows();
	pDataGridTable->m_ColumnCount = pDataGridTable->GetCols();
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::PNP_Generic_Options_DatagridSelected, new DatagridCellBackData());
	m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST),sTitle, TEXT_STATUS_CONST );
	ScreenHandlerBase::SCREEN_PNP_Generic_Options(PK_Screen, sOptions, iPK_PnpQueue);
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_butResponse1_CONST),0,"","","0");
}

bool ScreenHandler::PNP_Generic_Options_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::PNP_Generic_Options_DatagridSelected sel value %s text %s",
			pCellInfoData->m_sValue.c_str(),pCellInfoData->m_sText.c_str());

#endif
	DCE::CMD_Set_Pnp_Options CMD_Set_Pnp_Options(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn,
		pCellInfoData->m_sValue,atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str()),atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST).c_str()));
	m_pOrbiter->SendCommand(CMD_Set_Pnp_Options);

	m_pOrbiter->CMD_Remove_Screen_From_History(m_pOrbiter->m_pScreenHistory_Current->ScreenID(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());

	return true;  // Always return true since we're handing everything datagrid related here
}

void MediaFileBrowserOptions::ReacquireGrids()
{
	m_pOrbiter->Renderer()->UnHighlightObject();

	if( m_pObj_ListGrid )
	{
		if( !m_pObj_ListGrid->m_bParsed )
			m_pOrbiter->ParseGrid(m_pObj_ListGrid);
		m_pObj_ListGrid->m_GridCurCol = m_pObj_ListGrid->m_iInitialColNum;
		m_pObj_ListGrid->m_GridCurRow = m_pObj_ListGrid->m_iInitialRowNum;
		m_pObj_ListGrid->m_bFlushOnScreen=false;
		m_pObj_ListGrid->Flush();
	}
	if( m_pObj_PicGrid )
	{
		if( !m_pObj_PicGrid->m_bParsed )
			m_pOrbiter->ParseGrid(m_pObj_PicGrid);
		m_pObj_PicGrid->m_GridCurCol = m_pObj_PicGrid->m_iInitialColNum;
		m_pObj_PicGrid->m_GridCurRow = m_pObj_PicGrid->m_iInitialRowNum;
		m_pObj_PicGrid->m_bFlushOnScreen=false;
		m_pObj_PicGrid->Flush();
	}
}

void ScreenHandler::SCREEN_AdvancedOptions(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_AdvancedOptions(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::AdvancedOptions_ObjectSelected,	new ObjectInfoBackData());
}

bool ScreenHandler::AdvancedOptions_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butStartAVwizard_CONST )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::AdvancedOptions_ObjectSelected");
		int dwPK_Device = m_pOrbiter->m_dwPK_Device;
		// Check what room the orbiter is controlling at the moment
		if ( m_pOrbiter->m_pLocationInfo && m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
			dwPK_Device = m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector;
		DCE::CMD_Halt_Device CMD_Halt_Device(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			dwPK_Device,"A","");
		m_pOrbiter->SendCommand(CMD_Halt_Device);
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallPhonebook(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Users_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_MakeCallPhonebook(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallFavorites(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Users_CONST, "");

	ScreenHandlerBase::SCREEN_MakeCallFavorites(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallDialNumber(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Users_CONST, "");

	ScreenHandlerBase::SCREEN_MakeCallDialNumber(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallPlutoUser(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

	ScreenHandlerBase::SCREEN_MakeCallPlutoUser(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallIntercom(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Users_CONST, "");

	ScreenHandlerBase::SCREEN_MakeCallIntercom(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Active_Calls(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Active_Calls(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::Telecom_DataGridRendering, new DatagridAcquiredBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::Telecom_OnTimer, new CallBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_DatagridSelected, new DatagridCellBackData());

	m_pOrbiter->StartScreenHandlerTimer(2000);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SCREEN_Active_Calls my call %s", m_sMyCallID.c_str());

	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuActiveCalls_CONST) ".0.0." TOSTRING(DESIGNOBJ_butCallInProgress_CONST), 0, "", "", 
		m_sMyCallID.empty() ? "0" : "1");

	RefreshActiveCallsButtons();
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_MakeCallDevice(long PK_Screen)
{
	if(m_TelecomCommandStatus == tcsDirectDial)
	{
		ScreenHandlerBase::SCREEN_MakeCallDevice(PK_Screen);
		RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
	}
	else
	{
		string sPK_Users = m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Users_CONST);
		string sPhoneExtension = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
		string sPK_DeviceTo = m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_2_CONST);

		string sSecondPhoneCall;
		if(sPK_Users.empty() && sPhoneExtension.empty() && sPK_DeviceTo.empty())
			sSecondPhoneCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);

		HandleAssistedMakeCall(atoi(sPK_Users.c_str()), sPhoneExtension, 
			m_pOrbiter->m_dwPK_Device, atoi(sPK_DeviceTo.c_str()), sSecondPhoneCall);
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DevCallInProgress(long PK_Screen, string sPhoneCallerID, string sPhoneCallID, 
	string sChannel)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SCREEN_DevCallInProgress called with caller id %s, call id %s, channel %s",
		sPhoneCallerID.c_str(), sPhoneCallID.c_str(), sChannel.c_str());

	if(!sPhoneCallerID.empty())
		m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Caller_ID_CONST, sPhoneCallerID);

	if(!sPhoneCallID.empty())
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Call_ID_CONST, sPhoneCallID);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Current_Call_CONST, sPhoneCallID);
		m_sMyCallID = sPhoneCallID;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SCREEN_DevCallInProgress : my call is now %s", m_sMyCallID.c_str());
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SCREEN_DevCallInProgress : phone call id is empty!");
	}

	if(!sChannel.empty())
		m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Channel_ID_CONST, sChannel);

	if(
		m_pOrbiter->m_pScreenHistory_Current->PK_Screen() == SCREEN_Assisted_Transfer_In_Progress_CONST ||
		m_pOrbiter->m_pScreenHistory_Current->PK_Screen() == SCREEN_MakingLinPhoneBroadcast_CONST
	)
	{
		//don't go to call in progress
		return;
	}

	ScreenHandlerBase::SCREEN_DevCallInProgress(PK_Screen, sPhoneCallerID, sPhoneCallID, sChannel);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::Telecom_OnTimer, new CallBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, (ScreenHandlerCallBack) &ScreenHandler::Telecom_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
	RegisterCallBack(cbOnCustomRender, (ScreenHandlerCallBack) &ScreenHandler::Telecom_CustomRender, new RenderScreenCallBackData());

	m_pOrbiter->StartScreenHandlerTimer(2000);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DevIncomingCall(long PK_Screen, string sPhoneNumber, string sSource_Channel, 
	string sDestination_Channel, string sSource_Caller_ID, string sDestination_Caller_ID)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Source_Channel_ID_CONST, sSource_Channel);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Source_Caller_ID_CONST, sSource_Caller_ID);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Caller_number_CONST, sPhoneNumber);

	m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Channel_ID_CONST, sDestination_Channel);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Caller_ID_CONST, sDestination_Caller_ID);

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

	if(m_pOrbiter->m_pScreenHistory_Current->PK_Screen() == SCREEN_Assisted_Transfer_In_Progress_CONST)
	{
		//don't go to call in progress
		return;
	}
	
	ScreenHandlerBase::SCREEN_DevIncomingCall(PK_Screen, sPhoneNumber, sSource_Channel, sDestination_Channel,
		sSource_Caller_ID, sDestination_Caller_ID);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Telecom_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnCustomRender, (ScreenHandlerCallBack) &ScreenHandler::Telecom_CustomRender, new RenderScreenCallBackData());

	//we can only do a blind transfer from here
	m_TelecomCommandStatus = tcsBlindTransfer;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Call_Dropped(long PK_Screen, string sReason)
{
  m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Channel_ID_CONST, "");
  m_pOrbiter->CMD_Set_Variable(VARIABLE_My_Call_ID_CONST, "");
  m_sMyCallID.clear();
  
  LoggerWrapper::GetInstance()->Write(LV_STATUS, "SCREEN_Call_Dropped : my call is now %s", m_sMyCallID.c_str());
  
  // If something was playing, we go back to the now playing remote,
  // Otherwise, we bounce back to main. 
  if( m_pOrbiter->m_sNowPlaying.length())
    {
      m_pOrbiter->CMD_Goto_Screen("",SCREEN_CurrentlyActiveRemote_CONST);
    }
  else
    {
      m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
    }
  
  m_TelecomCommandStatus = tcsDirectDial;
  m_pOrbiter->CMD_Display_Alert("Call dropped. Reason: " + sReason, "", "5", interuptAlways);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Assisted_Transfer_In_Progress(long PK_Screen, bool bTrueFalse, string sTask)
{
	bool bTransferToCall = bTrueFalse;

	string sDescription = "Transfering, please wait...";
	
	string sProcessTask =
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + 
		StringUtils::itos(m_pOrbiter->m_dwPK_Device_TelecomPlugIn) + " " + 	
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + 
		StringUtils::itos(COMMAND_Process_Task_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_Task_CONST) + " " + sTask + " " + 
		StringUtils::itos(COMMANDPARAMETER_Job_CONST) + " ";

	//Drop transfer and CMD_PL_Join A to {X,Y}:
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
	string sButton1 = "Complete transfer now";
	string sCommand1 = sProcessTask + "<%=#34%>transfer<%=#34%>";

	//Drop transfer and CMD_PL_Join B to {X,Y}
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A,B,C} 
	string sButton2 = bTransferToCall ? "Merge calls" : "Conference";
	//TODO: transfer A si B ?
	string sCommand2 = sProcessTask + 
		(bTransferToCall ? "<%=#34%>merge calls<%=#34%>" : "<%=#34%>conference<%=#34%>");
	
	//Drop transfer and CMD_PL_Join A and B to {X,Y}
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A} & B
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A} & {B,C} 
	string sButton3 = "Cancel transfer/conference";
	string sCommand3 =  sProcessTask + "<%=#34%>cancel<%=#34%>";

	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, "0", true,
		sButton1, sCommand1, sButton2, sCommand2, sButton3, sCommand3, "", "");
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_DataGridRendering(CallBackData *pData)
{
	DatagridAcquiredBackData *pDatagridAcquiredBackData = dynamic_cast<DatagridAcquiredBackData *>(pData);

	if(NULL != pDatagridAcquiredBackData)
	{
		if(pDatagridAcquiredBackData->m_pObj->m_iPK_Datagrid==DATAGRID_Active_Channels_CONST)
		{
			list<string> listCalls;

			for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
			{
				DataGridCell *pCell = it->second;
				listCalls.push_back(pCell->m_Value);
			}

			string sCurrentCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);
			list<string>::iterator it_found = std::find(listCalls.begin(), listCalls.end(), sCurrentCall);

			if(it_found == listCalls.end() || listCalls.empty())
			{
				string sValue;
				if(!listCalls.empty())
					sValue = *listCalls.begin();

				m_pOrbiter->CMD_Set_Variable(VARIABLE_Current_Call_CONST, sValue);

				DesignObj_DataGrid *pObj = (DesignObj_DataGrid *)m_pOrbiter->FindObject(DESIGNOBJ_dgActiveUsers_CONST);
				m_pOrbiter->InitializeGrid(pObj);
				pObj->Flush();
				m_pOrbiter->Renderer()->RenderObjectAsync(pObj);
				NeedToRender render(m_pOrbiter, "Telecom_DataGridRendering");
			}
		}
	}

	RefreshActiveCallsButtons();
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHandler::MediaBrowser_DatagridSelected sel value %s text %s row %d col %d cell value %s",
		pCellInfoData->m_sValue.c_str(),pCellInfoData->m_sText.c_str(),
		pCellInfoData->m_Row,pCellInfoData->m_Column,
		pCellInfoData->m_pDataGridCell && pCellInfoData->m_pDataGridCell->m_Value ? pCellInfoData->m_pDataGridCell->m_Value : "*NONE*");

#endif

	if(NULL != pCellInfoData && NULL != pCellInfoData->m_pDesignObj_DataGrid &&
		pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgActiveCalls_CONST)
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Current_Call_CONST, pCellInfoData->m_sValue);
		RefreshActiveCallsButtons();
	}

	return false; //don't stop here
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);

	bool bDontProcessIt = false;

	if(NULL != pObjectInfoData)
	{
		int nPK_DesignObj_Parent = GetCurrentScreen_PK_DesignObj();
		
		if(nPK_DesignObj_Parent == DESIGNOBJ_devCallInProgress_CONST || nPK_DesignObj_Parent == DESIGNOBJ_mnuAudioCallInProgress_CONST)
		{
			switch(pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butTransferConference_CONST:
				bDontProcessIt = CallInProgress_TransferConference();
				break;

				case DESIGNOBJ_objVoiceMail_CONST:
				bDontProcessIt = CallInProgress_VoiceMail();
				break;
			}
		}
		else if(nPK_DesignObj_Parent == DESIGNOBJ_mnuActiveCalls_CONST)
		{
			switch(pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butJoin_CONST:
				bDontProcessIt = ActiveCalls_Join();
				break;

				case DESIGNOBJ_butAddToActiveCall_CONST:
				bDontProcessIt = ActiveCalls_AddToActiveCall();
				break;

				case DESIGNOBJ_butRemoveFromActiveCall_CONST:
				bDontProcessIt = ActiveCalls_RemoveFromActiveCall();
				break;

				case DESIGNOBJ_butCallInProgress_CONST:
				bDontProcessIt = ActiveCalls_CallInProgress();
				break;
			}
		}
		else if(nPK_DesignObj_Parent == DESIGNOBJ_mapTelecom_CONST)
		{
			switch(pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butCallInProgress_CONST:
					bDontProcessIt = ActiveCalls_CallInProgress();
					break;
			}
		}
	}

	return bDontProcessIt; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_OnTimer(CallBackData *pData)
{
	DesignObj_DataGrid *pObj_Calls = (DesignObj_DataGrid *)m_pOrbiter->FindObject(DESIGNOBJ_dgActiveCalls_CONST);
	DesignObj_DataGrid *pObj_Channels = (DesignObj_DataGrid *)m_pOrbiter->FindObject(DESIGNOBJ_dgActiveUsers_CONST);
	if(NULL != pObj_Calls)
	{
		//old highlightings
		int nHighlightRow = pObj_Calls->m_iHighlightedRow;
		int nHighlightCol = pObj_Calls->m_iHighlightedColumn;

		m_pOrbiter->InitializeGrid(pObj_Calls);
		pObj_Calls->Flush();

		pObj_Calls->m_iHighlightedRow = nHighlightRow;
		pObj_Calls->m_iHighlightedColumn = nHighlightCol;

		m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Calls);
	}
	
	if(NULL != pObj_Channels)
	{
		//old highlightings
		int nHighlightRow = pObj_Channels->m_iHighlightedRow;
		int nHighlightCol = pObj_Channels->m_iHighlightedColumn;

		m_pOrbiter->InitializeGrid(pObj_Channels);
		pObj_Channels->Flush();

		pObj_Channels->m_iHighlightedRow = nHighlightRow;
		pObj_Channels->m_iHighlightedColumn = nHighlightCol;

		m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Channels);
	}

	if(NULL != pObj_Calls || NULL != pObj_Channels)
	{
		NeedToRender render(m_pOrbiter, "Telecom_OnTimer");
		return true;
	}

	RefreshActiveCallsButtons();
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case (DESIGNOBJ_devCallInProgress_CONST || DESIGNOBJ_mnuAudioCallInProgress_CONST):
		{
			string sBuffer = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
	
			if(!sBuffer.empty())
			{
				string sButton = sBuffer.substr(sBuffer.length() - 1);
				CMD_Simulate_Keypress cmd_Simulate_Keypress(
					m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
					sButton, 0, "");
				m_pOrbiter->SendCommand(cmd_Simulate_Keypress);
			}

			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Telecom_CustomRender(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = dynamic_cast<RenderScreenCallBackData *>(pData);
	
	if(NULL != pRenderScreenCallBackData)
	{
		DesignObj_Orbiter *pObj = pRenderScreenCallBackData->m_pObj;
		PlutoRectangle rectTotal = pObj->m_rPosition;

		if(
			pObj->m_iBaseObjectID == DESIGNOBJ_objIncomingCallUser_CONST || 
			pObj->m_iBaseObjectID == DESIGNOBJ_objOutgoingCallUser_CONST
		)
		{
			string sChannel = 
				pObj->m_iBaseObjectID == DESIGNOBJ_objOutgoingCallUser_CONST ? 
				m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Channel_ID_CONST) :
				m_pOrbiter->m_mapVariable_Find(VARIABLE_Source_Channel_ID_CONST);

			char *pData = NULL;
			int nSize = 0;
			CMD_Get_Associated_Picture_For_Channel cmd_Get_Associated_Picture_For_Channel(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
				sChannel, &pData, &nSize);
			m_pOrbiter->SendCommand(cmd_Get_Associated_Picture_For_Channel);

			if(NULL != pData && 0 != nSize)
			{
				m_pOrbiter->Renderer()->UpdateObjectImage(pObj->m_ObjectID, "png", pData, nSize, "1", false);
				pObj->Renderer()->RenderGraphic(pObj->m_rPosition, pObj->m_bDisableAspectLock);
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ActiveCalls_Join()
{
	string sMyChannel = m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Channel_ID_CONST);

	if(sMyChannel.empty())
	{
		m_TelecomCommandStatus = tcsJoin;

		if(m_pOrbiter->m_bIsOSD)
		{
			string sSecondPhoneCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);
			HandleAssistedMakeCall(0, "", m_pOrbiter->m_dwPK_Device, 0, sSecondPhoneCall);
		}
		else
		{
			//if not OSD, find a device to dial from
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
			GotoScreen(SCREEN_MakeCallDevice_CONST);
		}
	}
	else
	{
		string sSecondPhoneCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);

		//{X,Y,A} & {B,C} => {X,Y} & {A,B,C} 
		m_TelecomCommandStatus = tcsTransferConference;
		HandleAssistedMakeCall(0, "", m_pOrbiter->m_dwPK_Device, 0, sSecondPhoneCall);
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ActiveCalls_AddToActiveCall()
{
	m_TelecomCommandStatus = tcsJoin;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	GotoScreen(SCREEN_MakeCallDialNumber_CONST);
	
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ActiveCalls_RemoveFromActiveCall()
{
	string sSelectedChannel = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Channel_On_Call_CONST);

	DCE::CMD_PL_Cancel cmd_PL_Cancel(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
		0, sSelectedChannel);
	m_pOrbiter->SendCommand(cmd_PL_Cancel);
	
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ActiveCalls_CallInProgress()
{
	string sCurrentCall = m_sMyCallID;

	if(sCurrentCall.empty())
		sCurrentCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);

	string sPhoneCallerID = m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Caller_ID_CONST);
	string sChannel = m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Channel_ID_CONST);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ActiveCalls_CallInProgress my call %s", m_sMyCallID.c_str());
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ActiveCalls_CallInProgress calling SCREEN_DevCallInProgress with "
		"caller id %s, call id %s, channel %s",
		sPhoneCallerID.c_str(), m_sMyCallID.c_str(), sChannel.c_str());

	SCREEN_DevCallInProgress(SCREEN_DevCallInProgress_CONST, sPhoneCallerID, m_sMyCallID, sChannel);

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::CallInProgress_TransferConference()
{
	m_TelecomCommandStatus = tcsTransferConference;
	GotoScreen(SCREEN_MakeCallDialNumber_CONST);
	
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::CallInProgress_VoiceMail()
{
	int iPK_Device = 0;
	int iPK_Users = 0;
	string sPhoneExtension = "100"; //TODO, hardcoding
	string sChannel_1 = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Channel_On_Call_CONST);
	string sChannel_2;

	CMD_PL_Transfer cmd_PL_Transfer(
		m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
		iPK_Device, iPK_Users, sPhoneExtension, sChannel_1, sChannel_2);
	m_pOrbiter->SendCommand(cmd_PL_Transfer);

	return true;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::HandleAssistedMakeCall(int iPK_Users,string sPhoneExtension,int iPK_Device_From,
										   int iPK_Device_To, string sSecondPhoneCall)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "HandleAssistedMakeCall: "
		"iPK_Users %d, sPhoneExtension %s, iPK_Device_From %d, iPK_Device_To %d",
		iPK_Users, sPhoneExtension.c_str(), iPK_Device_From, iPK_Device_To);

	string sResponse;
	string sMessage;

	switch(m_TelecomCommandStatus)
	{
		case tcsDirectDial:
		{
			// A & B => {A,B}

			DCE::CMD_Make_Call cmd_Make_Call(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
				iPK_Users, sPhoneExtension, StringUtils::itos(iPK_Device_From), iPK_Device_To);
			m_pOrbiter->SendCommand(cmd_Make_Call, &sResponse);

			if(sResponse != "OK")
			{
				sMessage = "Failed to make call!";
			}
			else
			{
				string sDescription = "Calling, please wait...";
				string sButton1 = "Cancel call";
				string sCommand1 = 
					StringUtils::itos(iPK_Device_From) + " " + 
					StringUtils::itos(m_pOrbiter->m_dwPK_Device_TelecomPlugIn) + " " + 	
					StringUtils::itos(MESSAGETYPE_COMMAND) + " " + 
					StringUtils::itos(COMMAND_Phone_Drop_CONST);

				SCREEN_PopupMessage(SCREEN_PopupMessage_CONST, 
					sDescription + "|" + sButton1, sCommand1, "calling", "0", "0", "1");
			}
		}
		break;

		case tcsJoin:
		{
			// {X,Y} & A => {X,Y,A}

			string sOptions;
			string sPhoneCallID = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);

			DCE::CMD_PL_Join_Call cmd_PL_Join_Call(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
				iPK_Users, sPhoneExtension, sPhoneCallID, iPK_Device_From);
			m_pOrbiter->SendCommand(cmd_PL_Join_Call, &sResponse);

			if(sResponse != "OK")
			{
				sMessage = "Failed to join!";
			}
			else
			{
				SCREEN_Active_Calls(SCREEN_Active_Calls_CONST);

				string sDescription = "Calling, please wait...";
				string sButton1 = "Cancel call";
				string sCommand1 = 
					StringUtils::itos(iPK_Device_From) + " " + 
					StringUtils::itos(m_pOrbiter->m_dwPK_Device_TelecomPlugIn) + " " + 	
					StringUtils::itos(MESSAGETYPE_COMMAND) + " " + 
					StringUtils::itos(COMMAND_Phone_Drop_CONST);

				SCREEN_PopupMessage(SCREEN_PopupMessage_CONST, 
					sDescription + "|" + sButton1, sCommand1, "calling", "0", "0", "1");
			}
		}
		break;

		case tcsBlindTransfer:
		{
			string sSourceChannel = m_pOrbiter->m_mapVariable_Find(VARIABLE_Source_Channel_ID_CONST);
			string sSourceCallerID = m_pOrbiter->m_mapVariable_Find(VARIABLE_Source_Caller_ID_CONST);

			DCE::CMD_PL_Transfer cmd_PL_Transfer(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
				iPK_Device_To, iPK_Users, sPhoneExtension, sSourceChannel, "");
			m_pOrbiter->SendCommand(cmd_PL_Transfer, &sResponse);

			if(sResponse != "OK")
			{
				sMessage = "Failed to transfer!";
			}
			else
			{
				string sDescription = "Transfering call from " + sSourceCallerID + ", please wait...";

				string sButton1 = "Cancel";
				string sCommand1 = 
					StringUtils::itos(iPK_Device_From) + " " + 
					StringUtils::itos(m_pOrbiter->m_dwPK_Device_TelecomPlugIn) + " " + 	
					StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_PL_Cancel_CONST) + " " +
					StringUtils::itos(COMMANDPARAMETER_Channel_CONST) + sSourceChannel; 

				SCREEN_PopupMessage(SCREEN_PopupMessage_CONST, 
					sDescription + "|" + sButton1, sCommand1, "transfering", "0", "0", "1");
			}
		}
		break;

		case tcsTransferConference:
		{
			//1) {X,Y,A} & B => {X,Y} & {A,B}
			//or 
			//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 

			string sMyChannel = m_pOrbiter->m_mapVariable_Find(VARIABLE_My_Channel_ID_CONST);
			string sMyCall = m_sMyCallID;

			string sTaskID;
			DCE::CMD_Assisted_Transfer cmd_Assisted_Transfer(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_TelecomPlugIn,
				iPK_Device_To, iPK_Users, sPhoneExtension, sSecondPhoneCall, sMyChannel, &sTaskID);
			m_pOrbiter->SendCommand(cmd_Assisted_Transfer, &sResponse);

			if(sResponse != "OK")
			{
				sMessage = "Failed to transfer!";
			}
			else
			{
				DCE::SCREEN_Assisted_Transfer_In_Progress screen_Assisted_Transfer_In_Progress(
					m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device,
					!sSecondPhoneCall.empty(), sTaskID);
				m_pOrbiter->SendCommand(screen_Assisted_Transfer_In_Progress);
			}
		}
		break;

		default:
			{
				sMessage = "Unknown telecom action: " +  StringUtils::ltos(m_TelecomCommandStatus) + "!";
				sResponse = "ERROR";
			}
			break;
	}

	if(sResponse != "OK")
	{
		SCREEN_PopupMessage(
			SCREEN_PopupMessage_CONST,  //screen id
			sMessage + ". " + sResponse + "|OK", //text
			"", //command line
			"error", //description
			"0", //prompt for reset
			"0", //without timeout
			"1"  //cannot go back
		);
	}

	//reset the status
	m_TelecomCommandStatus = tcsDirectDial;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RefreshActiveCallsButtons()
{
	string sCurrentCall = m_pOrbiter->m_mapVariable_Find(VARIABLE_Current_Call_CONST);
	string sMyCall = m_sMyCallID;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Current call %s, my call %s", sCurrentCall.c_str(), sMyCall.c_str());

	bool bActiveCallsPresent = !sCurrentCall.empty();
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuActiveCalls_CONST) ".0.0." TOSTRING(DESIGNOBJ_butJoin_CONST), 0, "", "", 
		(sCurrentCall != sMyCall && bActiveCallsPresent) ? "1" : "0");
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuActiveCalls_CONST) ".0.0." TOSTRING(DESIGNOBJ_butAddToActiveCall_CONST), 0, "", "", 
		bActiveCallsPresent ? "1" : "0");
	m_pOrbiter->CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuActiveCalls_CONST) ".0.0." TOSTRING(DESIGNOBJ_butRemoveFromActiveCall_CONST), 0, "", "", 
		bActiveCallsPresent ? "1" : "0");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Static_IP_settings(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Static_IP_settings(PK_Screen);

	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::SCREEN_Network_Settings_OnTimer, new CallBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::SCREEN_Network_Settings_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnVariableChanged, (ScreenHandlerCallBack) &ScreenHandler::SCREEN_Network_Settings_VariableChanged, new VariableCallBackData());

	string sStaticIP = m_mapNetworkSettings["EXTERNAL_IP"];
	string sNetmask = m_mapNetworkSettings["EXTERNAL_NETMASK"];
	string sGateway = m_mapNetworkSettings["GATEWAY"];
	string sDNS1 = m_mapNetworkSettings["DNS1"];

	m_pOrbiter->CMD_Set_Variable(VARIABLE_IP_Address_CONST, sStaticIP);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Net_Mask_CONST, sNetmask);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Gateway_CONST, sGateway);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_DNS_CONST, sDNS1);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Network_Settings(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Network_Settings(PK_Screen);

	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &ScreenHandler::SCREEN_Network_Settings_OnTimer, new CallBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::SCREEN_Network_Settings_ObjectSelected, new ObjectInfoBackData());

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Network_Status_CONST, "");
	m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuNetworkSettings_CONST) ".0.0", "Reading network settings. One moment...",TEXT_STATUS_CONST);
	m_pOrbiter->StartScreenHandlerTimer(500);
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::SCREEN_Network_Settings_OnTimer(CallBackData *pData)
{
	string sNetworkSettings;

	DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
	if( pDevice_Core )
	{
		DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
		if( pDevice_AppServer )
		{
			string sNetworkSettingsTxtFile = "/tmp/network_settings_" + StringUtils::ltos(static_cast<long>(time(NULL)));

			DCE::CMD_Spawn_Application CMD_Spawn_Application(
				m_pOrbiter->m_dwPK_Device,
				pDevice_AppServer->m_dwPK_Device,
				"/usr/pluto/bin/Network_DisplaySettings.sh",
				"network settings",
				"--out-file\t" + sNetworkSettingsTxtFile + "\t--all",
				"","",false,false,false,true);
			m_pOrbiter->SendCommand(CMD_Spawn_Application);

			Sleep(2000);

			char *pBuffer=NULL;
			int size=0;
			DCE::CMD_Request_File CMD_Request_File(
				m_pOrbiter->m_dwPK_Device,
				m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
				sNetworkSettingsTxtFile,
				&pBuffer,&size);
			m_pOrbiter->SendCommand(CMD_Request_File);

			if(NULL != pBuffer)
			{
				sNetworkSettings = string(pBuffer).substr(0, size);
			}
		}
	}

	m_mapNetworkSettings.clear();

	if(!sNetworkSettings.empty())
	{
		vector<string> vectLines;
		StringUtils::Tokenize(sNetworkSettings, "\n", vectLines);

		for(vector<string>::iterator it = vectLines.begin(); it != vectLines.end(); ++it)
		{
			string sLine = *it;

			vector<string> vectKeyValue;
			StringUtils::Tokenize(sLine, "=", vectKeyValue);
            
			if(vectKeyValue.size() == 2)
				m_mapNetworkSettings[vectKeyValue[0]] = vectKeyValue[1];
		}
	}

	NeedToRender render(m_pOrbiter, "SCREEN_Network_Settings_OnTimer");
	m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuNetworkSettings_CONST) ".0.0", "",TEXT_STATUS_CONST);
	
	if(!m_mapNetworkSettings.empty())
	{
		bool bUseDHCP = m_mapNetworkSettings["EXTERNAL_DHCP"] == "1";

		string sStaticIP = m_mapNetworkSettings["EXTERNAL_IP"];
		string sNetmask = m_mapNetworkSettings["EXTERNAL_NETMASK"];
		string sGateway = m_mapNetworkSettings["GATEWAY"];
		string sDNS1 = m_mapNetworkSettings["DNS1"];
		string sDNS2 = m_mapNetworkSettings["DNS2"];

		sNetworkSettings  = "Using DHCP: " + string(bUseDHCP ? "Yes" : "No") + "\n\n";
		sNetworkSettings += "IP: " + sStaticIP + "\n";
		sNetworkSettings += "Netmask: " + sNetmask + "\n";
		sNetworkSettings += "Gateway: " + sGateway + "\n";
		sNetworkSettings += "DNS1: " + sDNS1 + "\n";
		sNetworkSettings += "DNS2: " + sDNS2;

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Network_Status_CONST, sNetworkSettings);
	}
	else
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Network_Status_CONST, "Failed to read network settings, please try again later!");
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::SCREEN_Network_Settings_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if(pObjectInfoData->m_pObj->m_iBaseObjectID == DESIGNOBJ_butUseDHCP_CONST)
	{
		DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
		if( pDevice_Core )
		{
			DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
			if( pDevice_AppServer )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(
					m_pOrbiter->m_dwPK_Device,
					pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/Network_Config.sh",
					"network config",
					"--ext-dhcp",
					"","",false,false,false,true);
				m_pOrbiter->SendCommand(CMD_Spawn_Application);
			}
		}
		
		NeedToRender render(m_pOrbiter, "SCREEN_Network_Settings_ObjectSelected");
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuNetworkSettings_CONST) ".0.0", "",TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Network_Status_CONST, "Using DHCP, please wait...");
		m_pOrbiter->StartScreenHandlerTimer(10000);
	}
	else if(pObjectInfoData->m_pObj->m_iBaseObjectID == DESIGNOBJ_butApplyNetworkSettings_CONST)
	{
		string sStaticIP = m_pOrbiter->m_mapVariable_Find(VARIABLE_IP_Address_CONST);
		string sNetmask = m_pOrbiter->m_mapVariable_Find(VARIABLE_Net_Mask_CONST);
		string sGateway = m_pOrbiter->m_mapVariable_Find(VARIABLE_Gateway_CONST);
		string sDNS1 = m_pOrbiter->m_mapVariable_Find(VARIABLE_DNS_CONST);
		string sDNS2 = m_pOrbiter->m_mapVariable_Find(VARIABLE_DNS_CONST);

		DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Core_CONST);
		if( pDevice_Core )
		{
			DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfCategory( DEVICECATEGORY_App_Server_CONST );
			if( pDevice_AppServer )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(
					m_pOrbiter->m_dwPK_Device,
					pDevice_AppServer->m_dwPK_Device,
					"/usr/pluto/bin/Network_Config.sh",
					"network config",
					"--ext-static\t" + sStaticIP + "\t" + sNetmask + "\t" + sGateway + "\t" + sDNS1 + "\t" + sDNS2 + "",
					"","",false,false,false,true);
				m_pOrbiter->SendCommand(CMD_Spawn_Application);
			}
		}

		m_pOrbiter->CMD_Go_back("", "");

		NeedToRender render(m_pOrbiter, "SCREEN_Network_Settings_ObjectSelected");
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuNetworkSettings_CONST) ".0.0", "",TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Network_Status_CONST, "Reading network settings. One moment...");
		m_pOrbiter->StartScreenHandlerTimer(5000);		        
	}

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::SCREEN_Network_Settings_VariableChanged(CallBackData *pData)
{
	VariableCallBackData *pVariableInfoData = (VariableCallBackData *)pData;

	bool bSomethingChanged = false;

	switch(pVariableInfoData->m_nVariableKey)
	{
		case VARIABLE_IP_Address_CONST:
			m_mapNetworkSettings["EXTERNAL_IP"] = pVariableInfoData->m_sVariableValue;
			bSomethingChanged = true;
			break;

		case VARIABLE_Net_Mask_CONST:
			m_mapNetworkSettings["EXTERNAL_NETMASK"] = pVariableInfoData->m_sVariableValue;
			bSomethingChanged = true;
			break;

		case VARIABLE_Gateway_CONST:
			m_mapNetworkSettings["GATEWAY"] = pVariableInfoData->m_sVariableValue;
			bSomethingChanged = true;
			break;

		case VARIABLE_DNS_CONST:
			m_mapNetworkSettings["DNS1"] = pVariableInfoData->m_sVariableValue;
			bSomethingChanged = true;
			break;

		//ignore everything else
		default:
			break;
	}

	if(bSomethingChanged)
	{
		NeedToRender render(m_pOrbiter, "SCREEN_Network_Settings_ObjectSelected");

		DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(DESIGNOBJ_mnuStaticIPSettings_CONST);
		if(pObj)
		{
			m_pOrbiter->Renderer()->RenderObjectAsync(pObj);
		}
	}

	return false; // Keep processing it
}

//-----------------------------------------------------------------------------------------------------

void ScreenHandler::SCREEN_UserStatus(long PK_Screen)
{
  ScreenHandlerBase::SCREEN_UserStatus(PK_Screen);
  RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::UserStatus_Voicemail_DataGridSelected, new DatagridCellBackData());
}

bool ScreenHandler::UserStatus_Voicemail_DataGridSelected(CallBackData *pData)
{
  DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
  
  if (pCellInfoData && pCellInfoData->m_pDesignObj_DataGrid && 
      pCellInfoData->m_pDesignObj_DataGrid->m_iBaseObjectID == DESIGNOBJ_dgVoiceMailManager_CONST &&
      pCellInfoData->m_Column == 11)
    {
      int iRow=pCellInfoData->m_Row;
      DataGridTable *pDataGridTable = pCellInfoData->m_pDesignObj_DataGrid->m_pDataGridTable_Current_get();
      DataGridCell *pCell;
      pCell = pDataGridTable->GetData(1,pCellInfoData->m_Row);
      string sTimestamp = pCell->GetText();
      pCell = pDataGridTable->GetData(3,pCellInfoData->m_Row);
      string sCallerID = pCell->GetText();
      string sMessage=m_pOrbiter->m_mapTextString[TEXT_Confirm_Voicemail_Delete_CONST];
      StringUtils::Replace(&sMessage,"<%=TIMESTAMP%>",sTimestamp);
      StringUtils::Replace(&sMessage,"<%=CALLERID%>",sCallerID);

      string sMessageDoDelete=
	StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
	StringUtils::itos(m_pOrbiter->m_dwPK_Device_TelecomPlugIn) + " 1 " // Command
	TOSTRING(COMMAND_Delete_File_CONST) " "
	TOSTRING(COMMANDPARAMETER_Filename_CONST) " \"" + pCell->GetValue() + "\"";
      
      string sMessageCancel=
	StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " +
	StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + " 1 " // command
	TOSTRING(COMMAND_Goto_Screen_CONST) " "
	TOSTRING(COMMANDPARAMETER_PK_Screen_CONST) " "
	TOSTRING(SCREEN_UserStatus_CONST);
      
      DisplayMessageOnOrbiter(0, sMessage, false, "0", true,
			      m_pOrbiter->m_mapTextString[TEXT_YES_CONST],
			      sMessageDoDelete,
			      m_pOrbiter->m_mapTextString[TEXT_NO_CONST],
			      sMessageCancel);      
    }

  return false;
}
