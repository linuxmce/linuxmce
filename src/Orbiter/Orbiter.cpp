/*
Orbiter

Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

/*
Shortcut keys:
F1 = help
F2 = main menu, or 'now playing'
F3 = power off
*/


//<-dceag-d-b->
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "DesignObj_Orbiter.h"
#include "DataGrid.h"
#include "DataGridRenderer.h"
#include "SerializeClass/ShapesColors.h"

#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "pluto_main/Define_Direction.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_VertAlignment.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Button.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_MediaType.h"

#include "Floorplan.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_Text.h"
#include "PlutoUtils/minilzo.h"
#include "../Orbiter/RendererMNG.h"
#include "OrbiterFileBrowser.h"
#include "ScreenHandler.h"
#include "ScreenHistory.h"

#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
#include "OpenGL/VIA/ViaOverlay.h"
#endif

#if defined(USE_UIWINDOW) && defined(ORBITER_OPENGL)
#include "Win32/UIWindowManager.h"
#endif

#include "OrbiterRenderer.h"
#include "OrbiterRendererFactory.h"

#include "GraphicBuilder.h"
#include "Simulator.h"

#include "Xine_Player/AskXine_Socket.h"
#include "VFD_LCD/VFD_LCD_Base.h"

#define	MAX_REPEAT		5		// Don't go crazy if a user holds a button down

#ifdef ENABLE_MOUSE_BEHAVIOR
#include "MouseBehavior.h"
#include "MouseGovernor.h"
#endif

#ifdef TEST_OSD
#include "Linux/OSDScreenHandler.h"
#endif

#define KEY_DOWN_DURATION		1000

#ifdef HID_REMOTE
#include "Linux/HIDInterface.h"
#endif

#ifdef HID_REMOTE
#include "Linux/GenericHIDInterface.h"
#endif

//#define PROFILING_GRID
PlutoProfiler *g_PlutoProfiler = new PlutoProfiler();

#if defined(WIN32) && !defined(BLUETOOTH_DONGLE) && !defined(MOXI_ORBITER)
#include "MainDialog.h" //for debugging in main window
#else //linux
void WriteStatusOutput(const char *) {} //do nothing
#endif

#define EXTERN_REVISION_DATE
#include "version.h"

extern Command_Impl *g_pCommand_Impl;

#ifdef WINCE
#include "wince.h"

class cout_dummy
{
public:
	static cout_dummy cout_dummy_;
	cout_dummy& operator<<(const string& s) { return cout_dummy_;}
	cout_dummy& operator<<(const char& c) { return cout_dummy_;}
	cout_dummy& operator<<(const int& l) { return cout_dummy_;}
	cout_dummy& operator<<(const char* s) { return cout_dummy_;}
};
cout_dummy cout_dummy::cout_dummy_;
#define cout cout_dummy::cout_dummy_

#endif

// For DontRender
namespace DCE
{
	int g_iDontRender=0;
}

extern void (*g_pReceivePingHandler)(Socket *pSocket);
extern bool (*g_pSendPingHandler)(Socket *pSocket);
void ReceivePingHandler(Socket *pSocket);
bool SendPingHandler(Socket *pSocket);

//------------------------------------------------------------------------
// DelayedSelectObjectInfo
//------------------------------------------------------------------------
// Stuff for the delayed select object
class DelayedSelectObjectInfo
{
public:
	DesignObj_Orbiter *m_pObj;
	string m_sPK_DesignObj_CurrentScreen;
	timespec m_tsTime;

	DelayedSelectObjectInfo(DesignObj_Orbiter *pObj,string sPK_DesignObj_CurrentScreen,timespec tsTime)
	{
		m_pObj=pObj;
		m_sPK_DesignObj_CurrentScreen=sPK_DesignObj_CurrentScreen;
		m_tsTime=tsTime;
	}
};

//------------------------------------------------------------------------
// ContinuousRefreshInfo
//------------------------------------------------------------------------
// Stuff for the continuous refresh
class ContinuousRefreshInfo
{
public:
	DesignObj_Orbiter *m_pObj;
	int m_iInterval;

	ContinuousRefreshInfo(DesignObj_Orbiter *pObj,int Interval)
	{
		m_pObj=pObj;
		m_iInterval=Interval;
	}
};

///------------------------------------------------------------------------
template<class T> inline static T Dist( T x,  T y ) { return x * x + y * y; }
//------------------------------------------------------------------------
void *MaintThread(void *p);


void *UpdateTimeCodeThread(void *p);
static bool bMaintThreadIsRunning = false;
bool ScreenHistory::m_bAddToHistory=true;

//------------------------------------------------------------------------
// Orbiter
//------------------------------------------------------------------------

/*
EXTRAINFO codes for datagrids:
'H' means select a cell when a user highlights it
'R' or 'C' means only highlight/select whole rows/columns
Either 'A' means arrows scroll the grid.  If 'H' is not specified, the scrolling will be selecting cells.  Otherwise it will be highlighting them
'As' means arrows scroll the grid 1 cell at a time
'Ap' means arrows scroll the grid 1 page at a time
'P' means make the first/last cell a 'page up'/'page down'
'h' means that moving the highlight should de-select any selected cells so you don't have both a highlighted and selected cell.  Only meaningfull with 'H'.
'X' means that clear the selected cell entry whenever the user scrolls
'S' means that on the initial acquire it should scroll to the highlighted cell
'cx' means if this is a phone on Bluetooth dongle, render this grid as a single column on the phone using the column x
'T'  - (for PlutoMO only) PlutoMO will send SelectedItem everytime the position on the grid is changed.
'F'  - force rendering in Orbiter
*/

//--------------------------------------------------------------------------------------------------
Orbiter *Orbiter::m_pInstance = NULL; //the one and only - for non-bluetooth_dongle devices
//--------------------------------------------------------------------------------------------------
/*static*/ void Orbiter::DestroyInstance()
{
	if(NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
		return;
	}

	throw string("Orbiter::DestroyInstance: no instance to destroy"); 
}
//--------------------------------------------------------------------------------------------------
/*static*/ Orbiter *Orbiter::Instance()
{
	return m_pInstance;
}
//--------------------------------------------------------------------------------------------------

/*

Constructors/Destructor

*/

//<-dceag-const-b->!
Orbiter::Orbiter( int DeviceID, int PK_DeviceTemplate, string ServerAddress,  string sLocalDirectory,
				 bool bLocalMode,  int iImageWidth,  int iImageHeight, pluto_pthread_mutex_t* pExternalScreenMutex/*=NULL*/,
				 bool bFullScreen/*=false*/)
				 : Orbiter_Command( DeviceID,  ServerAddress, true, bLocalMode ), IRReceiverBase(),
				 m_MaintThreadMutex("MaintThread"), m_ScreenMutex( "rendering" ),
				 m_VariableMutex( "variable" ), m_DatagridMutex( "datagrid" ),
				 m_TimeCodeMutex( "timecode" ), m_TimeCodeID(0)
				 //<-dceag-const-e->
{
	IRReceiverBase::Setup(this);

#ifdef HID_REMOTE
    m_pHIDInterface=NULL;
    m_HidThreadID=(pthread_t)NULL;
    m_pGenericHIDInterface=NULL;
    m_GenericHidThreadID=(pthread_t)NULL;
#endif

	m_MaintThreadID = (pthread_t)NULL;
	
	WriteStatusOutput((char *)(string("Orbiter version: ") + VERSION).c_str());
	WriteStatusOutput("Orbiter constructor");

	m_bFullScreen = bFullScreen;
	m_bUsingLiveAVPath = m_bContainsVideo = false;
	m_pInstance = this;
	m_TypeToIgnore = Orbiter::Event::NONE;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter %p constructor",this);
	m_pOrbiterRenderer = OrbiterRendererFactory::CreateRenderer(this);

	m_nCallbackCounter = 0;
	m_dwPK_DeviceTemplate = PK_DeviceTemplate;
	m_iUiVersion = 0;
	m_tLastMouseMove = 0;
	m_iLastVideoObjectRendered = 0;
	m_iLastX=m_iLastY=-1;

	//initialize
	m_pObj_NowPlayingOnScreen = NULL;
	m_pObj_NowPlaying_MT_OnScreen = NULL;
	m_pObj_NowPlaying_Section_OnScreen = NULL;
	m_pObj_NowPlaying_TimeShort_OnScreen = NULL;
	m_pObj_NowPlaying_TimeLong_OnScreen = NULL; 
	m_pObj_NowPlaying_Speed_OnScreen = NULL;
	m_pAskXine_Socket = NULL;
	m_bScreenSaverActive = false;
	m_bUpdateTimeCodeLoopRunning = false;
	m_bShowingSpeedBar = false;

#ifdef ENABLE_MOUSE_BEHAVIOR
	m_pMouseBehavior = NULL;
#endif

	m_pScreenHandler = NULL;
	m_sLocalDirectory=sLocalDirectory;
	m_iImageWidth=iImageWidth;
	m_iImageHeight=iImageHeight;
	m_bStartingUp=true;
	m_pLocationInfo = NULL;
	m_dwPK_Users = 0;
	m_dwPK_Device_CaptureCard = m_dwPK_Device_NowPlaying_Video = m_dwPK_Device_NowPlaying_Audio = m_dwPK_Device_NowPlaying = 0;
	m_bPK_Device_NowPlaying_Audio_DiscreteVolume = false;
	m_pOrbiterFileBrowser_Collection=NULL;
	m_bForward_local_kb_to_OSD=false;
	m_bYieldScreen=false;
	m_bYieldInput=false;
	m_bRerenderScreen=false;
	m_bWeCanRepeat=false;
	m_bRepeatingObject=false;
	m_bShowShortcuts = false;
	m_bLoadDatagridImagesInBackground=true;
	m_iPK_Screen_Remote=m_iPK_DesignObj_Remote_Popup=m_iPK_Screen_FileList=m_iPK_Screen_RemoteOSD=m_iPK_Screen_OSD_Speed=m_iPK_Screen_OSD_Track=m_PK_Screen_ActiveApp_OSD=m_PK_Screen_ActiveApp_Remote=0;
	m_iPK_MediaType=0;
	m_iStreamID=0;
	m_pScreenHistory_Current=NULL;
	m_pObj_LastSelected=m_pObj_Highlighted_Last=NULL;
	m_dwPK_DesignObj_Toolbar_Last=0;
	m_pObj_Highlighted_set(NULL);
	m_iRow_Floorplan_LastSelected=-1;
	m_pObj_SelectedLastScreen=NULL;
	m_pGraphicBeforeHighlight=NULL;
	m_pContextToBeRestored=NULL;
	m_LastActivityTime=time( NULL );
	m_iLastEntryInDeviceGroup=-1;
	m_dwIDataGridRequestCounter=0;
	m_bCaptureKeyboard_OnOff = false;
	m_bCaptureKeyboard_Reset = false;
	m_iCaptureKeyboard_EditType = 0;
	m_bCaptureKeyboard_DataGrid = false;
	m_iCaptureKeyboard_PK_Variable = 0;
	m_sCaptureKeyboard_Text = "";
	m_sCaptureKeyboard_InternalBuffer = "";
	m_pCaptureKeyboard_Text = NULL;
	m_bBypassScreenSaver = false;
	m_bShiftDown = false;
	m_bControlDown = false;
	m_bAltDown = false;
	m_bCapsLock = false;
	m_pCacheImageManager = NULL;
	m_pScreenHistory_Pivot = NULL;
	m_pScreenHistory_New = NULL;
	m_bIgnoreMouse = false;

#ifdef ENABLE_MOUSE_BEHAVIOR
	m_pMouseBehavior = NULL;
#endif

	pthread_mutexattr_init( &m_MutexAttr );
	pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_VariableMutex.Init( &m_MutexAttr );
	m_DatagridMutex.Init( &m_MutexAttr );
	pthread_cond_init(&m_MaintThreadCond, NULL);
	m_MaintThreadMutex.Init(&m_MutexAttr,&m_MaintThreadCond);
	m_TimeCodeMutex.Init(NULL);

	m_bUsingExternalScreenMutex = NULL != pExternalScreenMutex;
	if(m_bUsingExternalScreenMutex)
	{
		m_ScreenMutex.mutex = pExternalScreenMutex->mutex;
		m_ScreenMutex.m_pthread_cond_t = pExternalScreenMutex->m_pthread_cond_t;
		m_ScreenMutex.m_bInitialized = pExternalScreenMutex->m_bInitialized = true;
	}
	else
		m_ScreenMutex.Init( &m_MutexAttr );

	pthread_create(&m_MaintThreadID, NULL, MaintThread, (void*)this);
	srand( (unsigned)time( NULL ) );  // For the screen saver

	m_pBackgroundImage = NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter::~Orbiter()
//<-dceag-dest-e->
{
#ifdef HID_REMOTE
  if (m_GenericHidThreadID)
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Waiting for Generic HID Handler Thread to finish...");
    pthread_join(m_GenericHidThreadID, NULL);
    m_GenericHidThreadID = 0;
  }
  
  delete m_pGenericHIDInterface;
  m_pGenericHIDInterface = NULL;

  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done with Generic HID Handler Thread");

	if(m_HidThreadID)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for HID thread to finish...");
		pthread_join(m_HidThreadID, NULL);
		m_HidThreadID = 0;
	}
	
	delete m_pHIDInterface;
	m_pHIDInterface=NULL;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done with HID thread.");
#endif

	m_pOrbiterRenderer->Destroy();

	WriteStatusOutput("Orbiter destructor");
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter  %p is exiting",this);

	//stop the simulator, if running
	if(Simulator::IsRunning())
		StopSimulatorThread();

#ifdef ENABLE_MOUSE_BEHAVIOR
	delete m_pMouseBehavior;
	m_pMouseBehavior=NULL;
#endif

	// Be sure we get the maint thread to cleanly exit
	KillMaintThread();

	PLUTO_SAFETY_LOCK( pm, m_MaintThreadMutex );
	map<int,PendingCallBackInfo *>::iterator itCallBackInfo;
	for(itCallBackInfo = m_mapPendingCallbacks.begin(); itCallBackInfo != m_mapPendingCallbacks.end(); itCallBackInfo++)
		delete (*itCallBackInfo).second;
	m_mapPendingCallbacks.clear();
	pm.Release();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Maint thread dead");

	if(NULL != m_pcRequestSocket && m_bRouterReloading==false ) // Will be NULL if we died without successfully getting our config
	{
		if(NULL != m_pcRequestSocket->m_pClientSocket)
			m_pcRequestSocket->m_pClientSocket->SetReceiveTimeout(3);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unregistering...");

		char *pData=NULL;int iSize=0;
		DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "0", 0, "", 0, &pData, &iSize );
		SendCommand( CMD_Orbiter_Registered );

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unregistered!");
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Destroying orbiter's objects...");

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
	for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
	{
		DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
		delete pObj;
	}
	m_mapObj_All.clear();
	
	RELEASE_GRAPHIC(m_pBackgroundImage);

	delete m_pOrbiterRenderer;
	m_pOrbiterRenderer = NULL;

	for(map<int,class FloorplanObject *>::iterator itFPO=m_mapFloorplanObject_Selected.begin();itFPO!=m_mapFloorplanObject_Selected.end();++itFPO)
		delete itFPO->second;
	m_mapFloorplanObject_Selected.clear();

	m_vectObjs_TabStops.clear();
	m_vectObjs_Selected.clear();
	m_ScreenMap.clear(); //shared with mapObj_All
	m_mapDeviceGroups.clear();
	m_mapUserIcons.clear();

	map < string, DesignObj_DataList * >::iterator itSuffix; //
	for(itSuffix = m_mapObj_AllNoSuffix.begin(); itSuffix != m_mapObj_AllNoSuffix.end(); itSuffix++)
	{
		delete (*itSuffix).second;
		(*itSuffix).second = NULL;
	}
	m_mapObj_AllNoSuffix.clear();

	//clearing device groups map
	map<int,class DeviceGroup *>::iterator itDeviceGroups;
	for(itDeviceGroups = m_mapDeviceGroups.begin(); itDeviceGroups != m_mapDeviceGroups.end(); itDeviceGroups++)
	{
		delete (*itDeviceGroups).second;
		(*itDeviceGroups).second = NULL;
	}

	//deallocate shortcuts messages
	map<char, Message *>::iterator itShortcuts;
	for(itShortcuts = m_mapShortcut.begin(); itShortcuts != m_mapShortcut.end(); ++itShortcuts)
		delete (*itShortcuts).second;

	m_mapShortcut.clear();
	

	//clearing floor plan object map
	map<int, FloorplanObjectVectorMap *>::iterator itFPObjVM;
	for(itFPObjVM = m_mapFloorplanObjectVector.begin(); itFPObjVM != m_mapFloorplanObjectVector.end(); itFPObjVM++)
	{
		FloorplanObjectVectorMap *fpObjVM = (*itFPObjVM).second;
		if( fpObjVM )
		{
			map<int,FloorplanObjectVector *>::iterator itFPObjVect;
			for(itFPObjVect=fpObjVM->begin();itFPObjVect!=fpObjVM->end();++itFPObjVect)
			{
				FloorplanObjectVector *fpObjVect = (*itFPObjVect).second;
				if( fpObjVect )
				{
					for(int i=0;i<(int) fpObjVect->size();++i)
					{
						FloorplanObject *fpObj = (*fpObjVect)[i];
						delete fpObj;
						fpObj = NULL;
					}
					delete fpObjVect;
					fpObjVect = NULL;
				}
			}
			delete fpObjVM;
			fpObjVM = NULL;
		}
	}
	m_mapFloorplanObjectVector.clear();

	//clearing screen history
	list < ScreenHistory * >::iterator itScreenHistory;
	for(itScreenHistory = m_listScreenHistory.begin(); itScreenHistory != m_listScreenHistory.end(); itScreenHistory++)
	{
		//ScreenHistory * psh = *itScreenHistory;
		if( *itScreenHistory==m_pScreenHistory_Current )
			m_pScreenHistory_Current=NULL;
		delete *itScreenHistory;
		*itScreenHistory = NULL;
	}
	m_listScreenHistory.clear();

	if(NULL != m_pScreenHistory_Current)
	{
		delete m_pScreenHistory_Current;
		m_pScreenHistory_Current = NULL;
	}

	//clearing device data map
	map< string, class DesignObj_DataGrid * >::iterator itDesignObjDataGrid;
	for(itDesignObjDataGrid = m_mapObjs_AllGrids.begin(); itDesignObjDataGrid != m_mapObjs_AllGrids.end(); itDesignObjDataGrid++)
	{
		//delete (*itDesignObjDataGrid).second;
		(*itDesignObjDataGrid).second = NULL;
	}
	m_mapObjs_AllGrids.clear();

	map< string, DesignObj_DataList * >::iterator itObj_Bound;
	for(itObj_Bound = m_mapObj_Bound.begin(); itObj_Bound != m_mapObj_Bound.end(); itObj_Bound++)
	{
		delete (*itObj_Bound).second;
		(*itObj_Bound).second = NULL;
	}
	m_mapObj_Bound.clear();

	if(NULL != m_pCacheImageManager)
	{
		delete m_pCacheImageManager;
		m_pCacheImageManager = NULL;
	}

	for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();++it)
		delete *it;
	m_listPopups.clear();

	for(map<int,Message *>::iterator itm=m_mapHardKeys.begin(); itm!=m_mapHardKeys.end();++itm)
	{
		Message *pMessage = itm->second;
		delete pMessage;
	}
	m_mapHardKeys.clear();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter's objects destroyed!");

	delete m_pOrbiterFileBrowser_Collection;
	m_pOrbiterFileBrowser_Collection = NULL;

	delete m_pScreenHistory_Pivot;
	m_pOrbiterFileBrowser_Collection = NULL;

	delete m_pScreenHandler;
	m_pScreenHandler = NULL; 

	vm.Release();

	//wait TimeCodeThread to finish
	if(m_TimeCodeID)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for TimeCode thread to finish...");
		pthread_join(m_TimeCodeID, NULL);
		m_TimeCodeID = 0;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done with TimeCode thread.");
	}

	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_VariableMutex.mutex);
	pthread_mutex_destroy(&m_DatagridMutex.mutex);
	pthread_mutex_destroy(&m_TimeCodeMutex.mutex);
	pthread_mutex_destroy(&m_MaintThreadMutex.mutex);

	if(!m_bUsingExternalScreenMutex)
		pthread_mutex_destroy(&m_ScreenMutex.mutex);
}

//<-dceag-getconfig-b->!
bool Orbiter::GetConfig()
{
	WriteStatusOutput("GetConfig");

	if(m_dwPK_Device == 0)
		m_dwPK_Device = atoi(Simulator::GetInstance()->m_sDeviceID.c_str());

	int PK_Device=m_dwPK_Device;
	bool bResult=Orbiter_Command::GetConfig();
	if( PK_Device!=m_dwPK_Device && m_dwPK_Device )  // We have another device id or host ip address
	{
		Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(m_dwPK_Device);
		Simulator::GetInstance()->SaveConfigurationFile();
	}
	if( !bResult )
	{
#ifdef WIN32
		string sDetectedIPAddress = DetectCoreIpAddress();
		if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_CannotConnect && m_sHostName != sDetectedIPAddress)
		{
			m_sHostName = sDetectedIPAddress;
			if( !Orbiter_Command::GetConfig() )
			{
				WriteStatusOutput("Couldn't connect to the router.");
				return false;
			}
			else
			{
				if( m_dwPK_Device )
					Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(m_dwPK_Device);
				Simulator::GetInstance()->m_sRouterIP = m_sIPAddress;
				Simulator::GetInstance()->SaveConfigurationFile();
			}
		}
		else
		{
#endif
			WriteStatusOutput("Cannot connect to the router.");
			return false;
#ifdef WIN32
		}
#endif
	}

	WriteStatusOutput("Got the config");

	m_nSelectionBehaviour = DATA_Get_Using_Infrared();
	m_bMemoryManagementEnabled = DATA_Get_Enable_Memory_Management();

	if(  !m_bLocalMode  )
	{
		GetEvents(  )->m_pClientSocket->SetReceiveTimeout( 15 );
		m_pcRequestSocket->m_pClientSocket->SetReceiveTimeout( 15 );
	}

	string::size_type pos=0;
	string sTimeout = DATA_Get_Timeout();
	m_iTimeoutScreenSaver = atoi(StringUtils::Tokenize(sTimeout,",",pos).c_str());
	m_iTimeoutBlank = atoi(StringUtils::Tokenize(sTimeout,",",pos).c_str());
	m_sCacheFolder = DATA_Get_CacheFolder();
	m_iCacheSize = DATA_Get_CacheSize();
	m_bUseOpenGL = m_pData->m_mapParameters_Find(DEVICEDATA_Use_OpenGL_effects_CONST)=="1";
	m_bUseComposite = m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_Use_alpha_blended_UI_CONST) == "1";
	m_bReportTimeCode = DATA_Get_Get_Time_Code_for_Media();
	m_bExpertMode = DATA_Get_Expert_Mode();

	int nWidth = atoi(m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_ScreenWidth_CONST).c_str());
	int nHeight = atoi(m_pEvent->GetDeviceDataFromDatabase(m_dwPK_Device, DEVICEDATA_ScreenHeight_CONST).c_str());

	if(nWidth)
		m_iImageWidth = nWidth;

	if(nHeight)
		m_iImageHeight = nHeight;

	m_sScreenSize=PlutoSize(m_iImageWidth,m_iImageHeight);

	if(m_iCacheSize > 0)
		m_pCacheImageManager = new CacheImageManager(FileUtils::IncludeTrailingSlash(m_sCacheFolder) + StringUtils::ltos(m_dwPK_Device), m_iCacheSize);

	m_iVideoFrameInterval = DATA_Get_VideoFrameInterval();
	if(!m_iVideoFrameInterval) //this device data doesn't exist or it's 0
		m_iVideoFrameInterval = 6000; //6 sec

	// See if any of our child devices are remote controls
	for(VectDeviceData_Impl::iterator it=m_pData->m_vectDeviceData_Impl_Children.begin();it!=m_pData->m_vectDeviceData_Impl_Children.end();++it)
	{
		DeviceData_Impl *pDeviceData_Impl = *it;
#ifdef HID_REMOTE
		if ( pDeviceData_Impl->WithinCategory(DEVICECATEGORY_Gestural_Remote_CONST) && 
		     pDeviceData_Impl->m_dwPK_DeviceTemplate != DEVICETEMPLATE_MCR_Remote_Dongle_CONST)
		{
		  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter::GetConfig() - Found Gestural Remote. Adding to GenericHIDInterface Bits.");
		  // Special case for Gestural remotes.
		  string sGyroDeviceToAdd = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Block_Device_CONST);
		  if (sGyroDeviceToAdd.size())
		    {
		      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Adding Block Devices %s to the GenericHIDInterface map.",sGyroDeviceToAdd.c_str());
		      if (sGyroDeviceToAdd.find("|") != string::npos)
			{
			  // A | separator was found, split directly into the map.
			  string::size_type pos=0;
			  while (pos<sGyroDeviceToAdd.size())
			    {
			      // get next device, and reset pos.
			      string sCurrentGyroDevice = StringUtils::Tokenize(sGyroDeviceToAdd,"|",pos);
			      if (sCurrentGyroDevice.size())
				{
				  m_vectGyroDevices.push_back(sCurrentGyroDevice);
				}
			    }
			}
		      else
			{
			  // No separator found, see if it is a single device.
			  if (sGyroDeviceToAdd.find("/dev/") != string::npos)
			    {
			      m_vectGyroDevices.push_back(sGyroDeviceToAdd);
			    }
			}
		    }
		}
#endif
		if( pDeviceData_Impl->WithinCategory(DEVICECATEGORY_Remote_Controls_CONST) )
		{
			string s = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Remote_Layout_CONST);
			if( s.size() )
				m_mapRemoteLayout[0]=s[0];

			s = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Configuration_CONST);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Scan codes for remote %d: %s", pDeviceData_Impl->m_dwPK_Device, s.c_str());
			
			vector<string> vectTokens;
			StringUtils::Tokenize(s,"\r\n",vectTokens);
			vector<string>::iterator it2;
			for(it2=vectTokens.begin();it2!=vectTokens.end();++it2)
			{
				// The string has the format keyname, scan code, [U|D|H for Up/Down/Hold], [Y|anything else] Y=Ignore this when yield to screen is active
				string::size_type pos = it2->find(',');
				string::size_type pos2 = (pos!=string::npos) ? it2->find(',',pos+1) : string::npos;
				string::size_type pos3 = (pos2!=string::npos) ? it2->find(',',pos2+1) : string::npos;
				if( pos!=string::npos && pos<it2->size() )
				{
					int ScanCode = atoi(it2->substr(pos + 1).c_str());
					char Action = (pos2!=string::npos && pos2<it2->size()) ? (*it2)[pos2+1] : 'D'; // Default to down
					if( pos3!=string::npos && pos3+1<it2->size() && (*it2)[pos3+1]=='Y' )
						m_mapScanCodeToIgnoreOnYield[ScanCode]=true;

					string sCode;
#ifdef WIN32
					m_mapScanCodeToRemoteButton[make_pair (ScanCode, Action)] = StringUtils::ToUpper(it2->substr(0, pos));
#else
					m_mapScanCodeToRemoteButton[ make_pair<int,char> (ScanCode,Action) ] = StringUtils::ToUpper(it2->substr(0,pos));
#endif // WIN32

					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added scan code %d -> %s Action: %d", 
						atoi( it2->substr(pos + 1).c_str()), StringUtils::ToUpper(it2->substr(0,pos)).c_str(),(int) Action);
				}
			}

			vectTokens.clear();
			s = pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_Mapping_CONST);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Mapping for remote %d: %s", pDeviceData_Impl->m_dwPK_Device, s.c_str());

			StringUtils::Tokenize(s,"\r\n",vectTokens);
			for(it2=vectTokens.begin();it2!=vectTokens.end();++it2)
			{
				string::size_type posEq = it2->find('=');
				string::size_type posComma1 = it2->find(',');
				string::size_type posComma2 = posEq!=string::npos ? it2->find(',',posEq) : string::npos;

				if( posEq!=string::npos && posComma1!=string::npos && posComma2!=string::npos )
				{
					int EventType1 = atoi( it2->c_str() );
					int EventID1 = atoi( it2->substr(posComma1+1).c_str() );
					int EventType2 = atoi( it2->substr(posEq+1).c_str() );
					int EventID2 = atoi( it2->substr(posComma2+1).c_str() );

#ifdef WIN32
					m_mapEventToSubstitute[make_pair (EventType1, EventID1)] = make_pair (EventType2, EventID2);
#else
					m_mapEventToSubstitute[ make_pair<int,int> (EventType1,EventID1) ] = make_pair<int,int> (EventType2,EventID2);
#endif

					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added replacement %d,%d = %d,%d",EventType1,EventID1,EventType2,EventID2);
				}
			}
		}
	}

	if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
	{
		RouterNeedsReload();
		return false;
	}
	IRReceiverBase::GetConfig(m_pData);
	m_pScreenHandler = CreateScreenHandler();
	m_pOrbiterRenderer->Configure();

	m_pDevice_ScreenSaver = m_pData->FindSelfOrChildWithinCategory(DEVICECATEGORY_Screen_Savers_CONST);

	return true;
}


void Orbiter::PostConnect()
{

#ifdef HID_REMOTE
	m_pHIDInterface=new PlutoHIDInterface(this);
	pthread_create(&m_HidThreadID, NULL, ProcessHIDEvents, (void*)m_pHIDInterface);
	m_pGenericHIDInterface=new GenericHIDInterface(this);
	pthread_create(&m_GenericHidThreadID, NULL, ProcessGenericHIDEvents, (void*)m_pGenericHIDInterface);
#endif

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter::Register()
//<-dceag-reg-e->
{
	return Connect(0);  // Don't validate the device template, since the same binary is used for lots of devices
}

/*
When you receive commands that are destined to one of your children,
then if that child implements DCE then there will already be a separate class
created for the child that will get the message.  If the child does not, then you will
get all commands for your children in ReceivedCommandForChild, where
pDeviceData_Base is the child device.  If you handle the message, you
should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Orbiter::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
When you received a valid command, but it wasn't for one of your children,
then ReceivedUnknownCommand gets called.  If you handle the message, you
should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Orbiter::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

/*

Protected methods

*/
//------------------------------------------------------------------------------------------------------------

/*
GENERAL PURPOSE METHODS
*/
//-----------------------------------------------------------------------------------------------------------
OrbiterRenderer *Orbiter::Renderer() 
{ 
	return m_pOrbiterRenderer; 
}
//-----------------------------------------------------------------------------------------------------------
int Orbiter::CurrentScreen() 
{ 
	return NULL != m_pScreenHistory_Current ? m_pScreenHistory_Current->PK_Screen() : 0;	
}
//-----------------------------------------------------------------------------------------------------------
void Orbiter::ScreenSaver( void *data )
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ScreenSaver Bypass screen saver now: %d obj %s ss %s",
		(int)m_bBypassScreenSaver,
		m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() ? m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : "*NONE*",
		m_pDesignObj_Orbiter_ScreenSaveMenu ? m_pDesignObj_Orbiter_ScreenSaveMenu->m_ObjectID.c_str() : "*NONE*");

#endif

#ifdef MAEMO_NOKIA770
	// To avoid start screen saver on the Maemo Orbiter			
	m_bBypassScreenSaver = true;
#endif
	
	if( m_bBypassScreenSaver )
		return;

	NeedToRender render( this, "Screen saver" );

	int Timeout = m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu ? m_iTimeoutBlank : m_iTimeoutScreenSaver;
	// See if there's been mouse movement since the screen saver timeout started.  If so, 
	// restart the timer
	if( time(NULL) - m_tLastMouseMove < Timeout )
	{
		// The mouse has moved.  Reset it
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::ScreenSaver mouse moved");
#endif
		CallMaintenanceInMiliseconds( ( m_tLastMouseMove + Timeout - time(NULL) ) * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
		return;
	}


	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu )
	{
		// Don't shut the tv off if we're playing media
		if( m_bIsOSD && m_iPK_Screen_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::ScreenSaver not turning off display because we're playing media");
#endif
		}
		else
			CMD_Display_OnOff("0",false);
	}
	else
	{
		StartScreenSaver();
	}
}

void Orbiter::Timeout( void *data )
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen %s timed out data: %p",m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),data);
#endif
	if( !data || data!=(void *) m_pScreenHistory_Current->GetObj() )
		return;

	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;
	if(  pObj->m_Action_TimeoutList.size(  )>0  )
		ExecuteCommandsInList( &pObj->m_Action_TimeoutList, pObj, smLoadUnload );
}

void Orbiter::ReselectObject( void *data )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) data;

	// Not used
	DesignObjZoneList::iterator iZone;
	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
	{
		DesignObjZone *pDesignObjZone = ( *iZone );
		ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, smLoadUnload, -1, -1, pObj->m_iRepeatParm+1 );
	}
}

void Orbiter::RedrawObject( void *iData )
{
	DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) iData;
	if( !pObj || !pObj->m_bOnScreen )
		return;

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	NeedToRender render( this, "RedrawObject" );

	if( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) pObj;
		pGrid->Flush();
	}

	m_pOrbiterRenderer->RenderObjectAsync(pObj);
	CallMaintenanceInMiliseconds(pObj->m_iRegenInterval,&Orbiter::RedrawObject,pObj,pe_Match_Data);
}
//-----------------------------------------------------------------------------------------------------------
void Orbiter::RealRedraw( void *data )
{
g_PlutoProfiler->ClearResults();
g_PlutoProfiler->Start("RealRedraw");
	m_pOrbiterRenderer->RefreshScreen(data);
g_PlutoProfiler->Stop("RealRedraw");
g_PlutoProfiler->DumpResults();
}
//-----------------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::NeedToChangeScreens( ScreenHistory *pScreenHistory/*, bool bAddToHistory*/ )
{
	if(m_bQuit_get())
		return;

	m_bShiftDown = false;
	m_sCaptureKeyboard_InternalBuffer = "";

#ifdef DEBUG
	if(pScreenHistory)
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::NeedToChangeScreens Need to change screens executed to %s",
		pScreenHistory->GetObj()->m_ObjectID.c_str());
#endif

	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	m_vectObjs_GridsOnScreen.clear(  );
	m_vectObjs_VideoOnScreen.clear(  );
	m_pObj_NowPlayingOnScreen=NULL;
	m_pObj_NowPlaying_MT_OnScreen=NULL;
	m_pObj_NowPlaying_Section_OnScreen=NULL;
	m_pObj_NowPlaying_TimeShort_OnScreen=NULL;
	m_pObj_NowPlaying_TimeLong_OnScreen=NULL;
	m_pObj_NowPlaying_Speed_OnScreen=NULL;
	m_pObj_Highlighted_Last = NULL;
	m_pObj_Highlighted_set(NULL);
	dg.Release(  );

	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );
	m_bRerenderScreen=true;
	m_bShowShortcuts=false;
	// The framework will call this when it's time to change screens.  This immediately calls RenderScreen.

	// ATTEN: The siganture was changed from RegionUp() -> RegionUp(int x, int y);
	// TODO: Make sure this is called properly (if the implementation will actually take the params into
	// account it will probably break something). )))
	RegionUp ( 0, 0);

	//  m_listDevice_Selected=""; // Nothing is selected anymore
	//  m_pLastSelectedObject=NULL;

	// todo 2.0?    m_bDrawBackFirst=true;
	// todo 2.0?    int Dim=100;

	if ( m_pScreenHistory_Current )
	{
		// If the screen saver is on, and we're not changing to the screen saver, in UI1 this means shut it off
		if( m_bScreenSaverActive && pScreenHistory->GetObj()!=m_pDesignObj_Orbiter_ScreenSaveMenu )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::NeedToChangeScreens Goto Screen -- wakign up from screen saver");
#endif
			StopScreenSaver();  // The function will ignore this for ui2 if there's no media playing

#ifdef ENABLE_MOUSE_BEHAVIOR
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::NeedToChangeScreens calling CMD_Show_Mouse_Pointer mb %p vis %d",
				m_pMouseBehavior, m_pMouseBehavior ? (int) m_pMouseBehavior->m_bMouseVisible : 0);
#endif
			if( NULL != m_pMouseBehavior && m_pMouseBehavior->m_bMouseVisible )  // If m_bMouseVisible is true, it should be visible since screen saver doesn't set this, and that way if the screen saver hid it we'll restore it
				CMD_Show_Mouse_Pointer("1");
#endif
		}

		m_pScreenHistory_Current->GetObj()->m_bActive=false;

		for(list<class PlutoPopup*>::iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.begin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.end();++it)
		{
			PlutoPopup *pPlutoPopup = *it;
			m_pOrbiterRenderer->ObjectOffScreen( pPlutoPopup->m_pObj );
		}

		PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

		//update the list only if the screen is changed
		if(ScreenHistory::m_bAddToHistory)
		{
			if(
				!m_listScreenHistory.size() ||
				(
					m_pScreenHistory_Current != pScreenHistory && 
					m_listScreenHistory.back()->PK_Screen() != m_pScreenHistory_Current->PK_Screen()
				)
			)
			{
				m_listScreenHistory.push_back( m_pScreenHistory_Current );
	#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::NeedToChangeScreens m_listScreenHistory Adding to screens history list screen %d size %d", m_pScreenHistory_Current->PK_Screen(),(int) m_listScreenHistory.size());
	#endif

				if ( m_listScreenHistory.size(  ) > 64 )
				{
					delete m_listScreenHistory.front();
					m_listScreenHistory.pop_front(  );
				}
			}
		}
	}

	//save context
	if(NULL == m_pContextToBeRestored && NULL != m_pScreenHistory_New && NULL != m_pScreenHistory_New->GetObj())
	{
		map<DesignObj_Orbiter *, bool> mapVisibilityContext;
		GetChildrenVisibilityContext(m_pScreenHistory_New->GetObj(), mapVisibilityContext);
		m_pScreenHistory_New->SaveContext(m_mapVariable, mapVisibilityContext);
	}

	//restoring variables and visibility status for objects
	if(NULL != m_pContextToBeRestored)
	{
		map<DesignObj_Orbiter *, bool> mapVisibilityContext;
		m_pContextToBeRestored->RestoreContext(m_mapVariable, mapVisibilityContext);
		SetChildrenVisibilityContext(m_pContextToBeRestored->GetObj(), mapVisibilityContext);
		m_pContextToBeRestored = NULL;
	}

	ScreenHistory::m_bAddToHistory = true;

	// todo 2.0 SelectFirstObject(  );
	if( (m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu) && !m_bBypassScreenSaver && m_iTimeoutBlank )
	{
		m_tTimeoutTime = time(NULL) + m_iTimeoutBlank;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::NeedToChangeScreens About to call maint for display off with timeout: %d ms", m_iTimeoutBlank * 1000);
#endif

		CallMaintenanceInMiliseconds( m_iTimeoutBlank * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}
	else if( !(m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu) && !m_bBypassScreenSaver && m_iTimeoutScreenSaver )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::NeedToChangeScreens About to call maint for screen saver with timeout: %d ms", m_iTimeoutScreenSaver * 1000);
#endif
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}

	m_pScreenHistory_Current=pScreenHistory;
	m_pScreenHistory_Current->GetObj()->m_bActive=true;
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::NeedToChangeScreens need to change timeout %d",m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds);
#endif

	if( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds )
	{
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::NeedToChangeScreens calling timeout");
#endif
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->GetObj(), pe_ALL );
	}

	m_cCurrentScreen=m_pScreenHistory_Current->GetObj()->m_cScreenType;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::NeedToChangeScreens Changing screen to %s ir %d type %c",
		m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(  ), m_dwPK_Device_LocalOsdIRReceiver, m_cCurrentScreen);
#endif

	if( m_dwPK_Device_LocalOsdIRReceiver && m_bIsOSD )
	{
		// Send a message to all IR transcievers
		DCE::CMD_Set_Screen_Type_Cat CMD_Set_Screen_Type_Cat(m_dwPK_Device,DEVICECATEGORY_Infrared_Receivers_CONST,true,BL_SameRoom,m_cCurrentScreen);
		// DCE::CMD_Set_Screen_Type CMD_Set_Screen_Type(m_dwPK_Device,m_dwPK_Device_LocalOsdIRReceiver,m_cCurrentScreen);
		SendCommand(CMD_Set_Screen_Type_Cat);
	}

//>> offscreen for all objects
//m_pOrbiterRenderer->ObjectOffScreenWrapper() 
	m_pOrbiterRenderer->ObjectOnScreenWrapper(  );

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	//if we are now in main menu, we'll purge the history; this is the initial nod from the navigation "tree"
	if(m_pScreenHistory_Current->GetObj()->m_iBaseObjectID == atoi(m_sMainMenu.c_str()) && m_pScreenHistory_Current->GetObj()!=m_pDesignObj_Orbiter_ScreenSaveMenu)
	{
		list<ScreenHistory *>::iterator it_end = m_listScreenHistory.end(); //cache the end of the list
		for(list<ScreenHistory *>::iterator it = m_listScreenHistory.begin(); it != it_end; ++it)
			if(m_pScreenHistory_Current != *it)
				delete *it;

		m_listScreenHistory.clear();
	}

#ifdef DEBUG
	DumpScreenHistory();
#endif

	// Service screens that should be queued, there may be some others queued up too
	ServiceInterruptionQueue();
}
//------------------------------------------------------------------------
void Orbiter::SelectedObject( DesignObj_Orbiter *pObj,  SelectionMethod selectionMethod, int X,  int Y)
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

	//it's the background object? it's not our object!
	if(NULL != pObj->m_pParentObject)
		m_pObj_SelectedLastScreen = pObj;

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbObjectSelected, ObjectInfoBackData, (pObj, X, Y, pObj->m_iBaseObjectID))
	if(ExecuteScreenHandlerCallback(cbObjectSelected))
		return;

	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = (DesignObj_DataGrid *) pObj;
		DataGridTable *pDataGridTable = pDesignObj_DataGrid->DataGridTable_Get();
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		if( X>=0 && Y>=0 )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_STATUS,  "Selected datagrid: %s", pObj->m_ObjectID.c_str(  ) );
#endif
			if ( !SelectedGrid( ( DesignObj_DataGrid * ) pObj,  X-pObj->m_rPosition.X,  Y-pObj->m_rPosition.Y, selectionMethod ) )
				return;
		}
		else if( pDataGridTable )
		{
			PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Enter key press. Status: iHighlightedColumn %d, GridCurCol %d, iHighlightedRow %d, GridCurRow %d",
				pDesignObj_DataGrid->m_iHighlightedColumn, pDesignObj_DataGrid->m_GridCurCol,
				pDesignObj_DataGrid->m_iHighlightedRow, pDesignObj_DataGrid->m_GridCurRow);
#endif
			int Column = pDesignObj_DataGrid->m_iHighlightedColumn != -1 ? 
				pDesignObj_DataGrid->m_iHighlightedColumn + pDesignObj_DataGrid->m_GridCurCol : 
				pDesignObj_DataGrid->m_GridCurCol;

			int Row = pDesignObj_DataGrid->m_iHighlightedRow != -1 ? 
				pDesignObj_DataGrid->m_iHighlightedRow + pDesignObj_DataGrid->m_GridCurRow :
				0;

			DataGridCell *pCell = pDataGridTable->GetData(Column, Row);
			if(pCell)
			{
				SelectedGrid(pDesignObj_DataGrid, pCell, selectionMethod,Row,Column);
				m_pOrbiterRenderer->RenderObjectAsync(pDesignObj_DataGrid);
			}
		}
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS,  "Selected objs: %s with %d zones", pObj->m_ObjectID.c_str(  ), ( int ) pObj->m_ZoneList.size(  ) );
#endif

	// There's a problem that we draw the selected state before we show or hide other objects,  and this causes
	// the other objects to be drawn on top of the selected state.  We'll execute the commands first so that
	// show/hides are executed before setting the selected state
	DesignObjZoneList::iterator iZone;

	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
	{
		DesignObjZone *pDesignObjZone = ( *iZone );
		if(  pDesignObjZone->m_Rect.Width==0 || pDesignObjZone->m_Rect.Height==0 || pDesignObjZone->m_Rect.Contains( X, Y )  )
		{
			ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, selectionMethod, X, Y );
		}
	}

	if(  pObj->m_vectSelectedGraphic.size() && pObj->m_GraphicToDisplay != GRAPHIC_SELECTED ) // TODO 2.0 && m_ChangeToScreen.length(  ) == 0 )
	{
		pObj->m_GraphicToDisplay_set("o1",GRAPHIC_SELECTED);
		m_pOrbiterRenderer->RenderObjectAsync(pObj);

		if(pObj->m_vectSelectedGraphic.size())
		{
			PlutoGraphic *pPlutoGraphic = pObj->m_vectSelectedGraphic[0];

			if(pPlutoGraphic->m_GraphicFormat != GR_MNG)
			{
				// Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  
				if(  !pObj->m_bDontResetState  )
					CallMaintenanceInMiliseconds( 500, &Orbiter::DeselectObjects, ( void * ) pObj, pe_NO );
			}
		}

		// Unless the screen's don't reset state is set,  we'll clear any other selected graphics
		if(  !m_pScreenHistory_Current->GetObj()->m_bDontResetState  )
		{
			for( size_t s=0;s<m_vectObjs_Selected.size(  );++s )
			{
				DesignObj_Orbiter *pObj_Sel = m_vectObjs_Selected[s];
				if(  pObj_Sel->m_GraphicToDisplay==GRAPHIC_SELECTED && !pObj_Sel->m_bDontResetState )
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to reset state for object with id %s",
						pObj_Sel->m_ObjectID.c_str());
#endif
					//if it is playing, cancel this
					pObj_Sel->m_pvectCurrentPlayingGraphic = NULL;
					pObj_Sel->m_iCurrentFrame = 0;
					pObj_Sel->m_GraphicToDisplay_set("o2",GRAPHIC_NORMAL);

#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "State reseted for object with id %s",
						pObj_Sel->m_ObjectID.c_str());
#endif
					m_pOrbiterRenderer->RenderObjectAsync(pObj_Sel);
				}
			}
			m_vectObjs_Selected.clear(  );
		}
		m_vectObjs_Selected.push_back( pObj );
	}

#ifdef PRONTO
	if ( pObj->m_ObjectType == DESIGNOBJTYPE_Pronto_File_CONST )
	{
		if ( pObj->m_pCCF )
			pObj->m_pCCF->Touch( X-pObj->m_rPosition.X,  Y-pObj->m_rPosition.Y );
		return;
	}
#endif

	//#pragma warning( "bad,  bad hack" )
	if(  pObj->m_iBaseObjectID==2706  )
	{
		// There's a problem when you select the video button.  The music button was already selected,  and saved a snapshop in the 'savebackgroundfordeselect'.
		// The new video objects are drawn,  then the deselected music object,  which includes a snapshot of the music datagrid.  If I put refresh in the video object
		// it has no effect,  since the commands are executed before the deselect state.  Until we find a solution,  hack in a redraw after resetting the video state
		m_pOrbiterRenderer->RenderObjectAsync(m_pScreenHistory_Current->GetObj());
	}
	/*
	string VariableData = "";
	map<string, string>::iterator ivv;
	for( ivv=pObj->m_VariableValue.begin(  );ivv!=pObj->m_VariableValue.end(  );++ivv )
	{
	if ( VariableData.length(  )==0 )
	VariableData = StringUtils::itos( pObj->m_VariableValue.size(  ) );

	VariableData+="|";
	VariableData+=( *ivv ).first+"|"+( *ivv ).second;
	m_mapVariable[atoi( ( *ivv ).first.c_str(  ) )] = ( *ivv ).second;
	}
	*/

	/* todo 2.0
	if( pObj->m_bProcessActionsAtServer )
	{
	Message *pThisMessage = new Message( m_dwPK_Device, DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_PROCESS_BUTTON_AT_SERVER_CONST, 3,
	COMMANDPARAMETER_PK_DesignObj_CONST, StringUtils::itos( pObj->m_iBaseObjectID ),
	C_COMMANDPARAMETER_ONLY_IF_ON_SCREEN_CONST, m_pVisibleScreen->m_ObjectID,
	COMMANDPARAMETER_Text_CONST,  m_mapVariable[VARIABLE_USER_INPUT_CONST] );
	if(  pButtonTouchMessage==NULL  )
	pButtonTouchMessage=pThisMessage;
	else
	pButtonTouchMessage->m_vectExtraMessages.push_back( pThisMessage );
	}
	*/

	/* todo 2.0
	if(  pButtonTouchMessage  )
	{
	#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_CONTROLLER,  "\x1b[33;1m#%d Selected object: %s # \x1b[0m  click: %d, %d", m_dwPK_Device, pObj->m_ObjectID.c_str(  ), X, Y );
	#endif
	Message *pThisMessage = new Message( m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Orbiter_BUTTON_TOUCHED_CONST, 4,
	C_EVENTPARAMETER_PK_OBJECT_CONST, pObj->m_ObjectID.c_str(  ),
	C_EVENTPARAMETER_ID_CONST, StringUtils::itos( CTRLCOMMAND_ACTIVATE ).c_str(  ),
	C_EVENTPARAMETER_XPOS_CONST, StringUtils::itos( X-pObj->m_rPosition.X ).c_str(  ),
	C_EVENTPARAMETER_YPOS_CONST, StringUtils::itos( Y-pObj->m_rPosition.Y ).c_str(  ) );

	pButtonTouchMessage->m_vectExtraMessages.push_back( pThisMessage );

	QueueMessageToRouter( pButtonTouchMessage );
	//      GetEvents(  )->Orbiter_Object_Actions( StringUtils::itos( NumActions ) + "\t" +ActionData,
	//          pObj->m_ObjectID,  StringUtils::itos( CTRLCOMMAND_ACTIVATE ),  StringUtils::itos( X-pObj->m_rPosition.X ),
	//          StringUtils::itos( Y-pObj->m_rPosition.Y ) );
	}
	*/

	if( X<0 && Y<0 )
	{
		// This wasn't selected by a touch.  With a touch all objects under that area are selected
		// automatically.  So if there are keys on a keyboard, both the key, and the keyboard are
		// selected.  If it wasn't selected by touch, such as by i/r or keyboard, we should select
		// our parent object's as well
		if( pObj->m_pParentObject )
			SelectedObject((DesignObj_Orbiter *)pObj->m_pParentObject,selectionMethod);
	}
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  int X,  int Y, SelectionMethod selectionMethod )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if ( !pDesignObj_DataGrid->DataGridTable_Get() )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Selected grid %s but DataGridTable_Get() is NULL",pDesignObj_DataGrid->m_ObjectID.c_str());
#endif
		return false;
	}

	bool bFoundSelection = false,  bFinishLoop = false;

	DataGridCell *pLastCell = NULL;

	int LastI,  LastJ;

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		string delSelections = "|"+m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable]+"|";
	string NewValue;

	vm.Release(  );

	// There's a variable associated with this,  so it's a selectable grid.  Which cell was touched?
	// Go through the cells on the screen
	int i, j;
	DataGridTable *pT = pDesignObj_DataGrid->DataGridTable_Get();

	// Let's say we have six rows,  and we are on the last page.  We'll only show 5 plus an
	// up,  so datagrid will set the row count to be 5 ( you can't past the number of rows left )
	// So,  if we have an up,  we need to loop one more time for the last row only if we
	// are showing all the rows that are left.  Otherwise,  row count will be correct
	for( i=0;i<=pT->m_RowCount && !bFinishLoop ;i++ )
	{
		for( j=pT->m_ColumnCount-1;j>=0 && !bFinishLoop;j-- )
		{
			int x, y, w, h;
			int DGColumn = ( j==0 && pT->m_bKeepColumnHeader ) ? 0 : j+pT->m_StartingColumn;
			int DGRow = ( i==0 && pT->m_bKeepRowHeader ) ? 0 : i+pT->m_StartingRow;

			DataGridCell *pCell=pT->GetData( DGColumn,  DGRow );
			if ( pCell )
			{
				pDesignObj_DataGrid->GetGridCellDimensions(pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				int ContainsX=X+pDesignObj_DataGrid->m_rPosition.X;
				int ContainsY=Y+pDesignObj_DataGrid->m_rPosition.Y;
				vm.Release(  );
				if ( ( delSelections.length(  ) > 2 && delSelections.find( "|"+string( pCell->GetValue(  ) )+"|" )!=string::npos ) )
				{
					if ( bFoundSelection )
					{
						if ( pDesignObj_DataGrid->RenderCell(pT,  pCell,  j,  i ,  GRAPHIC_NORMAL ) )
						{
							m_pOrbiterRenderer->RenderObjectAsync(pDesignObj_DataGrid);
						}
						bFinishLoop = true;
					}
					pLastCell = pCell;
					LastI = i;
					LastJ = j;
				}

				if ( PlutoRectangle( x,  y,  w,  h ).Contains( ContainsX,  ContainsY ) )
				{
					if( DGRow == pT->m_iUpRow )
					{
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
						if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, "");
						}
						dg.Release();
						CMD_Scroll_Grid( "",  pDesignObj_DataGrid->m_ObjectID,  DIRECTION_Up_CONST );
						return false;  // No more processing
					}
					else if( DGRow == pT->m_iDownRow )
					{
						pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
						if(  pDesignObj_DataGrid->m_sExtraInfo.find( 'X' )!=string::npos  )
						{
							CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, "");
						}
						dg.Release();
						CMD_Scroll_Grid( "",  pDesignObj_DataGrid->m_ObjectID,  DIRECTION_Down_CONST );
						return false;  // No more processing
					}

					if( pDesignObj_DataGrid->m_sExtraInfo.find( 'R' )==string::npos )
						pDesignObj_DataGrid->m_iHighlightedColumn=DGColumn;

					if(pDesignObj_DataGrid->m_sExtraInfo.find( 'C' )==string::npos && pDesignObj_DataGrid->DataGridTable_Get()->m_StartingRow)
						pDesignObj_DataGrid->m_iHighlightedRow = DGRow - pDesignObj_DataGrid->DataGridTable_Get()->m_StartingRow + 1;

					SelectedGrid( pDesignObj_DataGrid,  pCell, selectionMethod, DGRow, DGColumn );
					bFinishLoop = true;
					bFoundSelection = true; // Is this correct????  Hacked in this time
					m_pOrbiterRenderer->RenderObjectAsync(pDesignObj_DataGrid);
				}
			}
		}
	}

	return bFoundSelection;
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid,  DataGridCell *pCell, SelectionMethod selectionMethod, int iRow, int iColumn )
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::SelectedGrid cell %p row %d col %d",pCell,iRow,iColumn);
#endif
	if( pDesignObj_DataGrid->m_iPK_Datagrid==DATAGRID_Floorplan_Devices_CONST )
	{
		if( pDesignObj_DataGrid->m_pFloorplanObject==NULL )
			pDesignObj_DataGrid->m_pFloorplanObject = new FloorplanObject();

		if( pCell->m_Text )
			pDesignObj_DataGrid->m_pFloorplanObject->DeviceDescription = pCell->m_Text;
		if( pCell->m_Value )
		{
			pDesignObj_DataGrid->m_pFloorplanObject->PK_Device = atoi(pCell->m_Value);
			pDesignObj_DataGrid->m_pFloorplanObject->m_pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pDesignObj_DataGrid->m_pFloorplanObject->PK_Device);
			pDesignObj_DataGrid->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar = atoi(pCell->m_mapAttributes["PK_DesignObj_Toolbar"].c_str());
		}
		else
			pDesignObj_DataGrid->m_pFloorplanObject->m_pDeviceData_Base=NULL;

		SelectedFloorplan(pDesignObj_DataGrid,iRow);
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		string sValues;

		for(map<int,class DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
			sValues += StringUtils::itos(it->first) + "|";
		CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable,sValues);
	}

	list<DesignObj_DataGrid *> listObj_ToFlush; // A list of grids which are being redrawn within this action and must be refreshed

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbDataGridSelected, DatagridCellBackData, 
		(pCell, pDesignObj_DataGrid, iRow, iColumn, pCell->GetText(), pCell->GetValue(), pDesignObj_DataGrid->m_iPK_Datagrid))
	if(ExecuteScreenHandlerCallback(cbDataGridSelected))
		return true;

	// AB 2005-08-20 is this necessary?  It means you don't stay on the highlighted selection	pDesignObj_DataGrid->m_iHighlightedColumn=pDesignObj_DataGrid->m_iHighlightedRow=-1;
	m_sLastSelectedDatagrid=pCell->GetText();

	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	if(  pCell->m_pMessage  )
	{
		Message *pMessage = new Message( pCell->m_pMessage );
		// See if we are re-rendering this grid.  If so
		if(  pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND &&
			pMessage->m_dwID==COMMAND_Populate_Datagrid_CONST &&
			pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST]==pDesignObj_DataGrid->m_sGridID  )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_CONTROLLER, "re-rendering grid: ( %s", pDesignObj_DataGrid->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ).c_str(  ) );
#endif
			string GridID = pMessage->m_mapParameters[COMMANDPARAMETER_DataGrid_ID_CONST];
			DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen=NULL;
			// See if this grid is onscreen
			for( size_t s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
			{
				DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[s];
				if(  pDesignObj_DataGrid->m_sGridID == GridID  )
				{
					pDesignObj_DataGrid_OnScreen=pDesignObj_DataGrid;
					string::size_type posH;
					if(  (posH=pDesignObj_DataGrid->m_sExtraInfo.find( 'H' ))==string::npos && pDesignObj_DataGrid->sSelVariable.length()==0 )
					{
						pDesignObj_DataGrid->m_iHighlightedRow = (posH==pDesignObj_DataGrid->m_sExtraInfo.length() || pDesignObj_DataGrid->m_sExtraInfo[posH+1]!='C') ? 0 : -1;
						pDesignObj_DataGrid->m_iHighlightedColumn = (posH==pDesignObj_DataGrid->m_sExtraInfo.length() || pDesignObj_DataGrid->m_sExtraInfo[posH+1]!='R') ? 0 : -1;
					}
					else
					{
						pDesignObj_DataGrid->m_iHighlightedRow=-1;
						pDesignObj_DataGrid->m_iHighlightedColumn=-1;
					}
					listObj_ToFlush.push_back(pDesignObj_DataGrid);
				}
			}

			string Response;

			// This message has out parameters with a variable to assign
			Message *pMessage_Out = m_pcRequestSocket->SendReceiveMessage( pMessage );
			if(  pMessage_Out  )
			{
				int iPK_Variable = atoi( pMessage_Out->m_mapParameters[COMMANDPARAMETER_PK_Variable_CONST].c_str(  ) );

				if(iPK_Variable)
				{
					CMD_Set_Variable(iPK_Variable, pMessage_Out->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST]);
				}
				else
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "NOT setting the datagrid variable. It's 0!");
#endif
				}

				delete pMessage_Out;
			}
			pDesignObj_DataGrid->m_GridCurCol = pDesignObj_DataGrid->m_iInitialColNum;
			pDesignObj_DataGrid->m_GridCurRow = pDesignObj_DataGrid->m_iInitialRowNum;
			
		}
		else if( pMessage->m_dwPK_Device_To==m_dwPK_Device || pMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
		{
			pMessage->m_dwPK_Device_To=m_dwPK_Device;
			ReceivedMessage(pMessage);
		}
		else
		{
#ifdef ENABLE_MOUSE_BEHAVIOR
			if(UsesUIVersion2())
			{
				//#pragma warning( "Look into this and the sendreceive above.  one deletes,  the other not.  if it deletes,  you can't select the grid twice.  if not,  the framework will delete.  should make a copy I think" );
				if( selectionMethod==smMouseGovernor && m_pMouseBehavior && m_pMouseBehavior->m_pMouseGovernor )
					m_pMouseBehavior->m_pMouseGovernor->SendMessage(pMessage);
				else
					QueueMessageToRouter( pMessage );
			}
			else
				QueueMessageToRouter( pMessage );
#else
			QueueMessageToRouter( pMessage );
#endif
				

			//          QueueMessageToRouter( pMessage );  // I think this caused some grids not to immediately refresh  **TODO** look into this
			//          pMessage=NULL;  // Send message will delete the messages.  It's okay--it's just a copy anyway
		}
	}

	// WAS: selection on "click to select cell,  set variable,  click play button,  play what in the just set var" interface
	// Side-effect on single select interface: the clicked row was redrawn after drawing the new grid
	if ( !pDesignObj_DataGrid->m_iPK_Variable || !pCell->m_bSelectable )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "No datagrid variable was updated");
#endif
		return true;
	}

	string NewValue;
	int PK_Variable = pDesignObj_DataGrid->m_iPK_Variable;

	//handle multi select
	if(pDesignObj_DataGrid->m_bIsMultiSelect)
	{
		//DataGridTable *pT = pDesignObj_DataGrid->DataGridTable_Get();

		PLUTO_SAFETY_LOCK(vm, m_VariableMutex);
		string delSelections = "|"+m_mapVariable[pDesignObj_DataGrid->m_iPK_Variable]+"|";
		string Selections = m_mapVariable[PK_Variable];
		vm.Release();
		string::size_type valpos;

		int i, j;
		i = pCell->m_Colspan;
		j = pCell->m_Rowspan;

		if((valpos = delSelections.find("|" + string(pCell->GetValue()) + "|")) != string::npos)
		{
			NewValue = delSelections.substr(0, valpos) + Selections.substr(valpos + strlen(pCell->GetValue()));

			if(NewValue.length() > 0)
				NewValue = NewValue.substr(1);
		}
		else
		{
			if(Selections.length() > 0)
				NewValue = Selections + "|" + pCell->GetValue();
			else
				NewValue = pCell->GetValue();
		}
	}
	else
	{
		// Not multi-select.  Just replace it.
		NewValue =  pCell->GetValue(  );
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Need to update variable %d of the datagrid with value %s",
		pDesignObj_DataGrid->m_iPK_Variable, NewValue.c_str());
#endif

	if( pDesignObj_DataGrid->m_iPK_Datagrid!=DATAGRID_Floorplan_Devices_CONST )
		CMD_Set_Variable(PK_Variable, NewValue); // we already set this in the custom block above
	for(list<DesignObj_DataGrid *>::iterator it=listObj_ToFlush.begin();it!=listObj_ToFlush.end();++it)
	{
		m_pOrbiterRenderer->RenderObjectAsync(*it);
		(*it)->Flush();
	}
	return true;
}
//------------------------------------------------------------------------
bool Orbiter::SelectedGrid( int DGRow )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	//used in BD_PC_SelectedFromList
	//
	//this method selects the DGRow-st item from the grid
	//( should to be only one on current screen ) -> for mobile

	if( m_vectObjs_GridsOnScreen.size(  ) != 1 )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING,
			"It should be one and only one visible on this screen!" );

		return false;
	}

	vector<class DesignObj_DataGrid *>::iterator it = m_vectObjs_GridsOnScreen.begin(  );
	DesignObj_DataGrid *pDesignObj_DataGrid = *it;

	int iSelectedColumn = pDesignObj_DataGrid->m_iInitialColNum;
	//if 'c' - column  extraoption is specified, we'll send to phone the specified column
	size_t sPos;
	if((sPos = pDesignObj_DataGrid->m_sExtraInfo.find( 'c' )) != string::npos)
	{
		if(sPos + 1 < pDesignObj_DataGrid->m_sExtraInfo.size())
			iSelectedColumn = pDesignObj_DataGrid->m_sExtraInfo[sPos + 1] - '0';
	}

	DataGridCell *pCell = pDesignObj_DataGrid->DataGridTable_Get()->GetData( iSelectedColumn,  DGRow );

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if(m_pObj_Highlighted == pDesignObj_DataGrid) //datagrid already highlighted. remove first old highlighting
		m_pOrbiterRenderer->UnHighlightObject();

	m_pObj_Highlighted_set(pDesignObj_DataGrid);
	pDesignObj_DataGrid->m_iHighlightedColumn = -1;
	pDesignObj_DataGrid->m_iHighlightedRow = DGRow;
	pDesignObj_DataGrid->m_GridCurCol = iSelectedColumn;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected row: %d, selected column: %d",
		DGRow, iSelectedColumn);
#endif

	if( pCell )
		CMD_Set_Variable(pDesignObj_DataGrid->m_iPK_Variable, pCell->GetValue());

	return true;
}
void Orbiter::SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter)
{
	if( pDesignObj_Orbiter->m_pFloorplanObject )
		SelectedFloorplan(pDesignObj_Orbiter);

	if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butPINGo_CONST)
	{
		if( m_bRestrictedOp_IsUser )
		{
			bool bValid=false;
			DCE::CMD_Verify_PIN CMD_Verify_PIN(m_dwPK_Device, m_dwPK_Device_SecurityPlugIn,
				m_iRestrictedOp_ID,m_mapVariable[VARIABLE_Datagrid_Input_CONST],&bValid);
			if( SendCommand(CMD_Verify_PIN) && bValid )
			{
				CMD_Set_Current_User(m_iRestrictedOp_ID);
				CMD_Goto_DesignObj(0,m_sMainMenu,"","",false,false);
			}
			else
			{
				CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
				CMD_Set_Text(m_pScreenHistory_Current->GetObj()->m_ObjectID,"***INVALID***",TEXT_PIN_Code_CONST);
			}
		}
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_mnuPingPong_CONST)
	{
		Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_START_PING,m_dwPK_Device,0);
		QueueMessageToRouter(pMessage);
		g_pReceivePingHandler=ReceivePingHandler;
		g_pSendPingHandler=SendPingHandler;
		string sMessage = "Starting PING/PONG S" + StringUtils::itos(m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(m_MaintThreadMutex.m_NumLocks);
			//+ " R" + StringUtils::itos(m_NeedRedrawVarMutex.m_NumLocks);
		CMD_Set_Text(pDesignObj_Orbiter->m_ObjectID,sMessage,TEXT_STATUS_CONST);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butStopPing_CONST)
	{
		Message *pMessage = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_STOP_PING,m_dwPK_Device,0);
		QueueMessageToRouter(pMessage);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_objHouseStatusIndicator_CONST )
	{
		// This is actually at startup time.  Figure out what device group this is controlling
		int PK_DeviceGroup=0;
		for(DesignObjZoneList::iterator itZ=pDesignObj_Orbiter->m_ZoneList.begin();itZ!=pDesignObj_Orbiter->m_ZoneList.end();++itZ)
		{
			DesignObjZone *pDesignObjZone = *itZ;

			for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
			{
				DesignObjCommand *pDesignObjCommand = *it;
				if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
				{
					int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
					if( PK_Variable == VARIABLE_PK_DeviceGroup_CONST )
						PK_DeviceGroup = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				}
			}
		}
		CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "housemode" + StringUtils::itos(PK_DeviceGroup),true);
	}
	else if ( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butWeatherMain_CONST || pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoWeatherNow_CONST )
	{
		CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "weather", true);
	}
	else if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butUserStatus_CONST || pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoUserVoicemail_CONST )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter_With_Commands = pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_butUserStatus_CONST ? pDesignObj_Orbiter : (DesignObj_Orbiter *) pDesignObj_Orbiter->m_pParentObject;
		if( pDesignObj_Orbiter_With_Commands->m_ZoneList.size()==0 && pDesignObj_Orbiter_With_Commands->m_pParentObject )
			pDesignObj_Orbiter_With_Commands = (DesignObj_Orbiter *) pDesignObj_Orbiter_With_Commands->m_pParentObject;

		// This is actually at startup time.  Figure out what user this is controlling
		int PK_Users=0;
		for(DesignObjZoneList::iterator itZ=pDesignObj_Orbiter_With_Commands->m_ZoneList.begin();itZ!=pDesignObj_Orbiter_With_Commands->m_ZoneList.end();++itZ)
		{
			DesignObjZone *pDesignObjZone = *itZ;

			for(DesignObjCommandList::iterator it=pDesignObjZone->m_Commands.begin();it!=pDesignObjZone->m_Commands.end();++it)
			{
				DesignObjCommand *pDesignObjCommand = *it;
				if( pDesignObjCommand->m_PK_Command==COMMAND_Set_Variable_CONST )
				{
					int PK_Variable = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());
					if( PK_Variable == VARIABLE_PK_Users_CONST )
						PK_Users = atoi(pDesignObjCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
				}
			}
		}
	
		if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_icoUserVoicemail_CONST )
		{
			CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "vm" + StringUtils::itos(PK_Users),true);
			// Get the text object, and set it
			if( pDesignObj_Orbiter->m_vectDesignObjText.size() )
				pDesignObj_Orbiter->m_vectDesignObjText[0]->m_sText="<%=Bvm" + StringUtils::itos(PK_Users) + "%>";

		}
		else
			CMD_Bind_Icon(pDesignObj_Orbiter->m_ObjectID, "user" + StringUtils::itos(PK_Users),true);
	}
}

void Orbiter::SelectedFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter,int Row)
{
	// pDesignObj_Orbiter->m_pFloorplanObject->PK_Device will be >0 for a device, <0 for an entertainment area
	CMD_Set_Variable(VARIABLE_Array_ID_CONST,pDesignObj_Orbiter->m_pFloorplanObject->ObjectTypeDescription); // ie Ceiling light ( for an ea: Pluto Media Dir )
	CMD_Set_Variable(VARIABLE_Array_Desc_CONST,pDesignObj_Orbiter->m_pFloorplanObject->DeviceDescription); // ie Light switch (on/off) ( for an ea: Bedroom )
	CMD_Set_Variable(VARIABLE_Status_CONST,pDesignObj_Orbiter->m_pFloorplanObject->Status); // ie On

	// If this has a toolbar, and either (1) there is no previously selected object,
	// (2) there was a previously selected object but it wasn't a floorplan or (3) used a different toolbar
	// then we need to activate this toolbar and remove any previous one
	int PK_DesignObj_Toolbar_ToTurnOn=0,PK_DesignObj_Toolbar_ToTurnOff=0;
	if( pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar &&
		(!m_pObj_LastSelected || m_dwPK_DesignObj_Toolbar_Last!=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar) )
	{
		PK_DesignObj_Toolbar_ToTurnOn=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;
		if( m_dwPK_DesignObj_Toolbar_Last )
			PK_DesignObj_Toolbar_ToTurnOff=m_dwPK_DesignObj_Toolbar_Last;
	}
	else if( m_pObj_LastSelected && m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar &&
		pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar!=m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar )
		PK_DesignObj_Toolbar_ToTurnOff=m_pObj_LastSelected->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;


	// We've selected this object twice, cycle through the vector of device groups
	if( (m_pObj_LastSelected==pDesignObj_Orbiter && Row==-1) || (Row!=-1 && Row==m_iRow_Floorplan_LastSelected) )
	{
		// We went past the end, select nothing.  Or this is an ent area with no device pointer, so there are no groups anyway
		if( !pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base ||
			m_iLastEntryInDeviceGroup>=(int) pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup.size()-1 )
		{
			m_pOrbiterRenderer->RenderObjectAsync((DesignObj_Orbiter *) pDesignObj_Orbiter->m_pParentObject);

			m_mapDevice_Selected.clear();
			for(map<int,class FloorplanObject *>::iterator itFPO=m_mapFloorplanObject_Selected.begin();itFPO!=m_mapFloorplanObject_Selected.end();++itFPO)
				delete itFPO->second;
			m_mapFloorplanObject_Selected.clear();
			m_pObj_LastSelected=NULL;
			m_dwPK_DesignObj_Toolbar_Last=0;
			m_iRow_Floorplan_LastSelected=-1;
			m_iLastEntryInDeviceGroup=-1;  // Start at the beginning
			PK_DesignObj_Toolbar_ToTurnOn=0;
			PK_DesignObj_Toolbar_ToTurnOff=pDesignObj_Orbiter->m_pFloorplanObject->m_dwPK_DesignObj_Toolbar;
		}
		else
		{
			// Maybe this doesn't belong to any device groups, just toggle on and off
			if( pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup.size()==0 )
			{
				m_mapDevice_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base;
				m_mapFloorplanObject_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = new FloorplanObject(pDesignObj_Orbiter->m_pFloorplanObject);
				m_iLastEntryInDeviceGroup=0;  // If we touch it again, it will trigger the block above
			}
			else
			{
				// Select all the items in the group
				++m_iLastEntryInDeviceGroup;
				DeviceGroup *pDG = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base->m_vectDeviceGroup[m_iLastEntryInDeviceGroup];
				for(int i=0;i<(int) pDG->m_vectDeviceData_Base.size();++i)
				{
					DeviceData_Base *pD = pDG->m_vectDeviceData_Base[i];
					m_mapDevice_Selected[pD->m_dwPK_Device] = pD;
				}
			}
		}

	}
	else
	{
		if( m_mapDevice_Selected.find(pDesignObj_Orbiter->m_pFloorplanObject->PK_Device)!=m_mapDevice_Selected.end() )
			m_mapDevice_Selected.erase(pDesignObj_Orbiter->m_pFloorplanObject->PK_Device);
		else
		{
			m_mapDevice_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = pDesignObj_Orbiter->m_pFloorplanObject->m_pDeviceData_Base;
			m_mapFloorplanObject_Selected[pDesignObj_Orbiter->m_pFloorplanObject->PK_Device] = new FloorplanObject(pDesignObj_Orbiter->m_pFloorplanObject);
		}
		m_pObj_LastSelected=pDesignObj_Orbiter;
		m_iRow_Floorplan_LastSelected=Row;
	}

	// The toolbar will be a direct child of the topmost object, whether it's a screen or a popup
	if( PK_DesignObj_Toolbar_ToTurnOn )
	{
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOn),0,"","","1");
		m_dwPK_DesignObj_Toolbar_Last=PK_DesignObj_Toolbar_ToTurnOn;
	}
	if( PK_DesignObj_Toolbar_ToTurnOff )
		CMD_Show_Object(pDesignObj_Orbiter->TopMostObject()->m_ObjectID + "." + StringUtils::itos(PK_DesignObj_Toolbar_ToTurnOff),0,"","","0");

	CMD_Refresh("");
}
//------------------------------------------------------------------------
bool Orbiter::ClickedButton( DesignObj_Orbiter *pObj, int PK_Button )
{
	bool bFoundHandler=false;

	if ( pObj->IsHidden(  ) )
		return false;

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
		bool bResult=ClickedButton( pDesignObj_Orbiter, PK_Button );
		if(  !bFoundHandler && bResult  )
			bFoundHandler=true;
	}
	/*
	if( pObj->m_ObjectID.find("1255.2.0.3407")!=string::npos )
	{
	int k=2;
	}
	*/
	if(
		pObj->m_iPK_Button == PK_Button ||
		pObj->m_iPK_Button == BUTTON_Any_key_CONST ||
		(
			pObj->m_iPK_Button == BUTTON_Alphanumeric_keys_CONST &&
			(
				(PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST) ||
				(PK_Button >= BUTTON_A_CONST && PK_Button <= BUTTON_Z_CONST) ||
				(PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST)
			)
		)
	)
	{
		if ( UsesUIVersion2() )
		{
			m_pObj_Highlighted_set(pObj);
			CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), 0,"");
			return true;
		}
		SelectedObject( pObj, smKeyboard );
		
		return true;
	}

	return bFoundHandler;
}
//------------------------------------------------------------------------
bool Orbiter::ClickedRegion( DesignObj_Orbiter *pObj, int X, int Y, DesignObj_Orbiter *&pTopMostAnimatedObject )
{
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex );

	if(pObj->IsHidden())
		return false;

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		ClickedRegion( ( DesignObj_Orbiter * )( *iHao ), X, Y, pTopMostAnimatedObject );
	}

	if( pObj->m_rPosition.Contains( X, Y ) )
	{
		if(  pObj->m_bAnimate && !pTopMostAnimatedObject  )  // These get called from topmost down,  so don't overwrite if a higher object already did
		{
			pTopMostAnimatedObject = pObj;
		}

		if( pObj->m_dbHitTest.m_dwSize )
		{
			int LineWidth = pObj->m_rBackgroundPosition.Width / 8 + 1; // The width of each line
			int x = X - pObj->m_rPosition.Left();
			int y = Y - pObj->m_rPosition.Top();
			char c = pObj->m_dbHitTest.m_pBlock[ y * LineWidth + x/8 ];
			if( !(c & 1 << (x%8)) )
				return false;
		}

		SelectedObject( pObj, smMouse, X, Y );
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
/*virtual*/ void Orbiter::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if(sbNoSelection != m_nSelectionBehaviour)
		for(int i = 0; i < 4; i++)
			m_pOrbiterRenderer->HollowRectangle(
				point.X + pObj->m_rBackgroundPosition.X + i, point.Y + pObj->m_rBackgroundPosition.Y + i,
				pObj->m_rBackgroundPosition.Width - 2 * i - 2, pObj->m_rBackgroundPosition.Height - 2 * i - 2,
				i < 2 ? PlutoColor::Blue() : PlutoColor::White()
			);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::SelectObject\n");
#endif

}
//------------------------------------------------------------------------
/*
INITIALIZATION
*/
//-----------------------------------------------------------------------------------------------------------
void Orbiter::Initialize( GraphicType Type, int iPK_Room, int iPK_EntertainArea )
{
	if ( !m_bQuit_get())
	{
#ifdef WIN32
		size_t iSize;
		char * pData = FileUtils::ReadFileIntoBuffer("/usr/pluto/orbiter/skins/Basic/menu2/OrbiterBkg.png", iSize);
		if (pData)
		{
			m_pBackgroundImage = m_pOrbiterRenderer->CreateGraphic();
			m_pBackgroundImage->m_Filename = "OrbiterBkg.png";
			m_pBackgroundImage->LoadGraphic(pData, iSize);
			delete [] pData;
		}
#endif

		m_bStartingUp=true;
		{
			while(true)
			{
				string sStatus,sRegenStatus;
				int iRegenPercent;
				DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
					m_dwPK_Device,&sStatus,&sRegenStatus,&iRegenPercent);
				bool bResult = SendCommand(CMD_Get_Orbiter_Status_DT);
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Initialize Get_Orbiter_Status (%d) -%s-",(int) bResult,sStatus.c_str());
				int iResponse;
				if( !bResult || sStatus!="O" )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter plugin reported status of <%s>",sStatus.c_str());
					if( (iResponse=HandleNotOKStatus(sStatus,sRegenStatus,iRegenPercent))==0 )
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Handle not ok said to quit");
						m_bQuit_set(true);
						return;
					}
					else if( iResponse==1 )
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Handle not ok said to load anyway");
						break;
					}
					else
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Handle not ok said to try again");
				}
				else
					break;
			}

			NeedToRender render( this, "Initial config" );
			Message *pMessage=NULL;
			string Filename = "C" + StringUtils::itos( m_dwPK_Device ) + ".info";
			bool bFileExists;
			if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
			{
				if(  m_sLocalDirectory.length(  )==0  )
				{
#ifdef WIN32
					m_sLocalDirectory = "/pluto/orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#else
					m_sLocalDirectory = "/usr/pluto/orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
#endif
					if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
					{
						m_sLocalDirectory = "orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
						if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
						{
							m_sLocalDirectory = "../orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/";
							if(  !( bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename ) )  )
							{
								m_sLocalDirectory = "C" + StringUtils::itos( m_dwPK_Device ) + "/";
								bFileExists=FileUtils::FileExists( m_sLocalDirectory + Filename );
							}
						}
					}
				}
			}
			bool bReadLocalFile=false;
			if(  bFileExists  )
			{
				size_t size;
				char *buffer = FileUtils::ReadFileIntoBuffer( m_sLocalDirectory + "/" + Filename, size );

				if(  !buffer || !SerializeRead( ( long ) size, buffer, ( void * ) this ) || !ParseConfigurationData( Type ) )
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Unable to read Orbiter data from file: %s/%s.  Will try the serve", m_sLocalDirectory.c_str(  ), Filename.c_str(  ) );
				else
					bReadLocalFile=true;
				delete [] buffer;
			}
			if( !bReadLocalFile )
			{
				if(  m_sLocalDirectory.length(  )  )
				{
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Directory: %s doesn't contain the orbiter's .info file.  Requesting files from the server.", m_sLocalDirectory.c_str(  ) );
					m_sLocalDirectory="";  // It's not valid
				}

				// Request our config info
				char *pConfigFile=NULL;
				int iSizeConfigFile=0;
				// We can't send it to the General Info virtual device since we won't have that until we get our config data
				DCE::CMD_Request_File_Cat CMD_Request_File_Cat( m_dwPK_Device, DEVICECATEGORY_General_Info_Plugins_CONST, false,  BL_SameHouse, "orbiter/C" + StringUtils::itos( m_dwPK_Device ) + "/" + Filename + ".lzo",
					&pConfigFile, &iSizeConfigFile );
				SendCommand( CMD_Request_File_Cat );

				if ( iSizeConfigFile )  // Decompress it
				{
					int UncompressedLength = *((int *)pConfigFile);
					char *UncompressedData = new char[UncompressedLength];
					lzo_uint new_len;
					lzo1x_decompress((lzo_byte *)pConfigFile+4,iSizeConfigFile - 4,(lzo_byte *)UncompressedData,&new_len,NULL);
					delete[] pConfigFile;
					pConfigFile = UncompressedData;
					iSizeConfigFile=UncompressedLength;
				}

				if ( !iSizeConfigFile )
				{
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Unable to get Orbiter data" );
					m_pOrbiterRenderer->PromptUser("I cannot read the Orbiter configuration from the server.  I'll try to regenerate it");
					RegenOrbiter();
					Sleep(2000);
					OnReload();
					return;
				}

				// The framework needs a pointer to us when it's creating Orbiter objects,  pass in this as a void *

				if(  !SerializeRead( iSizeConfigFile, pConfigFile, ( void * ) this ) || !ParseConfigurationData( Type ) )
				{
					delete pMessage;
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Orbiter data corrupt" );
					m_pOrbiterRenderer->PromptUser("The Orbiter configuration from the server is corrupt.  I'll try to regenerate it");
					RegenOrbiter();
					Sleep(2000);
					OnReload();
					return;
				}

				delete pConfigFile;
			}

			if(  !m_bLocalMode  )
			{
				int Width = atoi( m_pEvent->GetDeviceDataFromDatabase( m_dwPK_Device, DEVICEDATA_ScreenWidth_CONST ).c_str() );
				int Height = atoi( m_pEvent->GetDeviceDataFromDatabase( m_dwPK_Device, DEVICEDATA_ScreenHeight_CONST ).c_str() );
				if( Width!=m_Width || Height!=m_Height )
				{
					LoggerWrapper::GetInstance()->Write( LV_WARNING,  "The resolution has changed" );
					m_pOrbiterRenderer->PromptUser("The resolution has changed.  I'll regenerate the user interface.");
					RegenOrbiter();
					Sleep(2000);
					OnReload();
					return;
				}

				PLUTO_SAFETY_LOCK_ERRORSONLY( sSM, GetEvents(  )->m_pClientSocket->m_SocketMutex );  // Don't log anything but failures
				string sResult = GetEvents(  )->SendReceiveString( "TIME" );

				if(  sResult.length(  )>5  )
				{
					char *Buffer = new char[sResult.length(  )+1];
					strcpy( Buffer,  sResult.c_str(  ) );
					SetTime( Buffer );
					delete [] Buffer;
				}
				sSM.Release(  );
			}

			if(NULL != m_pCacheImageManager && !m_pCacheImageManager->VerifyCache(StringUtils::ltos((long)m_tGenerationTime)))
			{
				delete m_pCacheImageManager;
				m_pCacheImageManager = NULL;
			}

			//      PreloadObjects(  );

			/*
			if(  !m_bLocalMode  )
			Event_Orbiter_Started( m_sMainMenu );
			*/

			m_pLocationInfo_Initial=NULL;
			for( size_t s=0;s<m_dequeLocation.size();++s)
			{
				class LocationInfo *pLocationInfo = m_dequeLocation[s];
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Checking %d/%d room: %d ea %d against %d %d",
					(int) s,pLocationInfo->iLocation,pLocationInfo->PK_Room,pLocationInfo->PK_EntertainArea,iPK_Room,iPK_EntertainArea);
#endif
				// If a room/ent area is passed in use that location instead of m_iLocation_Initial
				if( (pLocationInfo->iLocation==m_iLocation_Initial && !m_pLocationInfo_Initial) ||
					(pLocationInfo->PK_Room==iPK_Room && pLocationInfo->PK_EntertainArea==0) ||
					(iPK_EntertainArea && pLocationInfo->PK_EntertainArea==iPK_EntertainArea) )
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"using location in size: %d",(int) m_dequeLocation.size());
#endif
					m_pLocationInfo_Initial = pLocationInfo;
				}
			}

			if(  !m_pLocationInfo_Initial  )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "No initial Location" );
//				m_pOrbiterRenderer->PromptUser("Something went very wrong. No initial location!  I'll try to do a regen.");
				RegenOrbiter();
				Sleep(2000);
				OnReload();
			}
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting current location and initial screen %s",m_sInitialScreen.c_str());
#endif

			if( m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD )
			{
				string sCapabilities = m_pEvent->GetDeviceDataFromDatabase( m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD, DEVICEDATA_Capabilities_CONST );
				m_bSpeedIconsOnVfd = sCapabilities.find("SPEED_ICONS")!=string::npos;
			}

			CMD_Set_Current_Location(m_pLocationInfo_Initial->iLocation);
			m_pLocationInfo = m_pLocationInfo_Initial;
			m_dwPK_Users = DATA_Get_PK_Users();
			if( !m_dwPK_Users )
				m_dwPK_Users = m_dwPK_Users_Default;

			char *pData=NULL; int iSize=0;
			DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, "1",
				m_dwPK_Users,StringUtils::itos(m_pLocationInfo->PK_EntertainArea),m_pLocationInfo->PK_Room, &pData, &iSize);
			DCE::CMD_Set_Current_User CMD_Set_Current_User( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_dwPK_Users );
			CMD_Orbiter_Registered.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Current_User.m_pMessage);
			if( !SendCommand( CMD_Orbiter_Registered ) )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot register with router" );
				m_pOrbiterRenderer->PromptUser("Orbiter cannot register with the router. Please try again later.");

				exit(1);
				return;
			}
			m_pOrbiterFileBrowser_Collection = new OrbiterFileBrowser_Collection;
			m_pOrbiterFileBrowser_Collection->SerializeRead(iSize,pData);
			delete[] pData;

			DesignObj_OrbiterMap::iterator iHao=m_ScreenMap.begin(  );
			if ( iHao==m_ScreenMap.end(  ) )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "No screens found." );
				m_pOrbiterRenderer->PromptUser("Something went very wrong. No screens found!");
				exit( 1 );
			}
			else if( m_sInitialScreen.size() )
				CMD_Goto_Screen("", atoi(m_sInitialScreen.c_str()));
			else
				GotoMainMenu();

			// AB 3/26/2006.  The sequence here is tough.  We can't process received messages
			// until the m_pScreenHistory_Current is valid.  Normally it's not valid until the
			// screen goto is complete.  Previously the end of scope was here, so it did the
			// goto screen at this point.  But this meant the onload actions for the screen 
			// were executed before the onstartup.  So, we'll temporarily fake the current screen
			// so the startup commands can go through.
			m_pScreenHistory_Current=render.m_pScreenHistory_get();

			// AB 12/29/2005 -- Needed to move this up since orbiter registered may cause
			// bind icon commands to come back, which aren't available until after the startup commands
			// And we can't call the startup commands until the above scope is terminated so a goto a screen with NeedToRender
			if( m_pScreenHistory_Current )
			{
				DesignObj_OrbiterMap::iterator itDesignObjOrbiter;
				for(itDesignObjOrbiter = m_mapObj_All.begin(); itDesignObjOrbiter != m_mapObj_All.end(); itDesignObjOrbiter++)
				{
					DesignObj_Orbiter* pObj = (*itDesignObjOrbiter).second;
					if(  pObj->m_Action_StartupList.size(  )>0  )
						ExecuteCommandsInList( &pObj->m_Action_StartupList, pObj, smLoadUnload );
				}
			}

			m_bIgnoreFirstEvent = DATA_Get_Ignore_First_Event();
			if( !m_bNewOrbiter && DATA_Get_Leave_Monitor_on_for_OSD() )
				m_bDisplayOn=false;  // So the first touch will turn it on
			else
				m_bDisplayOn=true;  // The display should already be on.  If this is the first time it's run, the user must have left the tv on already
			if( m_bNewOrbiter )
				m_iTimeoutScreenSaver=m_iTimeoutBlank=0;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"monitor m_bDisplayOn initially set to %d",(int) m_bDisplayOn);
#endif

			// Create another map to go from remote id to device id
			for(map<string, pair<int,int> >::iterator it=m_pOrbiterFileBrowser_Collection->m_mapRemoteControls.begin();
				it!=m_pOrbiterFileBrowser_Collection->m_mapRemoteControls.end();++it)
					m_pOrbiterFileBrowser_Collection->m_mapRemoteID_Device[ it->second.second ] = it->second.first;

#if !defined(PROXY_ORBITER) && !defined(BLUETOOTH_DONGLE) && !defined(MOXI_ORBITER) && !defined(WIN32)
			CreateChildren();
			WaitForRelativesIfOSD();
#endif

			// Debug various issues with startup sequence irregularities
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::Initialize finished startup phase");
		} // Originally this context ended before the startup actions.  This meant onload actions,
		// like Send Orbiter Popups, were getting sent out before the orbiter registered command.  In the 
		// case of popups, this meant popups were disabled and then immediately enabled again

		if( !m_pScreenHistory_Current )
		{
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "No initial screen" );
			m_pOrbiterRenderer->PromptUser("Something went very wrong. No initial screen!");
			OnReload();
			return;
		}

		m_pOrbiterRenderer->AdjustWindowSize(m_pScreenHistory_Current->GetObj()->m_rPosition.Width,
			m_pScreenHistory_Current->GetObj()->m_rPosition.Height);

		// Do this here, not in GetConfig, so TranslateVirtualDevice has the virtual device mappings 
		vector<string> vectShortcuts;
		string sShortcuts=DATA_Get_Shortcut();
		StringUtils::Tokenize(sShortcuts,"\r\n",vectShortcuts);
 		for(vector<string>::iterator it_shortcuts=vectShortcuts.begin();it_shortcuts!=vectShortcuts.end();++it_shortcuts)
		{
			string::size_type pos=0;
			/*bool bAutomatic =*/ StringUtils::Tokenize( *it_shortcuts, "\t", pos )=="1";
			string sCharacter = StringUtils::Tokenize( *it_shortcuts, "\t", pos );
			string sMessage = StringUtils::Tokenize( *it_shortcuts, "\t", pos );

			if( sCharacter.empty() || sMessage.empty() )
				continue;

			Message *pMessage = new Message(sMessage);
			if( pMessage->m_dwPK_Device_From==0 )
				pMessage->m_dwPK_Device_From = m_dwPK_Device;

			if( pMessage->m_dwPK_Device_To<0 )
				TranslateVirtualDevice(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_To);
			m_mapShortcut[ sCharacter[0] ] = pMessage;
		}

		m_bStartingUp=false;

		m_pOrbiterRenderer->PostInitializeActions();
	}
}

void Orbiter::InitializeGrid( DesignObj_DataGrid *pObj )
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	pObj->m_bHighlightSelectedCell=false;
	if( !pObj->m_bParsed )
		ParseGrid(pObj);

	/* todo 2.0
	bool bEPG = ( GridID.substr( 0, 3 )=="EPG" || GridID.substr( 0, 5 )=="CURSH" );
	*/
	// Initially the first row and column will be highlighted
	string::size_type posH;
	if(  (posH=pObj->m_sExtraInfo.find( 'H' ))==string::npos && pObj->sSelVariable.length()==0 )
	{
		pObj->m_iHighlightedRow = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='C') ? 0 : -1;
		pObj->m_iHighlightedColumn = (posH==pObj->m_sExtraInfo.length() || pObj->m_sExtraInfo[posH+1]!='R') ? 0 : -1;
	}
	else if(sbAllowsSelected == m_nSelectionBehaviour)
	{
		pObj->m_iHighlightedRow = pObj->m_sExtraInfo.find( 'C' )==string::npos ? 0 : -1;
		pObj->m_iHighlightedColumn = pObj->m_sExtraInfo.find( 'R' )==string::npos ? 0 : -1;
		pObj->m_bHighlightSelectedCell=true;
	}
	else
	{
		pObj->m_iHighlightedRow=-1;
		pObj->m_iHighlightedColumn=-1;
	}

	if( pObj->m_bFlushOnScreen )
		pObj->Flush();

	if(  !pObj->m_bPersistXY || pObj->m_GridCurCol==-1 || pObj->m_GridCurRow==-1  )
	{
		pObj->m_GridCurCol = pObj->m_iInitialColNum;
		pObj->m_GridCurRow = pObj->m_iInitialRowNum;
	}

	++m_dwIDataGridRequestCounter;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_CONTROLLER, "Initializing grid: %d ( %s ) dev %d options: %s ( # %d )", pObj->m_iPK_Datagrid, pObj->m_sGridID.c_str(  ), m_dwPK_Device, pObj->m_sOptions.c_str(  ), m_dwIDataGridRequestCounter );
#endif

	if(m_bQuit_get())	
		return;

	// Don't populate if we're not passing in anything at this point
	if(  pObj->m_iPK_Datagrid  )
	{
		bool bResponse;
		int iPK_Variable=0;
		string sValue_To_Assign;
		WaitForMessageQueue();  // There might still be some messages in the queue which will affect this grid
		pObj->m_iPopulatedWidth=pObj->m_MaxCol;  // Pass in the grid's on screen width/height -- we'll get back the total populated size
		pObj->m_iPopulatedHeight=pObj->m_MaxRow;

		string sParams = SubstituteVariables( pObj->m_sOptions, pObj, 0, 0 );

		int PK_DeviceTemplate = pObj->m_iPK_DeviceTemplate;
		if( pObj->m_iPK_DeviceTemplate==-1 )
		{
			string sPK_DeviceTemplate = SubstituteVariables(pObj->GetParameterValue( DESIGNOBJPARAMETER_PK_DeviceTemplate_CONST ), pObj, 0, 0);
			PK_DeviceTemplate = atoi( sPK_DeviceTemplate.c_str(  ) );
		}

		DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ), pObj->m_sGridID,
			pObj->m_iPK_Datagrid, sParams, PK_DeviceTemplate, &iPK_Variable, &sValue_To_Assign, &bResponse, &pObj->m_iPopulatedWidth, &pObj->m_iPopulatedHeight  );
		if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Populate datagrid: %d failed", pObj->m_iPK_Datagrid );
		else if(iPK_Variable)
			CMD_Set_Variable(iPK_Variable, sValue_To_Assign);

		if( (pObj->m_GridCurCol>0 && pObj->m_GridCurCol >= pObj->m_iPopulatedWidth) || (pObj->m_GridCurRow>0 && pObj->m_GridCurRow >= pObj->m_iPopulatedHeight) )
			pObj->m_GridCurCol = pObj->m_GridCurRow = 0;
	}
	if( pObj->m_sExtraInfo.find('S')!=string::npos && !pObj->sSelVariable.empty(  ) )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		string s = m_mapVariable[atoi( pObj->sSelVariable.c_str(  ) )];
		if( !s.empty() )
			pObj->m_sSeek = "~" + s;
	}

}
//------------------------------------------------------------------------
bool Orbiter::ParseConfigurationData( GraphicType Type )
{
	m_LastActivityTime = time( NULL );

	int nIndex = 0;
	int nCount = int(m_ScreenMap.size());
	for(ScreenMap::iterator it = m_ScreenMap.begin(); it != m_ScreenMap.end(); ++it)
	{
		DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) ( *it ).second;
		ParseObject(pObj, pObj, pObj, Type, 0);
		ShowProgress(50 + (nIndex++) * 50 / nCount);
	}

	bool bCacheAll = DATA_Get_CacheSize()==-1;
	for( DesignObj_OrbiterMap::iterator oi = m_mapObj_All.begin(  ); oi != m_mapObj_All.end(  ); ++oi  )
	{
		DesignObj_Orbiter *pObj = ( *oi ).second;
		if( bCacheAll )
			pObj->m_bKeepGraphicInCache=true;

		// Now match up all the up/down/left/right
		if(  pObj->m_PK_DesignObj_Up  )
			pObj->m_pDesignObj_Orbiter_Up = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Up ), pObj );
		if(  pObj->m_PK_DesignObj_Down  )
			pObj->m_pDesignObj_Orbiter_Down = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Down ), pObj );
		if(  pObj->m_PK_DesignObj_Left  )
			pObj->m_pDesignObj_Orbiter_Left = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Left ), pObj );
		if(  pObj->m_PK_DesignObj_Right  )
			pObj->m_pDesignObj_Orbiter_Right = FindObject( StringUtils::itos( pObj->m_PK_DesignObj_Right ), pObj );
		if(  pObj->m_sPK_DesignObj_TiedTo.length(  )  )
		{
			// This may be in the form object:column:row
			string::size_type pos=0;
			string Object = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			string Column = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			string Row = StringUtils::Tokenize( pObj->m_sPK_DesignObj_TiedTo, ":", pos );
			pObj->m_pDesignObj_Orbiter_TiedTo = FindObject( Object, pObj );
			if( pObj->m_pDesignObj_Orbiter_TiedTo )
				pObj->m_pDesignObj_Orbiter_TiedTo->m_vectObj_TiedToUs.push_back(pObj);

			pObj->m_iColumnTiedTo = Column=="" ? -1 : atoi( Column.c_str(  ) );
			pObj->m_iRowTiedTo = Row=="" ? -1 : atoi( Row.c_str(  ) );
		}
	}

	m_pDesignObj_Orbiter_MainMenu=m_ScreenMap_Find( m_sMainMenu );
	m_pDesignObj_Orbiter_SleepingMenu=m_ScreenMap_Find( m_sSleepingMenu );
	m_pDesignObj_Orbiter_ScreenSaveMenu=m_ScreenMap_Find( m_sScreenSaveMenu );
	if(  !m_pDesignObj_Orbiter_MainMenu  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot find main menu: %s", m_sMainMenu.c_str(  ) );
		throw "Cannot find main menu";
	}

	if(  m_bLocalMode  )
		return true;

	/*
	// Send through event manager so it updates its value.  Event manager will pick a default user if this doesn't exist
	Message *pMessage = new Message(m_dwPK_Device,m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
	COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_PK_USERS_CONST).c_str(),COMMANDPARAMETER_Value_To_Assign_CONST,Data_Get_PK_Users().c_str());
	GetEvents()->SendMessage(pMessage);

	m_mapVariable[VARIABLE_MEDIA_CENTRAL_ENTGROUP_CONST]=m_mapVariable[VARIABLE_PK_ENTERTAIN_GROUP_CONST];

	if( m_mapVariable[VARIABLE_EPG_GUIDE_VIEW_CONST]=="" )
	{
	Message *pMessage = new Message(m_dwPK_Device,m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
	COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_EPG_GUIDE_VIEW_CONST).c_str(),
	COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(DESIGNOBJ_MNUTVEPG1_CONST).c_str());
	GetEvents()->SendMessage(pMessage);
	}
	*/
	string sResult;
	DCE::CMD_Get_Floorplan_Layout CMD_Get_Floorplan_Layout(m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, &sResult);
	SendCommand(CMD_Get_Floorplan_Layout);

	string::size_type pos=0;
	int NumPages = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Get Floorplan layout found %d pages in %s", NumPages,sResult.c_str());
#endif
	for(int PageCount=0;PageCount<NumPages;++PageCount)
	{
		int PageNum = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());

		FloorplanObjectVectorMap *pFloorplanObjectVectorMap = new FloorplanObjectVectorMap();
		m_mapFloorplanObjectVector[PageNum]=pFloorplanObjectVectorMap ;

		int NumTypes = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Page %d had %d types", PageNum, NumTypes);
#endif
		for(int TypeCount=0;TypeCount<NumTypes;++TypeCount)
		{
			int TypeNum = atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());

			FloorplanObjectVector *fpObjVector = new FloorplanObjectVector();
			(*pFloorplanObjectVectorMap)[TypeNum] = fpObjVector;

			string sNumObjects = StringUtils::Tokenize(sResult, "|", pos);
			int NumObjects = atoi(sNumObjects.c_str());
			for(int ObjectCount=0;ObjectCount<NumObjects;++ObjectCount)
			{
				FloorplanObject *fpObj = new FloorplanObject();

				fpObj->ObjectTypeDescription=StringUtils::Tokenize(sResult, "|", pos);
				fpObj->FillX=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
				fpObj->FillY=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
				fpObj->ObjectID=StringUtils::Tokenize(sResult, "|", pos);
				fpObj->Page=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
				fpObj->PK_Device=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
				fpObj->m_pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(fpObj->PK_Device);
				fpObj->DeviceDescription=StringUtils::Tokenize(sResult, "|", pos);
				fpObj->Type=atoi(StringUtils::Tokenize(sResult, "|", pos).c_str());
				fpObj->pObj = FindObject(fpObj->ObjectID);
				if( !fpObj->pObj )
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find floorplan object: %s",fpObj->ObjectID.c_str());

#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding device %d %s to floorplan", fpObj->PK_Device, fpObj->DeviceDescription.c_str());
#endif

				fpObjVector->push_back(fpObj);
				if( fpObj->pObj )
				{
					fpObj->pObj->m_pFloorplanObject=fpObj;
					// Be sure there is a 'requires special handling' action on this object
					if( fpObj->pObj->m_ZoneList.size()==0 )
					{
						DesignObjZone *pDesignObjZone = new DesignObjZone();
						pDesignObjZone->m_Rect.Width=pDesignObjZone->m_Rect.Height=0;
						DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
						pDesignObjCommand->m_PK_Command=COMMAND_Requires_Special_Handling_CONST;
						pDesignObjCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
						pDesignObjZone->m_Commands.push_back(pDesignObjCommand);
						fpObj->pObj->m_ZoneList.push_back(pDesignObjZone);
					}
				}
			}
		}
	}

	ParseHardKeys();

	return true;
}
//--------------------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::ParseHardKeys()
{
	string::size_type pos = 0;
	string sKeymapping=DATA_Get_Hard_Keys_mapping();
	while(pos<DATA_Get_Hard_Keys_mapping().size())
	{
		string sToken=StringUtils::Tokenize(sKeymapping,"\n",pos);
		string::size_type pos2=0;
		int iKey = atoi( StringUtils::Tokenize(sToken,"\t",pos2).c_str() );
		if( !iKey )
			continue;

		Message *pMessage = new Message(StringUtils::Tokenize(sToken,"\t",pos2));
		if( pMessage->m_dwPK_Device_To<0 )
			TranslateVirtualDevice(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_To );
		if( !pMessage->m_dwPK_Device_From )
			pMessage->m_dwPK_Device_From = m_dwPK_Device;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::ParseHardKeys key %d = message %s", iKey, sToken.c_str() );
#endif
		m_mapHardKeys[iKey] = pMessage;
	}
}
//--------------------------------------------------------------------------------------------------------------
void Orbiter::ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type,  int Lev )
{
	for(size_t s=0;s<pObj->m_vectDesignObjText.size();++s)
		pObj->m_vectDesignObjText[s]->m_pObject = pObj;

	m_mapObj_All[pObj->m_ObjectID] = pObj;  // Do this earlier since the bind to action may require looking this up.
	pObj->m_pObj_Screen=pObj_Screen;

	if(  pObj->m_bHideByDefault  )
		pObj->m_bHidden = true;

	enum eGraphicManagement eMgmt = GR_DISCARDONCHANGE;
	if(  pObj->m_sBackgroundFile.length(  )>0  )
		CreateVectorGraphic(pObj->m_vectGraphic, Type,  pObj->m_sBackgroundFile,  eMgmt,  m_pOrbiterRenderer );
	if(  pObj->m_sHighlightGraphicFilename.length(  )>0  )
		CreateVectorGraphic(pObj->m_vectHighlightedGraphic, Type,  pObj->m_sHighlightGraphicFilename,  eMgmt,  m_pOrbiterRenderer );
	if(  pObj->m_sSelectedFile.length(  )>0  )
		CreateVectorGraphic(pObj->m_vectSelectedGraphic, Type,  pObj->m_sSelectedFile,  eMgmt,  m_pOrbiterRenderer );
	for( size_t salt=0;salt<pObj->m_vectAltGraphicFilename.size(  );++salt )
	{
		VectorPlutoGraphic vectPlutoGraphic;
		CreateVectorGraphic(vectPlutoGraphic, Type,  pObj->m_vectAltGraphicFilename[salt],  eMgmt,  m_pOrbiterRenderer);
		pObj->m_vectAltGraphics.push_back( vectPlutoGraphic );
	}

	if(  pObj->m_bTabStop  )
		pObj_Screen->m_vectObj_TabStops.push_back( pObj );

	/*
	if(  pAction->m_PK_Command==COMMAND_USE_DEVICE_CONST  )
	{
	string ID = pAction->m_ParameterList[COMMANDPARAMETER_ID_CONST];
	string Device = pAction->m_ParameterList[COMMANDPARAMETER_PK_Device_CONST];
	GetEvents(  )->SendMessage( new Message( m_dwPK_Device,  DEVICEID_DATAGRID,  PRIORITY_NORMAL,
	MESSAGETYPE_COMMAND,  COMMAND_BIND_TO_DEVICE_STATUS_CONST,  4,
	C_COMMANDPARAMETER_Orbiter_ID_CONST,  StringUtils::itos( m_dwPK_Device ).c_str(  ),
	COMMANDPARAMETER_PK_DesignObj_CONST,  pObj->m_ObjectID.c_str(  ),
	COMMANDPARAMETER_ID_CONST,  ID.c_str(  ),
	COMMANDPARAMETER_PK_Device_CONST,  Device.c_str(  ) ) );
	}
	*/

	// Child objects have the format screen.version.page.objectid.x where x is an arbitrary increment to insure uniqueness
	// Some commands need to send messages to an object not knowing the value of x.  So,  we create another map without the suffix
	int CountDots=0;
	int DotPos=0, LastDotPos=-1, PriorToLastDotPos=-1;
	while(  ( DotPos=( int ) pObj->m_ObjectID.find( ".", DotPos+1 ) )>0  )
	{
		PriorToLastDotPos=LastDotPos;
		LastDotPos=DotPos;
		CountDots++;
	}

	if(  CountDots==4  )
	{
		string IDWithoutSuffix = pObj->m_ObjectID.substr( 0, LastDotPos );
		DesignObj_DataList *pObjectList;
		map<string, DesignObj_DataList *>::iterator it = m_mapObj_AllNoSuffix.find( IDWithoutSuffix );
		if(  it==m_mapObj_AllNoSuffix.end(  )  )
		{
			pObjectList = new DesignObj_DataList(  );
			m_mapObj_AllNoSuffix[IDWithoutSuffix] = pObjectList;
		}
		else
			pObjectList = ( *it ).second;
		pObjectList->push_back( pObj );
		string x=pObj->m_ObjectID.substr( PriorToLastDotPos+1 );
		pObj->m_iBaseObjectID = atoi( x.c_str(  ) );
	}
	else
	{
		pObj->m_iBaseObjectID = atoi( pObj->m_ObjectID.c_str(  ) );
	}

//if( pObj->m_iBaseObjectID==5399 )
//int k=2;
	// On any screen all child objects should inherit the screen's priority so the whole screen is cached
	pObj->m_Priority = pObj_Screen->m_Priority;

	if ( pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		pObj->m_bContainsDataGrid=true;
		DesignObj_DataGrid *pObj_Datagrid = ( DesignObj_DataGrid * ) pObj;
		// Fix up the parameters
		pObj_Datagrid->m_sGridID = SubstituteVariables( pObj->GetParameterValue( DESIGNOBJPARAMETER_Data_grid_ID_CONST ), pObj, 0, 0 );
		m_mapObjs_AllGrids[pObj_Datagrid->m_sGridID] = pObj_Datagrid;
	}
	else if( pObj->m_ObjectType==DESIGNOBJTYPE_App_Desktop_CONST && (pObj->m_rPosition.Width==0 || pObj->m_rPosition.Height==0) )
	{
		if( pObj->m_rPosition.Width==0 )
			pObj->m_rPosition.Width=m_iImageWidth;
		if( pObj->m_rPosition.Height==0 )
			pObj->m_rPosition.Height=m_iImageHeight;
	}

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *p = ( DesignObj_Orbiter * )( *iHao );
		ParseObject( p, pObj_Screen, pObj_Parent, Type, Lev );
	}
	if(  pObj->m_bContainsDataGrid==true && pObj_Parent  )
		pObj_Parent->m_bContainsDataGrid=true;

	pObj->m_pMidPoint.X = pObj->m_rPosition.X + (pObj->m_rPosition.Width/2);
	pObj->m_pMidPoint.Y = pObj->m_rPosition.Y + (pObj->m_rPosition.Height/2);
	/*
	Floorplan
	else if(  pObj->m_iBaseObjectID==DESIGNOBJ_GRPHOUSEINDICATORS_CONST  )
	{
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
	DesignObj_Orbiter *p = ( *iHao );
	if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW0_CONST  )
	m_FloorplanObjects.m_Windows->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW1_CONST  )
	m_FloorplanObjects.m_Windows->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW2_CONST  )
	m_FloorplanObjects.m_Windows->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAWINDOW3_CONST  )
	m_FloorplanObjects.m_Windows->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR0_CONST  )
	m_FloorplanObjects.m_Doors->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR1_CONST  )
	m_FloorplanObjects.m_Doors->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR2_CONST  )
	m_FloorplanObjects.m_Doors->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRADOOR3_CONST  )
	m_FloorplanObjects.m_Doors->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION0_CONST  )
	m_FloorplanObjects.m_Motions->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION1_CONST  )
	m_FloorplanObjects.m_Motions->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION2_CONST  )
	m_FloorplanObjects.m_Motions->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRAMOTION3_CONST  )
	m_FloorplanObjects.m_Motions->m_objTriggered = p;

	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA0_CONST  )
	m_FloorplanObjects.m_Cameras->m_objBypassed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA1_CONST  )
	m_FloorplanObjects.m_Cameras->m_objArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA2_CONST  )
	m_FloorplanObjects.m_Cameras->m_objNotArmed = p;
	else if(  p->m_iBaseObjectID==DESIGNOBJ_GRACAMERA3_CONST  )
	m_FloorplanObjects.m_Cameras->m_objTriggered = p;
	}
	}
	*/
}
//------------------------------------------------------------------------
bool Orbiter::RenderDesktop( class DesignObj_Orbiter *pObj,  PlutoRectangle rectTotal, PlutoPoint point )
{
    int x = point.X + pObj->m_rPosition.X;
    int y = point.Y + pObj->m_rPosition.Y;
    int width = pObj->m_rPosition.Width;
    int height = pObj->m_rPosition.Height;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter::RenderDesktop(pObj=%p) : Render desktop (%d,%d,%d,%d)",
                           pObj, x, y, width, height );
#endif
    // background color
    PlutoColor color( 0, 0, 255 );
    // in some cases, make the background transparent
	if( pObj->m_mapObjParms_Find(DESIGNOBJPARAMETER_In_Background_CONST)=="1" )
    {
        //int k=2;  // Do in background
        if (! m_bUseComposite)
        {
            color.SetAlpha(128); // optimized semi-transparency
            //color.SetAlpha(0); // full-transparency
        }
    }
#ifndef WIN32
	m_pOrbiterRenderer->SolidRectangle(x, y, width, height, color);
#endif
	return true;
}


/*
ACCEPT OUTSIDE INPUT
*/

void Orbiter::QueueEventForProcessing( void *eventData )
{
	//using std's "smart" point: event data will be deleted automatically when spEvent goes out of scope
	auto_ptr<Orbiter::Event> spEvent((Orbiter::Event*)eventData);

#ifdef DEBUG
	if( spEvent->type == Orbiter::Event::MOUSE_MOVE )
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing mouse x %d y %d",
			spEvent->data.region.m_iX,spEvent->data.region.m_iY);
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing type %d key %d keycode %d (sim=%d)",
			spEvent->type, spEvent->data.button.m_iPK_Button, spEvent->data.button.m_iKeycode, (int) spEvent->data.button.m_bSimulated);
#endif

	// For mouse events, use mouse button id (m_iButton)
	int iButton = spEvent->data.button.m_iKeycode;
	if (spEvent->type == Orbiter::Event::REGION_DOWN || spEvent->type == Orbiter::Event::REGION_UP)
	{
	        iButton = spEvent->data.region.m_iButton;
	}
#ifdef WIN32
	map< pair<int, int>, pair<int, int> >::iterator it = m_mapEventToSubstitute.find(make_pair (spEvent->type, iButton));
#else
	map< pair<int,int>,pair<int,int> >::iterator it = m_mapEventToSubstitute.find( make_pair<int,int> (spEvent->type,iButton) );
#endif
	if( it!=m_mapEventToSubstitute.end() )
	{
		spEvent->type = (DCE::Orbiter::Event::EventType) it->second.first;
		spEvent->data.button.m_iKeycode = it->second.second;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing translated to type %d key %d",
		  spEvent->type, spEvent->data.button.m_iKeycode);
#endif
	}

	PreprocessEvent(*spEvent.get());  // This will fill in the PK_Button if it's not already there

	if( (spEvent->type == Orbiter::Event::BUTTON_DOWN || spEvent->type == Orbiter::Event::REGION_DOWN) &&
        !GotActivity( spEvent->type == Orbiter::Event::BUTTON_DOWN ? spEvent->data.button.m_iPK_Button : 0 ) )  // Use spEvent->type not Type since if it's not a known type it won't map to m_iPK_Button
	{
		// Ignore the up when it comes in
		m_TypeToIgnore = spEvent->type == Orbiter::Event::BUTTON_DOWN ? Orbiter::Event::BUTTON_UP : Orbiter::Event::REGION_UP;
		return;
	}

	// Check if we were just woken up above and need to ignore the corresponding up
	if( spEvent->type==m_TypeToIgnore )
	{
		m_TypeToIgnore=Orbiter::Event::NONE;
		return;
	}
	m_TypeToIgnore=Orbiter::Event::NONE;

	if( spEvent->data.button.m_bSimulated==false && (spEvent->type == Orbiter::Event::BUTTON_DOWN || spEvent->type == Orbiter::Event::BUTTON_UP) 
		&& (m_bYieldInput==false || m_mapScanCodeToIgnoreOnYield.find( spEvent->data.button.m_iKeycode ) == m_mapScanCodeToIgnoreOnYield.end()) )  // Use Type instead of spEvent->type since the user may have some mapping for this
	{
		StopRepeatCode();  // In case the IRReceiverBase was repeating a key sequence

		map< pair<int,char>, string>::iterator it = m_mapScanCodeToRemoteButton.end();

		string sRepeatKey;  // This will not be empty if we're pressing a key that can be repeated.  It will be the key to repeat

		// and we're looking for scan codes
		//char cAction = 'D'; // Action is D=down, U=up, R=Repeat, H=Held Down
		if( spEvent->type == Orbiter::Event::BUTTON_UP )
		{
			timespec tButtonUp;
			gettimeofday(&tButtonUp,NULL);
			timespec m_tInterval = tButtonUp - m_tButtonDown;
			long tMilisecondsPassed = m_tButtonDown.tv_sec==0 ? 0 : m_tInterval.tv_sec * 1000 + m_tInterval.tv_nsec / 1000000;

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing m_tButtonDown %d.%d up %d.%d interval %d",
				(int) m_tButtonDown.tv_sec,(int) m_tButtonDown.tv_nsec,(int) tButtonUp.tv_sec,(int) tButtonUp.tv_nsec,(int) tMilisecondsPassed);
#endif

			if (tMilisecondsPassed > KEY_DOWN_DURATION)
			{
#ifdef WIN32
				it = m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'H'));
#else				
				it = m_mapScanCodeToRemoteButton.find(make_pair<int, char>(spEvent->data.button.m_iKeycode, 'H'));
#endif
			}

			if (it == m_mapScanCodeToRemoteButton.end())  // Either we didn't hold the button or there is no hold specific event
			{
#ifdef WIN32
				it = m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'U'));
#else				
				it = m_mapScanCodeToRemoteButton.find(make_pair<int, char>(spEvent->data.button.m_iKeycode, 'U'));
#endif
			}

			m_tButtonDown.tv_sec=0;

			// There's nothing for the 'up'.  See if there was a down or repeat.  If so we'll ignore
			// this up so it's not processed by the framework.  Otherwise the i/r mechanism may do something for
			// the down, and the framework something else for the up
			if( it == m_mapScanCodeToRemoteButton.end() ) 
			{
#ifdef WIN32
				if (m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'D')) != m_mapScanCodeToRemoteButton.end() ||
					m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'R')) != m_mapScanCodeToRemoteButton.end())
#else
				if( m_mapScanCodeToRemoteButton.find( make_pair<int,char> (spEvent->data.button.m_iKeycode, 'D'))!=m_mapScanCodeToRemoteButton.end() ||
					m_mapScanCodeToRemoteButton.find( make_pair<int,char> (spEvent->data.button.m_iKeycode, 'R'))!=m_mapScanCodeToRemoteButton.end() )
#endif
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing ignoring keycode %d up because there's a DOWN",spEvent->data.button.m_iKeycode);
#endif
					return;
				}
			}
		}
		else
		{
			gettimeofday(&m_tButtonDown,NULL);
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing m_tButtonDown %d.%d",(int) m_tButtonDown.tv_sec,(int) m_tButtonDown.tv_nsec);
#endif
#ifdef WIN32
			it = m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'D'));
#else
			it = m_mapScanCodeToRemoteButton.find( make_pair<int,char> (spEvent->data.button.m_iKeycode, 'D') );
#endif

#ifdef WIN32
			map< pair<int, char>, string>::iterator itRepeat = m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'R'));
#else
			map< pair<int,char>, string>::iterator itRepeat = m_mapScanCodeToRemoteButton.find( make_pair<int,char> (spEvent->data.button.m_iKeycode, 'R'));
#endif
			if( itRepeat!=m_mapScanCodeToRemoteButton.end() )
			{
				sRepeatKey = itRepeat->second;  // This is the key to repeat

				// it is the key we'll fire first for 'down'.  This can be different from the repeat.  However, if there is a repeat
				// and there is no down, then the repeat is the down
				if( it == m_mapScanCodeToRemoteButton.end() )
					it = itRepeat;
			}
			if( it==m_mapScanCodeToRemoteButton.end() && 
#ifdef WIN32
				m_mapScanCodeToRemoteButton.find(make_pair (spEvent->data.button.m_iKeycode, 'U')) != m_mapScanCodeToRemoteButton.end())
#else
				m_mapScanCodeToRemoteButton.find( make_pair<int,char> (spEvent->data.button.m_iKeycode, 'U') )!=m_mapScanCodeToRemoteButton.end() )
#endif
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing ignoring keycode %d down because there's an UP",spEvent->data.button.m_iKeycode);
#endif
				return;
			}
		}

		if( it!=m_mapScanCodeToRemoteButton.end() )
		{
			string sKey = it->second;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::QueueEventForProcessing received key %s repeat %s",sKey.c_str(),sRepeatKey.c_str());
#endif

			SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnRemoteKeyCodeIntercepted, RemoteKeyCodeCallBackData, (sKey))
			if(!ExecuteScreenHandlerCallback(cbOnRemoteKeyCodeIntercepted))
				ReceivedCode(0,sKey.c_str(),sRepeatKey.c_str());

			return;
		}
	}
    
	ProcessEvent(*spEvent.get());
}

bool Orbiter::PreprocessEvent(Orbiter::Event &event)
{
	// nothing. This is here because i need a prepropcessing phase in key translation ( the XRecord callback doesn't allow XLib calls inside it so i need to make in another thread );
	return true;
}
//temptest
#include "MediaBrowserMouseHandler.h"
bool Orbiter::ProcessEvent( Orbiter::Event &event )
{
	Renderer()->ProcessingInputEvent();

	if ( (event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::REGION_DOWN)
		&& !GotActivity( event.type == Orbiter::Event::BUTTON_DOWN ? event.data.button.m_iPK_Button : 0 ) )
			return true;

#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ProcessEvent1 type %d key %d",
//					  event.type, (event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::BUTTON_UP ? event.data.button.m_iPK_Button : -999));
#endif

	if(event.type == Orbiter::Event::BUTTON_DOWN && m_bControlDown && m_bAltDown)
	{
		if(event.data.button.m_iPK_Button == BUTTON_s_CONST)
		{
			//toggle surrender to OS
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Surrendering to OS: %d", !m_bYieldScreen);
			CMD_Surrender_to_OS(m_bYieldScreen ? "0" : "1", !m_bYieldScreen,false);
		}
	}

	if(event.type == Orbiter::Event::BUTTON_DOWN)
	{
		if(event.data.button.m_iPK_Button == BUTTON_F9_CONST)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Dumping mask to /home/mask-dump.png");
#if defined(VIA_OVERLAY) && defined(ORBITER_OPENGL)
			ViaOverlay::Instance().DumpMask();
#endif

#if defined(USE_UIWINDOW) && defined(ORBITER_OPENGL)
			UIWindowManager::UIWindow()->DumpMask(m_iImageWidth, m_iImageHeight);
#endif
		}
	}

	if ( event.type == Orbiter::Event::MOUSE_MOVE )
	{
#ifdef ENABLE_MOUSE_BEHAVIOR
		if(NULL != m_pMouseBehavior)
			m_pMouseBehavior->m_pLastPosition_set( event.data.region.m_iX, event.data.region.m_iY );
#endif

		if( event.data.region.m_iX==m_iLastX && event.data.region.m_iY==m_iLastY )
			return false;
		m_iLastX=event.data.region.m_iX;
		m_iLastY=event.data.region.m_iY;
		m_tLastMouseMove=time(NULL);
	}

#ifdef DEBUG
	// a switch would be good but kdevelop somehow doesn't like the syntax and mekes it red :-(
	if ( event.type != Orbiter::Event::QUIT && event.type != Orbiter::Event::NOT_PROCESSED )
	{
		if ( event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::BUTTON_UP )
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Button %s: button ID: %d m_bForward_local_kb_to_OSD %d", 
				(event.type == Orbiter::Event::BUTTON_DOWN) ? "down" : "up", event.data.button.m_iPK_Button, (int) m_bForward_local_kb_to_OSD );

		if ( event.type == Orbiter::Event::REGION_DOWN || event.type == Orbiter::Event::REGION_UP )
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Region %s: position: [%d, %d]", (event.type == Orbiter::Event::REGION_DOWN) ? "down" : "up", event.data.region.m_iX, event.data.region.m_iY);
	}
#endif

#ifdef ENABLE_MOUSE_BEHAVIOR
// some temporary stuff
if(UsesUIVersion2())
{
	bool bSkipProcessing=false;

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ProcessEvent2 %p type %d key %d",
					  m_pMouseBehavior, event.type, (event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::BUTTON_UP ? event.data.button.m_iPK_Button : -999));
#endif
	if(event.type == Orbiter::Event::BUTTON_DOWN && NULL != m_pMouseBehavior)
	{
		if(event.data.button.m_iPK_Button == BUTTON_F6_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_6_CONST)
			bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_6_CONST);
		else if(event.data.button.m_iPK_Button == BUTTON_F7_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_7_CONST)
			bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_7_CONST);
        else if(event.data.button.m_iPK_Button == BUTTON_F8_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_8_CONST)
		    bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_8_CONST);
	}

    if(event.type == Orbiter::Event::BUTTON_UP && NULL != m_pMouseBehavior)
    {
        if(event.data.button.m_iPK_Button == BUTTON_F6_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_6_CONST)
            bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_6_CONST);
        else if(event.data.button.m_iPK_Button == BUTTON_F7_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_7_CONST)
            bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_7_CONST);
        else if(event.data.button.m_iPK_Button == BUTTON_F8_CONST || event.data.button.m_iPK_Button == BUTTON_Mouse_8_CONST)
            bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_8_CONST);
    }
	
	// The cancel button
	if ( event.type == Orbiter::Event::BUTTON_DOWN && event.data.button.m_iPK_Button==BUTTON_F9_CONST && m_pMouseBehavior )
		bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_2_CONST);
	if ( event.type == Orbiter::Event::BUTTON_UP && event.data.button.m_iPK_Button==BUTTON_F9_CONST && m_pMouseBehavior )
		bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_2_CONST);

	// The left mouse button
	if( (event.type == Orbiter::Event::REGION_DOWN || event.type == Orbiter::Event::REGION_UP) && m_pMouseBehavior )
	{
		if ( event.type == Orbiter::Event::REGION_DOWN && event.data.region.m_iButton==1  )
			bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_1_CONST);
		if ( event.type == Orbiter::Event::REGION_UP && event.data.region.m_iButton==1 )
			bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_1_CONST);
		// The right mouse button
		if ( event.type == Orbiter::Event::REGION_DOWN && event.data.region.m_iButton==3 )
			bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_2_CONST);
		if ( event.type == Orbiter::Event::REGION_UP && event.data.region.m_iButton==3 )
			bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_2_CONST);
		// The middle mouse button, temporarily being used as a kill switch
		if ( event.type == Orbiter::Event::REGION_DOWN && event.data.region.m_iButton==2  )
			bSkipProcessing=m_pMouseBehavior->ButtonDown(BUTTON_Mouse_3_CONST);
		if ( event.type == Orbiter::Event::REGION_UP && event.data.region.m_iButton==2 )
			bSkipProcessing=m_pMouseBehavior->ButtonUp(BUTTON_Mouse_3_CONST);
	}
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ProcessEvent3 %d type %d key %d",
					  (int) bSkipProcessing, event.type, (event.type == Orbiter::Event::BUTTON_DOWN || event.type == Orbiter::Event::BUTTON_UP ? event.data.button.m_iPK_Button : -999));
#endif

	if( bSkipProcessing )
		return true;

	// Temporary hack to hide and show the cursor
	if ( event.type == Orbiter::Event::BUTTON_DOWN && event.data.button.m_iPK_Button==BUTTON_F5_CONST )
	{
		static bool bShowCursor=true;
		bShowCursor=!bShowCursor;
		if( m_pMouseBehavior )
			m_pMouseBehavior->ShowMouse(bShowCursor);
	}

	if(event.type == Orbiter::Event::BUTTON_DOWN && !bSkipProcessing && NULL != m_pMouseBehavior)
		bSkipProcessing=m_pMouseBehavior->ButtonDown(event.data.button.m_iPK_Button);
	if(event.type == Orbiter::Event::BUTTON_UP && !bSkipProcessing && NULL != m_pMouseBehavior)
		bSkipProcessing=m_pMouseBehavior->ButtonUp(event.data.button.m_iPK_Button);
	if( bSkipProcessing )
		return true;
}
#endif

	if ( event.type == Orbiter::Event::BUTTON_DOWN )
	{
		// We intercept F7 and always go to the main menu
		if( event.data.button.m_iPK_Button==BUTTON_F7_CONST && UsesUIVersion2()==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::ProcessEvents Main Menu");
			CMD_Set_Main_Menu("N");
			CMD_Goto_Screen("",SCREEN_Main_CONST);
			return true;
		}

	    if (m_bYieldInput)
		{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ignoring keyboard events, m_bYieldInput==%d button %d", m_bYieldInput,event.data.button.m_iPK_Button);
		    return true;
		}

		return ButtonDown(event.data.button.m_iPK_Button);
	}

	if ( event.type == Orbiter::Event::BUTTON_UP )
	{
	    if (m_bYieldInput)
		{
	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ignoring keyboard events, m_bYieldInput==%d button %d", m_bYieldInput,event.data.button.m_iPK_Button);
		    return true;
		}
		return ButtonUp(event.data.button.m_iPK_Button);
	}

	if ( event.type == Orbiter::Event::REGION_DOWN && event.data.region.m_iButton == 1)
		return RegionDown(event.data.region.m_iX, event.data.region.m_iY);

	if ( event.type == Orbiter::Event::REGION_UP && event.data.region.m_iButton == 1)
		return  RegionUp(event.data.region.m_iX, event.data.region.m_iY); // Shouldn't this be like the above?

#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		if ( event.type == Orbiter::Event::MOUSE_RELATIVE_MOVE && m_pMouseBehavior )
			m_pMouseBehavior->RelativeMove(event.data.region.m_iX, event.data.region.m_iY);
		if ( event.type == Orbiter::Event::MOUSE_MOVE && m_pMouseBehavior )
			m_pMouseBehavior->Move(event.data.region.m_iX, event.data.region.m_iY);
	}
#endif	

	return false;
}

/*virtual*/ bool Orbiter::HandleButtonEvent(int PK_Button)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "HandleButtonEvent button %d, shift %d, caps %d", PK_Button, m_bShiftDown, m_bCapsLock);
#endif
	
	if( !PK_Button || !m_pScreenHistory_Current )
		return false;

	NeedToRender render( this, "Button Down" );  // Redraw anything that was changed by this command
	class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

	bool bHandled=false;

	// 2005-dec-7  Do the capture keyboard stuff first in case one of the
	// objects we selected needs the current value
#ifndef PHONEKEYS
	if( m_bCaptureKeyboard_OnOff )
	{
		if( m_iCaptureKeyboard_PK_Variable )
		{
			if( PK_Button == BUTTON_space_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ' ' );
			else if( PK_Button == BUTTON_comma_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ',' );
			else if( PK_Button == BUTTON_dash_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '-' );
			else if( PK_Button == BUTTON_equals_sign_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '=' );
			else if( PK_Button == BUTTON_semicolumn_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ';' );
			else if( PK_Button == BUTTON_single_quote_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '\'' );
			else if( PK_Button == BUTTON_double_quote_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '"' );
			else if( PK_Button == BUTTON_dot_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '.' );
			if( PK_Button == BUTTON_Asterisk_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '*' );
			if( PK_Button == BUTTON_Pound_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '#' );
			if( PK_Button == BUTTON_right_parenthesis_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ')' );
			if( PK_Button == BUTTON_left_parenthesis_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '(' );
			if( PK_Button == BUTTON_exclamation_point_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '!' );
			if( PK_Button == BUTTON_at_sign_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '@' );
			if( PK_Button == BUTTON_dollar_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '$' );
			if( PK_Button == BUTTON_percent_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '%' );
			if( PK_Button == BUTTON_caret_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '^' );
			if( PK_Button == BUTTON_ampersand_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '&' );
			if( PK_Button == BUTTON_underscore_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '_' );
			if( PK_Button == BUTTON_plus_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '+' );
			if( PK_Button == BUTTON_colon_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( ':' );
			if( PK_Button == BUTTON_period_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '.' );
			if( PK_Button == BUTTON_slash_CONST )
				bHandled |= CaptureKeyboard_EditText_AppendChar( '/' );
			else if(
				( PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST ) ||
				( PK_Button >= BUTTON_A_CONST && PK_Button <= BUTTON_Z_CONST ) ||
				( PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST )
				)
			{
				char ch;

				if( PK_Button == BUTTON_0_CONST )
					ch = '0';
				else if ( PK_Button >= BUTTON_1_CONST && PK_Button <= BUTTON_0_CONST )
					ch = '1' + PK_Button - BUTTON_1_CONST;
				else
					ch = ( PK_Button >= BUTTON_a_CONST && PK_Button <= BUTTON_z_CONST ) ?
					'a' + PK_Button - BUTTON_a_CONST :
					'A' + PK_Button - BUTTON_A_CONST;

				bHandled |= CaptureKeyboard_EditText_AppendChar( ch );
			}
			else if( PK_Button == BUTTON_Back_CONST )
				bHandled |= CaptureKeyboard_EditText_DeleteLastChar(  );
			else if( PK_Button == BUTTON_Enter_CONST )
			{
				//TODO: send this message
				//CaptureKeyboard_UpdateVariableAndText( m_iCaptureKeyboard_PK_Variable,
				//  m_sCaptureKeyboard_InternalBuffer );
			}

			if(bHandled)
			{
				if(NULL != m_pCaptureKeyboard_Text && NULL != m_pCaptureKeyboard_Text->m_pObject)
				{
					m_pOrbiterRenderer->RenderObjectAsync(m_pCaptureKeyboard_Text->m_pObject);
				}
			}
		}
	}
#else
	if( m_bCaptureKeyboard_OnOff && m_iCaptureKeyboard_PK_Variable )
	{
		//TODO: if keys = 0,  1,  ... 9,  transform this to abc,  def,  etc.

		if( PK_Button == BUTTON_Phone_C_CONST )
			bHandled |= CaptureKeyboard_EditText_DeleteLastChar(  );
	}
#endif

	if(  PK_Button == BUTTON_Enter_CONST  )
	{
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(  m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden(  )  )
		{
			SelectedObject( m_pObj_Highlighted, smNavigation );
			bHandled=true;
		}
	}
	if(  bHandled==false && (bHandled=ClickedButton( m_pScreenHistory_Current->GetObj(),  PK_Button ))==false  )
	{
		if(  PK_Button>=BUTTON_Up_Arrow_CONST && PK_Button<=BUTTON_Right_Arrow_CONST  ) // up,  down,  left or right
		{
			if(  PK_Button==BUTTON_Up_Arrow_CONST  )
			{
				m_pOrbiterRenderer->HighlightNextObject( DIRECTION_Up_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Down_Arrow_CONST  )
			{
				m_pOrbiterRenderer->HighlightNextObject( DIRECTION_Down_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Left_Arrow_CONST  )
			{
				m_pOrbiterRenderer->HighlightNextObject( DIRECTION_Left_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Right_Arrow_CONST  )
			{
				m_pOrbiterRenderer->HighlightNextObject( DIRECTION_Right_CONST );
				PK_Button = -1;
			}
			bHandled=true;
		}
		else if(  PK_Button>=BUTTON_Shift_Up_Arrow_CONST && PK_Button<=BUTTON_Shift_Right_Arrow_CONST  )
		{
			if(  PK_Button==BUTTON_Shift_Up_Arrow_CONST  )
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Up_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Shift_Down_Arrow_CONST  )
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Down_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Shift_Left_Arrow_CONST  )
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Left_CONST );
				PK_Button = -1;
			}
			else if(  PK_Button==BUTTON_Shift_Right_Arrow_CONST  )
			{
				CMD_Scroll_Grid( "", "", DIRECTION_Right_CONST );
				PK_Button = -1;
			}
		}
	}

	if(  pScreenHistory != m_pScreenHistory_Current  )
		m_bRerenderScreen = true;

	return bHandled;
}

bool Orbiter::ButtonDown( int iPK_Button )
{
	if(NULL != m_pScreenHandler)
	{
		SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnKeyDown, KeyCallBackData, (iPK_Button))
		if(ExecuteScreenHandlerCallback(cbOnKeyDown))
			return true;
	}

	//purge any 'DeselectObjects' callbacks from pending callbacks map
	PLUTO_SAFETY_LOCK( mt, m_MaintThreadMutex );
	for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		if( pCallBackInfo->m_fnCallBack == &Orbiter::DeselectObjects)
		{
			DesignObj_Orbiter *pObj = (DesignObj_Orbiter *)pCallBackInfo->m_pData;
			pObj->m_GraphicToDisplay_set("o3",GRAPHIC_NORMAL);
			m_pOrbiterRenderer->RenderObjectAsync(pObj);

			m_mapPendingCallbacks.erase(it++);
		}
		else
			++it;
	}
	mt.Release();

	//if this is a repeated button, we'll handle it right away
	if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->GetObj(), iPK_Button, true))
	{
		m_bWeCanRepeat = true;
		return HandleButtonEvent(iPK_Button);
	}

	//this is not a repeated button
	if(iPK_Button == BUTTON_left_shift_CONST || iPK_Button == BUTTON_right_shift_CONST)
	{
		m_bShiftDown = true; //different logic with the on screen keyboard
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Button Down - shift");
#endif
	}
	else if(iPK_Button == BUTTON_caps_lock_CONST)
		m_bCapsLock = !m_bCapsLock;
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Button Down - key");
#endif
	}

	//this is not a repeated button
	//it will be handled in ButtonUp
	gettimeofday(&m_tButtonDown,NULL);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ButtonDown m_tButtonDown %d.%d",(int) m_tButtonDown.tv_sec,(int) m_tButtonDown.tv_nsec);
#endif
	return false;
}

bool Orbiter::ButtonUp( int iPK_Button )
{
	if(NULL != m_pScreenHandler)
	{
		SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnKeyUp, KeyCallBackData, (iPK_Button))
		if(ExecuteScreenHandlerCallback(cbOnKeyUp))
			return true;
	}

	if( m_mapHardKeys.find(iPK_Button)!=m_mapHardKeys.end() )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ButtonUp HardKey %d", iPK_Button);
#endif
		Message *pMessage = m_mapHardKeys[iPK_Button];
		if( pMessage->m_dwPK_Device_To==m_dwPK_Device )
			ReceivedMessage( pMessage );
		else
			QueueMessageToRouter(new Message(pMessage));
	}

	if(m_bQuit_get())
		return false;

	if( m_bForward_local_kb_to_OSD && m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_Orbiter )
	{
		GenerateSimulateKeypress(iPK_Button);
		return true;
	}

	//if this was a repeated button, we might want to stop all repeat related events
	if(m_pScreenHistory_Current && IsRepeatedKeyForScreen(m_pScreenHistory_Current->GetObj(), iPK_Button, false))
	{
		StopRepeatRelatedEvents();
		return false;
	}

	timespec tButtonUp;
	gettimeofday(&tButtonUp,NULL);
	timespec m_tInterval = tButtonUp - m_tButtonDown;
	long tMilisecondsPassed = m_tButtonDown.tv_sec==0 ? 0 : m_tInterval.tv_sec * 1000 + m_tInterval.tv_nsec / 1000000;

	if(tMilisecondsPassed > 500) //more then half of second
	{
		//it is a long key - let's get the right button code
		switch(	iPK_Button)
		{
		case BUTTON_0_CONST:        iPK_Button = BUTTON_Rept_0_CONST; break;
		case BUTTON_1_CONST:        iPK_Button = BUTTON_Rept_1_CONST; break;
		case BUTTON_2_CONST:        iPK_Button = BUTTON_Rept_2_CONST; break;
		case BUTTON_3_CONST:        iPK_Button = BUTTON_Rept_3_CONST; break;
		case BUTTON_4_CONST:        iPK_Button = BUTTON_Rept_4_CONST; break;
		case BUTTON_5_CONST:        iPK_Button = BUTTON_Rept_5_CONST; break;
		case BUTTON_6_CONST:        iPK_Button = BUTTON_Rept_6_CONST; break;
		case BUTTON_7_CONST:        iPK_Button = BUTTON_Rept_7_CONST; break;
		case BUTTON_8_CONST:        iPK_Button = BUTTON_Rept_8_CONST; break;
		case BUTTON_9_CONST:        iPK_Button = BUTTON_Rept_9_CONST; break;
		case BUTTON_Asterisk_CONST: iPK_Button = BUTTON_Rept_Asterisk_CONST;  break;
		case BUTTON_Pound_CONST:    iPK_Button = BUTTON_Rept_Pound_CONST;     break;

#ifdef PHONEKEYS
		case BUTTON_C_CONST:
		case BUTTON_c_CONST:
			iPK_Button = BUTTON_Rept_Phone_C_CONST;
			break;
		case BUTTON_P_CONST:
		case BUTTON_p_CONST:
			iPK_Button = BUTTON_Rept_Phone_Pencil_CONST;
			break;
		case BUTTON_T_CONST:
		case BUTTON_t_CONST:
			iPK_Button = BUTTON_Rept_Phone_Talk_CONST;
			break;
		case BUTTON_E_CONST:
		case BUTTON_e_CONST:
			iPK_Button = BUTTON_Rept_Phone_End_CONST;
			break;
		case BUTTON_L_CONST:
		case BUTTON_l_CONST:
			iPK_Button = BUTTON_Rept_Phone_Soft_left_CONST;
			break;
		case BUTTON_R_CONST:
		case BUTTON_r_CONST:
			iPK_Button = BUTTON_Rept_Phone_Soft_right_CONST;
			break;
#endif
		}

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Long key %d", iPK_Button);
#endif
	}
#ifdef DEBUG
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Short key %d", iPK_Button);
#endif

	if(m_bShiftDown && m_bShiftDownOnScreenKeyboard)
	{
		if(iPK_Button == BUTTON_Shift_Up_Arrow_CONST)
			iPK_Button = BUTTON_Up_Arrow_CONST;
		else if(iPK_Button == BUTTON_Shift_Down_Arrow_CONST)
			iPK_Button = BUTTON_Down_Arrow_CONST;
		else if(iPK_Button == BUTTON_Shift_Left_Arrow_CONST)
			iPK_Button = BUTTON_Left_Arrow_CONST;
		else if(iPK_Button == BUTTON_Shift_Right_Arrow_CONST)
			iPK_Button = BUTTON_Right_Arrow_CONST;
	}

	bool bResult = HandleButtonEvent(iPK_Button);

	if(iPK_Button != BUTTON_left_shift_CONST && iPK_Button != BUTTON_right_shift_CONST && iPK_Button > BUTTON_Enter_CONST)
	{
		if(m_bShiftDown && m_bShiftDownOnScreenKeyboard)
			m_bShiftDown = false;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Button up - key");
#endif
	}
	else
	{
		if(m_bShiftDown && !m_bShiftDownOnScreenKeyboard)
			m_bShiftDown = false;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Button up - shift");
#endif
	}

	if(!m_bShiftDown)
		m_bShiftDownOnScreenKeyboard = true;

	return bResult;

}

/*virtual*/ void Orbiter::StopRepeatRelatedEvents()
{
	m_bWeCanRepeat = false;
	if( m_bRepeatingObject )
	{
		PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack== &Orbiter::ReselectObject )
				pCallBackInfo->m_bStop=true;
		}
		m_bRepeatingObject=false;
	}
}

/*virtual*/ void Orbiter::GenerateSimulateKeypress(int iPK_Button)
{
	int nPK_Button_To_Send = iPK_Button;

	if((!m_bShiftDown && m_bCapsLock) || (m_bShiftDown && !m_bCapsLock))
	{
		if(iPK_Button >= BUTTON_A_CONST && iPK_Button <= BUTTON_Z_CONST)
			nPK_Button_To_Send = iPK_Button - BUTTON_A_CONST + BUTTON_a_CONST;
		else if(iPK_Button >= BUTTON_a_CONST && iPK_Button <= BUTTON_z_CONST)
			nPK_Button_To_Send = iPK_Button - BUTTON_a_CONST + BUTTON_A_CONST;
	}

	if(m_bShiftDown)
	{
		switch(iPK_Button)
		{
		case BUTTON_0_CONST:        nPK_Button_To_Send = BUTTON_right_parenthesis_CONST ; break;
		case BUTTON_1_CONST:        nPK_Button_To_Send = BUTTON_exclamation_point_CONST ; break;
		case BUTTON_2_CONST:        nPK_Button_To_Send = BUTTON_at_sign_CONST ; break;
		case BUTTON_3_CONST:        nPK_Button_To_Send = BUTTON_Pound_CONST ; break;
		case BUTTON_4_CONST:        nPK_Button_To_Send = BUTTON_dollar_CONST ; break;
		case BUTTON_5_CONST:        nPK_Button_To_Send = BUTTON_percent_CONST ; break;
		case BUTTON_6_CONST:        nPK_Button_To_Send = BUTTON_caret_CONST ; break;
		case BUTTON_7_CONST:        nPK_Button_To_Send = BUTTON_ampersand_CONST ; break;
		case BUTTON_8_CONST:        nPK_Button_To_Send = BUTTON_Asterisk_CONST ; break;
		case BUTTON_9_CONST:        nPK_Button_To_Send = BUTTON_left_parenthesis_CONST ; break;

		case BUTTON_dash_CONST:         nPK_Button_To_Send = BUTTON_underscore_CONST ; break;
		case BUTTON_plus_CONST:         nPK_Button_To_Send = BUTTON_equals_sign_CONST ; break;
		case BUTTON_semicolumn_CONST:   nPK_Button_To_Send = BUTTON_colon_CONST ; break;
		case BUTTON_single_quote_CONST: nPK_Button_To_Send = BUTTON_double_quote_CONST ; break;
		}
	}

	DCE::CMD_Simulate_Keypress CMD_Simulate_Keypress_(m_dwPK_Device, m_pLocationInfo->m_dwPK_Device_Orbiter,
		StringUtils::itos(nPK_Button_To_Send), 0, "");
	SendCommand(CMD_Simulate_Keypress_);
}

bool Orbiter::RegionUp( int x,  int y )
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		if( m_pMouseBehavior && x && y && m_pMouseBehavior->ButtonUp(BUTTON_Mouse_1_CONST) )  // if x & y are 0, it's being called from Orbiter::NeedToChangeScreen
			return true;
	}
#endif

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnMouseUp, MouseClickCallBackData, (x, y))
	if(ExecuteScreenHandlerCallback(cbOnMouseUp))
		return true;

	StopRepeatRelatedEvents();
	return false;
}
bool Orbiter::RegionDown( int x,  int y )
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		if( m_pMouseBehavior )
		{
			if( m_pMouseBehavior->ButtonDown(BUTTON_Mouse_1_CONST) )
				return true;
		}
	}
#endif

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnMouseDown, MouseClickCallBackData, (x, y))
	if(ExecuteScreenHandlerCallback(cbOnMouseDown))
		return true;

	NeedToRender render( this, "Region Down" );  // Redraw anything that was changed by this command
	if( !GotActivity( 0 ) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring click %d,%d",x,y);
#endif
		return true;
	}

	class ScreenHistory *pScreenHistory = m_pScreenHistory_Current;

	bool bHandled=false;
	if(  !m_pScreenHistory_Current   )
	{
		// todo 2.0     if(  m_NumTouchesBeforeScreenAppears++<3  )
		//          return;
		//      exit( 1 );  // Die an immediate death
	}

	if ( m_pScreenHistory_Current  && m_pScreenHistory_Current->GetObj()->m_rPosition.Contains( x,  y ) )
	{
#ifdef LOG_MOUSE_CLICKS
		LoggerWrapper::GetInstance()->Write( LV_MOUSE_CLICKS,  "./MessageSend localhost 0 %d 1 305 57 %d 58 %d", m_dwPK_Device, x, y );
#endif
		DesignObj_Orbiter *pTopMostAnimatedObject=NULL;

		//LACA_B4_0( "contains it,  calling clicked region" )
#if ( defined( PROFILING ) )
		clock_t clkStart = clock(  );
#endif

		//any popup in this location ?
		DesignObj_Orbiter* pTopMostObject = m_pScreenHistory_Current->GetObj();
		PlutoPoint RelativePoint(x, y);

		if( m_pScreenHistory_Current )
		{
			for(list<class PlutoPopup*>::reverse_iterator it=m_pScreenHistory_Current->GetObj()->m_listPopups.rbegin();it!=m_pScreenHistory_Current->GetObj()->m_listPopups.rend();++it)
			{
				PlutoPopup *pPopup = *it;
				if( pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->m_rPosition.Right() &&
					pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->m_rPosition.Bottom()
					)
				{
					pTopMostObject = pPopup->m_pObj;
					RelativePoint = PlutoPoint(x - pPopup->m_Position.X, y - pPopup->m_Position.Y);
					break;
				}
			}
		}

		for(list<class PlutoPopup*>::reverse_iterator it=m_listPopups.rbegin();it!=m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(
				pPopup->m_Position.X <= x && x <= pPopup->m_Position.X + pPopup->m_pObj->m_rPosition.Right()    &&
				pPopup->m_Position.Y <= y && y <= pPopup->m_Position.Y + pPopup->m_pObj->m_rPosition.Bottom())
			{
				pTopMostObject = pPopup->m_pObj;
				RelativePoint = PlutoPoint(x - pPopup->m_Position.X, y - pPopup->m_Position.Y);
				break;
			}
		}

		bHandled=ClickedRegion( pTopMostObject, RelativePoint.X, RelativePoint.Y, pTopMostAnimatedObject );

#if ( defined( PROFILING ) )
		clock_t clkFinished = clock(  );
		if(  m_pScreenHistory_Current   )
		{
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "SelectedObject took %d ms", clkFinished - clkStart );
		}
#endif
	}

	if(  pScreenHistory != m_pScreenHistory_Current  )
		m_bRerenderScreen = true;

	return bHandled;
}
//------------------------------------------------------------------------
bool Orbiter::GotActivity( int PK_Button )
{
	if( (PK_Button>=BUTTON_1_CONST && PK_Button<=BUTTON_0_CONST) || PK_Button==BUTTON_Asterisk_CONST || PK_Button==BUTTON_Pound_CONST )
		return true;  // The numeric keys usually execute shortcuts without turning a device on/off

	m_LastActivityTime=time( NULL );

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::GotActivity m_bDisplayOn is %d m_bScreenSaverActive %d ui2 %d PK_Button %d",
		(int) m_bDisplayOn,(int) m_bScreenSaverActive,(int) UsesUIVersion2(),(int) PK_Button);
#endif

	if(m_bQuit_get())
		return false;

	// If the display is off, or we're in screen saver mode
	if( !m_bDisplayOn || 
		(m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu) )
	{
		bool bReturnValue=!m_bIgnoreFirstEvent;  // Normally we won't want this key to be processed if we're set to ignore first events because we need to turn on the tv first, with one exception below
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"GotActiity monitor m_bDisplayOn is %d",(int) m_bDisplayOn);
#endif
		if( !m_bDisplayOn )
			CMD_Display_OnOff( "1",false );

		NeedToRender render( this, "GotActivity" );
		CMD_Set_Main_Menu("N");

		if( !UsesUIVersion2() ||
			(PK_Button != BUTTON_F6_CONST && PK_Button != BUTTON_Mouse_6_CONST && PK_Button != BUTTON_F7_CONST &&
			PK_Button != BUTTON_Mouse_7_CONST && PK_Button != BUTTON_F8_CONST && PK_Button != BUTTON_Mouse_8_CONST && PK_Button!=BUTTON_Power_CONST) )
		{
			if( m_pScreenHistory_Current && m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu )
				CMD_Go_back("","1");
		}
		else
		{
			bReturnValue=true;  // For the 3 menu nav keys in UI2, do process it
		}

		if( m_bScreenSaverActive )
		{
			StopScreenSaver();
#ifdef ENABLE_MOUSE_BEHAVIOR
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::GotActivity calling CMD_Show_Mouse_Pointer mb %p vis %d",
				m_pMouseBehavior, NULL != m_pMouseBehavior ? (int) m_pMouseBehavior->m_bMouseVisible : -1);
#endif
			if( m_pMouseBehavior && m_pMouseBehavior->m_bMouseVisible )   // If m_bMouseVisible is true, it should be visible since screen saver doesn't set this, and that way if the screen saver hid it we'll restore it
				CMD_Show_Mouse_Pointer("1");
#endif
		}

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring click because screen saver was active");
#endif
		return bReturnValue; // Don't do anything with this
	}

	if(  NULL != m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds  )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got activity - calling Timeout in %d ",m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds);
#endif
		CallMaintenanceInMiliseconds( m_pScreenHistory_Current->GetObj()->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) m_pScreenHistory_Current->GetObj(), pe_ALL );
	}

	// If the screen saver is not active, then start a countdown to activate it
	if( !m_bBypassScreenSaver && !(m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu) && !m_bScreenSaverActive && m_iTimeoutScreenSaver )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got activity - calling ScreenSaver in %d ",m_iTimeoutScreenSaver);
#endif
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
	}
	return true;
}

/*
UTILITIES
*/
//------------------------------------------------------------------------
class DesignObj_Orbiter *Orbiter::GetCurrentDesignObj()
{ 
	return m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj() : NULL; 
}
//------------------------------------------------------------------------
DesignObj_Orbiter *Orbiter::FindObject( string PK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter )
{
	if( pDesignObj_Orbiter==NULL )
		pDesignObj_Orbiter = GetCurrentDesignObj();

	PK_DesignObj = SubstituteVariables(PK_DesignObj, NULL, 0, 0);
	string OriginalValue;
	bool bSingleNumber=false;

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		DesignObj_OrbiterMap::iterator oi;

	// Could be a screen,  find the current version and page
	if(  PK_DesignObj.find( "." )==string::npos  )
	{
		OriginalValue=PK_DesignObj;
		bSingleNumber=true;
		// AB 1 Jul 05 - check for version & page first, otherwise popups use the wrong version
		if( pDesignObj_Orbiter )
			PK_DesignObj += "." + StringUtils::itos(pDesignObj_Orbiter->m_iVersion) + "." + StringUtils::itos(pDesignObj_Orbiter->m_iPage);
		else if (m_pLocationInfo)
			PK_DesignObj += "." + StringUtils::itos(m_pLocationInfo->iLocation) + ".0";

		if( (oi=m_mapObj_All.find(PK_DesignObj))!=m_mapObj_All.end(  ) )
			return oi->second;
		PK_DesignObj = OriginalValue + ".0.0";
	}

	oi=m_mapObj_All.find( PK_DesignObj );
	if ( oi==m_mapObj_All.end(  ) )
	{
		if(  bSingleNumber  )
		{
			// We were passed in another object to look at,  and assume this one is on the same screen
			if(  pDesignObj_Orbiter  )
			{
				string::size_type pos=0;
				for( int iDotCount=0;iDotCount<2 && pos!=string::npos;iDotCount++, pos++ )
					pos=pDesignObj_Orbiter->m_ObjectID.find( ".", pos );

				// pos should now be at teh third dot,  unless we were passed in a screen which only has 2 dots
				PK_DesignObj = ( pos==string::npos ? pDesignObj_Orbiter->m_ObjectID : pDesignObj_Orbiter->m_ObjectID.substr( 0, pos+1 ) ) + "." + OriginalValue;

			}
			else if(  m_pScreenHistory_Current  )
			{
				// Maybe it's an object on the current screen
				PK_DesignObj = m_pScreenHistory_Current->GetObj()->m_ObjectID + "." + OriginalValue;
			}
		}

		DesignObj_DataList *pObjectList;
		map<string, DesignObj_DataList *>::iterator it = m_mapObj_AllNoSuffix.find( PK_DesignObj );
		if(  it!=m_mapObj_AllNoSuffix.end(  )  )
		{
			pObjectList = ( *it ).second;
			if(  pObjectList->size(  )==1  )
				return ( DesignObj_Orbiter * )pObjectList->front(  );

			DesignObj_DataList::iterator itObjList;
			for( itObjList=pObjectList->begin(  );itObjList!=pObjectList->end(  );++itObjList )
			{
				DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * )( *itObjList );
				if(  pObj->m_bOnScreen && !pObj->IsHidden(  ) )
				{
					return pObj;
				}
			}

			// So we didn't find anything on screen,  just return the first
			if(  pObjectList->size(  )>0  )
				return ( DesignObj_Orbiter * ) pObjectList->front(  );
		}
		// We're going to have to hunt for it
		else if(  bSingleNumber  )
		{
			// Release the variable mutex first.  We don't want variables locked while we wait for overlay to become
			// available.
			vm.Release(  );
			// Now re-lock the variable mutex
			PLUTO_SAFETY_LOCK( vm2, m_VariableMutex );
			if(  pDesignObj_Orbiter  )
				FindSingleNumberObject( atoi( OriginalValue.c_str(  ) ), pDesignObj_Orbiter );
			else if(  m_pScreenHistory_Current  )
				FindSingleNumberObject( atoi( OriginalValue.c_str(  ) ), m_pScreenHistory_Current->GetObj() );
			else
				return NULL;
		}
	}
	else
	{
		return ( ( *oi ).second );
	}

	return NULL;
}
//------------------------------------------------------------------------
DesignObj_Orbiter* Orbiter::FindSingleNumberObject( int PK_Object,  DesignObj_Orbiter *pObj )
{
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
		if(  pChild->m_iBaseObjectID==PK_Object  )
			return pChild;
		pChild = FindSingleNumberObject( PK_Object, pChild );
		if(  pChild  )
			return pChild;
	}

	for( iHao=pObj->m_OverlayObjects.begin(  ); iHao != pObj->m_OverlayObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pChild = ( DesignObj_Orbiter * ) ( *iHao );
		if(  pChild->m_iBaseObjectID==PK_Object  )
			return pChild;

		pChild = FindSingleNumberObject( PK_Object, pChild );
		if(  pChild  )
			return pChild;
	}
	return NULL;
}
//------------------------------------------------------------------------
void Orbiter::ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList,
									DesignObj_Orbiter *pObj, SelectionMethod selectionMethod,
									int X,  int Y, int Repeat )
{
	if(m_bQuit_get())
		return;

	if(  pDesignObjCommandList->size(  )==0  )
		return;

	if( pObj->m_iRepeatIntervalInMS && m_bWeCanRepeat )
	{
		m_bRepeatingObject=true;
		pObj->m_iRepeatParm = Repeat;
		// Arbitrary decision that there should be a bit of a delay before the object starts repeating that
		// is 3 times the repeat interval
		CallMaintenanceInMiliseconds( (Repeat==0 ? pObj->m_iRepeatIntervalInMS * 3 : pObj->m_iRepeatIntervalInMS),
			&Orbiter::ReselectObject, pObj, pe_ALL, true );
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Executing %d commands in object: %s", ( int ) pDesignObjCommandList->size(  ),  pObj->m_ObjectID.c_str(  ) );
#endif

	bool bRefreshGrids=false;
	Message *pMessage=NULL;

	// The commands are in the list backwards, so use a reverse iterator so that they are executed in the same order they appear within designer
	DesignObjCommandList::reverse_iterator iCommand;
	bool bGetConfirmation=false;
	for(iCommand=pDesignObjCommandList->rbegin();iCommand!=pDesignObjCommandList->rend();++iCommand)
	{
		DesignObjCommand *pCommand = *iCommand;
		int PK_Command = pCommand->m_PK_Command;

		if(	
			Simulator::GetInstance()->IsRunning() &&
			(
				PK_Command == COMMAND_Regen_Orbiter_CONST	|| 
				PK_Command == COMMAND_Reload_CONST			||
				PK_Command == COMMAND_Delete_File_CONST
			)
		)
			continue;

		if( pCommand->m_bDeliveryConfirmation )
			bGetConfirmation=true;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Executing command %d in object: %s", PK_Command,  pObj->m_ObjectID.c_str(  ) );
#endif
//if( PK_Command==35 )
//int k=2;
		if(
			Simulator::GetInstance()->IsRunning() && 
			(
				PK_Command == COMMAND_Restart_DCERouter_CONST || 
				PK_Command == COMMAND_Halt_Device_CONST
			)
		)
			continue;  // While in stress-test simulator, don't try to reset the router
		else if(  PK_Command==COMMAND_Requires_Special_Handling_CONST  )
		{
			SpecialHandlingObjectSelected( pObj );
			continue;
		}
		else if( PK_Command==COMMAND_Set_Current_User_CONST && m_vectPK_Users_RequiringPIN.size() )
		{
			int PK_Users = atoi(pCommand->m_ParameterList[COMMANDPARAMETER_PK_Users_CONST].c_str());
			for(size_t s=0;s<m_vectPK_Users_RequiringPIN.size();++s)
			{
				if( PK_Users==m_vectPK_Users_RequiringPIN[s] )
				{
					// This user requires a PIN.  Change there and return
					m_bRestrictedOp_IsUser=true;
					m_iRestrictedOp_ID=PK_Users;
					CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_mnuUserPinCode_CONST),"","",false,true);
					return;
				}
			}
		}
		else if( PK_Command==COMMAND_Set_Current_Location_CONST )
		{
			// Only cc
			int iLocationID = atoi(pCommand->m_ParameterList[COMMANDPARAMETER_LocationID_CONST].c_str());
			LocationInfo *pLocationInfo = m_dequeLocation[iLocationID];
			if( pLocationInfo->m_vectAllowedUsers.size() )
			{
				// Only some users can switch to this location
				size_t s;
				for(s=0;s<pLocationInfo->m_vectAllowedUsers.size();++s)
				{
					if( m_dwPK_Users==pLocationInfo->m_vectAllowedUsers[s] )
						break; // this one's ok
				}
				if( s>=pLocationInfo->m_vectAllowedUsers.size() )
				{
					CMD_Set_Text( StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "You are not allowed to access this location", TEXT_STATUS_CONST);
					CMD_Goto_DesignObj( 0, StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST), "", "", false, true );
					return;
				}
			}
		}

		// We may attach a simulate keypress action to an onscreen button, like a keyboard key, which simulates pressing
		// that key if the user touches it.  In this case, we don't want to execute the simulate keypress command
		// if the button was selected because the user hit the button associated with it (ie x==-1 and y==-1 when input was ButtonDown)
		// and create an endless loop
		else if( PK_Command==COMMAND_Simulate_Keypress_CONST && selectionMethod==smKeyboard )
			continue;


		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT
		// HACK! HACK!
		// TODO - REMOVE THIS AND MAKE IT A PARAMETER FOR THE OBJECT

		if(  pCommand->m_PK_DeviceTemplate  )
		{
			if(  pCommand->m_PK_DeviceTemplate<0  )
			{
				pCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;  // Set this to a temporary value,  so we'll know if it's changes at teh end of the if
				if( pCommand->m_PK_DeviceTemplate!=DEVICEID_HANDLED_INTERNALLY )  // Just leave it there
				{
					TranslateVirtualDevice(pCommand->m_PK_DeviceTemplate,pCommand->m_PK_Device);
					if(  pCommand->m_PK_Device==DEVICEID_NULL  )
					{
						// We recognized it as one of the known cases in the switch block,  but apparently this option isn't sent
						LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Sending command to unidentified virtual device: %d", pCommand->m_PK_DeviceTemplate );
						pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // Maybe there's some interceptor that will know what to do with it
					}
					else if(  pCommand->m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
						pCommand->m_PK_Device = pCommand->m_PK_DeviceTemplate; // It's not a virtual device we know how to handle.  Let the interceptors have it
				}
			}
			else
				pCommand->m_PK_Device=DEVICEID_MASTERDEVICE;
		}
		else if(  pCommand->m_PK_DeviceCategory  )
			pCommand->m_PK_Device=DEVICEID_CATEGORY;

		if(  pCommand->m_PK_Device==0 && pCommand->m_PK_DeviceGroup==0  )
		{
			// See if it's one we handle locally
			if(  m_bLocalMode )
			{
				pCommand->m_PK_Device=DEVICEID_HANDLED_INTERNALLY;
			}
			else if( m_mapDevice_Selected.size() )
				pCommand->m_PK_Device = DEVICEID_LIST;
			else
			{
				PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
					pCommand->m_PK_Device = atoi( m_mapVariable[VARIABLE_PK_Device_CONST].c_str(  ) );
				vm.Release(  );
			}
		}

		if(  pCommand->m_PK_Device==DEVICEID_NULL  )
		{
			LoggerWrapper::GetInstance()->Write( LV_CONTROLLER,  "\x1b[33;1m****ACTION %d GOING TO DEVICEID_NULL***%d\x1b[0m\n", PK_Command );
		}
		else
		{
			if(PK_Command==COMMAND_Populate_Datagrid_CONST)
			{
				// If we already added messages to the queue, go ahead and send them out so that the populate datagrid is executed in the proper order
				if(  pMessage && !m_bLocalMode  )
				{
					if( !m_pcRequestSocket->SendMessage( pMessage ) )
						LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Send Message failed");
					pMessage = NULL;
				}

				PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
				string GridID = pCommand->m_ParameterList[COMMANDPARAMETER_DataGrid_ID_CONST];
				DesignObj_DataGrid *pDesignObj_DataGrid_OnScreen=NULL;
				// See if this grid is onscreen
				int iWidth=0,iHeight=0;
				for( size_t s=0;s<m_vectObjs_GridsOnScreen.size(  );++s )
				{
					DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[s];
					if(  pDesignObj_DataGrid->m_sGridID == GridID  )
					{
						pDesignObj_DataGrid_OnScreen=pDesignObj_DataGrid;
						pDesignObj_DataGrid_OnScreen->m_iHighlightedRow=pDesignObj_DataGrid_OnScreen->m_sExtraInfo.find( 'C' )==string::npos ? 0 : -1;
						pDesignObj_DataGrid_OnScreen->m_iHighlightedColumn=pDesignObj_DataGrid_OnScreen->m_sExtraInfo.find( 'R' )==string::npos ? 0 : -1;
						iWidth=pDesignObj_DataGrid_OnScreen->m_MaxCol;
						iHeight=pDesignObj_DataGrid_OnScreen->m_MaxRow;
						bRefreshGrids=true;
					}
				}

				bool bResponse;
				int iPK_Variable=atoi(pCommand->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST].c_str());;
				string sValue_To_Assign=pCommand->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST];
				/*
				if( iWidth<0 )
				{
				int k=2;
				}
				*/

				string sParams;
//				if(pDesignObj_DataGrid_OnScreen)  
//					sParams += SubstituteVariables( pDesignObj_DataGrid_OnScreen->m_sOptions, pObj, 0, 0 );  // AB 24-May-2006 What was the reason for this!  I resulted in a dup since the same options were below.  If we want the same options, wouldn't we just put them in the command parameters anyway??
				sParams += SubstituteVariables( pCommand->m_ParameterList[COMMANDPARAMETER_Options_CONST], pObj, 0, 0 );
				DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid( m_dwPK_Device,  m_dwPK_Device_DatagridPlugIn,  StringUtils::itos( m_dwIDataGridRequestCounter ),
					GridID, atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ),
					sParams,
					atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DeviceTemplate_CONST].c_str(  ) ),
					&iPK_Variable, &sValue_To_Assign, &bResponse, &iWidth, &iHeight );
				if(  !SendCommand( CMD_Populate_Datagrid ) || !bResponse  ) // wait for a response
					LoggerWrapper::GetInstance()->Write( LV_WARNING, "Populate datagrid from command: %d failed", atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DataGrid_CONST].c_str(  ) ) );
				else if(iPK_Variable)
					CMD_Set_Variable(iPK_Variable, sValue_To_Assign);

				if( m_vectObjs_GridsOnScreen.size() )
				{
					DesignObj_DataGrid *pObj = m_vectObjs_GridsOnScreen[0];
					if( pObj->m_sGridID==GridID && pObj->m_sExtraInfo.find('S')!=string::npos && pObj->m_sSeek.length()==0 && !pObj->sSelVariable.empty(  )  )
					{
						PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
							pObj->m_sSeek = "~" + m_mapVariable[atoi( pObj->sSelVariable.c_str(  ) )];
					}
				}


				continue; // processed this one ourselves
			}

			Message *pThisMessage = new Message( m_dwPK_Device, pCommand->m_PK_Device, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, PK_Command, 0 );
			if(  pCommand->m_PK_Device==DEVICEID_MASTERDEVICE  )
				pThisMessage->m_dwPK_Device_Template=pCommand->m_PK_DeviceTemplate;
			else if(  pCommand->m_PK_Device==DEVICEID_CATEGORY  )
				pThisMessage->m_dwPK_Device_Category_To=pCommand->m_PK_DeviceCategory;
			else if(  pCommand->m_PK_Device==DEVICEID_LIST  )
			{
				for(map<int,class DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				{
					pThisMessage->m_sPK_Device_List_To += StringUtils::itos((*it).first) + ",";
				}
			}

			bool bAlreadySetRepeat=false;
			map<int,  string>::iterator iap;
			for( iap=pCommand->m_ParameterList.begin(  );iap!=pCommand->m_ParameterList.end(  );++iap )
			{
				int nKey = iap->first;
				string sRawValue = iap->second;

				string Value = SubstituteVariables(sRawValue, pObj, X, Y);
				pThisMessage->m_mapParameters[nKey]=Value;
				if( nKey==COMMANDPARAMETER_Repeat_Command_CONST )
					bAlreadySetRepeat=true;

#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "DDD Parm %d = %s, raw value %s", nKey, Value.c_str(), sRawValue.c_str());
#endif
			}

			if( Repeat && !bAlreadySetRepeat )
				pThisMessage->m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST]=StringUtils::itos(max(Repeat,MAX_REPEAT));


			pThisMessage->m_dwPK_Device_Group_ID_To=pCommand->m_PK_DeviceGroup;

			if(  pCommand->m_PK_Device==DEVICEID_HANDLED_INTERNALLY  )
			{
				pThisMessage->m_dwPK_Device_To = m_dwPK_Device; // So the handler will loop back to ourselves
				ReceivedMessage( pThisMessage );
				delete pThisMessage;
				pThisMessage = NULL;

				// Don't bother sending grid movements either
				if(  PK_Command==COMMAND_Simulate_Keypress_CONST ||
					PK_Command==COMMAND_Scroll_Grid_CONST ||
					PK_Command==COMMAND_Show_Object_CONST  )
				{
					continue;
				}
			}
			else
				if(  pMessage==NULL  )
					pMessage=pThisMessage;
				else
					pMessage->m_vectExtraMessages.push_back( pThisMessage );
		}

		if(  pMessage && !m_bLocalMode  )
		{
			if( bGetConfirmation )
			{
				pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;  // i.e. just an "OK"
				string sResponse; // We'll use this only if a response wasn't passed in
				bool bResult = m_pcRequestSocket->SendMessage( pMessage, sResponse );
				if( !bResult || sResponse != "OK" )
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Send Message failed with result %d Response %s",(int) bResult,sResponse.c_str());
			}
			else if( pMessage->m_dwID==COMMAND_Bind_to_Media_Remote_CONST )
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ExecuteCommandInList sending bind to media remote");
#endif
				pMessage->m_eExpectedResponse = ER_ReplyMessage;  // a set now playing command
				Message *pResponse = m_pcRequestSocket->SendReceiveMessage( pMessage );
				if(NULL != pResponse)
				{
					if(pResponse->m_dwID)
						ReceivedMessage( pResponse );

					delete pResponse;
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::ExecuteCommandInList sending bind to media remote failed");

				pMessage = NULL;
			}
			else
			{
#ifdef ENABLE_MOUSE_BEHAVIOR
				if(UsesUIVersion2())
				{
					if( selectionMethod==smMouseGovernor && m_pMouseBehavior && m_pMouseBehavior->m_pMouseGovernor )
						m_pMouseBehavior->m_pMouseGovernor->SendMessage(pMessage);
					else 
						if( !m_pcRequestSocket->SendMessage( pMessage ) )
							LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Send Message failed");
				}
				else
					if( !m_pcRequestSocket->SendMessage( pMessage ) )
						LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Send Message failed");
#else
				if( !m_pcRequestSocket->SendMessage( pMessage ) )
					LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Send Message failed");
#endif
			}
			pMessage = NULL;
		}
	}

	if(  bRefreshGrids  )
	{
		// This includes a populate datagrid.  We are probably changing the contents of a
		// grid that's on screen,  so send this with confirmation to be sure the thread blocks
		// until it goes through,  and then refresh our grids
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
		for( size_t grid=0;grid<m_vectObjs_GridsOnScreen.size(  );++grid )
		{
			DesignObj_DataGrid *pDesignObj_DataGrid=m_vectObjs_GridsOnScreen[grid];
			pDesignObj_DataGrid->Flush();
		}

		m_pOrbiterRenderer->RenderObjectAsync(m_pScreenHistory_Current->GetObj());
	}
}
//------------------------------------------------------------------------
string Orbiter::SubstituteVariables( string Input,  DesignObj_Orbiter *pObj,  int X,  int Y )
{
	string Output;

	size_t pos=0;
	for( ;; )
	{
		size_t foundbegin;
		foundbegin=Input.find( "<%=",  pos );
		if ( foundbegin==string::npos )
			break;
		Output+=Input.substr( pos,  ( foundbegin-pos ) );
		size_t foundend=Input.find( "%>",  foundbegin );
		pos=foundbegin+3;
		if ( foundend==string::npos )
			break;

		int PK_Variable=0;
		string Variable = Input.substr( pos,  foundend-pos );

		if(Variable.size() >= 2 && Variable[0]=='#')
		{
			//<%=#xxxx%>, where xxxx is a ASCII for a char in decimal
			//e.g.: for quote : <%=#34%>

			Output += (char)atoi(Variable.substr(1).c_str());
		}
		if(  Variable=="M"  )
			Output += m_sMainMenu;
		else if(  Variable=="!"  )
			Output += StringUtils::itos( m_dwPK_Device );
		else if(  Variable=="LANIP"  )  // IP on local LAN
		{
			Output += m_sIPAddress;
		}
		else if(  Variable=="RIP"  )
		{
#ifdef WIN32
			Output += "router=" + m_sIPAddress;
#else
			string sCmdOutput;
			ProcessUtils::RunApplicationAndGetOutput("/usr/pluto/bin/Network_DisplaySettings.sh --orbiter", sCmdOutput);
			Output += sCmdOutput;
#endif
		}
		else if(Variable == "RSI")
		{
			//rip status indicator
			string sRippingStatus;
			DCE::CMD_Get_Ripping_Status cmd_Get_Ripping_Status(m_dwPK_Device, m_dwPK_Device_MediaPlugIn, &sRippingStatus);
			SendCommand(cmd_Get_Ripping_Status);

			Output += sRippingStatus;
		}
		else if(Variable == "LUV")
		{
			string sLastUpdatedVersion = "N/A";

			if(m_pData->m_dwPK_Device_Core)
				sLastUpdatedVersion = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_Core, DEVICEDATA_LastUpdate_CONST);
			else if(m_pData->m_dwPK_Device_MD)
				sLastUpdatedVersion = m_pEvent->GetDeviceDataFromDatabase(m_pData->m_dwPK_Device_MD, DEVICEDATA_LastUpdate_CONST);

			Output += sLastUpdatedVersion;
		}
		else if(  Variable[0]=='!'  )  // It starts with ! -- that's a not followed by a variable, if the variable is 1 we return 0, anything else we return 1
		{
			int PK_Variable = atoi( Variable.substr(1).c_str() );
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				Output+= (m_mapVariable[PK_Variable]!="1" ? "1" : "0");
		}
		else if(  Variable=="E" && m_pLocationInfo  )
			Output += StringUtils::itos( m_pLocationInfo->PK_EntertainArea );
		else if(  Variable=="MD" )
		{
			if( m_bIsOSD )
			{
				// Fixes #458 - tschak
				if ( m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
				{
					// Are we in a different room? if so, grab the device from Location
					Output += StringUtils::itos( m_pLocationInfo->m_dwPK_Device_MediaDirector );
				}
				else
				{
					// Get the Media Director's Device ID Directly.
					Output += StringUtils::itos( m_pData->m_dwPK_Device_ControlledVia );
				}
			}
			else if( m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
				Output += StringUtils::itos( m_pLocationInfo->m_dwPK_Device_MediaDirector );
		}
		else if(  Variable=="OS" )
		  {
		    long dwPK_Device_MD;
		    if( m_bIsOSD )
		      {
			if ( m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
			  {
			    // Are we in a different room? if so, grab the device from Location
			    dwPK_Device_MD = m_pLocationInfo->m_dwPK_Device_MediaDirector;
			  }
			else
			  {
			    dwPK_Device_MD = m_pData->m_dwPK_Device_ControlledVia;
			  }
		      }
		    else if( m_pLocationInfo && m_pLocationInfo->m_dwPK_Device_MediaDirector!=-1 )
		      dwPK_Device_MD=m_pLocationInfo->m_dwPK_Device_MediaDirector;

		    // At this point, we have the target MD to get device data from.
		    Output += m_pEvent->GetDeviceDataFromDatabase(dwPK_Device_MD, DEVICEDATA_Operating_System_CONST);
		  }
		else if(  Variable=="MDH"  )
			Output += StringUtils::itos( m_pLocationInfo_Initial->m_dwPK_Device_MediaDirector );
		else if(  Variable=="L:0" && m_pLocationInfo  )
			Output += StringUtils::itos(m_pLocationInfo->iLocation);
		else if(  Variable=="LD" && m_pLocationInfo  )
			Output += m_pLocationInfo->Description;
		else if(  Variable=="LDGC"  )
			Output += m_sLastSelectedDatagrid;
		else if(  Variable=="ID" && m_pScreenHistory_Current )
			Output += m_pScreenHistory_Current->ScreenID();
		else if(  Variable=="V" )
			Output += string(VERSION) + "(" + g_szCompile_Date + ")";
		else if(  Variable=="CLOCK" )
			Output += StringUtils::HourMinute(time(NULL),true);
		else if ( Variable=="CL:H")
			Output += StringUtils::Hour(time(NULL));
		else if ( Variable=="CL:M")
			Output += StringUtils::Minute(time(NULL));
		else if ( Variable=="CL:DOW")
			Output += StringUtils::DayOfWeek(time(NULL));
		else if ( Variable=="CL:DOM")
			Output += StringUtils::DayOfMonth(time(NULL));
		else if ( Variable=="CL:MON")
			Output += StringUtils::Month(time(NULL));
		else if(  Variable=="MT" )
		{
			Output += StringUtils::itos(m_iPK_MediaType);
		}
		else if( Variable=="MTB" )
		{
			Output += StringUtils::itos(m_pScreenHandler->mediaFileBrowserOptions.m_PK_MediaType);
		}
		else if( Variable=="MB_USERS" )
		{
			Output += m_pScreenHandler->mediaFileBrowserOptions.m_sPK_Users_Private;
		}
		else if( Variable=="FBO" )
		{
			if( m_pScreenHandler )
				Output += m_pScreenHandler->mediaFileBrowserOptions.ToString();
		}
		else if(  Variable=="NP" )
		{
			Output += m_sNowPlaying;
			m_pObj_NowPlayingOnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect(pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_MT" )
		{
			Output += m_sNowPlaying_MediaType;
			m_pObj_NowPlaying_MT_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect(pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_SEC" )
		{
			Output += m_sNowPlaying_Section;
			m_pObj_NowPlaying_Section_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_TIME_SHORT" )
		{
			Output += m_sNowPlaying_TimeShort;
			m_pObj_NowPlaying_TimeShort_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_TIME_LONG" )
		{
			Output += m_sNowPlaying_TimeLong;
			m_pObj_NowPlaying_TimeLong_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_SPEED" )
		{
			Output += m_sNowPlaying_Speed;
			m_pObj_NowPlaying_Speed_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_ANGLE" )
		{
			Output += m_sNowPlaying_Angle;
			m_pObj_NowPlaying_Angle_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_AUDIO" )
		{
			Output += m_sNowPlaying_Audio;
			m_pObj_NowPlaying_Audio_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NP_SUBTITLE" )
		{
			Output += m_sNowPlaying_Subtitle;
			m_pObj_NowPlaying_Subtitle_OnScreen=pObj;
			if( !pObj->m_pvectCurrentGraphic || pObj->m_pvectCurrentGraphic->size()==0 && !pObj->m_pGraphicToUndoSelect )
				m_pOrbiterRenderer->SaveBackgroundForDeselect( pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		}
		else if(  Variable=="NPD" )
			Output += StringUtils::itos(m_dwPK_Device_NowPlaying);
		else if(  Variable=="NPDT" )
		{
			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device_NowPlaying);
			if( pDevice )
				Output += StringUtils::itos(pDevice->m_dwPK_DeviceTemplate);
		}
		else if(  Variable=="NP_R" )
		{
			if( m_bIsOSD && m_iPK_Screen_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
				Output += StringUtils::itos(m_iPK_Screen_RemoteOSD);
			else
				Output += m_iPK_Screen_Remote ? StringUtils::itos(m_iPK_Screen_Remote) : "**NOMEDIA**";
		}
		else if(  Variable=="NP_PR" )
			Output += StringUtils::itos(m_iPK_DesignObj_Remote_Popup);
		else if(  Variable=="NP_FL" )
			Output += StringUtils::itos(m_iPK_Screen_FileList);
		else if(  Variable=="NP_RNF" )
			Output += StringUtils::itos(m_iPK_Screen_Remote);
		else if(  Variable=="B" )
			Output += "\t";
		else if(  Variable=="N" )
			Output += "\n";
		else if(  Variable=="ND" )
			Output += StringUtils::itos((int) m_mapDevice_Selected.size());
		else if(  Variable=="DFN"  )
			Output += m_sDefaultRippingName;
		else if(Variable=="DSDR")
			Output += StringUtils::ltos(m_nDefaultStorageDeviceForRipping);
		else if(Variable=="DSDRN")
			Output += m_sDefaultStorageDeviceForRippingName;
		else if(  Variable.length()>1 && Variable[0]=='G' && Variable[1]=='D' )
		{
			DesignObj_Orbiter *pObjGD = pObj;
			if( Variable.length()>2 )
			{
				pObjGD = FindObject(Variable.substr(2));
				if( !pObjGD )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find GD%s",Variable.substr(2).c_str());
					pObjGD = pObj;
				}
			}
			Output += StringUtils::itos(pObjGD->m_GraphicToDisplay);
		}
		else if( Variable[0]=='C' && Variable[1]=='A' )
		{
			if( Variable.length()>4 )  // Token must be at least 2 characters, like <%=CA:ID%>
			{
				if( Variable[2]=='H' )
				{
					// Syntax is CAH:ObjectID:Attribute
					string::size_type pos = Variable.find(':',4);
					if( pos!=string::npos )
					{
						DesignObj_Orbiter *pObj = FindObject(Variable.substr(4,pos-4));
						if( pObj && pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
						{
							DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj;
							DataGridCell *pCell = GetDataGridHighlightCell(pObj_Grid);
							if( !pCell )
								pCell = GetDataGridSelectedCell(pObj_Grid);

							if( pCell && pCell->m_mapAttributes.find( Variable.substr(pos+1) )!=pCell->m_mapAttributes.end() )
								Output += pCell->m_mapAttributes[ Variable.substr(pos+1)];
						}
					}
				}
				else if( pObj->m_pDesignObj_DataGrid )
				{
					// Syntax is CA:Attribute.  This is only valid if the object being rendered points to a cell
					DataGridTable *pDataGridTable = pObj->m_pDesignObj_DataGrid->DataGridTable_Get();
					if( pDataGridTable )
					{
						DataGridCell *pCell = pDataGridTable->GetData(pObj->m_iGridCol + pObj->m_pDesignObj_DataGrid->m_GridCurCol, pObj->m_iGridRow + pObj->m_pDesignObj_DataGrid->m_GridCurRow);
						if( pCell )
							Output += pCell->m_mapAttributes[ Variable.substr(3) ];
					}
				}
			}
			else if( Variable[2]=='V' && pObj->m_pDesignObj_DataGrid )
			{
				// Syntax is CAV for the value of the selected cell
				DataGridTable *pDataGridTable = pObj->m_pDesignObj_DataGrid->DataGridTable_Get();
				if( pDataGridTable )
				{
					DataGridCell *pCell = pDataGridTable->GetData(pObj->m_iGridCol + pObj->m_pDesignObj_DataGrid->m_GridCurCol, pObj->m_iGridRow + pObj->m_pDesignObj_DataGrid->m_GridCurRow);
					if( pCell && pCell->m_Value )
						Output += pCell->m_Value;
				}
			}
			else if( Variable[2]=='T' && pObj->m_pDesignObj_DataGrid )
			{
				// Syntax is CAT for the text of the selected cell
				DataGridTable *pDataGridTable = pObj->m_pDesignObj_DataGrid->DataGridTable_Get();
				if( pDataGridTable )
				{
					DataGridCell *pCell = pDataGridTable->GetData(pObj->m_iGridCol + pObj->m_pDesignObj_DataGrid->m_GridCurCol, pObj->m_iGridRow + pObj->m_pDesignObj_DataGrid->m_GridCurRow);
					if( pCell && pCell->m_Text )
						Output += pCell->m_Text;
				}
			}
		}
		else if(  Variable.length()>1 && Variable[0]=='K' )
		{
			if( m_pScreenHandler )
				Output += m_pScreenHandler->m_mapKeywords[Variable.substr(1)];
		}
		else if(  Variable.length()>1 && Variable[0]=='A' )
		{
			int PK_Attribute;
			string sAttribute;
			if( Variable[1]=='V' )
				PK_Attribute = atoi(m_mapVariable[atoi(Variable.substr(2).c_str())].c_str());
			else
				PK_Attribute = atoi(Variable.substr(2).c_str());
			DCE::CMD_Get_Attribute CMD_Get_Attribute(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,PK_Attribute,&sAttribute);
			SendCommand(CMD_Get_Attribute);
			Output += sAttribute;
		}
		else if(  Variable.length()>1 && Variable[0]=='E' )
		{
			Output += "<%=" + Variable.substr(1) + "%>";
		}
		else if(  Variable=="SKIN" )
			Output += m_sSkin;
		else if(Variable == "SIZE")
		{
			Output += StringUtils::itos(m_iImageWidth) + "x" + StringUtils::itos(m_iImageHeight);
		}
		else if(  Variable=="SG" )
		{
			time_t t = m_tGenerationTime;
			struct tm *p_tm = localtime( &t );
			Output += StringUtils::itos( p_tm->tm_mon+1 ) + "/" + StringUtils::itos( p_tm->tm_mday) + "/" + StringUtils::itos( p_tm->tm_year+1900 ) + " " +
				StringUtils::itos( p_tm->tm_hour ) + ":" + StringUtils::itos( p_tm->tm_min );
		}
		else if(  Variable=="CD" )
		{
			// Find if there are any selected object timeout's pending
			PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
			for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
			{
				PendingCallBackInfo *pCallBackInfo = (*it).second;
				if( pCallBackInfo->m_fnCallBack == &Orbiter::DelayedSelectObject && !pCallBackInfo->m_bStop )
				{
					DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) pCallBackInfo->m_pData;
					if( TestCurrentScreen(pDelayedSelectObjectInfo->m_sPK_DesignObj_CurrentScreen) )
					{
						timespec ts_Current,ts;
						gettimeofday(&ts_Current,NULL);
						ts = pDelayedSelectObjectInfo->m_tsTime - ts_Current;

						Output += StringUtils::itos(ts.tv_sec);
						break;
					}
				}
			}
		}
		else if(  Variable=="LS" && m_pObj_LastSelected && m_pObj_LastSelected->m_pFloorplanObject )
			Output += StringUtils::itos(m_pObj_LastSelected->m_pFloorplanObject->PK_Device);
		else if(  Variable=="SD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
				Output += StringUtils::itos((*it).first) + ",";
		else if(  Variable=="STE" ) //selected telecom extensions
		{
			Output = m_mapVariable_Find(VARIABLE_External_channels_CONST);
			Output = StringUtils::Replace(Output, "|", ",");
			
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
			{
				DeviceData_Base *pDeviceData_Base = it->second;
				if(NULL != pDeviceData_Base)
				{
					string sExtension;
					CMD_Get_Device_Data cmd_Get_Device_Data(
						m_dwPK_Device, m_dwPK_Device_GeneralInfoPlugIn,
						pDeviceData_Base->m_dwPK_Device, DEVICEDATA_PhoneNumber_CONST, true,
						&sExtension
					);
					SendCommand(cmd_Get_Device_Data);

					if(!Output.empty())
						Output += ",";

					Output += sExtension;
				}
			}
		}
		else if(  Variable=="SDD" )
			for(map<int,DeviceData_Base *>::iterator it=m_mapDevice_Selected.begin();it!=m_mapDevice_Selected.end();++it)
			{
				if(it->second)
				{
					Output += it->second->m_sDescription + ", ";
				}
				else
				{
					//this is a media floorplan object; it doesn't have a device associated
					FloorplanObject *pfObj = m_mapFloorplanObject_Selected[it->first];
					string sStatus = pfObj->Status == "offline" ? " (" + pfObj->Status + ")" : "";
					Output += pfObj->DeviceDescription + sStatus + ", ";
				}
			}
		else if(  Variable=="R" && m_pLocationInfo  )
			Output += StringUtils::itos( m_pLocationInfo->PK_Room );
		else if(  Variable=="U" )
			Output += StringUtils::itos( m_dwPK_Users );
		else if(  Variable=="UN" )
			Output += m_pOrbiterFileBrowser_Collection->m_mapUsernames_Find(m_dwPK_Users);
		else if(  Variable=="X"  )
			Output += StringUtils::itos( X );
		else if(  Variable=="Y"  )
			Output += StringUtils::itos( Y );
		else if(  Variable=="XS"  )
		{
			if(pObj->m_vectGraphic.size() && pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nWidth > 0)
			{
				int ClickX = X - pObj->m_rPosition.X;
				Output+=StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nWidth * ClickX / pObj->m_rPosition.Width );
			}
		}
		else if(  Variable=="YS"  )
		{
			if(pObj->m_vectGraphic.size() && pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nHeight > 0)
			{
				int ClickY = Y - pObj->m_rPosition.Y;
				Output += StringUtils::itos( pObj->m_vectGraphic[pObj->m_iCurrentFrame]->m_nHeight * ClickY / pObj->m_rPosition.Height );
			}
		}
		else if(  Variable=="S"  )
			Output += m_pScreenHistory_Current->GetObj()->m_ObjectID;
		else if(  Variable=="SID"  )
			Output += StringUtils::itos(m_pScreenHistory_Current->PK_Screen());
		else if(  Variable=="O"  )
			Output += pObj->m_ObjectID;
		else if( Variable.length()>1 && Variable[0]=='T'  )
		{
			int PK_Text=atoi(Variable.substr(1).c_str());
			Output += m_mapTextString[PK_Text];
		}
		else if( Variable.length()>1 && Variable[0]=='V'  )
		{
			long VirtDev=atoi(Variable.substr(1).c_str());
			TranslateVirtualDevice(VirtDev,VirtDev);
			Output += StringUtils::itos(VirtDev);
		}
		else if ( Variable.length() > 2 && Variable[0] =='D' && Variable[1] == 'D') 
		{
			int PK_DeviceData=atoi(Variable.substr(2).c_str());
			Output += m_pData->m_mapParameters_Find(PK_DeviceData);
		}
		else if( Variable.length()>1 && Variable[0]=='D'  )
		{
			int PK_Variable=atoi(Variable.substr(1).c_str());
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
				int PK_Device = atoi(m_mapVariable[PK_Variable].c_str());
			vm.Release(  );

			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find( PK_Device );
			if( pDevice )
			{
				Output += pDevice->m_sDescription;
			}
		}
		else if( Variable.length()>2 && Variable[0]=='B'  )
		{
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
			Output += m_mapBoundIconValues[Variable.substr(1)];
			vm.Release();
		}
		else
			PK_Variable = atoi( Variable.c_str(  ) );

		if(  PK_Variable>0  )
		{
			PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			string sVarValue = m_mapVariable[PK_Variable];
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS," Variable: %d has value %s",PK_Variable,sVarValue.c_str());
#endif

			vm.Release();

			Output += SubstituteVariables(sVarValue, NULL, 0, 0);
		}
		pos=foundend+2;
	}
	Output+=Input.substr( pos );

	return Output;
}

//------------------------------------------------------------------------
bool Orbiter::DoComparisson( int PK_Variable,  string oComparisson,  string oValue )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
		string Variable = m_mapVariable[PK_Variable];
	vm.Release(  );

	if(  Variable.length(  )==0 && oValue.length(  )==0 && oComparisson=="="  )
		return true;

	if(  Variable.length(  )==0 && oValue.length(  )==0  )
		return false;

	if(  ( Variable.length(  )==0 || oValue.length(  )==0 ) && ( oComparisson=="<>" || oComparisson=="<>" )  )
		return true;

	if(  Variable.length(  )==0 || oValue.length(  )==0  )
		return false;

	if(  oComparisson=="="  )
		return Variable == oValue;

	else if(  oComparisson=="<"  )
		return atoi( Variable.c_str(  ) ) < atoi( oValue.c_str(  ) );

	else if(  oComparisson==">"  )
		return atoi( Variable.c_str(  ) ) > atoi( oValue.c_str(  ) );

	else if(  oComparisson=="!=" || oComparisson=="<>"  )
		return Variable != oValue;

	return false;

}

//------------------------------------------------------------------------
void Orbiter::FindDGArrows( DesignObj_Orbiter *pObj, DesignObj_DataGrid *pDGObj )
{
	DesignObjZoneList::iterator iZone;
	for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
	{
		DesignObjCommandList::iterator iCommand;
		for( iCommand=( *iZone )->m_Commands.begin(  );iCommand!=( *iZone )->m_Commands.end(  );++iCommand )
		{
			DesignObjCommand *pCommand = ( *iCommand );

			if(  pCommand->m_PK_Command == COMMAND_Scroll_Grid_CONST  )
			{
				if(  pCommand->m_ParameterList[COMMANDPARAMETER_Level_CONST]=="-1"  )
					continue; // This is just a move to the top/bottom
				// If this Command points to this grid or to nothing,  it's a match
				if(  atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str(  ) ) == pDGObj->m_iBaseObjectID ||
					atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_DesignObj_CONST].c_str(  ) ) == 0  )
				{
					int PK_Direction = atoi( pCommand->m_ParameterList[COMMANDPARAMETER_PK_Direction_CONST].c_str(  ) );
					if(  PK_Direction == DIRECTION_Up_CONST  )
						pDGObj->m_pObjUp=pObj;
					else if(  PK_Direction == DIRECTION_Down_CONST  )
						pDGObj->m_pObjDown=pObj;
					else if(  PK_Direction == DIRECTION_Left_CONST  )
						pDGObj->m_pObjLeft=pObj;
					else if(  PK_Direction == DIRECTION_Right_CONST  )
						pDGObj->m_pObjRight=pObj;
				}
			}
		}
	}
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		FindDGArrows( ( DesignObj_Orbiter * )( *iHao ), pDGObj );
	}
}

void *MaintThread(void *p)
{
	bMaintThreadIsRunning = true;
	Orbiter* pOrbiter = (Orbiter *)p;

	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, pOrbiter->m_MaintThreadMutex);// Keep this locked to protect the map
	while(NULL != pOrbiter && !pOrbiter->m_bQuit_get())
	{
		if(pOrbiter->m_mapPendingCallbacks.size() == 0)
		{
			//nothing to process. let's sleep...
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread empty");
#endif
			cm.CondWait(); // This will unlock the mutex and lock it on awakening
		}
		else
		{
            //LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "MaintThread(%p) : processing callback", p);
			// We've got stuff to check out
			PendingCallBackInfo *pCallBackInfoGood = NULL;
			timespec ts_NextCallBack,ts_now;
			ts_NextCallBack.tv_sec=0;
			gettimeofday(&ts_now,NULL);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MaintThread woke up with %d callbacks",(int) pOrbiter->m_mapPendingCallbacks.size());
#endif

			//let's choose the one which must be processed first
			for(map<int,PendingCallBackInfo *>::iterator it=pOrbiter->m_mapPendingCallbacks.begin();it!=pOrbiter->m_mapPendingCallbacks.end();)
			{
				PendingCallBackInfo *pCallBackInfo = (*it).second;
				if( pCallBackInfo->m_bStop )
				{
#ifdef DEBUG
//					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread Going to delete member %X::%X %d:%d",pCallBackInfo->m_fnCallBack,(int) pCallBackInfo->m_abstime.tv_sec,(int) pCallBackInfo->m_abstime.tv_nsec);
#endif
					pOrbiter->m_mapPendingCallbacks.erase( it++ );  // This is dead anyway
					delete pCallBackInfo;
					continue;
				}
				else if(pCallBackInfo->m_abstime <= ts_now)
				{
#ifdef DEBUG
//					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread Going to execute member %X::%X %d:%d",pCallBackInfo->m_fnCallBack,(int) pCallBackInfo->m_abstime.tv_sec,(int) pCallBackInfo->m_abstime.tv_nsec);
#endif
					pOrbiter->m_mapPendingCallbacks.erase( it );
					pCallBackInfoGood = pCallBackInfo;
					break;  // We got one to execute now
				}
				else
				{
#ifdef DEBUG
//					LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread Going to wait for member %X::%X %d:%d",pCallBackInfo->m_fnCallBack,(int) pCallBackInfo->m_abstime.tv_sec,(int) pCallBackInfo->m_abstime.tv_nsec);
#endif
					if( ts_NextCallBack.tv_sec==0 || pCallBackInfo->m_abstime<ts_NextCallBack )
						ts_NextCallBack = pCallBackInfo->m_abstime;  // This is the next one to call
				}
				it++;
			}

			if( pCallBackInfoGood )
			{
				cm.Release(); // Don't keep the mutex locked while executing
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread calling member %X::%X %d:%d",pCallBackInfoGood->m_fnCallBack,(int) pCallBackInfoGood->m_abstime.tv_sec,(int) pCallBackInfoGood->m_abstime.tv_nsec);
#endif
				CALL_MEMBER_FN(*(pCallBackInfoGood->m_pOrbiter), pCallBackInfoGood->m_fnCallBack)(pCallBackInfoGood->m_pData);
				cm.Relock();
				delete pCallBackInfoGood;
			}
			else if( ts_NextCallBack.tv_sec!=0 ) // Should be the case
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MaintThread sleep ts_NextCallBack %d:%d",(int) ts_NextCallBack.tv_sec,(int) ts_NextCallBack.tv_nsec);
#endif
				cm.TimedCondWait(ts_NextCallBack);
			}
		}
	}

	bMaintThreadIsRunning = false;
	return NULL;
}

//------------------------------------------------------------------------
void Orbiter::CallMaintenanceInMiliseconds( time_t milliseconds, OrbiterCallBack fnCallBack,
										   void *data, ePurgeExisting e_PurgeExisting,
										   bool bPurgeTaskWhenScreenIsChanged/*= true*/
										   )
{
	PLUTO_SAFETY_LOCK_ERRORSONLY( cm, m_MaintThreadMutex );

	if( e_PurgeExisting!=pe_NO )
	{
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack==fnCallBack &&
				(e_PurgeExisting==pe_ALL || pCallBackInfo->m_pData==data) )
			{
				pCallBackInfo->m_bStop=true;
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CallMaintenanceInMiliseconds Stopping member %X::%X %d:%d",pCallBackInfo->m_fnCallBack,(int) pCallBackInfo->m_abstime.tv_sec,(int) pCallBackInfo->m_abstime.tv_nsec);
#endif
			}
		}
	}

	PendingCallBackInfo *pCallBackInfo = new PendingCallBackInfo(m_nCallbackCounter++, bPurgeTaskWhenScreenIsChanged );

	gettimeofday(&pCallBackInfo->m_abstime,NULL);
	pCallBackInfo->m_abstime += static_cast<long>(milliseconds);

	pCallBackInfo->m_fnCallBack=fnCallBack;
	pCallBackInfo->m_pData=data;
	pCallBackInfo->m_pOrbiter=this;

#ifdef DEBUG
//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CallMaintenanceInMiliseconds %X::%X %d ms started with mapPendingCallbacks size: %d time %d:%d",
//		fnCallBack,(int) milliseconds,m_mapPendingCallbacks.size(),(int) pCallBackInfo->m_abstime.tv_sec,(int) pCallBackInfo->m_abstime.tv_nsec);
#endif

	m_mapPendingCallbacks[pCallBackInfo->m_nCallbackID]=pCallBackInfo;


	cm.Release();

	pthread_cond_broadcast(&m_MaintThreadCond);
}

/*
MAINTENANCE CALL BACKS

When you need something to be done at a particular time,  like popup buttons,  call 'CallMaintenance',  passing in the time you need the function
called,  and any data you want the function to get,  as a void *.  The call backs must be void Function( void *data ) syntax.
*/

void Orbiter::DeselectObjects( void *data )
{
	NeedToRender render( this, "Deselect Objects" );  // Redraw anything that was changed by this command

	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
		DesignObj_Orbiter *pObj = ( DesignObj_Orbiter * ) data;

	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()!=pObj->m_pObj_Screen /*&& !GetActivePopup()*/)
		return; // We must have since changed screens

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Deselecting %s object, state '%s'", pObj->m_ObjectID.c_str(), pObj->m_GraphicToDisplay==GRAPHIC_SELECTED ? "'selected'" : "'normal'");
#endif

	if(pObj->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		pObj->m_GraphicToDisplay_set("o4",GRAPHIC_NORMAL);
		m_pOrbiterRenderer->RenderObjectAsync(pObj);

		// Remove it from the list
		for( vector<class DesignObj_Orbiter *>::iterator it=m_vectObjs_Selected.begin(  );it!=m_vectObjs_Selected.end(  );++it )
		{
			DesignObj_Orbiter *pObj_Sel = *it;
			if(  pObj == pObj_Sel  )//m_vectObjs_Selected[s]  )
			{
				m_vectObjs_Selected.erase( it );
				break;
			}
		}
	}
	else
	{
		//int foo=2;
	}
}

void Orbiter::StartCachingGrid( void *iData )
{
	DesignObj_DataGrid *pObj = (DesignObj_DataGrid *) iData;
	if( pObj && pObj->m_bOnScreen )
		pObj->CacheGrid();
}

void Orbiter::GetVideoFrame( void *data )
{
	// We'll render just one of the frames and use m_iLastVideoObjectRendered so we can do them in order
	// This is because if we have multiple devices on the screen, and each may block for several seconds while
	// waiting for the video device to provide the frame, we don't want to block the maint thread for too long
	PLUTO_SAFETY_LOCK( vm, m_ScreenMutex )
	if( m_vectObjs_VideoOnScreen.size()==0 )
	{
		m_iLastVideoObjectRendered=0;
		return;
	}

	if( m_iLastVideoObjectRendered>=int(m_vectObjs_VideoOnScreen.size()) || m_iLastVideoObjectRendered<0 )
		m_iLastVideoObjectRendered=0;

	DesignObj_Orbiter *pObj = m_vectObjs_VideoOnScreen[m_iLastVideoObjectRendered++];
	vm.Release();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::GetVideoFrame() The target object is: %s", pObj->m_ObjectID.c_str());
#endif
	if( pObj->m_bOnScreen && !pObj->IsHidden(  )  )
	{
		char *pBuffer=NULL; int Size=0;  string sFormat = "jpg";
		int PK_Device = atoi( pObj->GetParameterValue( DESIGNOBJPARAMETER_Source_CONST ).c_str(  ) );
		if( PK_Device )
		{
			DCE::CMD_Get_Video_Frame CMD_Get_Video_Frame( m_dwPK_Device, PK_Device, "0",  0 /* stream */, pObj->m_rPosition.Width, pObj->m_rPosition.Height, &pBuffer, &Size, &sFormat );
			if(  SendCommand( CMD_Get_Video_Frame ) && pBuffer  )
			{
				CMD_Update_Object_Image( pObj->m_ObjectID,  sFormat ,  pBuffer,  Size, "0" );
				delete [] pBuffer; //we don't need it anymore
			}
		}
	}

	CallMaintenanceInMiliseconds( m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
	NeedToRender render( this, "GetVideoFrame" );  // Block this at the end so other renderings may occur in the meantime
}

/*

COMMANDS TO IMPLEMENT

*/

//<-dceag-c1-b->

	/** @brief COMMAND: #1 - Capture Keyboard To Variable */
	/** As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object. */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text Object */
		/** @param #4 PK_Variable */
			/** The variable in which to store the input */
		/** @param #8 On/Off */
			/** If 0, this stops capturing */
		/** @param #14 Type */
			/** 1=normal, 2=pin, 3=phone number */
		/** @param #24 Reset */
			/** if true, the next keypress will clear the variable and start new */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */
		/** @param #55 DataGrid */
			/** If 1, we'll scroll the data grid too when typing keys. */

void Orbiter::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
//<-dceag-c1-e->
{
	BuildCaptureKeyboardParams(
		sPK_DesignObj,
		iPK_Variable,
		sOnOff,
		sType,
		sReset,
		iPK_Text,
		bDataGrid
		);
}

//<-dceag-c2-b->

	/** @brief COMMAND: #2 - Orbiter Beep */
	/** Make the orbiter beep */

void Orbiter::CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage)
//<-dceag-c2-e->
{
	cout << "Need to implement command #2 - Orbiter Beep" << endl;
}

//<-dceag-c3-b->

	/** @brief COMMAND: #3 - Display On/Off */
	/** Turn the display on or off */
		/** @param #8 On/Off */
			/** 0=Off, 1=On */
		/** @param #125 Already processed */
			/** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

void Orbiter::CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage)
//<-dceag-c3-e->
{
	m_bDisplayOn = sOnOff=="1";
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Display_OnOff Setting monitor display on/off: %s %d m_bDisplayOn: %d",sOnOff.c_str(),(int) bAlready_processed,(int) m_bDisplayOn);
#endif
	if( bAlready_processed )
		return;

	if( m_bDisplayOn )
	{
		DCE::CMD_On CMD_On(m_dwPK_Device,m_dwPK_Device,0,"");
		CMD_On.m_pMessage->m_mapParameters[COMMANDPARAMETER_Force_CONST]="1";
		SendCommand(CMD_On);
	}
	else
	{
		DCE::CMD_Off CMD_Off(m_dwPK_Device,m_dwPK_Device,0);
		CMD_Off.m_pMessage->m_mapParameters[COMMANDPARAMETER_Force_CONST]="1";
		SendCommand(CMD_Off);
	}

}

//<-dceag-c4-b->

	/** @brief COMMAND: #4 - Go back */
	/** Make the orbiter go back to the prior screen, like the back button in a web browser */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen */
		/** @param #21 Force */
			/** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

void Orbiter::CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c4-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Go_back currently: %s (%d) cs: %s",m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),m_pScreenHistory_Current->PK_Screen(),sPK_DesignObj_CurrentScreen.c_str());

	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

#ifdef DEBUG
	DumpScreenHistory();
#endif

	int PK_Screen_OnScreen = m_pScreenHistory_Current->PK_Screen();
	if( PK_Screen_OnScreen==SCREEN_Main_CONST && m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() != m_pDesignObj_Orbiter_ScreenSaveMenu )
	{
		// If the user hits the menu button from the main menu, go to the screen saver, unless there's a computing app
		if( m_PK_Screen_ActiveApp_OSD )
		{
			NeedToRender render( this, "m_PK_Screen_ActiveApp_OSD" );
#ifdef ENABLE_MOUSE_BEHAVIOR
			if( m_pMouseBehavior )
				m_pMouseBehavior->Clear(false);
#endif
			CMD_Goto_Screen("",m_PK_Screen_ActiveApp_OSD);
		}
		else
		{
			if(m_sSkin == AUDIO_STATION_SKIN) //TODO: change to audio station
			{
				CMD_Goto_Screen("",SCREEN_Main_CONST);	
			}
			else
			{
				StartScreenSaver(true); 
			}
		}
		return;
	}

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	ScreenHistory *pScreenHistory=NULL;

	if(m_pScreenHistory_Current && m_pScreenHistory_Current->GoBack())
		pScreenHistory = m_pScreenHistory_Current;
	else
	{
		while(  m_listScreenHistory.size(  )  )
		{
			// The last screen we went to
			pScreenHistory = m_listScreenHistory.back(  );

			if(m_pScreenHistory_Current->PK_Screen() == pScreenHistory->PK_Screen())
			{
				m_listScreenHistory.pop_back();
				continue;
			}

			if(!pScreenHistory->GoBack())
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Removing from screen history screen %d", pScreenHistory->PK_Screen());
#endif

				// We now took the prior screen off teh list
				m_listScreenHistory.pop_back(  );
				if( pScreenHistory->CantGoBack() && sForce!="1"  )
					continue;
			}
			else if(pScreenHistory->HistoryEmpty())
				m_listScreenHistory.pop_back();

			break;   // We got one to go back to
		}
	}
	vm.Release();

#ifdef DEBUG
	DumpScreenHistory();
#endif

	// We've got a screen to go back to
	if(  pScreenHistory  )
	{
		ScreenHistory::m_bAddToHistory = false;
		ScreenHistory *pOldScreenHistory = m_pScreenHistory_Current;
		m_pScreenHistory_Current = pScreenHistory;
 
		if(!pOldScreenHistory->m_bPutObjectsOffScreen)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "ObjectOffScreen : screen id %d, obj id %s",
				pOldScreenHistory->PK_Screen(), pOldScreenHistory->GetObj()->m_ObjectID.c_str());

			m_pOrbiterRenderer->ObjectOffScreen(pOldScreenHistory->GetObj());
		}
		m_pScreenHistory_Current->m_bPutObjectsOffScreen = true;

		if(m_sSkin == AUDIO_STATION_SKIN && m_pScreenHistory_Current->PK_Screen() == SCREEN_Main_CONST) //TODO: change to audio station
		{
			GotoMainMenu();
			return;
		}

		string sCMD_Result;
		if( m_mapPK_Screen_GoBackToScreen.find(m_pScreenHistory_Current->PK_Screen())!=m_mapPK_Screen_GoBackToScreen.end() )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Go_back to %d is a gobacktoscreen",m_pScreenHistory_Current->PK_Screen());
#endif
			Message *pMessageTemp=NULL;
			if( pScreenHistory->m_mapParameters.size() )
			{
				pMessageTemp = new Message();
				for(map<long, string>::iterator it = pScreenHistory->m_mapParameters.begin(); it != pScreenHistory->m_mapParameters.end(); it++)
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Go_back to %d with %d=%s",pScreenHistory->PK_Screen(),it->first,it->second.c_str());
#endif
					pMessageTemp->m_mapParameters[it->first]=it->second;
				}

			}

			CMD_Goto_Screen(m_pScreenHistory_Current->ScreenID(),m_pScreenHistory_Current->PK_Screen(),interuptAlways,false,false,sCMD_Result,pMessageTemp);
			delete pMessageTemp;
		}
		else
		{
			PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
			m_pContextToBeRestored = pScreenHistory;  // Don't do this if we're going to the screen
			m_pScreenHistory_Pivot = m_pScreenHistory_Current;
			m_pScreenHistory_New = m_pScreenHistory_Pivot;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Go_back to %d is not a gobacktoscreen",m_pScreenHistory_Pivot->PK_Screen());
			cm.Release();

			CMD_Goto_DesignObj(0, pScreenHistory->GetObj()->m_ObjectID, pScreenHistory->ScreenID(),
				"", false, pScreenHistory->GetObj()->m_bCantGoBack);
		}
	}
	else
		GotoMainMenu();

#ifdef DEBUG
	DumpScreenHistory();
#endif

	//ScreenHistory::m_bAddToHistory = true;
}

//<-dceag-c5-b->

	/** @brief COMMAND: #5 - Goto DesignObj */
	/** Goto a specific design obj */
		/** @param #2 PK_Device */
			/** For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset */
		/** @param #3 PK_DesignObj */
			/** The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen # */
		/** @param #10 ID */
			/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use). */
		/** @param #22 Store Variables */
			/** If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen */
		/** @param #114 Cant Go Back */
			/** If true, then when this screen goes away the user won't be able to return to it -- it will be skipped over, unless Go Back with Force=1 is used.  This prevents layers of popup screens. */

void Orbiter::CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c5-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Goto_DesignObj: %s with cant go back %d",sPK_DesignObj.c_str(),(int) bCant_Go_Back);

	PLUTO_SAFETY_LOCK( sm, m_ScreenMutex );  // Nothing more can happen

	bool bIsRemote=false;
	// We're using a popup remote, so just go to the main menu
	if( sPK_DesignObj=="<%=NP_R%>" )
	{
		if( m_iPK_DesignObj_Remote_Popup>0 && m_sObj_Popop_RemoteControl.size() )
			sPK_DesignObj = "<%=M%>";
		else
		{
			string sDestScreen = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
			if( m_mapDesignObj.find(atoi(sDestScreen.c_str())) != m_mapDesignObj.end() )
				sPK_DesignObj = StringUtils::itos(m_mapDesignObj[atoi(sDestScreen.c_str())]);
			bIsRemote=true;
		}
	}

	else if( sPK_DesignObj=="**NOMEDIA**" )
	{
		sPK_DesignObj = StringUtils::itos(DESIGNOBJ_mnuPopupMessage_CONST);
		CMD_Set_Text(sPK_DesignObj, m_mapTextString[TEXT_No_Media_CONST], TEXT_STATUS_CONST);
	}

	m_pOrbiterRenderer->HidePopups(NULL);

	if( !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	if(sPK_DesignObj == "")
		sPK_DesignObj = StringUtils::ltos(m_pDesignObj_Orbiter_MainMenu->m_iBaseObjectID);

	string sDestScreen = SubstituteVariables( sPK_DesignObj, NULL, 0, 0 );
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

	DesignObj_Orbiter *pObj_New=m_ScreenMap_Find( sDestScreen );
	if(  !pObj_New  )
		pObj_New=m_ScreenMap_Find( StringUtils::itos(atoi(sDestScreen.c_str())) + ".0.0" );

	if ( !pObj_New )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter::CMD_Goto_DesignObj Could not find design object matching this specification: %s", sDestScreen.c_str());
		CMD_Go_back("","1");
		return;
	}

	if( pObj_New->m_iBaseObjectID == atoi(m_sMainMenu.c_str()) )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Goto_DesignObj Forcing go to the main menu");
#endif
		pObj_New=m_ScreenMap_Find( m_sMainMenu );
	}

	if(  !pObj_New  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "cannot find screen %s in goto", sPK_DesignObj.c_str(  ) );
		// Just go to the main menu since maybe we're stuck!
		if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()==m_pDesignObj_Orbiter_MainMenu )
			return;
		if( !m_pScreenHistory_Current )
			return;

		pObj_New = m_pScreenHistory_Current->GetObj();
	}

	//if the simulator is running, we won't go to pluto admin screen
	if(Simulator::GetInstance()->IsRunning() && (pObj_New->m_iBaseObjectID==DESIGNOBJ_mnuAdvancedOptions_CONST || pObj_New->m_iBaseObjectID==DESIGNOBJ_mnuPower_CONST || pObj_New->m_iBaseObjectID==DESIGNOBJ_PVR_FS_CONST) )
		return;

	if( bIsRemote || pObj_New->m_iBaseObjectID==m_iPK_Screen_Remote || pObj_New->m_iBaseObjectID==m_iPK_Screen_RemoteOSD )
		pObj_New->m_bIsARemoteControl=true;

	if(NULL != m_pScreenHistory_Current)
	{
		if(!m_pScreenHistory_Current->m_bPutObjectsOffScreen)
		{
			ScreenHistory *pScreenHistory = m_pScreenHistory_Current;
			vm.Release();
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::CMD_Goto_DesignObj ObjectOffScreen : screen id %d, obj id %s",
				m_pScreenHistory_Current->PK_Screen(), m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());

			m_pOrbiterRenderer->ObjectOffScreen(m_pScreenHistory_Current->GetObj());
			vm.Relock();
			if( pScreenHistory!=m_pScreenHistory_Current )
			{
				// ObjectOffScreen was blocking the variable mutex too long with bind to media remote.  So I release the mutex,
				// but as a sanity check and confirming that while it was released nothing modified m_pScreenHistory_Current
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter::CMD_Goto_DesignObj m_pScreenHistory_Current while doing the object offscreen");
				return;
			}
		}

		m_pScreenHistory_Current->m_bPutObjectsOffScreen = false;
	}

	bool bNewScreenHistoryItem = true;
	ScreenHistory *pScreenHistory_New = NULL;
	if(NULL != m_pScreenHistory_Pivot)
	{
		pScreenHistory_New = m_pScreenHistory_Pivot; //just create by CMD_Goto_Screen
		m_pScreenHistory_Pivot = NULL;
	}
	else
	{
		bNewScreenHistoryItem = false;
		pScreenHistory_New = m_pScreenHistory_Current; //another designobj for the same screen
		pScreenHistory_New->AddToHistory();
	}

	m_pScreenHistory_New = pScreenHistory_New;

	string sLastObject = pScreenHistory_New->GetObj() ? pScreenHistory_New->GetObj()->m_ObjectID : "";
	bool bLastCantGoBack = pScreenHistory_New->CantGoBack();

	pScreenHistory_New->SetObj(pObj_New);

	//we have to restore the context. nothing to save, it's all there
	if(NULL == m_pContextToBeRestored)
	{
        if(sID != "" && pScreenHistory_New->ScreenID() == "")
			pScreenHistory_New->ScreenID(sID);
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have all in screen history item. Nothing new to save.");



#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Goto_DesignObj: %s pScreenHistory_New->m_bCantGoBack %d bCant_Go_Back %d pObj_New->m_bCantGoBack %d",
		sPK_DesignObj.c_str(),(int) pScreenHistory_New->CantGoBack(),(int) bCant_Go_Back,(int) pObj_New->m_bCantGoBack);
#endif

	if(bNewScreenHistoryItem)
		pScreenHistory_New->CantGoBack(bCant_Go_Back ? true : pObj_New->m_bCantGoBack);

	if( pObj_New==m_pDesignObj_Orbiter_ScreenSaveMenu )
	{
		pObj_New->m_bCantGoBack=true;
		ScreenHistory::m_bAddToHistory = false;
		StartScreenSaver(false);
	}
	else if( ScreenHistory::m_bAddToHistory && m_pScreenHistory_Current != pScreenHistory_New)
	{
		if(pScreenHistory_New->GetObj() && !bLastCantGoBack && sLastObject != pObj_New->m_ObjectID && sLastObject != "")
			pScreenHistory_New->AddToHistory();
		else if(m_pScreenHistory_Current && m_pScreenHistory_Current != pScreenHistory_New && m_pScreenHistory_Current->GetObj() && !m_pScreenHistory_Current->GetObj()->m_bCantGoBack)
			m_pScreenHistory_Current->AddToHistory();
	}

#ifdef DEBUG
	DumpScreenHistory();
#endif

	vm.Release(  );
	NeedToRender::NeedToChangeScreens( this, pScreenHistory_New);
}

//<-dceag-c6-b->

	/** @brief COMMAND: #6 - Show Object */
	/** Change an objects visible state. */
		/** @param #3 PK_DesignObj */
			/** The object to show or hide */
		/** @param #4 PK_Variable */
			/** The variable to use in the comparisson.  See Comparisson Value. */
		/** @param #6 Comparisson Operator */
			/** A type of comparisson: =  <  <>  !=  > */
		/** @param #7 Comparisson Value */
			/** If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true */
		/** @param #8 On/Off */
			/** 1=show object, 0=hide object */

void Orbiter::CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c6-e->
{
	if(  iPK_Variable ||
		sComparisson_Operator.length(  )==0 ||
		sComparisson_Value.length(  )==0 ||
		DoComparisson( iPK_Variable, sComparisson_Operator, sComparisson_Value )  )
	{
		sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
		DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
		if(  !pObj  )
		{
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str(  ) );
			return;
		}
		bool bShow=sOnOff!="0";

		if(  sOnOff.substr( 0, 1 )=="!"  )
		{
			string s = SubstituteVariables( sOnOff.substr( 1 ), NULL, 0, 0 );
			if(  s=="" || s=="-1"  )
				bShow=true;
			else
				bShow=false;
		}

		//PLUTO_SAFETY_LOCK_ERRORSONLY( vm, m_VariableMutex );
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(  m_pObj_Highlighted==pObj && !bShow  )
			m_pOrbiterRenderer->UnHighlightObject();

		pObj->m_bHidden = !bShow;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Object: %s visible: %d", pObj->m_ObjectID.c_str(), (int) !pObj->m_bHidden );
#endif
		m_pOrbiterRenderer->RenderObjectAsync(pObj);// Redraw even if the object was already in this state,  because maybe we're hiding this and something that
		if( pObj->m_bHidden && pObj->m_pParentObject )
			m_pOrbiterRenderer->RenderObjectAsync((DesignObj_Orbiter *) pObj->m_pParentObject);
	}
#ifdef DEBUG
	else
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Ignoring show object for %s",sPK_DesignObj.c_str() );
#endif
}

//<-dceag-c7-b->

	/** @brief COMMAND: #7 - Terminate Orbiter */
	/** Causes the Orbiter application to exit */

void Orbiter::CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage)
//<-dceag-c7-e->
{
	m_bQuit_set(true);
}

//<-dceag-c8-b->

	/** @brief COMMAND: #8 - Remove Screen From History */
	/** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
		/** @param #10 ID */
			/** If specified, only screens that match this ID will be removed */
		/** @param #159 PK_Screen */
			/** The screen to remove */

void Orbiter::CMD_Remove_Screen_From_History(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c8-e->
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Remove_Screen_From_History %d - %s size: %d",iPK_Screen,sID.c_str(),(int) m_listScreenHistory.size());
	DumpScreenHistory();
#endif

	for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();)
	{
		ScreenHistory *pScreenHistory = *it;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Comparing %s - %s",pScreenHistory->GetObj()->m_ObjectID.c_str(),pScreenHistory->ScreenID().c_str());
#endif
		if( pScreenHistory->PK_Screen() == iPK_Screen && (sID.length()==0 || sID == pScreenHistory->ScreenID()) )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Remove_Screen_From_History deleting %d - %s",iPK_Screen,pScreenHistory->ScreenID().c_str());
#endif
			delete (*it);
			it = m_listScreenHistory.erase( it );
		}
		else
			++it;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"After CMD_Remove_Screen_From_History %d - %s size: %d",iPK_Screen,sID.c_str(),(int) m_listScreenHistory.size());
	DumpScreenHistory();
#endif

	if( m_pScreenHistory_Current && m_pScreenHistory_Current->PK_Screen() == iPK_Screen && (sID.length()==0 || sID==m_pScreenHistory_Current->ScreenID()) )
	{
		// If we're on a screen and not in the middle of changing to another screen, remove the one we're on
		if( NeedToRender::m_pScreenHistory_get()!=NULL )
		{
			// We're changing screens anyway.  No need to do a go back.  Just don't add it to the history
			ScreenHistory::m_bAddToHistory = false;
		}
		else 
		{
			m_pScreenHistory_Current->PurgeHistory(); // User wants to delete this entire screen instance.  If we leave this populated, we may just go to a prior design obj in this screen
			vm.Release();
			CMD_Go_back("","");
		}
	}
}

//<-dceag-c9-b->

	/** @brief COMMAND: #9 - Scroll Grid */
	/** Scroll a datagrid */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to scroll the grid */

void Orbiter::CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c9-e->
{
	Scroll_Grid(sRelative_Level,sPK_DesignObj,iPK_Direction);
}

// Do this is a separate function that will return false when it can't scroll anymore
bool Orbiter::Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::Scroll_Grid %s/%s/%d",sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction);
#endif
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	PLUTO_SAFETY_LOCK( dng, m_DatagridMutex );  // Lock them in the same order as render screen
	// todo 2.0?    NeedsUpdate( 2 ); // Moving grids is slow; take care of an animation if necessary

	DesignObj_Orbiter *pObj=NULL;
	//m_pObj_Highlighted_set(NULL);

	int LoopNum=-1;
	if ( sPK_DesignObj.empty(  ) )
		LoopNum=0;
	else
		pObj = FindObject( sPK_DesignObj );

	DesignObj_DataGrid *pObj_Datagrid = NULL;
	if( pObj && pObj->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		pObj_Datagrid = (DesignObj_DataGrid *) pObj;
	else if( pObj )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to scroll a non-grid %s",sPK_DesignObj.c_str());
		return false;
	}

	bool bResult=false;
	while( LoopNum<( int ) m_vectObjs_GridsOnScreen.size(  ) )
	{
		if(  LoopNum!=-1  )
		{
			pObj_Datagrid = m_vectObjs_GridsOnScreen[LoopNum++];
		}
		if (pObj_Datagrid)
		{
			dng.Release();
            if( pObj_Datagrid->Scroll_Grid(sRelative_Level, iPK_Direction) )
				bResult=true;
			dng.Relock();
		}
		if(  LoopNum==-1  )
			break;
	}
	return bResult;
}

//<-dceag-c10-b->

	/** @brief COMMAND: #10 - Move Highlight */
	/** Move the current highlight pointer */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to move the highlight */

void Orbiter::CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c10-e->
{
	cout << "Need to implement command #10 - Move Highlight" << endl;
	cout << "Parm #1 - Relative_Level=" << sRelative_Level << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #30 - PK_Direction=" << iPK_Direction << endl;
}

///<-dceag-c13-b->

	/** @brief COMMAND: #13 - Play Sound */
	/** Plays a sound file on the orbiter */
		/** @param #19 Data */
			/** A pointer to a block of memory representing the sound file to play */
		/** @param #20 Format */
			/** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

void Orbiter::CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c13-e->
{
	cout << "Need to implement command #13 - Play Sound" << endl;
	cout << "Parm #19 - Data  ( data value )" << endl;
	cout << "Parm #20 - Format=" << sFormat << endl;
}

//<-dceag-c14-b->

	/** @brief COMMAND: #14 - Refresh */
	/** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

void Orbiter::CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c14-e->
{
	//forward it to renderer
	m_pOrbiterRenderer->HandleRefreshCommand(sDataGrid_ID);
}

//<-dceag-c15-b->

	/** @brief COMMAND: #15 - Regen Screen */
	/** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

void Orbiter::CMD_Regen_Screen(string &sCMD_Result,Message *pMessage)
//<-dceag-c15-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Regen_Screen");
#endif
//	m_pMessage from screen history with parms

	NeedToRender render( this, "Regen Screen" );
	if( m_pScreenHistory_Current )
	{
		Message message; // Create a temporary message to store the parameters
		for( map<long, string>::iterator it=m_pScreenHistory_Current->m_mapParameters.begin();
			it!=m_pScreenHistory_Current->m_mapParameters.end();++it )
				message.m_mapParameters[ it->first ] = it->second;

		string sResult;
		CMD_Goto_Screen(m_pScreenHistory_Current->ScreenID(),m_pScreenHistory_Current->PK_Screen(),interuptAlways,false,false,sResult,&message);
	}
}

//<-dceag-c16-b->

	/** @brief COMMAND: #16 - Requires Special Handling */
	/** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

void Orbiter::CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage)
//<-dceag-c16-e->
{
	cout << "Need to implement command #16 - Requires Special Handling" << endl;
}

//<-dceag-c17-b->

	/** @brief COMMAND: #17 - Seek Data Grid */
	/** Causes a datagrid to seek to a particular position. */
		/** @param #9 Text */
			/** If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one. */
		/** @param #11 Position X */
			/** The column to jump to.  If Text is not blank, the column to search. */
		/** @param #12 Position Y */
			/** The row to jump to.  Ignored if Text is not blank */
		/** @param #15 DataGrid ID */
			/** The datagrid to scroll.  If not specified, the first visible one will be used */

void Orbiter::CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c17-e->
{
	DesignObj_DataGrid *pObj_Datagrid=NULL;
	if( sDataGrid_ID.length() )
		pObj_Datagrid = m_mapObjs_AllGrids[sDataGrid_ID];
	else if( m_vectObjs_GridsOnScreen.size() )
		pObj_Datagrid = m_vectObjs_GridsOnScreen[0];

	if( !pObj_Datagrid )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot seek to unknown grid");
	else
	{
		pObj_Datagrid->m_sSeek = sText;
		CMD_Refresh(pObj_Datagrid->m_sGridID);
	}
}

//<-dceag-c18-b->

	/** @brief COMMAND: #18 - Set Graphic To Display */
	/** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
		/** @param #3 PK_DesignObj */
			/** The object to set */
		/** @param #10 ID */
			/** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

void Orbiter::CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c18-e->
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
	if(  !pObj  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot find object in CMD_Show_Object: %s", sPK_DesignObj.c_str(  ) );
		return;
	}

	cm.Release();
	PurgeCallBack(&Orbiter::DeselectObjects,(void *) pObj);
	cm.Relock();

	pObj->m_GraphicToDisplay_set("o5",atoi( sID.c_str(  ) ));
	if(  pObj->m_GraphicToDisplay==GRAPHIC_SELECTED  )
	{
		m_pOrbiterRenderer->SaveBackgroundForDeselect(pObj, pObj->m_pPopupPoint);  // Whether it's automatically unselected,  or done by selecting another object,  we should hold onto this
		m_vectObjs_Selected.push_back( pObj );
	}

	m_pOrbiterRenderer->RenderObjectAsync(pObj);
}

//<-dceag-c20-b->

	/** @brief COMMAND: #20 - Set Object Parameter */
	/** changes one of the object's DesignObjParameters */
		/** @param #3 PK_DesignObj */
			/** The object to change */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #27 PK_DesignObjParameter */
			/** The parameter */

void Orbiter::CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage)
//<-dceag-c20-e->
{
	cout << "Need to implement command #20 - Set Object Parameter" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #27 - PK_DesignObjParameter=" << iPK_DesignObjParameter << endl;
}

//<-dceag-c21-b->

	/** @brief COMMAND: #21 - Set Object Position */
	/** Change an objects's position on the screen */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c21-e->
{
	cout << "Need to implement command #21 - Set Object Position" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c22-b->

	/** @brief COMMAND: #22 - Set Object Size */
	/** Change an object's size */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c22-e->
{
	cout << "Need to implement command #22 - Set Object Size" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c23-b->

	/** @brief COMMAND: #23 - Set Pos Rel To Parent */
	/** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

void Orbiter::CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c23-e->
{
	cout << "Need to implement command #23 - Set Pos Rel To Parent" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c24-b->

	/** @brief COMMAND: #24 - Set Size Rel To Parent */
	/** Change an object's size, relative to it's parent object */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/** The percentage of the parent object's width.  100=the parent's full width. */
		/** @param #12 Position Y */
			/** The percentage of the parent object's height.  100=the parent's full height. */

void Orbiter::CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c24-e->
{
	cout << "Need to implement command #24 - Set Size Rel To Parent" << endl;
	cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c25-b->

	/** @brief COMMAND: #25 - Set Text */
	/** Change the text within a text object on the fly */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #9 Text */
			/** The text to assign */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */

void Orbiter::CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage)
//<-dceag-c25-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Set_Text %s/%d=%s",sPK_DesignObj.c_str(),iPK_Text,sText.c_str());
#endif
	DesignObj_Orbiter *pObj=( m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj() : NULL );
	if(  sPK_DesignObj.length(  )  )
		pObj = FindObject( sPK_DesignObj );

	if( !pObj )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "SetText: cannot find text object %d", iPK_Text );
		return;
	}

	DesignObjText *pText = FindText( pObj, iPK_Text );
	if(  !pText  )
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "SetText: cannot find object %s text %d", sPK_DesignObj.c_str(  ), iPK_Text );
	else
	{
		pText->m_sText = sText;
	}

	if(pObj->m_bOnScreen)
		m_pOrbiterRenderer->RenderObjectAsync(pObj);
}

//<-dceag-c26-b->

	/** @brief COMMAND: #26 - Set Bound Icon */
	/** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
		/** @param #5 Value To Assign */
			/** The value corresponding to an alt graphic. */
		/** @param #9 Text */
			/** Text for the binding */
		/** @param #14 Type */
			/** The type of bound icon. */

void Orbiter::CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c26-e->
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	m_mapBoundIconValues[sType]=sText;
	vm.Release();

	DesignObj_DataList *pDesignObj_DataList = m_mapObj_Bound_Find(sType);
	if( !pDesignObj_DataList )
		return;  // Happens when an Orbiter doesn't have the screen with the bound icon, like the mobile phone

	int iValue = atoi(sValue_To_Assign.c_str());
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	for(DesignObj_DataList::iterator it=pDesignObj_DataList->begin();it!=pDesignObj_DataList->end();++it)
	{
		DesignObj_Orbiter *pObj = (DesignObj_Orbiter *) *it;
		pObj->m_bDontResetState=true;
		pObj->m_GraphicToDisplay_set("o6",iValue, true, true);
		if(pObj->m_bOnScreen)
			m_pOrbiterRenderer->RenderObjectAsync(pObj);
	}
}

//<-dceag-c27-b->

	/** @brief COMMAND: #27 - Set Variable */
	/** Change the value of a variable */
		/** @param #4 PK_Variable */
			/** The variable to change */
		/** @param #5 Value To Assign */
			/** The value to assign */

void Orbiter::CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c27-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Variable: %d set to %s",iPK_Variable,sValue_To_Assign.c_str());
#endif
	
#ifndef ARMV4I
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
#endif	

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnVariableChanged, VariableCallBackData, (iPK_Variable, sValue_To_Assign))
	ExecuteScreenHandlerCallback(cbOnVariableChanged);

	m_mapVariable[iPK_Variable] = sValue_To_Assign;

	//hook for capture keyboard
	if(m_bCaptureKeyboard_OnOff && m_iCaptureKeyboard_PK_Variable == iPK_Variable)
		m_sCaptureKeyboard_InternalBuffer = sValue_To_Assign;
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Orbiter::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	Orbiter::Event orbiterEvent;
	orbiterEvent.data.button.m_iPK_Button = atoi(sPK_Button.c_str());
	orbiterEvent.data.button.m_bSimulated = true;

	orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
	ProcessEvent(orbiterEvent);

	orbiterEvent.type = Orbiter::Event::BUTTON_UP;
	ProcessEvent(orbiterEvent);
}

//<-dceag-c29-b->

	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simulates a mouse click or touch on the indicated x & y coordinates */
		/** @param #11 Position X */
			/** position X */
		/** @param #12 Position Y */
			/** position Y */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
	cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
	cout << "Parm #11 - Position_X=" << iPosition_X << endl;
	cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c30-b->

	/** @brief COMMAND: #30 - Stop Sound */
	/** If a sound file is being played, it will be stopped. */

void Orbiter::CMD_Stop_Sound(string &sCMD_Result,Message *pMessage)
//<-dceag-c30-e->
{
	cout << "Need to implement command #30 - Stop Sound" << endl;
}

//<-dceag-c31-b->

	/** @brief COMMAND: #31 - Store Variables */
	/** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

void Orbiter::CMD_Store_Variables(string &sCMD_Result,Message *pMessage)
//<-dceag-c31-e->
{
	if( !m_pScreenHistory_Current )
		return;

	for(VariableMap::iterator it=m_mapVariable.begin();it!=m_mapVariable.end();++it)
		CMD_Set_Variable((*it).first, (*it).second);
}

//<-dceag-c32-b->

	/** @brief COMMAND: #32 - Update Object Image */
	/** Changes the background image within an object */
		/** @param #3 PK_DesignObj */
			/** The object in which to put the bitmap */
		/** @param #14 Type */
			/** 1=bmp, 2=jpg, 3=png */
		/** @param #19 Data */
			/** The contents of the bitmap, like reading from the file into a memory buffer */
		/** @param #23 Disable Aspect Lock */
			/** If 1, the image will be stretched to fit the object */

void Orbiter::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Update_Object_Image obj %s size %d",sPK_DesignObj.c_str(),iData_Size);
	m_pOrbiterRenderer->UpdateObjectImage(sPK_DesignObj, sType, pData, iData_Size, sDisable_Aspect_Lock);
}

void Orbiter::DelayedSelectObject( void *data )
{
	DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) data;
	if( TestCurrentScreen(pDelayedSelectObjectInfo->m_sPK_DesignObj_CurrentScreen) )
		SelectedObject(pDelayedSelectObjectInfo->m_pObj,smCommand);

	delete pDelayedSelectObjectInfo;
}

//<-dceag-c66-b->

	/** @brief COMMAND: #66 - Select Object */
	/** The same as clicking on an object. */
		/** @param #3 PK_DesignObj */
			/** The object to select. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** Will only happen if this is the current screen. */
		/** @param #102 Time */
			/** If specified, rather than happening immediately it will happen in x seconds. */

void Orbiter::CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c66-e->
{
	DesignObj_Orbiter *pDesignObj_Orbiter = FindObject( sPK_DesignObj );
	if(  !pDesignObj_Orbiter  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got select object %s,  but can't find it", sPK_DesignObj.c_str(  ) );
		return;
	}

	if( sTime.length()==0 && !TestCurrentScreen(sPK_DesignObj_CurrentScreen) )
		return;

	if( sTime.length()!=0 && atoi(sTime.c_str()) )
	{
		timespec ts;
		gettimeofday(&ts,NULL);
		long TimeInMS = atoi(sTime.c_str()) * 1000;
		ts += TimeInMS;
		// We don't want to purge all select objects.  Only those pending for the same object.  So we'll have to do this by hand
		DelayedSelectObjectInfo *pDelayedSelectObjectInfo = new DelayedSelectObjectInfo(pDesignObj_Orbiter,sPK_DesignObj_CurrentScreen,ts);

		PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
		for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
		{
			PendingCallBackInfo *pCallBackInfo = (*it).second;
			if( pCallBackInfo->m_fnCallBack== &Orbiter::DelayedSelectObject )
			{
				DelayedSelectObjectInfo *pDelayedSelectObjectInfo = (DelayedSelectObjectInfo *) pCallBackInfo->m_pData;
				if( pDelayedSelectObjectInfo->m_pObj==pDesignObj_Orbiter )
					pCallBackInfo->m_bStop=true;
			}
		}
		cm.Release();

		CallMaintenanceInMiliseconds( TimeInMS, &Orbiter::DelayedSelectObject, pDelayedSelectObjectInfo, pe_NO );
	}
	else
		SelectedObject( pDesignObj_Orbiter, smCommand );
}

//<-dceag-c72-b->

	/** @brief COMMAND: #72 - Surrender to OS */
	/** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
		/** @param #8 On/Off */
			/** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
		/** @param #54 Fully release keyboard */
			/** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys.  True also releases the mouse. */
		/** @param #225 Always */
			/** If true, the mouse will always be ignored */

void Orbiter::CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways,string &sCMD_Result,Message *pMessage)
//<-dceag-c72-e->
{
	m_bYieldScreen = ( sOnOff=="1" );
	m_bYieldInput = bFully_release_keyboard;
	m_bIgnoreMouse = bAlways;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Surrender_to_OS: grab keyboard/mouse %d, ignore keyboard %d, ignore mouse %d", !m_bYieldScreen, m_bYieldInput, m_bIgnoreMouse);
}

bool Orbiter::IsYieldScreen() const
{
	return m_bYieldScreen;
}

bool Orbiter::IsYieldInput() const
{
	return m_bYieldInput;
}

DesignObjText *Orbiter::FindText( DesignObj_Orbiter *pObj,  int iPK_Text )
{
	if( !pObj )
		return NULL;

	DesignObjText *pText = NULL;
	vector<DesignObjText *>::iterator it_text;
	for( it_text = pObj->m_vectDesignObjText.begin(  );
		it_text != pObj->m_vectDesignObjText.end(  );
		it_text++
		)
	{
		DesignObjText *pText_Temp = *it_text;

		if( pText_Temp->m_PK_Text == iPK_Text )
		{
			pText = pText_Temp;
			break;
		}
	}

	if(  pText  )
		return pText;

	// Otherwise,  search all our children
	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		pText = FindText( ( DesignObj_Orbiter * )*iHao, iPK_Text );
		if(  pText  )
			return pText;
	}


	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find text object: %d", iPK_Text);
	return NULL;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid )
{
	//on/off parameter
	m_bCaptureKeyboard_OnOff = sOnOff == "0" ? false : true;

	//normal/pin paramter
	m_iCaptureKeyboard_EditType = atoi(sType.c_str());

	//reset parameter
	m_bCaptureKeyboard_Reset = sReset != "0";

	//data grid
	m_bCaptureKeyboard_DataGrid = bDataGrid;

	//set var
	m_iCaptureKeyboard_PK_Variable = iPK_Variable;

	if( iPK_Variable && m_bCaptureKeyboard_Reset )
		CMD_Set_Variable(iPK_Variable, "");

	//find the parent object
	DesignObj_Orbiter *pObj;
	if(  sPK_DesignObj.length(  ) > 0  )
		pObj = FindObject( sPK_DesignObj );
	else
		if( NULL == m_pScreenHistory_Current )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: m_pScreenHistory_Current is null!" );
#endif
			return true;
		}
		else
			pObj = m_pScreenHistory_Current->GetObj();


	if(!pObj)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: pObj is null!" );
#endif
		return true;
	}

	//find the text object
	m_pCaptureKeyboard_Text = FindText( pObj,  iPK_Text );

	if( iPK_Variable && NULL != m_pCaptureKeyboard_Text && m_bCaptureKeyboard_Reset )
	{
		PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
			m_pCaptureKeyboard_Text->m_sText = m_mapVariable[iPK_Variable];
	}

	//build the text string
	m_sCaptureKeyboard_Text = "";
	if( NULL == m_pCaptureKeyboard_Text )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_CONTROLLER,  "CMD_Capture_Keyboard_To_Variable: text object is null!" );
#endif
		return true;
	}
	else
	{
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.X ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Y ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Width ) + ",";
		m_sCaptureKeyboard_Text += StringUtils::itos( m_pCaptureKeyboard_Text->m_rPosition.Height );
	}

	return true;
}

void Orbiter::GetButtonsInObject( DesignObj_Data *pObj, vector<int> &vectButtons )
{
	if( pObj->m_iPK_Button )
		vectButtons.push_back(pObj->m_iPK_Button);

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
		GetButtonsInObject(*iHao,vectButtons);
}

//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_DeleteLastChar(  )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	string OldValue = m_sCaptureKeyboard_InternalBuffer;
	string NewValue = "";

	if( OldValue.size(  ) )
		NewValue = OldValue.substr( 0,  OldValue.size(  ) - 1 );

	m_sCaptureKeyboard_InternalBuffer = NewValue;
	CMD_Set_Variable(m_iCaptureKeyboard_PK_Variable, NewValue);
	vm.Release();

	ExecuteScreenHandlerCallback(cbCapturedKeyboardBufferChanged);

	if(m_iCaptureKeyboard_EditType == 2)
		std::fill_n(NewValue.begin(), NewValue.length(), '*');

	if( NULL != m_pCaptureKeyboard_Text )
	{
		m_pCaptureKeyboard_Text->m_sText = NewValue;
		if( NULL != m_pCaptureKeyboard_Text )
		{
			m_pOrbiterRenderer->RenderTextAsync(m_pCaptureKeyboard_Text);
		}
		else
			m_bRerenderScreen = true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool Orbiter::CaptureKeyboard_EditText_AppendChar( char ch )
{
	if((!m_bShiftDown && m_bCapsLock) || (m_bShiftDown && !m_bCapsLock))
	{
		if(ch >= 'A' && ch <= 'Z')
			ch = ch - 'A' + 'a';
		else if(ch >= 'a' && ch <= 'z')
			ch = ch - 'a' + 'A';
	}

	if(m_bShiftDown)
	{
		char cOldCh = ch;
		switch(cOldCh)
		{
		case '0': ch = ')'; break;
		case '1': ch = '!'; break;
		case '2': ch = '@'; break;
		case '3': ch = '#'; break;
		case '4': ch = '$'; break;
		case '5': ch = '%'; break;
		case '6': ch = '^'; break;
		case '7': ch = '&'; break;
		case '8': ch = '*'; break;
		case '9': ch = '('; break;
		case '-': ch = '_'; break;
		case '+': ch = '='; break;
		case '=': ch = '+'; break;
		case ';': ch = ':'; break;
		case '\'': ch = '"'; break;
		}
	}

	PLUTO_SAFETY_LOCK( vm, m_VariableMutex )
	string OldValue = m_sCaptureKeyboard_InternalBuffer;
	string NewValue = OldValue + ch;
	CMD_Set_Variable(m_iCaptureKeyboard_PK_Variable, NewValue);
	vm.Release();

	m_sCaptureKeyboard_InternalBuffer = NewValue;

	if(m_iCaptureKeyboard_EditType == 2)
		std::fill_n(NewValue.begin(), NewValue.length(), '*');

	ExecuteScreenHandlerCallback(cbCapturedKeyboardBufferChanged);

	if( NULL != m_pCaptureKeyboard_Text )
	{
		m_pCaptureKeyboard_Text->m_sText = NewValue;

		if( NULL != m_pCaptureKeyboard_Text )
			m_pOrbiterRenderer->RenderTextAsync(m_pCaptureKeyboard_Text);
		else
			m_bRerenderScreen = true;
	}

	return false;
}

//<-dceag-sample-b->!
//<-dceag-c85-b->

	/** @brief COMMAND: #85 - Reset Highlight */
	/** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */
		/** @param #3 PK_DesignObj */
			/** If specified, this object will be highlighted.  Otherwise the first detected one. */

void Orbiter::CMD_Reset_Highlight(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c85-e->
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if( sPK_DesignObj.c_str() )
	{
		DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj);
		if( pObj )
		{
			if( m_pObj_Highlighted && m_pObj_Highlighted!=pObj )
			{
				m_pOrbiterRenderer->UnHighlightObject();
				ExecuteCommandsInList( &m_pObj_Highlighted->m_Action_UnhighlightList, m_pObj_Highlighted, smHighlight, 0, 0 );
			}
			m_pObj_Highlighted_set(pObj);
			return;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::CMD_Reset_Highlight Cannot find object %s",sPK_DesignObj.c_str());
	}

	m_pOrbiterRenderer->HighlightFirstObject(  );
}

ReceivedMessageResult Orbiter::ReceivedMessage( class Message *pMessageOriginal )
{
	while( !m_pScreenHistory_Current && !m_bQuit_get())
		Sleep(100);

	if(m_bQuit_get())
		return rmr_NotProcessed;

	string strMessage = string("ReceivedMessage: ") + StringUtils::itos(pMessageOriginal->m_dwID);

	if(m_pScreenHandler->m_mapCallBackData_Find(cbMessageIntercepted) && ScreenHandlerMsgInterceptor(this, pMessageOriginal, NULL, NULL))
		return rmr_Processed;

	NeedToRender render( this, strMessage.c_str() );  // Redraw anything that was changed by this command

	ReceivedMessageResult Result = Orbiter_Command::ReceivedMessage( pMessageOriginal );
	return Result;
}

void Orbiter::CaptureKeyboard_UpdateVariableAndText( int iVariable,  string sVariable ) //for capture keyboard cmd
{
	CMD_Set_Variable(iVariable, sVariable);
}

//<-dceag-c58-b->

	/** @brief COMMAND: #58 - Set Current User */
	/** Sets what user is currently using the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

void Orbiter::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
	m_dwPK_Users=iPK_Users;
	DCE::CMD_Set_Current_User CMD_Set_Current_User( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_dwPK_Users );
	SendCommand( CMD_Set_Current_User );
}

void Orbiter::FireEntAreaRoomCommands()
{
	if( !m_pLocationInfo )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Orbiter::FireEntAreaRoomCommands no location");
		return;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"set current location to ea %d room %d",
		m_pLocationInfo->PK_EntertainArea,m_pLocationInfo->PK_Room);
#endif
	DCE::CMD_Set_Entertainment_Area CMD_Set_Entertainment_Area( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(m_pLocationInfo->PK_EntertainArea) );
	SendCommand( CMD_Set_Entertainment_Area );
	DCE::CMD_Set_Current_Room CMD_Set_Current_Room( m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, m_pLocationInfo->PK_Room );
	SendCommand( CMD_Set_Current_Room );
}

//<-dceag-c88-b->

	/** @brief COMMAND: #88 - Set Current Location */
	/** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
		/** @param #65 LocationID */
			/** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

void Orbiter::CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage)
//<-dceag-c88-e->
{
	if(  iLocationID<0 || iLocationID>=( int ) m_dequeLocation.size(  )  )
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got set current location with invalid location" );
	else
	{
		LocationInfo *pLocationInfo = m_dequeLocation[iLocationID];
		m_pLocationInfo = pLocationInfo;
		if( !m_bStartingUp )
		{
			FireEntAreaRoomCommands();
		}
#ifdef DEBUG
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Not firing set current location because we're starting up");
#endif

		m_sMainMenu = StringUtils::itos( atoi( m_sMainMenu.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";

		string sMainMenu = StringUtils::itos( atoi( m_pDesignObj_Orbiter_MainMenu->m_ObjectID.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";
		DesignObj_Orbiter *pObj = FindObject( sMainMenu );
		if( !pObj )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to change to non-existant main menu: %s",sMainMenu.c_str());
		else
			m_pDesignObj_Orbiter_MainMenu = pObj;

		string sSleepingMenu;
		if(m_pDesignObj_Orbiter_SleepingMenu)
			sSleepingMenu = StringUtils::itos( atoi( m_pDesignObj_Orbiter_SleepingMenu->m_ObjectID.c_str(  ) ) ) + "." + StringUtils::itos( iLocationID ) + ".0";

		pObj = FindObject( sSleepingMenu );
		if( !pObj )
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to change to non-existant sleeping menu: %s",sSleepingMenu.c_str());
		else
			m_pDesignObj_Orbiter_SleepingMenu = pObj;

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Set location %d ea %d rm %d (%s) now %s",iLocationID,pLocationInfo->PK_EntertainArea,
			pLocationInfo->PK_Room,pLocationInfo->Description.c_str(),m_sMainMenu.c_str());
#endif
	}
}

void Orbiter::RenderFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen, PlutoPoint point)
{
	int Type = atoi(pDesignObj_Orbiter->GetParameterValue(DESIGNOBJPARAMETER_Type_CONST).c_str());
	int selectedFloorplan = atoi(m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
	bool showThisFloorplan = pDesignObj_Orbiter->m_iPage == selectedFloorplan;
	if (!showThisFloorplan)
	{
	        return;
	}

	string sResult;
	DCE::CMD_Get_Current_Floorplan CMD_Get_Current_Floorplan(m_dwPK_Device, m_dwPK_Device_OrbiterPlugIn, StringUtils::itos(pDesignObj_Orbiter->m_iPage), Type, &sResult);
	SendCommand(CMD_Get_Current_Floorplan);

	FloorplanObjectVectorMap *pFloorplanObjectVectorMap = m_mapFloorplanObjectVector_Find(pDesignObj_Orbiter->m_iPage);
	FloorplanObjectVector *fpObjVector = NULL;

	//  FloodFill(150,241, RGB(0,0,0), RGB(255,255,0));

	if( pFloorplanObjectVectorMap )
		fpObjVector = (*pFloorplanObjectVectorMap)[Type];

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Get current floorplan %d page %d returned %s",Type, pDesignObj_Orbiter->m_iPage, sResult.c_str());
#endif

	if( fpObjVector )
	{
		m_pOrbiterRenderer->PrepareFloorplan(pDesignObj_Orbiter);

		string::size_type pos = 0;
		FloorplanObjectVector::iterator it;
		for(it = fpObjVector->begin(); it != fpObjVector->end(); ++it)
		{
			FloorplanObject *fpObj = *it;

			// Color is the color to fill the icon with, Description is the status which
			// appears at the bottom of the floorplan when the item is selected, OSD is
			// the text will be put into any text object within the icon (like the temperature
			// next to a thermastat, and PK_DesignObj_Toolbar is the toolbar to activate
			// when the object is selected.
			int Color = atoi(StringUtils::Tokenize(sResult,"|",pos).c_str());
			fpObj->Status = StringUtils::Tokenize(sResult,"|",pos);
			string OSD = StringUtils::Tokenize(sResult,"|",pos);
			fpObj->m_dwPK_DesignObj_Toolbar = atoi(StringUtils::Tokenize(sResult,"|",pos).c_str());

			if( fpObj->pObj )
			{
				if( fpObj->pObj->m_rBackgroundPosition.X+fpObj->pObj->m_rBackgroundPosition.Width>m_Width ||
					fpObj->pObj->m_rBackgroundPosition.Y+fpObj->pObj->m_rBackgroundPosition.Height>m_Height )
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Floorplan object %s is out of bounds ",fpObj->DeviceDescription.c_str());
					continue;
				}

				if( Color )
				{
					PlutoColor Magenta(255,102,255);

#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Replacing obj %s %d,%d-%d,%d with color %d",
						fpObj->pObj->m_ObjectID.c_str(),fpObj->pObj->m_rBackgroundPosition.X,fpObj->pObj->m_rBackgroundPosition.Y,fpObj->pObj->m_rBackgroundPosition.Width,
						fpObj->pObj->m_rBackgroundPosition.Height, (int) Color);
#endif
					m_pOrbiterRenderer->ReplaceColorInRectangle(point.X + fpObj->pObj->m_rBackgroundPosition.X, point.Y + fpObj->pObj->m_rBackgroundPosition.Y,fpObj->pObj->m_rBackgroundPosition.Width,
						fpObj->pObj->m_rBackgroundPosition.Height, Magenta, Color, fpObj->pObj);

				}

				if( fpObj->pObj->m_vectDesignObjText.size()==1 )
				{
					DesignObjText *pDesignObjText = fpObj->pObj->m_vectDesignObjText[0];
					pDesignObjText->m_sText = OSD;
				}
				//FloodFill(fpObj->pObj->m_rectBackground.X + fpObj->FillX,fpObj->pObj->m_rectBackground.Y + fpObj->FillY, RGB(0,0,0), Color);
			}
		}

		m_pOrbiterRenderer->UpdateFloorplan(pDesignObj_Orbiter);
	}
}

ScreenHistory *NeedToRender::m_pScreenHistory=NULL;
//////////////////////////////////////////////////////////////////////////
//
// class NeedToRender
//
//////////////////////////////////////////////////////////////////////////
NeedToRender::NeedToRender( class Orbiter *pOrbiter, const char *pWhere )
{
	m_pWhere = pWhere;
	m_pOrbiter = pOrbiter;
	g_iDontRender++;
}
//-----------------------------------------------------------------------------------------------------
NeedToRender::~NeedToRender()
{
	g_iDontRender--;
	if ( g_iDontRender == 0 )
	{
		if( m_pScreenHistory )
		{
			class ScreenHistory *pScreenHistory = m_pScreenHistory;
			m_pScreenHistory=NULL;

			m_pOrbiter->NeedToChangeScreens(pScreenHistory);
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "NeedToRender::~NeedToRender() calling redraw for: %s", m_pWhere);
#endif
		if(!m_pOrbiter->m_bQuit_get())
			m_pOrbiter->Renderer()->RedrawObjects();
	}
}
//-----------------------------------------------------------------------------------------------------
ScreenHistory *NeedToRender::m_pScreenHistory_get() 
{ 
	return m_pScreenHistory; 
}
//-----------------------------------------------------------------------------------------------------
void NeedToRender::NeedToChangeScreens( Orbiter *pOrbiter, ScreenHistory *pScreenHistory/*, bool bAddToHistory*/ )
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Need to change screens logged to %s",pScreenHistory->GetObj()->m_ObjectID.c_str());
#endif
	m_pScreenHistory = pScreenHistory;

	//purge pending tasks, if need it.  Do it here, so that things will happen in the right order.
	//ie: CMD_GotoScreen, CMD_SelectObject. The real need to change screens won't get called until
	//after all messages are processed, wiping out CMD_SelectObject
	//Orbiter *pOrbiter = m_pOrbiter(Orbiter *) g_pCommand_Impl;
	if( pOrbiter )
	{
		PLUTO_SAFETY_LOCK( pm, pOrbiter->m_MaintThreadMutex );
		map<int,PendingCallBackInfo *>::iterator itCallBackInfo;
		for(itCallBackInfo = pOrbiter->m_mapPendingCallbacks.begin(); itCallBackInfo != pOrbiter->m_mapPendingCallbacks.end();)
		{
			PendingCallBackInfo *pCallBackInfo = (*itCallBackInfo).second;
			if(pCallBackInfo->m_bPurgeTaskWhenScreenIsChanged)
			{
				pOrbiter->m_mapPendingCallbacks.erase(itCallBackInfo++);
				delete pCallBackInfo;
			}
			else
				itCallBackInfo++;
		}
		pm.Release();

		// We also want to handle reseting the selected states here for the same reason,
		// Otherwise we may have a show_object message come in and get processed afterward
		pOrbiter->ResetState(pScreenHistory->GetObj());
	}
}
//////////////////////////////////////////////////////////////////////////


//<-dceag-c242-b->

	/** @brief COMMAND: #242 - Set Now Playing */
	/** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
		/** @param #3 PK_DesignObj */
			/** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote, guide */
		/** @param #5 Value To Assign */
			/** The description of the media */
		/** @param #9 Text */
			/** The description of the current section (ie chapter in a dvd, etc.) */
		/** @param #29 PK_MediaType */
			/** The type of media playing */
		/** @param #41 StreamID */
			/** The ID of the current stream */
		/** @param #48 Value */
			/** The track number or position in the playlist */
		/** @param #50 Name */
			/** The name of the window for the application to remain in the foreground */
		/** @param #103 List PK Device */
			/** (comma-delimited list): The current source device, video device, the current audio device, 1/0 if audio device supports discrete volume */
		/** @param #120 Retransmit */
			/** If true, it will re-request the plist (current playlist) grid */

void Orbiter::CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit,string &sCMD_Result,Message *pMessage)
//<-dceag-c242-e->
{
	if(m_bQuit_get())
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received 'Set Now Playing' with window %s", sName.c_str());
#endif

    PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pMouseBehavior && m_iStreamID!=iStreamID )  // If we're changing media
		m_pMouseBehavior->SetMediaInfo("","","","","",false);

	if(NULL != m_pMouseBehavior && iStreamID == 0)
		m_pMouseBehavior->MediaStopped();
#endif
//

	m_iStreamID=iStreamID;
	m_iPK_MediaType=iPK_MediaType;
	m_sNowPlaying_Window = sName;
	m_sNowPlaying = SubstituteVariables(sValue_To_Assign, NULL, 0, 0);
	m_sNowPlaying_Section = SubstituteVariables(sText, NULL, 0, 0);
	string::size_type pos=0;
	m_dwPK_Device_NowPlaying = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
	m_dwPK_Device_NowPlaying_Video = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
	m_dwPK_Device_NowPlaying_Audio = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
	m_dwPK_Device_CaptureCard = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
	m_bPK_Device_NowPlaying_Audio_DiscreteVolume = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str())==1;
	m_bContainsVideo = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str())==1;
	m_bUsingLiveAVPath = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str())==1;
	CMD_Set_Variable(VARIABLE_Track_or_Playlist_Positio_CONST, StringUtils::itos(iValue));
	pos=0;
	m_iPK_Screen_Remote=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_DesignObj_Remote_Popup=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());  // ON UI2 the leftmost popup menu on the main menu
	m_iPK_Screen_FileList=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_Screen_RemoteOSD=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_Screen_OSD_Speed=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());
	m_iPK_Screen_OSD_Track=atoi(StringUtils::Tokenize(sPK_DesignObj,",",pos).c_str());

	m_sNowPlaying_TimeShort="";
	m_sNowPlaying_TimeLong="";

	map<int,string>::iterator it;
	if( (it = m_mapPK_MediaType_Description.find(m_iPK_MediaType)) != m_mapPK_MediaType_Description.end() )
		m_sNowPlaying_MediaType = it->second;
	else
		m_sNowPlaying_MediaType = "";

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Set_Now_Playing value: %s sdesign: %s m_dwPK_Device_NowPlaying %d m_dwPK_Device_NowPlaying_Video %d m_dwPK_Device_NowPlaying_Audio %d m_dwPK_Device_CaptureCard %d m_bPK_Device_NowPlaying_Audio_DiscreteVolume %d m_bContainsVideo %d m_bUsingLiveAVPath %d m_iPK_Screen_Remote %d m_iPK_DesignObj_Remote_Popup %d m_iPK_Screen_FileList %d m_iPK_Screen_RemoteOSD %d m_iPK_Screen_OSD_Speed %d m_iPK_Screen_OSD_Track %d",
		sValue_To_Assign.c_str(),sPK_DesignObj.c_str(),
		m_dwPK_Device_NowPlaying, m_dwPK_Device_NowPlaying_Video, m_dwPK_Device_NowPlaying_Audio, m_dwPK_Device_CaptureCard, (int) m_bPK_Device_NowPlaying_Audio_DiscreteVolume, 
		(int) m_bContainsVideo, (int) m_bUsingLiveAVPath, m_iPK_Screen_Remote, m_iPK_DesignObj_Remote_Popup, m_iPK_Screen_FileList, m_iPK_Screen_RemoteOSD, m_iPK_Screen_OSD_Speed, m_iPK_Screen_OSD_Track);
#endif

	if( bRetransmit )
	{
		vector<DesignObj_DataGrid*>::iterator it;
		for(it = m_vectObjs_GridsOnScreen.begin(); it != m_vectObjs_GridsOnScreen.end(); ++it)
		{
			DesignObj_DataGrid* pDesignObj = *it;
			if(pDesignObj->m_sGridID.size()>6 && pDesignObj->m_bOnScreen && (pDesignObj->m_sGridID.substr(0,4)=="plns" || pDesignObj->m_sGridID.substr(0,5)=="plist" || pDesignObj->m_sGridID.substr(0,6)=="tracks") )
			{
				PLUTO_SAFETY_LOCK( cm, m_DatagridMutex );

				int nOldHightlightedRow = pDesignObj->m_iHighlightedRow;
				int nOldHightlightedCol = pDesignObj->m_iHighlightedColumn;
				int nGridCurRow = pDesignObj->m_GridCurRow;
				int nGridCurCol = pDesignObj->m_GridCurCol;
				int nOldSelectedIndex = atoi(m_mapVariable[atoi(pDesignObj->sSelVariable.c_str())].c_str());
				int nStartingRow = pDesignObj->DataGridTable_Get() ? pDesignObj->DataGridTable_Get()->m_StartingRow : 0;

				InitializeGrid(pDesignObj);
				pDesignObj->Flush();
				m_pOrbiterRenderer->RenderObjectAsync(pDesignObj);

				if(nOldHightlightedRow + nStartingRow == nOldSelectedIndex + !!nStartingRow)
				{
					//we changed the current playing song,
					//so we can restore the datagrid context (selected item, highlighted item)
					pDesignObj->m_iHighlightedRow = nOldHightlightedRow;
					pDesignObj->m_iHighlightedColumn = nOldHightlightedCol;
					pDesignObj->m_GridCurRow = nGridCurRow;
					pDesignObj->m_GridCurCol = nGridCurCol;
				}
			}
		}

	}
	DesignObj_Orbiter *pObj_Popop_RemoteControl=NULL;
	if( m_sObj_Popop_RemoteControl.size() && (pObj_Popop_RemoteControl=FindObject(m_sObj_Popop_RemoteControl)) && m_iPK_DesignObj_Remote_Popup>0 )
		CMD_Show_Popup(StringUtils::itos(m_iPK_DesignObj_Remote_Popup),m_Popop_RemoteControl_X,m_Popop_RemoteControl_Y,pObj_Popop_RemoteControl->m_ObjectID,"remote",false,false);
	else if( pObj_Popop_RemoteControl && m_iPK_DesignObj_Remote_Popup==0 )
		CMD_Remove_Popup(pObj_Popop_RemoteControl->m_ObjectID,"remote");
	else
	{
		if( m_pObj_NowPlayingOnScreen )
			m_pOrbiterRenderer->RenderObjectAsync(m_pObj_NowPlayingOnScreen);
		if( m_pObj_NowPlaying_MT_OnScreen )
			m_pOrbiterRenderer->RenderObjectAsync(m_pObj_NowPlaying_MT_OnScreen);
		if( m_pObj_NowPlaying_Section_OnScreen )
			m_pOrbiterRenderer->RenderObjectAsync(m_pObj_NowPlaying_Section_OnScreen);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Set_Now_Playing device %d video %d",m_dwPK_Device_NowPlaying,(int) m_bContainsVideo);
	if( m_dwPK_Device_NowPlaying )
	{
	        if ( m_bContainsVideo && UsesUIVersion2() )
			StopScreenSaver();

		if( m_sActiveApplication_Window.empty()==false && m_pScreenHistory_Current
			&& (m_pScreenHistory_Current->PK_Screen()==m_PK_Screen_ActiveApp_OSD || m_pScreenHistory_Current->PK_Screen()==m_PK_Screen_ActiveApp_Remote) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::CMD_Set_Now_Playing media is playing, but so is the application %s",m_sActiveApplication_Window.c_str());
			CMD_Activate_Window(m_sActiveApplication_Window);
		}
		else 
		{
		        // No application active, we can show the NowPlayingWindow or start screen saver if audio only
		        if (m_bContainsVideo)
			        CMD_Activate_Window(m_sNowPlaying_Window);
			else
			{
			        HideWindow(m_sNowPlaying_Window);
				if( UsesUIVersion2() )
				        StartScreenSaver(false);  // Don't go to the menu, just start the app in the background
			}
		}        
	}
	else
	{
	        // device is not playing, make sure NowPlayingWindow is hidden and screen saver is started
		HideWindow(m_sNowPlaying_Window);

		if( UsesUIVersion2() )
			StartScreenSaver(false);  // Don't go to the menu, just start the app in the background
	}

	if(m_bReportTimeCode && !m_bUpdateTimeCodeLoopRunning )
	{
		m_bUpdateTimeCodeLoopRunning=true;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode starting thread");

		if(m_TimeCodeID)
		{
			pthread_join(m_TimeCodeID, NULL);
			m_TimeCodeID = 0;
		}
		
		pthread_create(&m_TimeCodeID, NULL, UpdateTimeCodeThread, (void*)this);
	}

	if(m_sSkin == AUDIO_STATION_SKIN)
	{
		NeedToRender render(this, "Now playing - audio station");  
		CMD_Show_Object(TOSTRING(DESIGNOBJ_mnuMenuAudioServer_CONST) ".0.0." TOSTRING(DESIGNOBJ_butGotoRemote_CONST), 0, "", "", 
			m_sNowPlaying.empty() ? "0" : "1"); 
	}
}

bool Orbiter::TestCurrentScreen(string &sPK_DesignObj_CurrentScreen)
{
	//PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	if( sPK_DesignObj_CurrentScreen.length(  ) && ( !m_pScreenHistory_Current || atoi( sPK_DesignObj_CurrentScreen.c_str(  ) )!=m_pScreenHistory_Current->GetObj()->m_iBaseObjectID )  ) // It should be at the beginning
	{
		// Be sure it's not a -1 telling us to be at the main menu
		if(  sPK_DesignObj_CurrentScreen=="-1" )
		{
			if( !m_pScreenHistory_Current || !m_pScreenHistory_Current->GetObj() )
				return false;

			if( m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_MainMenu->m_iBaseObjectID ||
				(m_pDesignObj_Orbiter_SleepingMenu && m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_SleepingMenu->m_iBaseObjectID) ||
				(m_pDesignObj_Orbiter_ScreenSaveMenu && m_pScreenHistory_Current->GetObj()->m_iBaseObjectID==m_pDesignObj_Orbiter_ScreenSaveMenu->m_iBaseObjectID) )
				return true;
			else
				return false;
		}
		return false;
	}
	return true;
}

void Orbiter::ContinuousRefresh( void *data )
{
	ContinuousRefreshInfo *pContinuousRefreshInfo = (ContinuousRefreshInfo *) data;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ContinuousRefresh %d %p %p %s %s",
		pContinuousRefreshInfo->m_iInterval,m_pScreenHistory_Current->GetObj(),pContinuousRefreshInfo->m_pObj,
		m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str(),
		(pContinuousRefreshInfo->m_pObj ? pContinuousRefreshInfo->m_pObj->m_ObjectID.c_str() : "NULL"));
#endif

	if( m_pScreenHistory_Current->GetObj()!=pContinuousRefreshInfo->m_pObj )
		delete pContinuousRefreshInfo;
	else
	{
		CMD_Refresh("*");
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ContinuousRefresh restarting");
#endif
		CallMaintenanceInMiliseconds( pContinuousRefreshInfo->m_iInterval * 1000, &Orbiter::ContinuousRefresh, pContinuousRefreshInfo, pe_ALL );
	}
}

//<-dceag-c238-b->

	/** @brief COMMAND: #238 - Continuous Refresh */
	/** Continuously refresh the current page.  Used when the page contains constantly changing data. */
		/** @param #102 Time */
			/** The interval time in seconds */

void Orbiter::CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c238-e->
{
	ContinuousRefreshInfo *pContinuousRefreshInfo = new ContinuousRefreshInfo(m_pScreenHistory_Current->GetObj(),atoi(sTime.c_str()));
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Continuous_Refresh %d %p",
		pContinuousRefreshInfo->m_iInterval,&Orbiter::ContinuousRefresh);
#endif
	CallMaintenanceInMiliseconds( pContinuousRefreshInfo->m_iInterval * 1000, &Orbiter::ContinuousRefresh, pContinuousRefreshInfo, pe_ALL );
}

//<-dceag-c254-b->

	/** @brief COMMAND: #254 - Bind Icon */
	/** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
		/** @param #3 PK_DesignObj */
			/** The object which contains the icon, or whose child objects contain the icon. */
		/** @param #14 Type */
			/** The type of binding, like "housemode", "userstatus_39288", etc. */
		/** @param #104 Child */
			/** If true, it will set the property for the child object(s), rather than the designated object. */

void Orbiter::CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage)
//<-dceag-c254-e->
{
	// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Bind_Icon %s %s %d (size %d)",sPK_DesignObj.c_str(),sType.c_str(),(int) bChild,(int) m_mapObj_Bound.size());
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj );
	if( !pObj )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find object: %s for bind icon",sPK_DesignObj.c_str());
		return;
	}

	if( sType=="room_button" )
	{
		int iLocation=-1;
		DesignObjZoneList::iterator iZone;
		for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
		{
			DesignObjCommandList::iterator iAction;
			for( iAction=( *iZone )->m_Commands.begin(  );iAction!=( *iZone )->m_Commands.end(  );++iAction )
			{
				//int c = ( *iAction )->m_PK_Command;
				if(  ( *iAction )->m_PK_Command==COMMAND_Set_Current_Location_CONST  )
				{
					map<int,  string>::iterator iap;
					for( iap=( *iAction )->m_ParameterList.begin(  );iap!=( *iAction )->m_ParameterList.end(  );++iap )
					{
						if(  ( *iap ).first==COMMANDPARAMETER_LocationID_CONST  )
						{
							iLocation=atoi( ( ( *iap ).second ).c_str(  ) );
							break;
						}
					}
				}
			}
		}
		if(  iLocation!=-1 && iLocation < int(m_dequeLocation.size()))
		{
			LocationInfo *pLocation = m_dequeLocation[iLocation];
			pLocation->m_pvectGraphic = &(pObj->m_vectGraphic);
		}
		return;
	}

	if( sType=="room" )
	{
		pObj->m_bIsBoundToLocation=true;
		return;
	}
	if( sType=="user_button" )
	{
		int PK_Users=0;
		DesignObjZoneList::iterator iZone;
		for( iZone=pObj->m_ZoneList.begin(  );!PK_Users && iZone!=pObj->m_ZoneList.end(  );++iZone )
		{
			DesignObjCommandList::iterator iAction;
			for( iAction=( *iZone )->m_Commands.begin(  );!PK_Users && iAction!=( *iZone )->m_Commands.end(  );++iAction )
			{
				if(  ( *iAction )->m_PK_Command==COMMAND_Set_Current_User_CONST  )
				{
					map<int,  string>::iterator iap;
					for( iap=( *iAction )->m_ParameterList.begin(  );iap!=( *iAction )->m_ParameterList.end(  );++iap )
					{
						if(  ( *iap ).first==COMMANDPARAMETER_PK_Users_CONST  )
						{
							PK_Users=atoi( ( ( *iap ).second ).c_str(  ) );
							break;
						}
					}
				}
			}
		}
		if(  PK_Users  )
			m_mapUserIcons[PK_Users]= &(pObj->m_vectGraphic);
		return;
	}

	if( sType=="user" )
	{
		pObj->m_bIsBoundToUser=true;
		return;
	}

	DesignObj_DataList *pDesignObj_DataList = m_mapObj_Bound_Find(sType);
	if( !pDesignObj_DataList )
	{
		pDesignObj_DataList = new DesignObj_DataList();
		m_mapObj_Bound[sType] = pDesignObj_DataList;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"CMD_Bind_Icon added %s %s %d (size %d)",sPK_DesignObj.c_str(),sType.c_str(),(int) bChild,(int) m_mapObj_Bound.size());

	if( bChild && !pObj->m_ChildObjects.empty())
	{
		for( DesignObj_DataList::iterator it=pObj->m_ChildObjects.begin(  ); it != pObj->m_ChildObjects.end(  ); ++it )
			pDesignObj_DataList->push_back( (DesignObj_Orbiter *) *it);
	}
	else
	{
		pDesignObj_DataList->push_back(pObj);
	}
}

/*virtual*/ void Orbiter::SimulateMouseClick(int x, int y)
{
	//don't simulate any event; we're reloading
	if(m_bQuit_get())
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Simulate mouse click at position: %d, %d", x, y);
#endif

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	m_pOrbiterRenderer->BeginPaint();
	PlutoColor color(255, 0, 0, 100);
	m_pOrbiterRenderer->SolidRectangle(x - 5, y - 5, 10, 10, color);
	m_pOrbiterRenderer->UpdateRect(PlutoRectangle(x - 5, y - 5, 10, 10), PlutoPoint(0, 0));

	//render current screen id
	m_pOrbiterRenderer->SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->GetObj());
	text2.m_rPosition = rect2;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Current screen: " + this->GetCurrentScreenID();
	m_pOrbiterRenderer->RenderText(sText,&text2, pTextStyle);
	m_pOrbiterRenderer->UpdateRect(PlutoRectangle(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25), PlutoPoint(0, 0));
	m_pOrbiterRenderer->EndPaint();

	Orbiter::Event orbiterEvent;
	orbiterEvent.data.region.m_iButton = 1;
	orbiterEvent.data.region.m_iX = x;
	orbiterEvent.data.region.m_iY = y;

	orbiterEvent.type = Orbiter::Event::REGION_DOWN;
	ProcessEvent(orbiterEvent);

	orbiterEvent.type = Orbiter::Event::REGION_UP;
	ProcessEvent(orbiterEvent);
}

/*virtual*/ void Orbiter::SimulateMouseMovements(int x, int y)
{
	//don't simulate any event; we're reloading
	if(m_bQuit_get())
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Simulate mouse move to position: %d, %d", x, y);
#endif

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	m_pOrbiterRenderer->BeginPaint();
	PlutoColor color = PlutoColor::Green();
	const int nCursorSize = 3;
	m_pOrbiterRenderer->SolidRectangle(x - nCursorSize, y - nCursorSize, nCursorSize * 2, nCursorSize * 2, color);
	m_pOrbiterRenderer->UpdateRect(PlutoRectangle(x - nCursorSize, y - nCursorSize, nCursorSize * 2, nCursorSize * 2), PlutoPoint(0, 0));
	m_pOrbiterRenderer->EndPaint();

	Orbiter::Event orbiterEvent;
	orbiterEvent.data.region.m_iButton = 0;
	orbiterEvent.data.region.m_iX = x;
	orbiterEvent.data.region.m_iY = y;
	orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
	ProcessEvent(orbiterEvent);
}

/*virtual*/ void Orbiter::SimulateKeyPress(long key)
{
	//don't simulate any event; we're reloading
	if(m_bQuit_get())
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Simulate key press. Key code: %d", key);
#endif

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	m_pOrbiterRenderer->BeginPaint();

	//render a text with the
	PlutoColor color(200, 200, 200, 100);
	m_pOrbiterRenderer->SolidRectangle(5, m_iImageHeight - 30, 200, 25, color);
	PlutoRectangle rect(5, m_iImageHeight - 30, 200, 25);
	DesignObjText text(m_pScreenHistory_Current->GetObj());
	text.m_rPosition = rect;
	TextStyle *pTextStyle = m_mapTextStyle_Find( 1 );
	string sText = "Key code: " + StringUtils::ltos(key);
	m_pOrbiterRenderer->RenderText(sText,&text, pTextStyle);

	//render current screen id
	PlutoColor color2(255, 0, 0, 100);
	m_pOrbiterRenderer->SolidRectangle( m_iImageWidth - 250, m_iImageHeight - 30, 250, 25, color2);
	PlutoRectangle rect2(m_iImageWidth - 250, m_iImageHeight - 30, 250, 25);
	DesignObjText text2(m_pScreenHistory_Current->GetObj());
	text2.m_rPosition = rect2;
	sText = "Current screen: " + this->GetCurrentScreenID();
	m_pOrbiterRenderer->RenderText(sText,&text2, pTextStyle);
	m_pOrbiterRenderer->UpdateRect(PlutoRectangle(5, m_iImageHeight - 30, 200, 25), PlutoPoint(0, 0));
	m_pOrbiterRenderer->EndPaint();

	Orbiter::Event orbiterEvent;
	orbiterEvent.data.button.m_iPK_Button = key;
	orbiterEvent.data.button.m_bSimulated = true;

	orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
	ProcessEvent(orbiterEvent);

	orbiterEvent.type = Orbiter::Event::BUTTON_UP;
	ProcessEvent(orbiterEvent);
}

time_t Orbiter::GetLastScreenChangedTime()
{
	return NULL != m_pScreenHistory_Current ? m_pScreenHistory_Current->TimeCreated() : time(NULL);
}

string Orbiter::GetCurrentScreenID()
{
	return
		(
		NULL != m_pScreenHistory_Current				&&
		NULL != m_pScreenHistory_Current->GetObj()
		)
		?
		m_pScreenHistory_Current->GetObj()->m_ObjectID :
	"UNKNOWN";
}//<-dceag-c258-b->

	/** @brief COMMAND: #258 - Clear Selected Devices */
	/** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
		/** @param #3 PK_DesignObj */
			/** If specified, the object referenced here will be invalidated and redrawn. */

void Orbiter::CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c258-e->
{
	m_mapDevice_Selected.clear();
	DesignObj_Orbiter *pObj=NULL;
	if( sPK_DesignObj.length() && (pObj=FindObject(sPK_DesignObj))!=NULL )
		m_pOrbiterRenderer->RenderObjectAsync(pObj);
}

/*virtual*/ void Orbiter::GetRepeatedKeysForScreen(DesignObj_Orbiter* pObj, string& sKeysList)
{
	if(pObj->m_bRepeatParm)
	{
		if(sKeysList.length())
			sKeysList += ",";

		sKeysList += StringUtils::ltos(pObj->m_iPK_Button);
	}

	DesignObj_DataList::iterator it;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*it);
		GetRepeatedKeysForScreen(pDesignObj_Orbiter, sKeysList);
	}
}

/*virtual*/ bool Orbiter::IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button, bool bDown)
{
	//no valid button
	if(iPK_Button == 0 || m_bQuit_get())
		return false;

	static DesignObj_Orbiter *pObj_Highlighted_With_Repeat = NULL;
	if(bDown && NULL != m_pObj_Highlighted && m_pObj_Highlighted->m_bRepeatParm)
	{
		pObj_Highlighted_With_Repeat = m_pObj_Highlighted;
		return true;
	}

	if(!bDown && NULL != pObj_Highlighted_With_Repeat && pObj_Highlighted_With_Repeat->m_bRepeatParm)
	{
		pObj_Highlighted_With_Repeat = NULL;
		return true;
	}

	if(pObj->m_bRepeatParm && pObj->m_iPK_Button == iPK_Button)
		return true;

	DesignObj_DataList::iterator it;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)(*it);
		if(IsRepeatedKeyForScreen(pDesignObj_Orbiter, iPK_Button, bDown))
			return true;
	}

	return false;
}

/*virtual*/ void Orbiter::PlayMNG_CallBack(void *data)
{
	DesignObj_Orbiter* pObj = (DesignObj_Orbiter*)data;

	if(NULL == pObj->m_pvectCurrentPlayingGraphic)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MNG playing was canceled for object with id %s",
			pObj->m_ObjectID.c_str());
#endif
		return; //playing was canceled
	}

	vector<PlutoGraphic*> *pVectorPlutoGraphic = pObj->m_pvectCurrentPlayingGraphic;

	if(pVectorPlutoGraphic->size() == 0) //we have nothing to render
		return;

	int iFrameNum = int(pVectorPlutoGraphic->size());

	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[pObj->m_iCurrentFrame];

	if(!pPlutoGraphic->IsEmpty())
	{
		m_pOrbiterRenderer->BeginPaint();
		m_pOrbiterRenderer->RenderGraphic(pPlutoGraphic, pObj->m_rBackgroundPosition, pObj->m_bDisableAspectLock);

		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(pObj == m_pObj_Highlighted)
			m_pOrbiterRenderer->DoHighlightObject();

		m_pOrbiterRenderer->UpdateRect(pObj->m_rPosition, pObj->m_pPopupPoint);
		m_pOrbiterRenderer->EndPaint();
	}

	pObj->m_iCurrentFrame++;

	unsigned int iDelay = 0; //temp
#ifndef WINCE
	iDelay = 15;
#endif

	if(pObj->m_iCurrentFrame >= iFrameNum) //this is the last one
	{
		pObj->m_iCurrentFrame = 0;
		pObj->m_pvectCurrentPlayingGraphic = NULL;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "MNG playing was completed for object with id %s",
			pObj->m_ObjectID.c_str());
#endif
		CallMaintenanceInMiliseconds( iDelay, &Orbiter::DeselectObjects, ( void * ) pObj, pe_NO );
	}
	else
	{
		CallMaintenanceInMiliseconds( iDelay, &Orbiter::PlayMNG_CallBack, pObj , pe_NO );
	}
}

//<-dceag-c260-b->

	/** @brief COMMAND: #260 - Set Main Menu */
	/** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
		/** @param #9 Text */
			/** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

void Orbiter::CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c260-e->
{
	sText = StringUtils::ToUpper(sText); // we don't have to worry about lower case

	if( sText.length()>2 && sText[1]=='=' )
	{
		// We're assigning a new menu
		DesignObj_Orbiter *pObj = FindObject( sText.substr(2) );
		if( pObj )
		{
			if( sText[0]=='N' )
				m_pDesignObj_Orbiter_MainMenu=pObj;
			else if( sText[0]=='S' && m_pDesignObj_Orbiter_SleepingMenu )
				m_pDesignObj_Orbiter_SleepingMenu=pObj;
			else if( sText[0]=='V' && m_pDesignObj_Orbiter_ScreenSaveMenu )
				m_pDesignObj_Orbiter_ScreenSaveMenu=pObj;
		}
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't assign new main menu %s",sText.c_str());
	}

	if( sText.length() )  // Be sure it's not empty
	{
		// Just the normal N, S or V
		if( sText[0]=='N' )
			m_sMainMenu = m_pDesignObj_Orbiter_MainMenu->m_ObjectID;
		else if( sText[0]=='S' && m_pDesignObj_Orbiter_SleepingMenu )
			m_sMainMenu = m_pDesignObj_Orbiter_SleepingMenu->m_ObjectID;
		else if( sText[0]=='V' && m_pDesignObj_Orbiter_ScreenSaveMenu )
			m_sMainMenu = m_pDesignObj_Orbiter_ScreenSaveMenu->m_ObjectID;
	}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Set menu: %s now %s",sText.c_str(),m_sMainMenu.c_str());
#endif
}
//<-dceag-c265-b->

	/** @brief COMMAND: #265 - Quit */
	/** Exits the orbiter application */

void Orbiter::CMD_Quit(string &sCMD_Result,Message *pMessage)
//<-dceag-c265-e->
{
	OnQuit();
}
//<-dceag-createinst-b->!

void Orbiter::KillMaintThread()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Kill Maint Thread %d",(int) bMaintThreadIsRunning);
#endif
	m_bQuit_set(true);
	pthread_cond_broadcast(&m_MaintThreadCond);  // Wake it up, it will quit when it sees the quit
	time_t tTime = time(NULL);
	while(bMaintThreadIsRunning)
	{
		Sleep(10);
		if( tTime + 20 < time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Maint Thread had blocked!!!!");
			exit(1);
		}
	}

	if(m_MaintThreadID)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for Maint thread to finish...");
		pthread_join(m_MaintThreadID, NULL);
		m_MaintThreadID = 0;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done with Maint thread.");
	}
}

void Orbiter::StopSimulatorThread()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stopping simulator thread...");
	Simulator::GetInstance()->StopRandomEventGenerator();

	time_t tTime = time(NULL);
	while(Simulator::GetInstance()->IsRunning())
	{
		Sleep(10);
		if( tTime + 5 < time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to stop the simulator thread!");
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Simulator thread stopped.");
}

void Orbiter::TranslateVirtualDevice(int PK_DeviceTemplate,long &PK_Device)
{
	// This is going to a virtual device
	switch(  PK_DeviceTemplate )
	{
	case DEVICETEMPLATE_VirtDev_IR_Receiver_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_IRReceiver;
		break;

	case DEVICETEMPLATE_VirtDev_Disc_Drive_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_DiscDrive;
		break;

	case DEVICETEMPLATE_VirtDev_LCDVFD_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_LCD_VFD;
		break;

	case DEVICETEMPLATE_VirtDev_AppServer_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_AppServer;
		break;

	case DEVICETEMPLATE_VirtDev_Orbiter_Onscreen_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_Orbiter;
		break;

	case DEVICETEMPLATE_VirtDev_Media_Director_CONST:
		PK_Device=m_pLocationInfo->m_dwPK_Device_MediaDirector;
		break;

	case DEVICETEMPLATE_VirtDev_Local_Media_Director_CONST:
		PK_Device=m_pLocationInfo_Initial->m_dwPK_Device_MediaDirector;
		break;

	case DEVICETEMPLATE_This_Orbiter_CONST:
		PK_Device=m_dwPK_Device;
		break;

	case DEVICETEMPLATE_This_Orbiters_Now_Playing_CONST:
		PK_Device=m_dwPK_Device_NowPlaying;
		break;

	case DEVICETEMPLATE_VirtDev_Security_Plugin_CONST:
		PK_Device=m_dwPK_Device_SecurityPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Telecom_Plugin_CONST:
		PK_Device=m_dwPK_Device_TelecomPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Media_Plugin_CONST:
		PK_Device=m_dwPK_Device_MediaPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST:
		PK_Device=m_dwPK_Device_ClimatePlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Lighting_PlugIn_CONST:
		PK_Device=m_dwPK_Device_LightingPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Infrared_Plugin_CONST:
		PK_Device=m_dwPK_Device_InfraredPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_General_Info_Plugin_CONST:
		PK_Device=m_dwPK_Device_GeneralInfoPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Event_Plugin_CONST:
		PK_Device=m_dwPK_Device_EventPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Datagrid_Plugin_CONST:
		PK_Device=m_dwPK_Device_DatagridPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Orbiter_Plugin_CONST:
		PK_Device=m_dwPK_Device_OrbiterPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Plug_And_Play_PlugI_CONST:
		PK_Device=m_dwPK_Device_PlugAndPlayPlugIn;
		break;

	case DEVICETEMPLATE_VirtDev_Local_AppServer_CONST:
		PK_Device=m_dwPK_Device_LocalAppServer;
		break;

	case DEVICETEMPLATE_VirtDev_Local_Media_Player_CONST:
		PK_Device=m_dwPK_Device_LocalMediaPlayer;
		break;

	}
}

//<-dceag-c324-b->

	/** @brief COMMAND: #324 - Set Timeout */
	/** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
		/** @param #3 PK_DesignObj */
			/** The screen to set the timeout on.  If blank the current screen. */
		/** @param #102 Time */
			/** The timeout in seconds.  0 or blank means no timeout. */

void Orbiter::CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c324-e->
{
	DesignObj_Orbiter *pObj;
	if( atoi(sPK_DesignObj.c_str())==0 )
		pObj = m_pScreenHistory_Current->GetObj();
	else
	{
		sPK_DesignObj = SubstituteVariables( sPK_DesignObj,  NULL,  0,  0 );
		pObj = FindObject( sPK_DesignObj );
	}
	if(  !pObj  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot find object in CMD_Set_Timeout: %s", sPK_DesignObj.c_str(  ) );
		return;
	}


	pObj->m_dwTimeoutSeconds = atoi(sTime.c_str());

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "set timeout on %s to %d  %p = %p",pObj->m_ObjectID.c_str(),pObj->m_dwTimeoutSeconds,pObj,m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj() : NULL );
#endif
	if( NULL != m_pScreenHistory_Current && pObj==m_pScreenHistory_Current->GetObj() && pObj->m_dwTimeoutSeconds )
		CallMaintenanceInMiliseconds( pObj->m_dwTimeoutSeconds * 1000, &Orbiter::Timeout, (void *) pObj, pe_ALL, true );
}
//<-dceag-c325-b->

	/** @brief COMMAND: #325 - Keep Screen On */
	/** Allow or don't allow the screen to blank with the screen saver. */
		/** @param #8 On/Off */
			/** If other than "0", the screen saver will be disabled. */

void Orbiter::CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c325-e->
{
	m_bBypassScreenSaver = sOnOff!="0";
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bypass screen saver now: %d",(int)m_bBypassScreenSaver);
#endif
}
//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn the device on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Orbiter::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	m_bDisplayOn = true;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd_on m_bDisplayOn set to %d",(int) m_bDisplayOn);
#endif
	if( !(m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj() == m_pDesignObj_Orbiter_ScreenSaveMenu) && !m_bBypassScreenSaver && m_iTimeoutScreenSaver )
		CallMaintenanceInMiliseconds( m_iTimeoutScreenSaver * 1000, &Orbiter::ScreenSaver, NULL, pe_ALL );
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn the device off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Orbiter::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	m_bDisplayOn = false;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"cmd_off m_bDisplayOn set to %d",(int) m_bDisplayOn);
#endif
}
//<-dceag-c330-b->

	/** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
	/** Positions the on-screen mouse pointer centered over a certain object */
		/** @param #3 PK_DesignObj */
			/** The object to center the mouse over. */

void Orbiter::CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c330-e->
{
	DesignObj_Orbiter *pObj = FindObject( sPK_DesignObj, m_pScreenHistory_Current->GetObj() );
	if( !pObj )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MousePointerOverUnknownObject: %s",sPK_DesignObj.c_str());
		return;
	}
	int X = pObj->m_rPosition.X + pObj->m_rPosition.Width/2;
	int Y = pObj->m_rPosition.Y + pObj->m_rPosition.Height/2;
	SetMousePointer(X,Y);
}

//<-dceag-c354-b->

	/** @brief COMMAND: #354 - Show Mouse Pointer */
	/** Shows or hides the mouse pointer. */
		/** @param #8 On/Off */
			/** 1=show it, 0=hide it. */

void Orbiter::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c354-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::CMD_Show_Mouse_Pointer %s", sOnOff.c_str());
#endif
#ifdef ENABLE_MOUSE_BEHAVIOR
	if(	m_pMouseBehavior )
		m_pMouseBehavior->ShowMouse(sOnOff=="1");
#endif
}

//<-dceag-c366-b->

	/** @brief COMMAND: #366 - Activate Window */
	/** ActivateApplication - Used by Linux On Screeen Orbiters only */
		/** @param #50 Name */
			/** Name as known by ratpoison. */

void Orbiter::CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c366-e->
{
	m_sApplicationName = sName;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received 'Activate Window' for %s", sName.c_str());
#endif
}

bool Orbiter::OkayToDeserialize(int iSC_Version)
{
	if( iSC_Version==ORBITER_SCHEMA )
		return true;

	string sErrorMessage = "Cannot deserialize .info file.  It is version " + StringUtils::ltos(iSC_Version) +
		" and I need " + StringUtils::ltos(ORBITER_SCHEMA) + ".";
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, sErrorMessage.c_str());

	if( iSC_Version>ORBITER_SCHEMA )
	{
		m_pOrbiterRenderer->PromptUser("I'm sorry.  This version of Orbiter is too old.  It uses schema " + StringUtils::itos(ORBITER_SCHEMA)
			+ " instead of " + StringUtils::itos(iSC_Version) + ".  Please install a newer version.");
		OnReload();
		exit(1);
	}
	else
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes";
		mapPrompts[prNo]     = "No";
		int iResponse = m_pOrbiterRenderer->PromptUser("The user interface which your Core generated (" + StringUtils::itos(iSC_Version) + ") is too old for this Orbiter(" + StringUtils::itos(ORBITER_SCHEMA) + ").  Shall I ask the Core to rebuild it now?",0,&mapPrompts);
		if( iResponse==prNo )
		{
			OnReload();
			exit(1);
			return false;
		}
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "UI is %d not %d", iSC_Version, (int) ORBITER_SCHEMA );

		RegenOrbiter();
		OnReload();

#ifdef WIN32
		OnUnexpectedDisconnect(); //force a reload
#else
		exit(1);
#endif
	}

	return false;
}

// Temporary function to debug a problem with the screen history
void Orbiter::DumpScreenHistory()
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	string s = "Screen history: size: " + StringUtils::itos(int(m_listScreenHistory.size())) + "; "
		+ "Current: " + (m_pScreenHistory_Current ? m_pScreenHistory_Current->ToString() : "*NONE*")
		+ "screens: ";

	for(list < ScreenHistory * >::iterator it=m_listScreenHistory.begin();it!=m_listScreenHistory.end();++it)
	{
		ScreenHistory *pScreenHistory = *it;
		s += pScreenHistory->ToString() + ", ";
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s", s.c_str());
}
//<-dceag-c59-b->

	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** If you don't know the location, you can also set just the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

void Orbiter::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
	deque<class LocationInfo *>::iterator it;
	for(it = m_dequeLocation.begin(); it != m_dequeLocation.end(); ++it)
	{
		LocationInfo *pLocationInfo = *it;
		if( pLocationInfo->PK_EntertainArea == atoi(sPK_EntertainArea.c_str()) )
		{
			CMD_Set_Current_Location(pLocationInfo->iLocation);
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Can't set ea to %s",sPK_EntertainArea.c_str() );
}

//<-dceag-c77-b->

	/** @brief COMMAND: #77 - Set Current Room */
	/** If you don't know the location, you can also set just the room */
		/** @param #57 PK_Room */
			/** The room */

void Orbiter::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
	deque<LocationInfo *>::iterator it;
	for(it = m_dequeLocation.begin(); it != m_dequeLocation.end(); ++it)
	{
		LocationInfo *pLocationInfo = *it;
		if( pLocationInfo->PK_Room == iPK_Room )
		{
			CMD_Set_Current_Location(pLocationInfo->iLocation);
			return;
		}
	}
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Can't set ea to %d",iPK_Room );
}
//<-dceag-c389-b->

	/** @brief COMMAND: #389 - Send Message */
	/** Sends a message stored in a parameter as a text object. */
		/** @param #9 Text */
			/** The message in command line-style format */
		/** @param #144 Go Back */
			/** Go back after sending the command if it does not contain another goto screen or go back */

void Orbiter::CMD_Send_Message(string sText,bool bGo_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c389-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Send_Message %s",sText.c_str());
#endif
	bool bContainsGoto=false;
	string sMessage = SubstituteVariables(sText,NULL,0,0);

	vector<string> vectMessages;
	StringUtils::Tokenize(sMessage, ";", vectMessages);

	for(vector<string>::iterator it = vectMessages.begin(); it != vectMessages.end(); ++it)
	{
		string sLocalMessage = *it;

		Message *pMessageOut = new Message(sLocalMessage);
		if( pMessageOut->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
			pMessageOut->m_dwPK_Device_To=m_dwPK_Device;
		if( pMessageOut->m_dwMessage_Type==MESSAGETYPE_COMMAND && 
				(pMessageOut->m_dwID==COMMAND_Go_back_CONST || pMessageOut->m_dwID==COMMAND_Goto_DesignObj_CONST || pMessageOut->m_dwID==COMMAND_Goto_Screen_CONST || pMessageOut->m_dwID==COMMAND_Remove_Screen_From_History_CONST) )
			bContainsGoto=true;

		for(vector<Message *>::iterator it = pMessageOut->m_vectExtraMessages.begin();
			it != pMessageOut->m_vectExtraMessages.end(); ++it)
		{
			Message *pExtraMessage = *it;
			if( pExtraMessage->m_dwPK_Device_To==DEVICETEMPLATE_This_Orbiter_CONST )
				pExtraMessage->m_dwPK_Device_To=m_dwPK_Device;
			if( pExtraMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && (pExtraMessage->m_dwID==COMMAND_Go_back_CONST || pExtraMessage->m_dwID==COMMAND_Goto_DesignObj_CONST || pExtraMessage->m_dwID==COMMAND_Goto_Screen_CONST || pExtraMessage->m_dwID==COMMAND_Remove_Screen_From_History_CONST) )
				bContainsGoto=true;
		}

		if(Simulator::GetInstance()->IsRunning() && pMessageOut->m_dwID == COMMAND_Delete_File_CONST)
		{
			CMD_Go_back("","");
			return;
		}

		QueueMessageToRouter(pMessageOut);
	}

	if( bGo_Back && !bContainsGoto )
		CMD_Go_back("","");
}

void Orbiter::ResetState(DesignObj_Orbiter *pObj, bool bDontResetState)
{
	if(  pObj->m_bDontResetState || pObj->m_bOneTimeDontReset  )
	{
		pObj->m_bOneTimeDontReset=false;
		bDontResetState=true;
	}
	else if(  !bDontResetState  )
	{
		pObj->m_GraphicToDisplay_set("o7",GRAPHIC_NORMAL);
		pObj->m_bHidden=pObj->m_bHideByDefault;
	}

	DesignObj_DataList::iterator iHao;
	for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
		ResetState( (DesignObj_Orbiter * )( *iHao ), bDontResetState );
}

PlutoPopup *Orbiter::FindPopupByName(DesignObj_Orbiter *pObj,string sName)
{
	if( pObj )
	{
		for(list<class PlutoPopup*>::reverse_iterator it=pObj->m_listPopups.rbegin();it!=pObj->m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(pPopup->m_sName == sName )
				return pPopup;
		}
	}
	else
	{
		for(list<class PlutoPopup*>::reverse_iterator it=m_listPopups.rbegin();it!=m_listPopups.rend();++it)
		{
			PlutoPopup *pPopup = *it;
			if(pPopup->m_sName == sName )
				return pPopup;
		}
	}

	return NULL;
}

//<-dceag-c397-b->

	/** @brief COMMAND: #397 - Show Popup */
	/** Shows a screen as a popup, at position x, y */
		/** @param #3 PK_DesignObj */
			/** The ID of the screen */
		/** @param #11 Position X */
			/** X position */
		/** @param #12 Position Y */
			/** Y position */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If specified the popup will be local to this screen, otherwise it will be global.  Global and local popups are treated separately */
		/** @param #50 Name */
			/** The popup name */
		/** @param #126 Exclusive */
			/** Hide any other popups that are also visible, unless don't hide is set. */
		/** @param #127 Dont Auto Hide */
			/** If true, this popup will not be automatically hidden when the screen changes or another exclusive is shown.  It must be explicitly hidden. */

void Orbiter::CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage)
//<-dceag-c397-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"show popup %s/%s",sName.c_str(),sPK_DesignObj.c_str());
#endif

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	DesignObj_Orbiter *pObj_Popup = FindObject(sPK_DesignObj);
	if( !pObj_Popup )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"CMD_Show_Popup bad popup %s",sPK_DesignObj.c_str());
		return;
	}
	DesignObj_Orbiter *pObj_Screen = FindObject(sPK_DesignObj_CurrentScreen);
	if( bExclusive )
		m_pOrbiterRenderer->HidePopups(pObj_Screen);

	PlutoPoint pt(iPosition_X,iPosition_Y);
	if( m_iRotation==90 )
	{
		int x=pt.X;
		pt.X = m_sScreenSize.Height - pt.Y - pObj_Popup->m_rPosition.Height;
		pt.Y = x;
	}

	PlutoPopup *pPopup = new PlutoPopup(pObj_Popup, sName, pt);
	bool bSuccessful;
	if( pObj_Screen )
		bSuccessful=AddPopup(pObj_Screen->m_listPopups,pPopup);
	else
		bSuccessful=AddPopup(m_listPopups,pPopup);

	if( !bSuccessful )
		return; // Popup must have already been there

	ResetState(pObj_Popup);

	VectDesignObj_Orbiter vectDesignObj_Orbiter_OnScreen;
	m_pOrbiterRenderer->ObjectOnScreen( &vectDesignObj_Orbiter_OnScreen, pObj_Popup, &pt );
	HandleNewObjectsOnScreen( &vectDesignObj_Orbiter_OnScreen );

	pPopup->m_bDontAutohide = bDont_Auto_Hide;

	//
	/*
	if( sName=="remote" )
	{
	int k=2;
	}
	*/

	if(!Renderer()->HandleShowPopup(pPopup, PlutoPoint(iPosition_X, iPosition_Y), 1))
		CMD_Refresh("");
}

//<-dceag-c398-b->

	/** @brief COMMAND: #398 - Remove Popup */
	/** Hides a popup. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** (optional).  The screen on which it's a local popup */
		/** @param #50 Name */
			/** The name of the popup.  If not specified all popups will be removed */

void Orbiter::CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c398-e->
{
	/*
	if( sName=="remote" )
	{
	int k=2;
	}
	*/
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object

	DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj_CurrentScreen);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"remove popup %s",sName.c_str());
#endif

	bool bExistsPopupToHide = false;
	bool bNeedRefresh = false;

	if( pObj )
	{
		for(list<class PlutoPopup*>::iterator it=pObj->m_listPopups.begin();it!=pObj->m_listPopups.end();)
		{
			if( (*it)->m_sName==sName || sName.size()==0 )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					m_pOrbiterRenderer->ObjectOffScreen((*it)->m_pObj);
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());

				bExistsPopupToHide = true;
				if(!Renderer()->HandleHidePopup(*it))
					bNeedRefresh = true;

				delete *it;
				it = pObj->m_listPopups.erase(it);
			}
			else
				++it;
		}
	}
	else
	{
		for(list<class PlutoPopup*>::iterator it=m_listPopups.begin();it!=m_listPopups.end();)
		{
			if( (*it)->m_sName==sName || sName.size()==0 )
			{
				if( (*it)->m_pObj->m_bOnScreen )
					m_pOrbiterRenderer->ObjectOffScreen((*it)->m_pObj);
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Popup %s was already off screen",(*it)->m_pObj->m_ObjectID.c_str());
					
				bExistsPopupToHide = true;
				if(!Renderer()->HandleHidePopup(*it))
					bNeedRefresh = true;

				delete *it;
				it = m_listPopups.erase(it);
			}
			else
				++it;
		}
	}

	if(bExistsPopupToHide && bNeedRefresh)
		CMD_Refresh("");
}

//<-dceag-c399-b->

	/** @brief COMMAND: #399 - Show Shortcuts */
	/** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

void Orbiter::CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage)
//<-dceag-c399-e->
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );
	m_bShowShortcuts = true;
	CallMaintenanceInMiliseconds( 10000, &Orbiter::RemoveShortcuts, NULL, pe_ALL );
	CMD_Refresh("");
}

void Orbiter::RemoveShortcuts( void *data )
{
	if( m_bQuit_get())
		return;
	m_bShowShortcuts = false;
	CMD_Refresh("");
}

int Orbiter::PreprocessMediaType(int iPK_MediaType)
{
  switch (iPK_MediaType)
    {
    case MEDIATYPE_lmce_Game_a2600_CONST:
    case MEDIATYPE_lmce_Game_a5200_CONST:
    case MEDIATYPE_lmce_Game_a7800_CONST:
    case MEDIATYPE_lmce_Game_coleco_CONST:
    case MEDIATYPE_lmce_Game_intv_CONST:
    case MEDIATYPE_lmce_Game_famicom_CONST:
    case MEDIATYPE_lmce_Game_nes_CONST:
    case MEDIATYPE_lmce_Game_sms_CONST:
    case MEDIATYPE_lmce_Game_sg1000_CONST:
    case MEDIATYPE_lmce_Game_vectrex_CONST:
    case MEDIATYPE_lmce_Game_tg16_CONST:
    case MEDIATYPE_lmce_Game_genesis_CONST:
    case MEDIATYPE_lmce_Game_snes_CONST:
    case MEDIATYPE_lmce_Game_megadriv_CONST:
    case MEDIATYPE_lmce_Game_snespal_CONST:
    case MEDIATYPE_lmce_Game_sgx_CONST:
    case MEDIATYPE_lmce_Game_pce_CONST:
    case MEDIATYPE_lmce_Game_apple2_CONST:
    case MEDIATYPE_lmce_Game_ps1_CONST:
    case MEDIATYPE_lmce_Game_ps2_CONST:
    case MEDIATYPE_lmce_Game_jaguar_CONST:
    case MEDIATYPE_lmce_Game_vic20_CONST:
    case MEDIATYPE_lmce_Game_c64_CONST:
    case MEDIATYPE_lmce_Game_Atari800_CONST:
      iPK_MediaType = MEDIATYPE_lmce_Game_CONST;
      break;
    default:
      iPK_MediaType = iPK_MediaType;
    }
  
  return iPK_MediaType;
}

//<-dceag-c401-b->

	/** @brief COMMAND: #401 - Show File List */
	/** Shows the file list */
		/** @param #29 PK_MediaType */
			/** The type of media the user wants to browse. */

void Orbiter::CMD_Show_File_List(int iPK_MediaType,string &sCMD_Result,Message *pMessage)
//<-dceag-c401-e->
{
	if( !m_pOrbiterFileBrowser_Collection || !m_pScreenHandler )
		return; // Should never happen

	OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry =
		m_pOrbiterFileBrowser_Collection->m_mapOrbiterFileBrowser[iPK_MediaType];

	iPK_MediaType = PreprocessMediaType(iPK_MediaType);

	if( !pOrbiterFileBrowser_Entry )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No way to show file list for %d",iPK_MediaType);
		return;
	}

	m_pScreenHandler->mediaFileBrowserOptions.ClearAll(iPK_MediaType,pOrbiterFileBrowser_Entry->m_PK_Screen,m_mapPK_MediaType_PK_Attribute_Sort[iPK_MediaType]);
	CMD_Set_Variable(VARIABLE_Filename_CONST, pOrbiterFileBrowser_Entry->m_sFilename);
	CMD_Set_Variable(VARIABLE_PK_MediaType_CONST, StringUtils::itos(iPK_MediaType));
	CMD_Goto_Screen("",pOrbiterFileBrowser_Entry->m_PK_Screen);
}

//<-dceag-c402-b->

	/** @brief COMMAND: #402 - Use Popup Remote Controls */
	/** If this command is executed the remote controls will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen on which to put the popup */

void Orbiter::CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c402-e->
{
	m_sObj_Popop_RemoteControl = sPK_DesignObj_CurrentScreen;
	m_Popop_RemoteControl_X = iPosition_X;
	m_Popop_RemoteControl_Y = iPosition_Y;

	DCE::CMD_Set_Auto_Switch_to_Remote CMD_Set_Auto_Switch_to_Remote(m_dwPK_Device,m_dwPK_Device_OrbiterPlugIn,m_dwPK_Device,false);
	SendCommand(CMD_Set_Auto_Switch_to_Remote);
}

//<-dceag-c403-b->

	/** @brief COMMAND: #403 - Use Popup File List */
	/** If this command is executed the file lists will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen to put the popup on */

void Orbiter::CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c403-e->
{
	m_sObj_Popop_FileList = sPK_DesignObj_CurrentScreen;
	m_Popop_FileList_X = iPosition_X;
	m_Popop_FileList_Y = iPosition_Y;
}

bool Orbiter::AddPopup(list<class PlutoPopup*> &listPopups,class PlutoPopup *pPopup)
{
	for(list<class PlutoPopup*>::iterator it=listPopups.begin();it!=listPopups.end();++it)
	{
		PlutoPopup *p = *it;
		if( p->m_sName == pPopup->m_sName )
		{
			if( p->m_pObj==pPopup->m_pObj && p->m_Position==pPopup->m_Position )
				return false;

			CMD_Remove_Popup("", pPopup->m_sName);
			break;
		}
	}

	listPopups.push_back(pPopup);
	return true;
}

void Orbiter::HandleNewObjectsOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter)
{
	VectDesignObj_Orbiter::iterator it;
	for(it = pVectDesignObj_Orbiter->begin(); it != pVectDesignObj_Orbiter->end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = *it;
		if(  pDesignObj_Orbiter!=m_pScreenHistory_Current->GetObj()  )  // We just did the screen itself above
		{
			ExecuteCommandsInList( &pDesignObj_Orbiter->m_Action_LoadList, pDesignObj_Orbiter, smLoadUnload, 0, 0 );
		}
	}

	for(it = pVectDesignObj_Orbiter->begin(); it != pVectDesignObj_Orbiter->end(); ++it)
	{
		DesignObj_Orbiter *pDesignObj_Orbiter = *it;
		if ( pDesignObj_Orbiter->m_ObjectType == DESIGNOBJTYPE_Datagrid_CONST )
		{
			InitializeGrid( ( DesignObj_DataGrid * )pDesignObj_Orbiter  );
			m_vectObjs_GridsOnScreen.push_back( ( DesignObj_DataGrid * ) pDesignObj_Orbiter );
		}
	}
}//<-dceag-c405-b->

	/** @brief COMMAND: #405 - Scale this object */
	/** If you add this command to the startup list of an object it will cause Orbiter Gen to scale this object and all it's children. */
		/** @param #48 Value */
			/** The value to scale to.  100=full size, 50=half size */

void Orbiter::CMD_Scale_this_object(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c405-e->
{
	// this only applies to orbitergen
}
//<-dceag-c407-b->

	/** @brief COMMAND: #407 - Set Floorplan */
	/** Sets the object to use for one of the following types:
light, media, climate, security, telecom */
		/** @param #3 PK_DesignObj */
			/** The screen to use for this floorplan */
		/** @param #14 Type */
			/** One of the following:
light, climate, media, security, telecom */
		/** @param #119 True/False */
			/** True if this is a popup.  False if it's full screen */

void Orbiter::CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c407-e->
{
	DesignObj_Orbiter *pObj = FindObject(sPK_DesignObj);
	m_mapFloorplan[sType]=pObj;
}

//<-dceag-c408-b->

	/** @brief COMMAND: #408 - Show Floorplan */
	/** Shows the floorplan */
		/** @param #11 Position X */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #12 Position Y */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #14 Type */
			/** The type of floorplan */

void Orbiter::CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c408-e->
{
	DesignObj_Orbiter *pObj = m_mapFloorplan[sType];
	if( !pObj )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No floorplan for type %s",sType.c_str());
		return;
	}
	if( !m_pScreenHistory_Current || (pObj->m_rPosition.Width==m_pScreenHistory_Current->GetObj()->m_rPosition.Width && pObj->m_rPosition.Height==m_pScreenHistory_Current->GetObj()->m_rPosition.Height) )
	{
#ifdef ENABLE_MOUSE_BEHAVIOR
		// With the new UI 
		if(UsesUIVersion2() && m_pMouseBehavior )
			m_pMouseBehavior->Clear();
#endif
		DCE::SCREEN_Floorplan SCREEN_Floorplan(m_dwPK_Device,m_dwPK_Device,pObj->m_ObjectID);
		string sResult;
		CMD_Goto_Screen("", SCREEN_Floorplan_CONST,interuptAlways,false,false, sResult, SCREEN_Floorplan.m_pMessage);
		delete SCREEN_Floorplan.m_pMessage;
	}
	else
		CMD_Show_Popup(pObj->m_ObjectID,iPosition_X,iPosition_Y,"","floorplan",false,false);
}
//<-dceag-c413-b->

	/** @brief COMMAND: #413 - Forward local k/b to OSD */
	/** Means this orbiter's keyboard should be controlling the application running on the media director. */
		/** @param #119 True/False */
			/** If 1, do it.  It 0, stop */

void Orbiter::CMD_Forward_local_kb_to_OSD(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c413-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Forward_local_kb_to_OSD %d", (int) bTrueFalse);
#endif
	m_bForward_local_kb_to_OSD=bTrueFalse;
}
//<-dceag-c415-b->

	/** @brief COMMAND: #415 - Set Mouse Position Relative */
	/** Move the mouse relative to its current position */
		/** @param #11 Position X */
			/** The X Position to move */
		/** @param #12 Position Y */
			/** The Y Position to move */

void Orbiter::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c415-e->
{
}
//<-dceag-c416-b->

	/** @brief COMMAND: #416 - Simulate Mouse Click At Present Pos */
	/** Simulates clicking the mouse button */
		/** @param #14 Type */
			/** If L or empty, the left button.  If R the right button. */

void Orbiter::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c416-e->
{
}

void Orbiter::ParseGrid(DesignObj_DataGrid *pObj_Datagrid)
{
	pObj_Datagrid->m_sExtraInfo = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Extra_Info_CONST];
	pObj_Datagrid->m_FixedRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Row_Height_CONST ).c_str(  ) );
	pObj_Datagrid->m_FixedColumnWidth =  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Fixed_Column_Width_CONST ).c_str(  ) );
	pObj_Datagrid->m_RowSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Row_Spacing_CONST ).c_str(  ) );
	pObj_Datagrid->m_ColumnSpacing = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Column_Spacing_CONST ).c_str(  ) );
	pObj_Datagrid->m_FirstRowHeight = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Row_Height_CONST ).c_str(  ) );
	pObj_Datagrid->m_FirstColumnWidth =  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_First_Column_Width_CONST ).c_str(  ) );
	pObj_Datagrid->m_bKeepRowHeader = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Keep_Row_Header_CONST )=="1";
	pObj_Datagrid->m_bKeepColHeader = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Keep_Column_Header_CONST )=="1";
	pObj_Datagrid->m_bPersistXY = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Persist_XY_Position_CONST )=="1";
	pObj_Datagrid->m_iInitialRowNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Row_Number_CONST ).c_str(  ) );
	pObj_Datagrid->m_iInitialColNum = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Initial_Column_Numb_CONST ).c_str(  ) );
	pObj_Datagrid->m_MaxCol = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Columns_CONST ).c_str(  ) );
	pObj_Datagrid->m_sSeek = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Seek_Value_CONST );
	pObj_Datagrid->m_iSeekColumn = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Seek_Column_CONST ).c_str(  ) );

	if(m_sSkin == AUDIO_STATION_SKIN && pObj_Datagrid->m_iBaseObjectID == DESIGNOBJ_dgFileList2_Pics_CONST)
		pObj_Datagrid->m_MaxCol = 4;

	if ( pObj_Datagrid->m_MaxCol == 0 )
	{
		if(  pObj_Datagrid->m_FixedColumnWidth==0  )
			pObj_Datagrid->m_MaxCol=1;
		else if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_MaxCol = ( pObj_Datagrid->m_rPosition.Height - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
		else
			pObj_Datagrid->m_MaxCol = ( pObj_Datagrid->m_rPosition.Width - ( pObj_Datagrid->m_FirstColumnWidth +1  ) ) / ( pObj_Datagrid->m_FixedColumnWidth+1 );
		if( pObj_Datagrid->m_FirstColumnWidth )
			pObj_Datagrid->m_MaxCol++;  // WE deducted the first column above
	}
	else if ( pObj_Datagrid->m_FixedColumnWidth == 0 ) // Do we know the number of columns but not their size?
	{
		if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->m_rPosition.Height / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
		else
			pObj_Datagrid->m_FixedColumnWidth = ( pObj_Datagrid->m_rPosition.Width / pObj_Datagrid->m_MaxCol ) - ( ( pObj_Datagrid->m_MaxCol-1 ) * pObj_Datagrid->m_ColumnSpacing );
	}
	pObj_Datagrid->m_MaxRow = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Num_of_Rows_CONST ).c_str(  ) );
	if ( pObj_Datagrid->m_MaxRow == 0 )
	{
		if(  pObj_Datagrid->m_FixedRowHeight==0  )
			pObj_Datagrid->m_MaxRow=1;
		else if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_MaxRow = ( pObj_Datagrid->m_rPosition.Width - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
		else
			pObj_Datagrid->m_MaxRow = ( pObj_Datagrid->m_rPosition.Height - ( pObj_Datagrid->m_FirstRowHeight +1  ) ) / ( pObj_Datagrid->m_FixedRowHeight+1 );
		if( pObj_Datagrid->m_FirstRowHeight )
			pObj_Datagrid->m_MaxRow++;  // WE deducted the first column above
	}
	else if ( pObj_Datagrid->m_FixedRowHeight == 0 ) // Do we know the number of columns but not their size?
	{
		if( m_iRotation==90 || m_iRotation==270 )
			pObj_Datagrid->m_FixedRowHeight = (pObj_Datagrid->m_rPosition.Width - (pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1))/pObj_Datagrid->m_MaxRow;
		else
			pObj_Datagrid->m_FixedRowHeight = (pObj_Datagrid->m_rPosition.Height - (pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1))/pObj_Datagrid->m_MaxRow;
	}
	if(  ( pObj_Datagrid->m_FixedColumnWidth == 0 && pObj_Datagrid->m_MaxCol == 0 ) || ( pObj_Datagrid->m_FixedRowHeight == 0 && pObj_Datagrid->m_MaxRow == 0 )  )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL,  "Neither the column/row size,  nor the the column/row count are specified in the parameters on grid %s",  pObj_Datagrid->m_ObjectID.c_str(  ) );
	}

	// For purposes of calculation,  take the bigger of the two here when not fixed
	if(  !pObj_Datagrid->m_bKeepRowHeader  )
	{
		pObj_Datagrid->m_FirstRowHeight = max( pObj_Datagrid->m_FirstRowHeight,  pObj_Datagrid->m_FixedRowHeight );
	}
	else
	{
		if ( pObj_Datagrid->m_FirstRowHeight == 0 )
			pObj_Datagrid->m_FirstRowHeight = pObj_Datagrid->m_FixedRowHeight;
	}
	if (  !pObj_Datagrid->m_FirstColumnWidth  )
	{
		pObj_Datagrid->m_FirstColumnWidth = max( pObj_Datagrid->m_FirstColumnWidth,  pObj_Datagrid->m_FixedColumnWidth );
	}
	else
	{
		if ( pObj_Datagrid->m_FirstColumnWidth == 0 )
			pObj_Datagrid->m_FirstColumnWidth = pObj_Datagrid->m_FixedColumnWidth;
	}

	pObj_Datagrid->m_iPK_Variable = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST ).c_str(  ) );
	pObj_Datagrid->m_sOptions = pObj_Datagrid->m_mapObjParms[DESIGNOBJPARAMETER_Options_CONST];  // No substitution since this usually has tokens in it to parse at runtime
	pObj_Datagrid->m_iPK_Datagrid = atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Datagrid_CONST ).c_str(  ) );
	string sPK_DeviceTemplate = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_DeviceTemplate_CONST, false ); // Don't substitute since this can change in realtime when the grid is rendered
	pObj_Datagrid->m_iPK_DeviceTemplate = sPK_DeviceTemplate.find("<%")!=string::npos ? -1 : atoi( sPK_DeviceTemplate.c_str(  ) );
	pObj_Datagrid->m_bDontShowSelection = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST )=="-1";
	pObj_Datagrid->m_bIsMultiSelect = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_Is_Multi_Select_CONST )=="1";

	string::size_type pos = 0;
	/* todo 2.0 pObj_Datagrid->BorderWidth = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ", ",  pos ).c_str(  ) );
	pObj_Datagrid->BorderColor = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ";",  pos ).c_str(  ) );
	if ( pos<pObj_Datagrid->BorderStyle.length(  ) )
	pObj_Datagrid->BorderColor2 = atoi( StringUtils::Tokenize( pObj_Datagrid->BorderStyle,  ";",  pos ).c_str(  ) );
	else
	pObj_Datagrid->BorderColor2 = pObj_Datagrid->BorderColor;
	*/
	pObj_Datagrid->sSelVariable = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Variable_CONST );

	pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_CONST ).c_str(  ) )  );
	if(  !pObj_Datagrid->m_pTextStyle  )
		pObj_Datagrid->m_pTextStyle = m_mapTextStyle_Find(  1  );
	pObj_Datagrid->m_pTextStyle_FirstCol = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstColumn_CONST ).c_str(  ) )  );
	pObj_Datagrid->m_pTextStyle_FirstRow = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_FirstRow_CONST ).c_str(  ) )  );
	pObj_Datagrid->m_pTextStyle_Selected = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Selected_CONST ).c_str(  ) )  );
	pObj_Datagrid->m_pTextStyle_Highlighted = m_mapTextStyle_Find(  atoi( pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Highlighted_CONST ).c_str(  ) )  );

	if(  !pObj_Datagrid->m_pTextStyle_FirstCol  )
		pObj_Datagrid->m_pTextStyle_FirstCol = pObj_Datagrid->m_pTextStyle;
	if(  !pObj_Datagrid->m_pTextStyle_FirstRow  )
		pObj_Datagrid->m_pTextStyle_FirstRow = pObj_Datagrid->m_pTextStyle;
	if(  !pObj_Datagrid->m_pTextStyle_Selected  )
		pObj_Datagrid->m_pTextStyle_Selected = pObj_Datagrid->m_pTextStyle;

	pos=0;
	string AltStyles = pObj_Datagrid->GetParameterValue( DESIGNOBJPARAMETER_PK_Style_Alt_CONST );
	int Counter=0;
	while( true )
	{
		string Style = StringUtils::Tokenize( AltStyles, ", ", pos );
		if(  Style.length(  )==0  )
			break;
		TextStyle *pTextStyle = m_mapTextStyle_Find(  atoi( Style.c_str(  ) )  );
		if(  pTextStyle  )
		{
			while(int(pObj_Datagrid->m_vectTextStyle_Alt.size()) < Counter)
				pObj_Datagrid->m_vectTextStyle_Alt.push_back(NULL);
			pObj_Datagrid->m_vectTextStyle_Alt.push_back(pTextStyle);
			//              pObj_Datagrid->m_vectTextStyle_Alt[Counter] = pTextStyle;
		}
		Counter++;
	}

	// When there are child cells, don't resize the grid because the cells are placed correctly, not rounded to Row/ColHeight
	if( pObj_Datagrid->m_mapChildDgObjects.size()==0 )
	{
		//also adjust the datagrid height not to show the black slide on the bottom.
		if( m_iRotation == 90 || m_iRotation == 270 )
			pObj_Datagrid->m_rPosition.Width =
			pObj_Datagrid->m_MaxRow * pObj_Datagrid->m_FixedRowHeight +
			(pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1);
		else
			pObj_Datagrid->m_rPosition.Height =
			pObj_Datagrid->m_MaxRow * pObj_Datagrid->m_FixedRowHeight +
			(pObj_Datagrid->m_MaxRow - 1) * (pObj_Datagrid->m_RowSpacing + 1);
	}
	// TODO -- Pull the m_mapNumberOfPagesToCache from a designobjparameter
	pObj_Datagrid->m_mapNumberOfPagesToCache[DIRECTION_Up_CONST]=10;
	pObj_Datagrid->m_mapNumberOfPagesToCache[DIRECTION_Down_CONST]=10;
	pObj_Datagrid->m_mapNumberOfPagesToCache[DIRECTION_Left_CONST]=10;
	pObj_Datagrid->m_mapNumberOfPagesToCache[DIRECTION_Right_CONST]=10;
	pObj_Datagrid->m_bParsed=true;
}

//<-dceag-c126-b->

	/** @brief COMMAND: #126 - Guide */
	/** Go to the media guide if applicable, otherwise forward to media plugin. */

void Orbiter::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
// AB May-03-06, don't think this is used anymore
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** First time stops any media playing.  Second time turns the display off.  Third time powers off the media director. */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void Orbiter::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"toggle power m_bDisplayOn was %d",(int) m_bDisplayOn);
#endif
	if( m_dwPK_Device_NowPlaying )
	{
		DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,0,0,0,StringUtils::itos( m_pLocationInfo->PK_EntertainArea ),false);
		SendCommand(CMD_MH_Stop_Media);
	}
	else
		CMD_Goto_Screen( "", SCREEN_Halt_System_CONST );
	/*
	{
		if( m_bDisplayOn )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Powering off monitor");
#endif
		CMD_Display_OnOff("0",false);
	}
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Monitor already off");
#endif
		CMD_Display_OnOff("0",false);
	  TODO -- This makes it too easy to power off accidentally by hitting the button multiple times
		For now the user will just hit the 'power' option
		DCE::CMD_Halt_Device CMD_Halt_Device(m_dwPK_Device,m_dwPK_Device_LocalAppServer,m_dwPK_Device,"0");
		SendCommand(CMD_Halt_Device);
	}
		*/
}

//<-dceag-c240-b->

	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** If at a remote control, forward to media plugin.  Otherwise go back 1 screen. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_bIsARemoteControl )
	{
		DCE::CMD_Back_Prior_Menu CMD_Back_Prior_Menu(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,0);
		SendCommand(CMD_Back_Prior_Menu);
	}
	else
		CMD_Go_back("","");
}

//<-dceag-c363-b->

	/** @brief COMMAND: #363 - Back / Clear Entry */
	/** If at a remote control, forward to media plugin.  Otherwise clear typing */

void Orbiter::CMD_Back_Clear_Entry(string &sCMD_Result,Message *pMessage)
//<-dceag-c363-e->
{
	if( m_pScreenHistory_Current && m_pScreenHistory_Current->GetObj()->m_bIsARemoteControl )
	{
		DCE::CMD_Back_Clear_Entry CMD_Back_Clear_Entry(m_dwPK_Device,m_dwPK_Device_MediaPlugIn);
		SendCommand(CMD_Back_Clear_Entry);
	}
	else
		CaptureKeyboard_EditText_DeleteLastChar(  );
}
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the highlighted item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
	if(GotActivity(BUTTON_Enter_CONST))
	{
		/*
		PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
		if(m_pObj_Highlighted && !m_pObj_Highlighted->IsHidden() )
		{
			// Selected the highlighted object as well as it's parents.  If this was clicked, this would happen
			// automatically, but since we're using the navigation we have to walk the tree ourselves
			DesignObj_Orbiter *pObj = m_pObj_Highlighted;
			while(pObj)
			{
				SelectedObject(pObj,smNavigation);
				pObj = (DesignObj_Orbiter *) pObj->m_pParentObject;
			}
		}

		HandleButtonEvent(BUTTON_Any_key_CONST);
		*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Enter_CONST), 0, "");
	}
#ifdef DEBUG
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring enter");
	}
#endif
}

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlight up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	if(GotActivity(BUTTON_Up_Arrow_CONST))
	{
		/*
		HighlightNextObject(DIRECTION_Up_CONST);
		HandleButtonEvent(BUTTON_Any_key_CONST);
		*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Up_Arrow_CONST), 0, "");
	}
#ifdef DEBUG
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlight down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	if(GotActivity(BUTTON_Down_Arrow_CONST))
	{
		/*
		HighlightNextObject( DIRECTION_Down_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Down_Arrow_CONST), 0, "");
	}
#ifdef DEBUG
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlight left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	if( GotActivity( BUTTON_Left_Arrow_CONST ) )
	{
		/*
		HighlightNextObject( DIRECTION_Left_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Left_Arrow_CONST), 0, "");
	}
#ifdef DEBUG
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring move");
	}
#endif
}

//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlight right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	if( GotActivity(BUTTON_Right_Arrow_CONST) )
	{
		/*
		HighlightNextObject( DIRECTION_Right_CONST );
		HandleButtonEvent(BUTTON_Any_key_CONST);*/
		CMD_Simulate_Keypress(StringUtils::ltos(BUTTON_Right_Arrow_CONST), 0, "");
	}
#ifdef DEBUG
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring move");
	}
#endif
}
//<-dceag-c689-b->

	/** @brief COMMAND: #689 - Update Time Code */
	/** Update the time code of the current media */
		/** @param #41 StreamID */
			/** The Stream to update */
		/** @param #102 Time */
			/** The current time.  If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
		/** @param #132 Total */
			/** If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
		/** @param #133 Speed */
			/** The current speed */
		/** @param #134 Title */
			/** For DVD's, the title */
		/** @param #135 Section */
			/** For DVD's, the section */

void Orbiter::CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,Message *pMessage)
//<-dceag-c689-e->
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	string::size_type tabTime = sTime.find('\t');
	string::size_type tabTotal = sTotal.find('\t');

	if( tabTime==string::npos )
	{
		if( m_pObj_NowPlaying_TimeShort_OnScreen )
			m_sNowPlaying_TimeShort = sTime;

		if( m_pObj_NowPlaying_TimeLong_OnScreen )
			m_sNowPlaying_TimeLong = sTime;
	}
	else
	{
		if( m_pObj_NowPlaying_TimeShort_OnScreen || m_pObj_NowPlaying_TimeLong_OnScreen )
			m_sNowPlaying_TimeShort = sTime.substr(0,tabTime);

		if( m_pObj_NowPlaying_TimeLong_OnScreen )
			m_sNowPlaying_TimeLong = m_sNowPlaying_TimeShort + " (" + sTime.substr(tabTime) + ")";
	}

	if( sTotal.size() )
	{
		m_sNowPlaying_TimeLong += m_mapTextString[TEXT_Total_CONST];

		if( tabTotal==string::npos )
			m_sNowPlaying_TimeLong += sTotal;
		else
			m_sNowPlaying_TimeLong = sTotal.substr(0,tabTotal) + " (" + sTime.substr(tabTotal) + ")";
	}

	m_sNowPlaying_Speed = sSpeed;

	DesignObj_Orbiter *pObj_NowPlaying_TimeShort_OnScreen = m_pObj_NowPlaying_TimeShort_OnScreen && m_pObj_NowPlaying_TimeShort_OnScreen->m_bOnScreen ? m_pObj_NowPlaying_TimeShort_OnScreen : NULL;
	DesignObj_Orbiter *pObj_NowPlaying_TimeLong_OnScreen = m_pObj_NowPlaying_TimeLong_OnScreen && m_pObj_NowPlaying_TimeLong_OnScreen->m_bOnScreen ? m_pObj_NowPlaying_TimeLong_OnScreen : NULL;
	DesignObj_Orbiter *pObj_NowPlaying_Speed_OnScreen = m_pObj_NowPlaying_Speed_OnScreen && m_pObj_NowPlaying_Speed_OnScreen->m_bOnScreen && m_pObj_NowPlaying_Speed_OnScreen!=m_pObj_NowPlaying_TimeShort_OnScreen ? m_pObj_NowPlaying_Speed_OnScreen : NULL;

	vm.Release();

	NeedToRender render( this, "Time Code" );
	if( pObj_NowPlaying_TimeShort_OnScreen )
		m_pOrbiterRenderer->RenderObjectAsync(pObj_NowPlaying_TimeShort_OnScreen);
	if( pObj_NowPlaying_TimeLong_OnScreen )
		m_pOrbiterRenderer->RenderObjectAsync(pObj_NowPlaying_TimeLong_OnScreen); // warning: will never be executed ?
	if( pObj_NowPlaying_Speed_OnScreen )
		m_pOrbiterRenderer->RenderObjectAsync(pObj_NowPlaying_Speed_OnScreen);

#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		if( m_pMouseBehavior )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Update_Time_Code SetMediaInfo-%s-%s-%s-%s-%s",
				sTime.c_str(),sTotal.c_str(),sSpeed.c_str(),sTitle.c_str(),sSection.c_str());
#endif
			m_pMouseBehavior->SetMediaInfo(sTime,sTotal,sSpeed,sTitle,sSection,true);
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Update_Time_Code SetMediaInfo np:%s showing %d",
			m_sNowPlaying_Speed.c_str(),(int) m_bShowingSpeedBar);
#endif
		if( m_sNowPlaying_Speed.empty()==true && m_bShowingSpeedBar )
		{
			// We displayed a speed bar, per below.  Remove it
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Update_Time_Code m_pScreenHistory_Current %p current %d osd %d",
			m_pScreenHistory_Current,m_pScreenHistory_Current->PK_Screen(),m_iPK_Screen_OSD_Speed);
#endif
			if( m_pScreenHistory_Current && m_pScreenHistory_Current->PK_Screen()==m_iPK_Screen_OSD_Speed )
				CMD_Go_back("","1");
			/*
			if( m_bIsOSD && m_iPK_Screen_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
				CMD_Goto_Screen("",m_iPK_Screen_RemoteOSD);
			else if( m_iPK_Screen_Remote )
				CMD_Goto_Screen("",m_iPK_Screen_Remote);
				*/
			if( m_pMouseBehavior )
				m_pMouseBehavior->Clear(false);
			m_bShowingSpeedBar=false;
		}
		else if( m_sNowPlaying_Speed.empty()==false && m_bContainsVideo && m_iPK_Screen_OSD_Speed && m_pScreenHistory_Current && 
			m_pScreenHistory_Current->PK_Screen()!=m_iPK_Screen_OSD_Speed && !m_bShowingSpeedBar 
			&& m_pMouseBehavior && m_pMouseBehavior->m_tIgnoreSpeedChangesUntil<time(NULL) )
		{
			// If we're not on the speed screen already (ie the speed was adjusted with an i/r remote or other means besides this osd)
			// Display the speed bar just as a goto designobj, not a full screen, and we'll go back when the speed returns to normal
			m_bShowingSpeedBar=true;  // SpeedMouseHandler knows to ignore mouse movements when this is true
			CMD_Goto_Screen("",m_iPK_Screen_OSD_Speed);
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Update_Time_Code going to %d m_sNowPlaying_Speed %s m_bContainsVideo %d PK_Screen %d time %d/%d",
				m_iPK_Screen_OSD_Speed,m_sNowPlaying_Speed.c_str(),(int) m_bContainsVideo,m_pScreenHistory_Current->PK_Screen(),
				(int) m_pMouseBehavior->m_tIgnoreSpeedChangesUntil,(int) time(NULL));
#endif
		}
	}
#endif
}

int Orbiter::HandleNotOKStatus(string sStatus,string sRegenStatus,int iRegenPercent)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleNotOKStatus %s",sStatus.c_str());
	if( sStatus=="R" || sStatus=="r" )
		return MonitorRegen(m_dwPK_Device);

	if( sStatus=="N" || ((sStatus=="D" || sStatus=="U") && !m_dwPK_Device) )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes - Reset it now";
		mapPrompts[prNo]     = "No - I'll do it later";
		int iResponse = m_pOrbiterRenderer->PromptUser("This new Orbiter is ready to go.  But all your devices need to do a reload before you can use it.  This takes about 15 seconds.", 0, &mapPrompts);
		if( iResponse==prYes )
		{
			string sResponse;
			Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
			event_Impl.m_pClientSocket->SendString( "RELOAD" );
			if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
			{
				CannotReloadRouter();
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
			}
			Sleep(10000);
			return 2; // Retry
		}
		else
		{
			OnReload();
			exit(1);
		}

		return 0; // Quit
	}
	else if( sStatus=="n" )
	{
		RegenOrbiter();
		OnReload();
		exit(2);
		return 0;
	}
	else if( sStatus=="D" || sStatus=="U" )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes]    = "Yes";
		mapPrompts[prNo]     = "No";

		int iResponse = prNo;
		if( sStatus=="D" )
			iResponse = m_pOrbiterRenderer->PromptUser("Something went wrong. The device number, " + StringUtils::itos(m_dwPK_Device) + ", doesn't seem to be an orbiter.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);
		else
			iResponse = m_pOrbiterRenderer->PromptUser("Something went wrong.  The device number, " + StringUtils::itos(m_dwPK_Device) + ", is not known to the Core.  Reset the device number and try next time to determine it automatically?",0,&mapPrompts);

		if( iResponse==prYes )
		{
			m_dwPK_Device = 0;
			Simulator::GetInstance()->m_sDeviceID = "";
			Simulator::GetInstance()->SaveConfigurationFile();
		}
		OnReload();
		exit(1);
		return 0;
	}

	return 1;
}

bool Orbiter::RouterNeedsReload()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::RouterNeedsReload");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	string sStatus,sRegenStatus;
	int iRegenPercent;
	DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		m_dwPK_Device,&sStatus,&sRegenStatus,&iRegenPercent);
	CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		m_pOrbiterRenderer->PromptUser("Sorry.  There is a problem creating the new orbiter");
		return 0;
	}
	CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
	delete pResponse;

	HandleNotOKStatus(sStatus,sRegenStatus,iRegenPercent);
	return false;  // If the user wants to do a reload, this will have already sent the request
}

int Orbiter::DeviceIdInvalid()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::DeviceIdInvalid");
	if( m_dwPK_Device )
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo};
		mapPrompts[prYes] = "Yes - Clear the stored device id";
		mapPrompts[prNo]  = "No - I'll do it later";

		int nResponse = m_pOrbiterRenderer->PromptUser(
			"The device number, " + StringUtils::itos(m_dwPK_Device) + 
				", is reported as invalid. Shall I clear the stored device id and try again?",
				300, &mapPrompts);

		if(prYes != nResponse)
		{
			OnReload();
			exit(1);
			return 0;
		}
	}

	int PK_Device = 0;
	if(IsSelfInstallable())
	{
		//Skip prompt dialogs. Create the orbiter automatically, using preseeded install values
		PK_Device = SetupNewOrbiter();
	}
	else
	{
		map<int,string> mapPrompts;
		enum PromptsResp {prYes, prNo, prCancel};
		mapPrompts[prYes]    = "Yes - This is a new Orbiter";
		mapPrompts[prNo]     = "No - There is already a Device ID for this Orbiter";
		mapPrompts[prCancel] = "Cancel";
		int iResponse = m_pOrbiterRenderer->PromptUser("This seems to be a new Orbiter.  Shall I set it up for you?", 0, &mapPrompts);
		if( iResponse == prCancel || PROMPT_CANCEL == iResponse )
		{
			OnReload();
			exit(1);
			return 0;
		}
		if( iResponse == prYes )
			PK_Device=SetupNewOrbiter();
		else
		{
			PK_Device=PickOrbiterDeviceID();
			if(PROMPT_CANCEL == PK_Device)
			{
				OnReload();
				exit(1);
				return 0;
			}
		}
	}

	if( !PK_Device )
	{
		OnReload();
		exit(1);
	}

	return PK_Device;
}

int Orbiter::PickOrbiterDeviceID()
{
	map<int,string> mapDevices;
	GetDevicesByCategory(DEVICECATEGORY_Orbiter_CONST,&mapDevices);
	return m_pOrbiterRenderer->PromptUser("Which Orbiter is this?  Be careful.  Don't choose an Orbiter that is running on another device or it will be disconnected when this one connects.",0, &mapDevices);
}
//-----------------------------------------------------------------------------------------------------
int Orbiter::SetupNewOrbiter()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"start SetupNewOrbiter");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	while(true)
	{
		string sResponse;
		if( !event_Impl.m_pClientSocket->SendString("READY") ||
			!event_Impl.m_pClientSocket->ReceiveString(sResponse) ||
			sResponse.size()==0 )
			return 0;  // Something went wrong

		if( sResponse=="YES" )
			break;
		Sleep(2000);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SetupNewOrbiter prompting for inputs");

	int PK_Users = m_pOrbiterRenderer->PromptFor("Users");
	if( PROMPT_CANCEL == PK_Users )
		return 0;

	int PK_Room = m_pOrbiterRenderer->PromptFor("Room");
	if( PROMPT_CANCEL == PK_Room )
		return 0;

	int PK_Skin = m_pOrbiterRenderer->PromptFor("Skin");
	if( PROMPT_CANCEL == PK_Skin )
		return 0;

	int PK_Language = m_pOrbiterRenderer->PromptFor("Language");
	if( PROMPT_CANCEL == PK_Language )
		return 0;

	map<int,string> mapResponseYNC;
	mapResponseYNC[0]="Yes";
	mapResponseYNC[1]="No";
	mapResponseYNC[2]="Cancel";

	int Width=0,Height=0;
	string sType;
#ifdef WIN32
#ifdef WINCE
	sType="CE";
#else
	sType="Windows";
#endif
	RECT rc;
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &rc);
	Width=rc.right;
	Height=rc.bottom;
#else
	sType="Linux";
	// HOW to do this??  TODO
#endif

#ifdef PADORBITER
	int PK_Size=1;
	Width=800;
	Height=600;
#else
	int PK_Size=0;
	PK_Size=m_pOrbiterRenderer->PromptFor("Size");
	if( PROMPT_CANCEL == PK_Size)
		return 0;
#endif

#ifdef PADORBITER
	int PK_Device=0;
	DCE::CMD_New_Orbiter_DT CMD_New_Orbiter_DT(m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse, sType,
		PK_Users,DEVICETEMPLATE_Orbiter_CONST,m_sMacAddress,PK_Room,Width,Height,PK_Skin,PK_Language,PK_Size,&PK_Device);
#else
	int PK_Device=0;
	DCE::CMD_New_Orbiter_DT CMD_New_Orbiter_DT(m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse, sType,
		PK_Users,m_dwPK_DeviceTemplate,m_sMacAddress,PK_Room,Width,Height,PK_Skin,PK_Language,PK_Size,&PK_Device);

#endif

	CMD_New_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
	Message *pResponse = event_Impl.SendReceiveMessage( CMD_New_Orbiter_DT.m_pMessage );
	if( !pResponse || pResponse->m_dwID != 0 )
	{
		if(pResponse)
			delete pResponse;

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter");
		m_pOrbiterRenderer->PromptUser("Sorry.  There is a problem creating the new orbiter.  Please check the logs.");
		return 0;
	}
	CMD_New_Orbiter_DT.ParseResponse( pResponse );
	delete pResponse;

	if( !PK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SetupNewOrbiter unable to create orbiter #2");
		m_pOrbiterRenderer->PromptUser("Sorry.  Orbiter Plugin could not create the device for some reason.  Please check the logs.");
		return 0;
	}

	Simulator::GetInstance()->m_sRouterIP = m_sIPAddress;
	Simulator::GetInstance()->m_sDeviceID = StringUtils::itos(PK_Device);
	Simulator::GetInstance()->SaveConfigurationFile();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SetupNewOrbiter new orbiter %d",PK_Device);

	if( MonitorRegen(PK_Device)==0 )  // User hit cancel
	{
		OnReload();
		exit(1);
		return 0; // Don't retry to load now
	}


	return PK_Device;  // Retry loading as the specified device
}
//-----------------------------------------------------------------------------------------------------
int Orbiter::MonitorRegen(int PK_Device)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MonitorRegen - starting");
	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	while(true)
	{
		string sStatus,sRegenStatus;
		int iRegenPercent;
		DCE::CMD_Get_Orbiter_Status_DT CMD_Get_Orbiter_Status_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
			PK_Device,&sStatus,&sRegenStatus,&iRegenPercent);

		CMD_Get_Orbiter_Status_DT.m_pMessage->m_eExpectedResponse = ER_ReplyMessage;
		Message *pResponse = event_Impl.SendReceiveMessage( CMD_Get_Orbiter_Status_DT.m_pMessage );
		if( !pResponse || pResponse->m_dwID != 0 )
		{
			if(pResponse)
				delete pResponse;

			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MonitorRegen - unable to check status");
			m_pOrbiterRenderer->DisplayProgress("",-1);
			m_pOrbiterRenderer->PromptUser("Sorry.  There is a problem creating the new orbiter");
			return 0;
		}
		CMD_Get_Orbiter_Status_DT.ParseResponse( pResponse );
		delete pResponse;

		if( sStatus!="r" && sStatus!="R" )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MonitorRegen - DONE");
			break;
		}

		if( m_pOrbiterRenderer->DisplayProgress(sRegenStatus,iRegenPercent < 100 ? iRegenPercent : 100) )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MonitorRegen - User cancelled");
			m_pOrbiterRenderer->DisplayProgress("",-1);
			return 0; // Don't try to start
		}
		Sleep(5000);
	}

	m_pOrbiterRenderer->DisplayProgress("",-1);
	OnReload();
	return 2; // Try again
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::RegenOrbiter()
{
	LoggerWrapper::GetInstance()->Write( LV_STATUS,  "Orbiter::RegenOrbiter" );

	Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sIPAddress);
	string sResponse;
	DCE::CMD_Regen_Orbiter_DT CMD_Regen_Orbiter_DT( m_dwPK_Device, DEVICETEMPLATE_Orbiter_Plugin_CONST, BL_SameHouse,
		m_dwPK_Device,"","");
	CMD_Regen_Orbiter_DT.m_pMessage->m_eExpectedResponse = ER_DeliveryConfirmation;
	if( !event_Impl.SendMessage(CMD_Regen_Orbiter_DT.m_pMessage,sResponse) || sResponse!="OK" )
	{
		m_pOrbiterRenderer->PromptUser("Sorry.  I was unable to send this message to the Core.  Please try again or use the Pluto Admin site.");
		return true;
	}
	m_pOrbiterRenderer->PromptUser("The UI is being regenerated.  This will take 15-30 minutes.  If you get this same message again after the regeneration is finished, then that means the generator on the Core is too old and you will need to reset your Core so it can update itself.  Click 'OK' to monitor the progress.");
	return false;
}


void ReceivePingHandler(Socket *pSocket)
{

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivePingHandler - got ping for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
	pSocket->SendString("PONG");
	Orbiter *pOrbiter = (Orbiter *) g_pCommand_Impl;
	if( !pOrbiter || pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_mnuPingPong_CONST )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	else
	{
		string sMessage = StringUtils::PrecisionTime() + " Got PING: " +
			" S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			//+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + 
			+ "#" + StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		NeedToRender render( pOrbiter, "ReceivePingHandler" );
		DesignObjText *pText = pOrbiter->FindText( pOrbiter->m_pScreenHistory_Current->GetObj(), TEXT_STATUS_CONST );
		if( pText  )
			pText->m_sText = sMessage + pText->m_sText;

		pOrbiter->Renderer()->RenderObjectAsync(pOrbiter->m_pScreenHistory_Current->GetObj());
	}
}

bool SendPingHandler(Socket *pSocket)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivePingHandler - sending ping for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());

	Orbiter *pOrbiter = (Orbiter *) g_pCommand_Impl;
	if( !pOrbiter )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	NeedToRender render( pOrbiter, "SendPingHandler" );
	DesignObjText *pText = pOrbiter->FindText( pOrbiter->m_pScreenHistory_Current->GetObj(), TEXT_STATUS_CONST );
	if( pText  )
	{
		string sMessage = StringUtils::PrecisionTime() + " Send PING: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			//+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->Renderer()->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
	}

	string sResponse=pSocket->SendReceiveString("PING",PING_TIMEOUT);
	if( sResponse!="PONG" )
	{
		string sMessage = StringUtils::PrecisionTime() + " ***FAILED TO GET PONG***: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			//+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->Renderer()->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivePingHandler - failed to get pong for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
		return false;
	}
	if( !g_pCommand_Impl || pOrbiter->m_bQuit_get()) // The app closed on us
		return true;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReceivePingHandler - got pong for %p %d %s",pSocket,pSocket->m_Socket,pSocket->m_sName.c_str());
	if( pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID!=DESIGNOBJ_mnuPingPong_CONST )
	{
		g_pReceivePingHandler=NULL;
		g_pSendPingHandler=NULL;
	}
	else if( pText )
	{
		string sMessage = StringUtils::PrecisionTime() + " GOT PONG: "
			+ " S" + StringUtils::itos(pOrbiter->m_ScreenMutex.m_NumLocks) + " V" + StringUtils::itos(pOrbiter->m_VariableMutex.m_NumLocks)
			+ " D" + StringUtils::itos(pOrbiter->m_DatagridMutex.m_NumLocks) + " M" + StringUtils::itos(pOrbiter->m_MaintThreadMutex.m_NumLocks)
			//+ " R" + StringUtils::itos(pOrbiter->m_NeedRedrawVarMutex.m_NumLocks) + "#"
			+ StringUtils::itos(int(pSocket->m_Socket)) + " " + pSocket->m_sName + "\n";
		pText->m_sText = sMessage + pText->m_sText;
		pOrbiter->Renderer()->RenderObjectAsync( pOrbiter->m_pScreenHistory_Current->GetObj() );
	}
	return true;
}

bool Orbiter::WaitForRelativesIfOSD()
{
	if(m_bQuit_get())
		return true;

#ifdef NO_WAIT_FOR_RELATIVES
    return true;
#endif

	if( !m_bIsOSD )
		return true;

	time_t tTimeout=time(NULL) + 20; // Wait 20 seconds for child devices to register
	map<int,bool> mapUnregisteredRelatives;
  map<string, bool> mapChildDevices;
	while( true )
	{
		int iUnregisteredRelatives = FindUnregisteredRelatives(&mapUnregisteredRelatives);
		if( !iUnregisteredRelatives )
			break;
		if( time(NULL)>tTimeout )
		{
			string sDevices;
			for(map<int,bool>::iterator it=mapUnregisteredRelatives.begin();it!=mapUnregisteredRelatives.end();++it)
			{
				if( it->second==false )
					sDevices += StringUtils::itos(it->first) + ",";
			}

			string sMessage = m_mapTextString[TEXT_Not_all_devices_started_CONST];
			CMD_Display_Alert(sMessage, "relatives_failed", "10", interuptAlways);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Continuing anyway with %d devices not registered: %s",iUnregisteredRelatives,sDevices.c_str());
			break;
		}

		string sDescription = m_mapTextString[TEXT_Waiting_for_related_devices_CONST];

		for(map<int,bool>::iterator it=mapUnregisteredRelatives.begin();it!=mapUnregisteredRelatives.end();++it)
		{
			if( it->second )
				continue;
			DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(it->first);
			sDescription += "  #" + StringUtils::itos(it->first) + "/" + (pDevice ? pDevice->m_sDescription : string(""));

		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Waiting %d devices %s",iUnregisteredRelatives,sDescription.c_str());
		if( m_pOrbiterRenderer->DisplayProgress(sDescription,100-(iUnregisteredRelatives*100/int(mapUnregisteredRelatives.size()))) )
		{
			m_pOrbiterRenderer->DisplayProgress("",-1);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::WaitForRelativesIfOSD user wants to abort");
			OnReload();
			return false;
		}
		Sleep(1000); // Sleep and try again
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::WaitForRelativesIfOSD exiting");
	m_pOrbiterRenderer->DisplayProgress("",-1);
	return true;
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-c741-b->

	/** @brief COMMAND: #741 - Goto Screen */
	/** Goto a specific screen. */
		/** @param #10 ID */
			/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
		/** @param #159 PK_Screen */
			/** The screen id. */
		/** @param #251 Interruption */
			/** Indicates at what times to ignore the screen change if it would interrupt the user.  A value in: enum eInterruption */
		/** @param #252 Turn On */
			/** If true, turn the display on if it's off. */
		/** @param #253 Queue */
			/** If true, then if the screen change was ignored so as not to interrpt the user, queue it for when the user is done */

void Orbiter::CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c741-e->
{
	if(iPK_Screen == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Can't go to screen 0");
		return;
	}

	if( m_pScreenHistory_Current && m_mapScreen_Interrupt[m_pScreenHistory_Current->PK_Screen()]==0 && !OkayToInterrupt(iInterruption) )
	{
		if (bQueue)
		{
#ifdef WIN32
			m_listPendingGotoScreens.push_back(make_pair (iInterruption, new Message(pMessage)));
#else
			m_listPendingGotoScreens.push_back(make_pair<int, Message *>(iInterruption, new Message(pMessage)));
#endif
		}
		return;
	}

	if( bTurn_On && !m_bDisplayOn )
	{
		// If this is an OSD, and it's displaying an external media source, and we're going to the remote we assume Media_Plugin turned the display on, in all other cases turn it on. 
		if ( m_bIsOSD && m_bUsingLiveAVPath && iPK_Screen==m_iPK_Screen_RemoteOSD)
		{
			// HACK: ideally orbiter should know if Media_Plugin turned the display pipes on or not 
			m_bDisplayOn=true;
		}
		else
		{
			CMD_Display_OnOff( "1",false );
		}

	}
	
	// If this is an OSD, and it's displaying an external media source, and the a/v equipment's inputs are switched to that source, switch back if we're going to a screen other than the remote
	if( m_bIsOSD && m_iPK_Screen_RemoteOSD && m_iLocation_Initial==m_pLocationInfo->iLocation && m_bUsingLiveAVPath && iPK_Screen!=m_iPK_Screen_RemoteOSD )
	{
		DCE::CMD_Live_AV_Path CMD_Live_AV_Path(m_dwPK_Device,m_dwPK_Device_MediaPlugIn,StringUtils::itos(m_pLocationInfo->PK_EntertainArea),false);
		SendCommand(CMD_Live_AV_Path);
	}

	if( iPK_Screen==SCREEN_Main_CONST )
	{
		if( m_bNewOrbiter && atoi(m_sInitialScreen.c_str())==SCREEN_VideoWizard_CONST )
			iPK_Screen=SCREEN_Wizard_Done_CONST;  // If the user wants to get out, take the user to the end so he can press 'start using the system'
	}

	SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbOnGotoScreen, GotoScreenCallBackData, (iPK_Screen, pMessage))
	if(ExecuteScreenHandlerCallback(cbOnGotoScreen))
		return;

	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	m_pContextToBeRestored = NULL; // Won't restore any pending context.  It's possible there was a go back that set this and hasn't yet finished rendering before a goto screen
	cm.Release();

	bool bCreatedMessage=pMessage==NULL;
	if( pMessage==NULL )
		pMessage = new Message();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Goto_Screen message id %d params: message %p", iPK_Screen, pMessage);
	for(map<long, string>::iterator it = pMessage->m_mapParameters.begin(); it != pMessage->m_mapParameters.end(); it++)
	{
		string sValue = it->second;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Param id: %5d, value: %s", it->first, sValue.c_str());
	}

#ifdef ENABLE_MOUSE_BEHAVIOR
	// With the new UI 
	if(UsesUIVersion2() && m_pMouseBehavior )
		m_pMouseBehavior->Clear();
#endif

	m_pScreenHistory_Pivot = new ScreenHistory(iPK_Screen, m_pScreenHistory_Current,pMessage,this);
	m_pScreenHistory_Pivot->ScreenID(sID);
	m_pScreenHistory_New = m_pScreenHistory_Pivot;

	cm.Relock();
	m_pScreenHandler->ReceivedGotoScreenMessage(iPK_Screen, pMessage);
	cm.Release();

	if( bCreatedMessage )
	{
		delete pMessage;
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ ScreenHandler *Orbiter::CreateScreenHandler()
{
#ifdef TEST_OSD
	return new OSDScreenHandler(this, &m_mapDesignObj);
#else
	return new ScreenHandler(this, &m_mapDesignObj);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::ExecuteScreenHandlerCallback(CallBackType aCallBackType)
{
	if(NULL != m_pScreenHandler)
	{
		ScreenHandlerCallBack pPersistentCallBack = m_pScreenHandler->m_mapPersistentCallBack_Find(aCallBackType);
		if(NULL != pPersistentCallBack)
			if(CALL_MEMBER_FN(*m_pScreenHandler, pPersistentCallBack)(m_pScreenHandler->m_mapPersistentCallBackData_Find(aCallBackType)))
				return true;

		ScreenHandlerCallBack pCallBack = m_pScreenHandler->m_mapCallBack_Find(aCallBackType);
		if(NULL != pCallBack)
			return CALL_MEMBER_FN(*m_pScreenHandler, pCallBack)(m_pScreenHandler->m_mapCallBackData_Find(aCallBackType));
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::GotoMainMenu()
{
	DCE::SCREEN_Main SCREEN_Main_(m_dwPK_Device, m_dwPK_Device,
		StringUtils::ltos(m_pLocationInfo->iLocation));
	string sResult;
	CMD_Goto_Screen("", SCREEN_Main_CONST,interuptAlways,false,false, sResult, SCREEN_Main_.m_pMessage);
	delete SCREEN_Main_.m_pMessage;
}
//-----------------------------------------------------------------------------------------------------
DesignObj_DataGrid *Orbiter::FindGridOnScreen(string sGridID)
{
	for(vector<class DesignObj_DataGrid *>::iterator it = m_vectObjs_GridsOnScreen.begin();
		it != m_vectObjs_GridsOnScreen.end(); ++it)
	{
		DesignObj_DataGrid *pDesignObj_DataGrid = *it;
		if(pDesignObj_DataGrid->m_sGridID == sGridID)
			return pDesignObj_DataGrid;
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::ServiceScreenHandler(void *data)
{
	PLUTO_SAFETY_LOCK(vm, m_VariableMutex);
	if(!ExecuteScreenHandlerCallback(cbOnTimer))
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ServiceScreenHandler stopping");
		return;
	}

LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ServiceScreenHandler keep going");
	int *pInterval = (int *)data;
	StartScreenHandlerTimer(*pInterval);
	delete pInterval;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::StartScreenHandlerTimer(int nInterval /*in miliseconds*/)
{
	CallMaintenanceInMiliseconds(nInterval, &Orbiter::ServiceScreenHandler, new int(nInterval), pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::ScreenHandlerMsgInterceptor( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );

	CallBackData *pCallBackData = m_pScreenHandler->m_mapCallBackData_Find(cbMessageIntercepted);
	if(pCallBackData)
	{
		SETUP_SCREEN_HANDLER_CALLBACK(m_pScreenHandler, cbMessageIntercepted, MsgInterceptorCellBackData, 
			(pSocket, pMessage, pDeviceFrom, pDeviceTo))
		return ExecuteScreenHandlerCallback(cbMessageIntercepted);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::PendingCallbackScheduled(OrbiterCallBack fnCallBack)
{
	PLUTO_SAFETY_LOCK( cm, m_MaintThreadMutex );
	for(map<int, PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		if(pCallBackInfo->m_fnCallBack == fnCallBack)
			return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter::IsSelfInstallable()
{
	Simulator *pSimulator = Simulator::GetInstance();

	return
		pSimulator->PreseededInstallValueDefined("Users")		&&
		pSimulator->PreseededInstallValueDefined("Room")		&&
		pSimulator->PreseededInstallValueDefined("Skin")		&&
		pSimulator->PreseededInstallValueDefined("Language")	&&
		pSimulator->PreseededInstallValueDefined("Size");
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::GetChildrenVisibilityContext(DesignObj_Orbiter *pObj,
										  map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	if(NULL != pObj)
	{
		DesignObj_DataList::iterator it;
		for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
		{
			DesignObj_Orbiter *pChildObj = (DesignObj_Orbiter *)*it;
			mapVisibilityContext[pChildObj] = pChildObj->IsHidden();

			GetChildrenVisibilityContext(pChildObj, mapVisibilityContext);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void Orbiter::SetChildrenVisibilityContext(DesignObj_Orbiter *pObj,
											const map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	DesignObj_DataList::const_iterator it;
	map<DesignObj_Orbiter *, bool>::const_iterator itmap;
	for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
	{
		DesignObj_Orbiter *pChildObj = (DesignObj_Orbiter *)*it;

		if(NULL == pChildObj)
			continue;

		itmap = mapVisibilityContext.find(pChildObj);
		if(itmap != mapVisibilityContext.end() && pChildObj->m_bHidden != itmap->second)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "* Changing visibility status for %s from %d to %d",
				pChildObj->m_ObjectID.c_str(), pChildObj->m_bHidden, itmap->second);
			pChildObj->m_bHidden = itmap->second;
		}

		SetChildrenVisibilityContext(pChildObj, mapVisibilityContext);
	}
}
//-----------------------------------------------------------------------------------------------------

//<-dceag-c795-b->

	/** @brief COMMAND: #795 - Set Mouse Behavior */
	/** Indicates if the mouse should be locked to horizontal or vertical movements, how to handle range of motion, etc. */
		/** @param #3 PK_DesignObj */
			/** An object to lock the movement to */
		/** @param #39 Options */
			/** The following letter(s): [r/a]ramp/absolute */
		/** @param #126 Exclusive */
			/** If true, all existing mouse behavior will be removed */
		/** @param #211 Direction */
			/** a letter: [h]orizontal, [v]ertical, [b]oth */

void Orbiter::CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection,string &sCMD_Result,Message *pMessage)
//<-dceag-c795-e->
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	if(UsesUIVersion2())
	{
		if( m_pMouseBehavior )
			m_pMouseBehavior->Set_Mouse_Behavior(sOptions,bExclusive,sDirection,sPK_DesignObj);
	}
#endif
}

DataGridCell *Orbiter::GetDataGridHighlightCell(DesignObj_DataGrid *pGrid)
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	DataGridTable *pDataGridTable = pGrid->DataGridTable_Get();
	if( !pDataGridTable )
		return NULL;

	if( pGrid->m_iHighlightedColumn==-1 && pGrid->m_iHighlightedRow==-1 )
	{
		if(  pGrid->m_sExtraInfo.find( 'C' )==string::npos )
			pGrid->m_iHighlightedRow=0;
		if(  pGrid->m_sExtraInfo.find( 'R' )==string::npos )
			pGrid->m_iHighlightedColumn=0;
	}

	//adjustments
	if(pGrid->m_iHighlightedColumn >= pGrid->m_MaxCol)
		pGrid->m_iHighlightedColumn = pGrid->m_MaxCol - 1;

	int nHColumn = pGrid->m_iHighlightedColumn!=-1 ? pGrid->m_iHighlightedColumn + pGrid->m_GridCurCol : pGrid->m_GridCurCol;
	int nHRow = pGrid->m_iHighlightedRow!=-1 ? pGrid->m_iHighlightedRow + pGrid->m_GridCurRow - (pDataGridTable->m_iUpRow >= 0 ? 1 : 0) : 0;

	if( nHColumn==-1 && nHRow==-1 )
		return NULL;

	if(nHRow < pDataGridTable->m_StartingRow)
		pGrid->m_iHighlightedRow = 1;

	nHRow = pDataGridTable->m_StartingRow + pGrid->m_iHighlightedRow; //set the highlighted row

	return pDataGridTable->GetData(nHColumn, nHRow);
}

void Orbiter::GetDataGridHighlightCellCoordinates(DesignObj_DataGrid *pGrid,PlutoRectangle &rect)
{
	PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );
	DataGridCell *pCell = GetDataGridHighlightCell(pGrid);

	if(NULL == pCell)
	{
		pGrid->m_iHighlightedRow--;
		pCell = GetDataGridHighlightCell(pGrid);
	}

	if( !pCell )
	{
		rect.X=rect.Y=rect.Width=rect.Height=0;
		return;
	}

	PlutoRectangle r;
	pGrid->GetGridCellDimensions(
		pGrid->m_iHighlightedColumn==-1 ? pGrid->m_MaxCol : (pCell ? pCell->m_Colspan : 1),
		pGrid->m_iHighlightedRow==-1 ? pGrid->m_MaxRow : (pCell ? pCell->m_Rowspan : 1),
		pGrid->m_iHighlightedColumn==-1 ? 0 : pGrid->m_iHighlightedColumn,
		pGrid->m_iHighlightedRow==-1 ? 0 : pGrid->m_iHighlightedRow,
		r.X,  r.Y,  r.Width,  r.Height );

	rect.X = max(0,r.X);
	rect.Y = max(0,r.Y);
	rect.Right( min(r.Right(),m_Width-1) );
	rect.Bottom( min(r.Bottom(),m_Height-1) );
}

/*virtual*/ bool Orbiter::UsesUIVersion2() 
{ 
	return 2 == m_iUiVersion; 
}
//<-dceag-c801-b->

	/** @brief COMMAND: #801 - Set Mouse Sensitivity */
	/** 1=Sensitive, 3=not sensitive */
		/** @param #48 Value */
			/** 1=sensitive, 3=least sensitive */

void Orbiter::CMD_Set_Mouse_Sensitivity(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c801-e->
{
#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pMouseBehavior )
		m_pMouseBehavior->m_MouseSensitivity.SetSensitivity(iValue);
#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter::OnReplaceHandler(string sIP)
{
    if(sIP != m_sIPAddress)
    {
        string sMessage = "An Orbiter with the same device id was started on " + sIP + ".\r\nThis Orbiter will be closed.";
        m_pOrbiterRenderer->PromptUser(sMessage);
		OnQuit();
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::ShowProgress(int nPercent) 
{
	m_pOrbiterRenderer->ShowProgress(nPercent < 100 ? nPercent : 100);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::OnQuit() 
{ 
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter quiting..."); 
	Orbiter_Command::OnQuit(); 

	if(NULL != m_pOrbiterRenderer)
		m_pOrbiterRenderer->OnQuit();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::OnReload() 
{ 
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter reloading..."); 
	Orbiter_Command::OnReload(); 
	OnQuit(); 

	m_pOrbiterRenderer->OnReload();
}  
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter::RenderFrame(void *data)
{
	m_pOrbiterRenderer->RenderFrame(data);
}
//-----------------------------------------------------------------------------------------------------
//<-dceag-c809-b->

	/** @brief COMMAND: #809 - Display Alert */
	/** Displays a short alert that appears on the orbiter as a discrete popup message that goes away automatically. */
		/** @param #9 Text */
			/** The text in the alert */
		/** @param #70 Tokens */
			/** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
		/** @param #182 Timeout */
			/** Make the alert go away after this many seconds */
		/** @param #251 Interruption */
			/** How to interrupt the user if something is happening */

void Orbiter::CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage)
//<-dceag-c809-e->
{
	string sAlertFilter = DATA_Get_Alert_Filter_Level();

	if(sAlertFilter == "none" || (sAlertFilter == "important" && iInterruption != interuptAlways))
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Display_Alert skipping: "
			"%s level of interruption %d because alert filter is '%s'",
			sText.c_str(), iInterruption, sAlertFilter.c_str());
		return;
	}

	if( m_pScreenHistory_Current && m_mapScreen_Interrupt[m_pScreenHistory_Current->PK_Screen()]==0 && !OkayToInterrupt(iInterruption) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Display_Alert skipping: %s", sText.c_str());
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Display_Alert showing: %s", sText.c_str());

	PlutoAlert *pPlutoAlert=NULL;
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

	// See if there's an existing alert with this token.  If so, we'll re-use it
	for( list<PlutoAlert *>::iterator it=m_listPlutoAlert.begin();it!=m_listPlutoAlert.end();++it )
	{
		PlutoAlert *p = *it;
		if( p->m_sToken == sTokens )
		{
			m_listPlutoAlert.erase(it); // We're going to re-add it below anyway
			pPlutoAlert = p;
			break;
		}
	}

	if( !pPlutoAlert )
	{
		pPlutoAlert = new PlutoAlert();
		pPlutoAlert->m_sToken = sTokens;
	}

	pPlutoAlert->m_sMessage = sText;
	pPlutoAlert->m_tStartTime = time(NULL);
	if( sTimeout=="NONE")
		pPlutoAlert->m_tStopTime = 0;
	else
	{
		pPlutoAlert->m_tStopTime = pPlutoAlert->m_tStartTime + atoi(sTimeout.c_str());

		// Must be a positive number
		if( pPlutoAlert->m_tStopTime<=pPlutoAlert->m_tStartTime )
			pPlutoAlert->m_tStopTime = pPlutoAlert->m_tStartTime + 3; // Default to 3 seconds

		// and no more than 60 seconds
		if( pPlutoAlert->m_tStopTime-pPlutoAlert->m_tStartTime>60 )
			pPlutoAlert->m_tStopTime = pPlutoAlert->m_tStartTime + 60; // Truncating to 60 seconds
	}

	m_listPlutoAlert.push_back(pPlutoAlert);
	vm.Release();
	CallMaintenanceInMiliseconds(0,&Orbiter::ServiceAlerts,NULL,pe_ALL,false);	
}

void Orbiter::ServiceAlerts( void *iData )
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );

	string sMessages;
	time_t t = time(NULL);
	time_t tNextStop = 0;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ServiceAlerts in queue: %d",(int) m_listPlutoAlert.size());
#endif

	// See what alerts we need to display
	for( list<PlutoAlert *>::iterator it=m_listPlutoAlert.begin();it!=m_listPlutoAlert.end(); )
	{
		PlutoAlert *p = *it;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ServiceAlerts alert %s stop: %d now: %d",p->m_sMessage.c_str(),(int) p->m_tStopTime,(int) t);
#endif
		if( p->m_tStopTime && p->m_tStopTime <= t )
		{
			delete p;
			m_listPlutoAlert.erase(it++);
		}
		else
		{
			if( sMessages.size() )
				sMessages += "\n";
			sMessages += p->m_sMessage;
			if( p->m_tStopTime && (tNextStop==0 || tNextStop>p->m_tStopTime) )
				tNextStop = p->m_tStopTime;
			it++;
		}
	}
	vm.Release();

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::ServiceAlerts in queue: %d messages %s",(int) m_listPlutoAlert.size(),sMessages.c_str());
#endif

	if( sMessages.size() )
	{
		DesignObj_Orbiter *pObj = FindObject(DESIGNOBJ_popAlertMessage_CONST);
		if( !pObj )
			return; // Shouldn't ever happen
		Renderer()->RenderObjectAsync(pObj);
		DesignObjText *pText = FindText( pObj,TEXT_STATUS_CONST );
		if( pText )
		{
			pText->m_sText = sMessages;

			TextStyle *pTextStyle = pText->m_mapTextStyle_Find(0);
			pTextStyle->m_iPixelHeight = sMessages.size() > 120 ? 12 : 16;
		}
		CMD_Show_Popup(TOSTRING(DESIGNOBJ_popAlertMessage_CONST),m_rSpacing.X,m_rSpacing.Y,"","popup_alert",false,true);
	}
	else
		CMD_Remove_Popup("","popup_alert");

	if( tNextStop )
		CallMaintenanceInMiliseconds((tNextStop - t)*1000,&Orbiter::ServiceAlerts,NULL,pe_ALL,false);	
}
//<-dceag-c810-b->

	/** @brief COMMAND: #810 - Set Active Application */
	/** Tells an On screen orbiter what application is currently active */
		/** @param #50 Name */
			/** A description of the app */
		/** @param #159 PK_Screen */
			/** The Screen for the OSD */
		/** @param #216 Identifier */
			/** The window identifier */
		/** @param #226 PK_Screen_GoTo */
			/** The screen for the orbiter remote */

void Orbiter::CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage)
//<-dceag-c810-e->
{
	m_sActiveApplication_Description=sName;
	m_sActiveApplication_Window=sIdentifier;
	m_PK_Screen_ActiveApp_OSD = iPK_Screen;
	m_PK_Screen_ActiveApp_Remote = iPK_Screen_GoTo;
	
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Set_Active_Application m_sActiveApplication_Description %s m_sNowPlaying_Window %s",
			m_sActiveApplication_Description.c_str(),m_sNowPlaying_Window.c_str());
		if( m_sActiveApplication_Description.empty() )
		{
			if( m_sNowPlaying_Window.empty()==false )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Set_Active_Application activating media that's playing simultaneously");;
				StopScreenSaver();
				CMD_Activate_Window(m_sNowPlaying_Window);
			}
			else
			{
				StartScreenSaver(false);  // Don't go to the menu, just start the app in the background
			}
		}
		else
		{
			StopScreenSaver();
			CMD_Activate_Window(m_sActiveApplication_Window);
		}
	}
}

//<-dceag-c811-b->

	/** @brief COMMAND: #811 - Get Active Application */
	/** Return the currently active application */
		/** @param #50 Name */
			/** A description of the app */
		/** @param #159 PK_Screen */
			/** The Screen for the OSD */
		/** @param #216 Identifier */
			/** The window identifier */
		/** @param #226 PK_Screen_GoTo */
			/** The Screen for the orbiter remote */

void Orbiter::CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage)
//<-dceag-c811-e->
{
	*sName=m_sActiveApplication_Description;
	*sIdentifier=m_sActiveApplication_Window;
	*iPK_Screen = m_PK_Screen_ActiveApp_OSD;
	*iPK_Screen_GoTo = m_PK_Screen_ActiveApp_Remote;
}

void Orbiter::ForceCurrentScreenIntoHistory()
{
	PLUTO_SAFETY_LOCK( vm, m_VariableMutex );
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::ForceCurrentScreenIntoHistory m_listScreenHistory Adding to screens history list screen %d", m_pScreenHistory_Current->PK_Screen());
	ScreenHistory *pScreenHistory = NeedToRender::m_pScreenHistory_get();
	if( !pScreenHistory )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter::ForceCurrentScreenIntoHistory nothing to do!");
		return;
	}
	m_pScreenHistory_Current = pScreenHistory;
	pScreenHistory->AddToHistory();
	m_listScreenHistory.push_back( pScreenHistory );
}

void *UpdateTimeCodeThread(void *p)
{
	Orbiter* pOrbiter = (Orbiter *)p;

	if(NULL != pOrbiter && !pOrbiter->m_bQuit_get())
	{
		pOrbiter->UpdateTimeCodeLoop();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode thread stopped");
		pOrbiter->m_bUpdateTimeCodeLoopRunning=false;
	}

	return NULL;
}

void Orbiter::UpdateTimeCodeLoop()
{
	if( m_bQuit_get()|| !m_bReportTimeCode )
		return;

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCodeLoop starting...");
#endif
	PLUTO_SAFETY_LOCK(tcm, m_TimeCodeMutex);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCodeLoop: got the mutex, we are ready to go! %p %d %d id %d",
		m_pAskXine_Socket, m_pAskXine_Socket==NULL ? -1 : m_pAskXine_Socket->m_dwPK_Device, m_dwPK_Device_NowPlaying,m_pAskXine_Socket==NULL ? -1 : m_pAskXine_Socket->m_Socket);
#endif

	// If this is a xine, determine the ip address and connect to it to pull time code info
	if( !m_pAskXine_Socket || m_pAskXine_Socket->m_Socket == INVALID_SOCKET || m_pAskXine_Socket->m_dwPK_Device!=m_dwPK_Device_NowPlaying )
	{
		if( m_pAskXine_Socket )
		{
			delete m_pAskXine_Socket;
			m_pAskXine_Socket=NULL;
		}
		DeviceData_Base *pDevice = m_dwPK_Device_NowPlaying ? m_pData->m_AllDevices.m_mapDeviceData_Base_Find(m_dwPK_Device_NowPlaying) : NULL;
		if( 
			NULL != pDevice && 
			(
				pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_Xine_Player_CONST ||
				pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_MPlayer_Player_CONST ||
				pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_OMX_Player_CONST ||
				pDevice->m_dwPK_DeviceTemplate==DEVICETEMPLATE_VLC_Player_CONST
			)
		)
		{
			string sIPAddress = pDevice->m_sIPAddress;
			if( sIPAddress.empty() )
			{
				if( pDevice->m_pDevice_MD && !pDevice->m_pDevice_MD->m_sIPAddress.empty() )
					sIPAddress = pDevice->m_pDevice_MD->m_sIPAddress;
				else if( pDevice->m_pDevice_Core && !pDevice->m_pDevice_Core->m_sIPAddress.empty() )
					sIPAddress = pDevice->m_pDevice_Core->m_sIPAddress;
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"Orbiter::CMD_Set_Now_Playing  Xine has no IP address");
					return;
				}
			}

			string sPort = m_pEvent->GetDeviceDataFromDatabase(pDevice->m_dwPK_Device, DEVICEDATA_Port_CONST);

			string sConnectInfo = sIPAddress + ":" + sPort;
			string sName = "ask-xine-socket";
			m_pAskXine_Socket = new AskXine_Socket(sConnectInfo, sName);
			m_pAskXine_Socket->m_dwPK_Device = pDevice->m_dwPK_Device;
			m_pAskXine_Socket->m_dwMaxRetries=1;  // Only try once
			
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode Connecting to %s",sConnectInfo.c_str());
			if (!m_pAskXine_Socket->Connect())
			{
				// Don't try again if this failed once.  Otherwise we may block for long periods of time if we're not 
				// on the same network that can access this
				m_bReportTimeCode=false; 
				delete m_pAskXine_Socket;
				m_pAskXine_Socket=NULL;
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot connect to xine for time code information");
				return;
			}
		}
		else
			return;
	}

	while(!m_bQuit_get() && NULL != m_pAskXine_Socket && m_pAskXine_Socket->m_Socket != INVALID_SOCKET)
	{
		if( m_pAskXine_Socket->m_dwPK_Device!=m_dwPK_Device_NowPlaying )
		{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode exiting -- not active anymore");
			return;
		}
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode BEFORE");
#endif
		string sLine;
		if( !m_pAskXine_Socket->ReceiveString(sLine, -2) )  // Wait at most 1 second for the line
		{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode EMPTY %d %s",(int) sLine.size(),sLine.c_str());
#endif
			Sleep(500);
			continue;
		}

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode AFTER %d %s",(int) sLine.size(),sLine.c_str());
#endif        
		string::size_type pos=0;
		int iSpeed = atoi(StringUtils::Tokenize( sLine,",",pos ).c_str());
		m_sTime = StringUtils::Tokenize( sLine,",",pos );
		m_sTotalTime = StringUtils::Tokenize( sLine,",",pos );
		int iStreamID = atoi(StringUtils::Tokenize( sLine,",",pos ).c_str());
		string sTitle = StringUtils::Tokenize( sLine,",",pos );
		string sChapter = StringUtils::Tokenize( sLine,",",pos );

		// Strip fractions of a second
		if( (pos=m_sTime.find('.'))!=string::npos )
			m_sTime = m_sTime.substr(0,pos);

		if( (pos=m_sTotalTime.find('.'))!=string::npos )
			m_sTotalTime = m_sTotalTime.substr(0,pos);

		string sSpeed;
		if( iSpeed!=1000 ) // normal playback
		{
			if( iSpeed>=1000 || iSpeed<=-1000 )
				sSpeed = StringUtils::itos( iSpeed / 1000 ) + "x";  // A normal speed
			else if( iSpeed==250 )
				sSpeed = "1/4x";
			else if( iSpeed==-250 )
				sSpeed = "-1/4x";
			else if( iSpeed==500 )
				sSpeed = "1/2x";
			else if( iSpeed==-500 )
				sSpeed = "-1/2x";
			else
				sSpeed = "." + StringUtils::itos(iSpeed) + "x";
		}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCode calling CMD_Update_Time_Code");
#endif

		if( m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD>0 )
		{
			DCE::CMD_Display_Message CMD_Display_Message_TC(m_dwPK_Device,m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD,
				m_sTime, //pos_TimeCode==string::npos ? m_sTime : m_sTime.substr(0,pos_TimeCode),
				StringUtils::itos(VL_MSGTYPE_NOW_PLAYING_TIME_CODE),"tc","","");

			if( m_bSpeedIconsOnVfd==false )
			{
				// Don't display the speed as a message if the device will show it iconically
				DCE::CMD_Display_Message CMD_Display_Message_SP(m_dwPK_Device,m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD,
					sSpeed,
					StringUtils::itos(VL_MSGTYPE_NOW_PLAYING_SPEED),"tc","","");
				CMD_Display_Message_TC.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message_SP.m_pMessage);
			}

			static int s_iSpeed=-9999;
			static int s_PK_MediaType=-9999;

			if( iSpeed!=s_iSpeed || m_iPK_MediaType!=s_PK_MediaType )
			{
				DCE::CMD_Show_Media_Playback_State CMD_Show_Media_Playback_State(m_dwPK_Device,m_pLocationInfo_Initial->m_dwPK_Device_LCD_VFD,
					StringUtils::itos(iSpeed), m_iPK_MediaType, "UNKNOWN");
				CMD_Display_Message_TC.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Media_Playback_State.m_pMessage);
				s_iSpeed=iSpeed;
				s_PK_MediaType=m_iPK_MediaType;
			}

			SendCommand(CMD_Display_Message_TC);

		}

		CMD_Update_Time_Code(iStreamID,m_sTime,m_sTotalTime,sSpeed,sTitle,sChapter);
		Sleep(50);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"UpdateTimeCodeLoop ended. %p %d",m_pAskXine_Socket,m_pAskXine_Socket==NULL ? -1 : m_pAskXine_Socket->m_Socket);
}

void Orbiter::StartScreenSaver(bool bGotoScreenSaverDesignObj)
{
	NeedToRender render( this, "StartScreenSaver" );  // May result in starting a new screen saver session
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::StartScreenSaver %d",(int) bGotoScreenSaverDesignObj);

	// If we're watching video, go to that instead
	if( m_bIsOSD && m_iPK_Screen_RemoteOSD && m_bContainsVideo && m_iLocation_Initial==m_pLocationInfo->iLocation )
	{
		if( bGotoScreenSaverDesignObj && (!m_pScreenHistory_Current || m_pScreenHistory_Current->PK_Screen()!=m_iPK_Screen_RemoteOSD) && (!m_pScreenHistory_Pivot || m_pScreenHistory_Pivot->PK_Screen()!=m_iPK_Screen_RemoteOSD) )
			CMD_Goto_Screen("",m_iPK_Screen_RemoteOSD); // Go to the remote instead
		return;
	}

	if( m_pDesignObj_Orbiter_ScreenSaveMenu && bGotoScreenSaverDesignObj )
	{
		CMD_Set_Main_Menu("V");
		GotoDesignObj(m_sMainMenu);
#ifdef ENABLE_MOUSE_BEHAVIOR
		if(UsesUIVersion2() && m_pMouseBehavior )
		{
			bool bMouseVisible=m_pMouseBehavior->m_bMouseVisible;  // Save the state
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Orbiter::StartScreenSaver calling CMD_Show_Mouse_Pointer vis %d",
				(int) m_pMouseBehavior->m_bMouseVisible);
#endif
			CMD_Show_Mouse_Pointer("0");
			m_pMouseBehavior->Clear();
			m_pMouseBehavior->m_bMouseVisible=bMouseVisible;  // Restore it
		}
#endif
		
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::StartScreenSaver goto screen m_bDisplayOn = true; %p", m_pDesignObj_Orbiter_ScreenSaveMenu);
#endif
	}

	if( m_pDevice_ScreenSaver && m_pDevice_ScreenSaver->m_bDisabled==false )
	{
		PurgeCallBack(&Orbiter::SendOffToSS);
		CallMaintenanceInMiliseconds( 0, &Orbiter::SendOnToSS, NULL, pe_ALL, false );
	}

	m_bScreenSaverActive=true;
}

void Orbiter::StopScreenSaver()
{
	// Don't actually stop the background app if this is ui2 and we're not playing media and we don't have an active computing app
	if( UsesUIVersion2() && (!m_dwPK_Device_NowPlaying || !m_bContainsVideo) && m_sActiveApplication_Description.empty() )  
		return;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::StopScreenSaver");
	m_bScreenSaverActive = false;

	if( m_pDevice_ScreenSaver && m_pDevice_ScreenSaver->m_bDisabled==false )
	{
		PurgeCallBack(&Orbiter::SendOnToSS);
		CallMaintenanceInMiliseconds( 0, &Orbiter::SendOffToSS, NULL, pe_ALL, false );
	}
}
//<-dceag-c548-b->

	/** @brief COMMAND: #548 - Menu */
	/** Shows the main menu */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Orbiter::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Menu display %d",(int) m_bDisplayOn);
#endif

	GotActivity(0);  // Keep processing this regardless

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pMouseBehavior )
	{
		m_pMouseBehavior->ButtonDown(BUTTON_Mouse_7_CONST);
		m_pMouseBehavior->ButtonUp(BUTTON_Mouse_7_CONST);
	}
	else
		GotoMainMenu();
#else
	GotoMainMenu();
#endif
}

//<-dceag-c834-b->

	/** @brief COMMAND: #834 - Execute Shortcut */
	/** Execute the shortcut associated with a key.  Called when a key is held down. */
		/** @param #48 Value */
			/** The ascii value of the key (ie 65='A').  Valid are 0-9,*,#,A-Z */

void Orbiter::CMD_Execute_Shortcut(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c834-e->
{
	char cKey = (char) iValue;
	map< char, Message *>::iterator it=m_mapShortcut.find(cKey);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::CMD_Execute_Shortcut %d %d", iValue, (int) (it!=m_mapShortcut.end()));
	if( it!=m_mapShortcut.end() )
	{
		Message *pMessage = it->second;
		if( pMessage->m_dwPK_Device_To==m_dwPK_Device )
			ReceivedMessage( pMessage );
		else
			QueueMessageToRouter(new Message(pMessage));
	}
}
//<-dceag-c838-b->

	/** @brief COMMAND: #838 - Bind to Wireless Keyboard */
	/** If the USB RF dongle is attached it causes it to do a bind request and add a remote. */

void Orbiter::CMD_Bind_to_Wireless_Keyboard(string &sCMD_Result,Message *pMessage)
//<-dceag-c838-e->
{
#ifdef HID_REMOTE
    if( m_pHIDInterface )
		m_pHIDInterface->LegacyBind();
#endif
}

bool Orbiter::OkayToInterrupt( int iInterruption )
{
	if( iInterruption==interuptAlways )
		return true;

	if( m_dwPK_Device_NowPlaying )
		// If it's video it's always false.  If it's audio only, it's true only if there's no video
		return m_bContainsVideo ? false : iInterruption==interuptOnlyAudio; 

	// Nothing is playing.  So unless it's interuptNever and there's an outside application we're ok
	if( (iInterruption==interuptNever || iInterruption==interuptOnlyAudio) && m_sActiveApplication_Window.empty()==false )
		return false;
	return true;
}

void Orbiter::ServiceInterruptionQueue()
{
	for(list< pair<int,Message *> >::iterator it=m_listPendingGotoScreens.begin();it!=m_listPendingGotoScreens.end();)
	{
		if( !m_pScreenHistory_Current || m_mapScreen_Interrupt[m_pScreenHistory_Current->PK_Screen()]==1 || OkayToInterrupt(it->first) )
		{
			QueueMessageToRouter(it->second);
			m_listPendingGotoScreens.erase(it++);
		}
		else
			++it;
	}
}

char *Orbiter::ReadFileIntoBuffer( string sFileName, size_t &Size )
{
	char *pGraphicData=NULL;
	if ( m_bIsOSD)
	{
		pGraphicData = FileUtils::ReadFileIntoBuffer(sFileName, Size);
		if( pGraphicData )
			return pGraphicData;
	}
	int Length=0;
	DCE::CMD_Request_File CMD_Request_File( m_dwPK_Device, m_dwPK_Device_GeneralInfoPlugIn, sFileName,
		&pGraphicData, &Length );
	SendCommand( CMD_Request_File );
	Size = Length;
	return pGraphicData;
}

void Orbiter::PurgeCallBack(OrbiterCallBack fnCallBack,void *pVoidPtr)
{
	PLUTO_SAFETY_LOCK( mt, m_MaintThreadMutex );
	for(map<int,PendingCallBackInfo *>::iterator it=m_mapPendingCallbacks.begin();it!=m_mapPendingCallbacks.end();++it)
	{
		PendingCallBackInfo *pCallBackInfo = (*it).second;
		if( pCallBackInfo->m_fnCallBack == fnCallBack && (pVoidPtr==NULL || pVoidPtr==pCallBackInfo->m_pData) )
			pCallBackInfo->m_bStop=true;
	}
}

void Orbiter::SendOffToSS( void *data )
{
	string sResponse;
	DCE::CMD_Off CMD_Off(m_dwPK_Device,m_pDevice_ScreenSaver->m_dwPK_Device,0);
	if( !SendCommand(CMD_Off,&sResponse) || sResponse!="OK" )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::SendOffToSS screen saver not available.  try again in 1 second");
#endif

#ifndef TEST_OSD
		CallMaintenanceInMiliseconds( 1000, &Orbiter::SendOffToSS, NULL, pe_ALL, false );
#endif
	}
}

void Orbiter::SendOnToSS( void *data )
{
	string sResponse;
	DCE::CMD_On CMD_On(m_dwPK_Device,m_pDevice_ScreenSaver->m_dwPK_Device,0,"");
    if( SendCommand(CMD_On,&sResponse) && sResponse=="OK" )
	{
	    string sName = m_pDevice_ScreenSaver->m_mapParameters_Find(DEVICEDATA_Name_CONST);
		CMD_Activate_Window(sName);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::SendOnToSS saver activate app m_bDisplayOn = true;");
#endif
	}
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::SendOnToSS screen saver not available.  try again in 1 second");
#endif
		CallMaintenanceInMiliseconds( 1000, &Orbiter::SendOnToSS, NULL, pe_ALL, false );
	}
}
//<-dceag-c912-b->

	/** @brief COMMAND: #912 - Activate PC Desktop */
	/** Activate or de-activate the PC-desktop */
		/** @param #119 True/False */
			/** If true, switch to the last PC desktop.  If false, switch to Orbiter's desktop */

void Orbiter::CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c912-e->
{
}

string Orbiter::DetectCoreIpAddress() 
{
#ifdef WIN32

	//create datagram socket
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	//make it nonblocking
	int nonblocking = 1;
	ioctlsocket(s, FIONBIO, (unsigned long*) &nonblocking);

	//set opt broadcast
	int on = 1;
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof(on));

	//setup sockaddr
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	memset((char *) &si_other, sizeof(si_other), 0);
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(33333);
	si_other.sin_addr.s_addr = inet_addr("255.255.255.255");

	//send query
	char send_buf[] = "Search DCERouter";
	sendto(s, send_buf, sizeof(send_buf), 0, (const sockaddr *)&si_other, slen);

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Search for a router...");

	//wait for response
	char recv_buf[128];
	memset(recv_buf, 0, sizeof(recv_buf));
	time_t start_time = time(0);
	while ((time(0) - start_time) < 2) 
	{
		int a = recvfrom(s, recv_buf, sizeof(recv_buf), 0, (sockaddr *)&si_other, &slen);
		if(a > 0)
			break;
	}

	//close socket
	closesocket(s);

	//get ip address
	char *ret = inet_ntoa(si_other.sin_addr);

	string sIP;
	if(NULL != ret)
		sIP = ret;

	if(sIP != "255.255.255.255")
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found router on %s", sIP.c_str());
		return sIP;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to find a router");

#endif

	return "";
} 
//<-dceag-c923-b->

	/** @brief COMMAND: #923 - Assisted Make Call */
	/** Send make call command back to Orbiter and let it decide if we are going to make a direct call or a transfer/conference */
		/** @param #17 PK_Users */
			/** The called user. Only one is supported now. */
		/** @param #83 PhoneExtension */
			/** The phone number to be called. */
		/** @param #184 PK_Device_From */
			/** The device which starts the call. */
		/** @param #263 PK_Device_To */
			/** The called device. */

void Orbiter::CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage)
//<-dceag-c923-e->
{
	m_pScreenHandler->HandleAssistedMakeCall(iPK_Users, sPhoneExtension, atoi(sPK_Device_From.c_str()), iPK_Device_To, "");
}
//<-dceag-c67-b->

	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawn the given application.  Mainly used for windows orbiters. */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments, tab delimited */
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */
		/** @param #115 Show logo */
			/** If this is set then we will first select the logo  before spawning the application. */
		/** @param #120 Retransmit */
			/** If false, and if Exclusive is true and another instance is killed, the 'send messages on termination' will not be sent. */
		/** @param #126 Exclusive */
			/** If true, then kill other apps with this same name */
		/** @param #241 Detach */
			/** Detach application after spawning / Don't kill this app on reload. */

void Orbiter::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
#if defined(WIN32) && !defined(WINCE)
		//starting the new orbiter
		PROCESS_INFORMATION pi;
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = 0;

		string sCompleteCmdLine = sFilename + " \"" + sArguments + "\"";
		::CreateProcess(NULL, const_cast<char *>(sCompleteCmdLine.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
#endif 
}
//<-dceag-c975-b->

	/** @brief COMMAND: #975 - XPromptReload */
	/** This command displays a message using custom drawn X primitives and asks the user if they want to reload the router. */
		/** @param #9 Text */
			/** Text to display on the screen */

void Orbiter::CMD_XPromptReload(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c975-e->
{
	map<int,string> mapPrompts;
	enum PromptsResp {prYes, prNo};
	mapPrompts[prYes]    = "Yes - Reload now";
	mapPrompts[prNo]     = "No - I'll do it later";
	int iResponse = m_pOrbiterRenderer->PromptUser(sText, 0, &mapPrompts);
	if( iResponse==prYes )
	{
		string sResponse;
		Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
		event_Impl.m_pClientSocket->SendString( "RELOAD" );
		if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
		{
			CannotReloadRouter();
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
		}
		Sleep(10000);
	}

}
