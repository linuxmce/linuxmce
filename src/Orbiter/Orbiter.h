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
/**
*
* @file Orbiter.h
* @brief header file for the Orbiter and NeedToRender classes
* @author
* @todo notcommented
* @warning do not change the code between the tags(  <-dceag-...-b-> < -dceag-...-e->  ) ,  it will be overriden by the generator (  unless '!' specified  )
*
*/

//<-dceag-d-b->
#ifndef Orbiter_h
#define Orbiter_h

//	DCE Implemenation for #8 Orbiter

#include "Gen_Devices/OrbiterBase.h"
//<-dceag-d-e->

#include "DCE/Logger.h"
#include "OrbiterData.h"
#include "DesignObj_Orbiter.h"
#include "CacheImageManager.h"
#include "SerializeClass/ShapesColors.h"
#include "CallBackTypes.h"
#include "pluto_main/Define_UI.h"
#include "PlutoUtils/Profiler.h"
#include "LIRC_DCE/IRReceiverBase.h"
#include "Floorplan.h"
#include "Gen_Devices/AllScreens.h"

class OrbiterFileBrowser_Collection;
class ScreenHandler;
class OSDScreenHandler;
class OrbiterRenderer;
class DesignObj_DataGrid;
class AskXine_Socket;

class BD_PC_SetVariable;
class DataGridRenderer;

/** For brevity,  DesignObj_Orbiter will be abbreviated Obj */
#ifdef HID_REMOTE
	class PlutoHIDInterface;
        class GenericHIDInterface;
#endif

#define AUDIO_STATION_SKIN "NuForce"

namespace DCE
{

	enum SelectionBehaviour
	{
		sbSelectOnFirstMove,
		sbAllowsSelected,
		sbNoSelection
	};

	enum SelectionMethod
	{
		smKeyboard,
		smMouse,
		smNavigation,
		smCommand,
		smLoadUnload,
		smHighlight,
		smMouseGovernor
	};


#define PROMPT_CANCEL -1

	typedef void ( Orbiter::*OrbiterCallBack )( void *data );
	enum ePurgeExisting { pe_NO=0, pe_ALL, pe_Match_Data };

	//todo : need to serialize it ? 
	class PlutoPopup
	{
	public:
		string m_sName;
		PlutoPoint m_Position;
		bool m_bDontAutohide;
		DesignObj_Orbiter *m_pObj;

		PlutoPopup(DesignObj_Orbiter *pObj, string sName, PlutoPoint Position)
		{
			m_pObj = pObj;
			m_sName = sName;
			m_Position = Position;    
			m_bDontAutohide = false;
		}
		~PlutoPopup() {
		}
	};

	class PlutoAlert
	{
	public:
		time_t m_tStartTime;
		time_t m_tStopTime;
		string m_sMessage;
		string m_sToken;
	};

	class PendingCallBackInfo;

	//<-dceag-decl-b->! custom
	/**
	* @brief
	*/
	class Orbiter : public Orbiter_Command,  public OrbiterData, public IRReceiverBase
	{
	private:
		class PlutoGraphic * m_pBackgroundImage;
		static Orbiter *m_pInstance; 
		list<PlutoAlert *> m_listPlutoAlert;  // Current alerts displayed on the screen as a popup message
		string DetectCoreIpAddress();

	protected:
		void DumpScreenHistory(); // temporary function

		//<-dceag-decl-e->

		OrbiterRenderer *m_pOrbiterRenderer;

		friend class ::BD_PC_SetVariable; /** < Needs to maniuplate our variables */
		friend class NeedToRender; /** < Needs to maniuplate our variables */
		friend class ::ScreenHandler;
		friend class ::OSDScreenHandler;
		friend class ::OrbiterRenderer;
		friend class ::DesignObj_Orbiter;
		friend class ::DataGridRenderer;
		friend class ::ObjectRenderer;
#ifdef HID_REMOTE
		friend class ::PlutoHIDInterface;
		friend class ::GenericHIDInterface;
#endif

#ifdef ENABLE_MOUSE_BEHAVIOR
		friend class MouseBehavior;
		friend class MouseIterator;
		friend class MouseGovernor;
		friend class HorizMenuMouseHandler;
		friend class MediaBrowserMouseHandler;
		friend class LightMouseHandler;
		friend class SpeedMouseHandler;
		friend class VolumeMouseHandler;
		friend class MediaMouseHandler;
		friend class EPGMouseHandler;
		friend class LockedMouseHandler;
		friend class KeyboardMouseHandler;
		friend class DatagridMouseHandlerHelper;
#endif

	public: //data
		class Event
		{
		public:
			typedef enum _EventType {
				QUIT,
				NOT_PROCESSED,
				BUTTON_DOWN, // keyboard
				BUTTON_UP,
				REGION_DOWN, // mouse
				REGION_UP,
				MOUSE_MOVE,
				MOUSE_RELATIVE_MOVE,
				HID,
				NONE
			} EventType;

			EventType type;

			union {
				struct {
					int m_iPK_Button;
					int m_iKeycode;  // The internal keycode, not Pluto's button code
					bool m_bSimulated;
				} button;

				struct {
					int m_iButton;
					int m_iX;
					int m_iY;
				} region;

				struct {
					unsigned char m_pbHid[6];
				} hid;
			} data;
		};

		//these two methods should not be used from within bluetooth dongle
		static void DestroyInstance();
		static Orbiter *Instance(); 

		OrbiterRenderer *Renderer();
		ScreenHandler *m_pScreenHandler_get() { return m_pScreenHandler; }
		string m_sApplicationName_get() { return m_sApplicationName; }

		int CurrentScreen();

		int m_iImageWidth; /** < image width */
		int m_iImageHeight; /** < image height */
		bool m_bFullScreen,m_bUseOpenGL,m_bUseComposite,m_bReportTimeCode;
		bool m_bLoadDatagridImagesInBackground;
		bool m_bShowingSpeedBar; // For UI2 this means we temporarily are displaying the speed bar because we're not at normal 1x speed
		bool m_bSpeedIconsOnVfd; // True if there's a VFD that can show speed icons
		bool m_bExpertMode; // A device data, if true we support quick launch
		bool m_bIgnoreMouse; // Completely ignore mouse events
		class DesignObj_Orbiter *GetCurrentDesignObj();
		map< pair<int,char>,string> m_mapScanCodeToRemoteButton; /** < Map of scan codes to remote button names used by Infrared Plugin.  The char is the action (Up,Down,Hold meaning repeat). */
		map< int, bool > m_mapScanCodeToIgnoreOnYield; /** Map of scan codes (=true) where they should be ignored when yield screen is true */
		map< pair<int,int>,pair<int,int> > m_mapEventToSubstitute; /** < Replace a combination of event,button with event,button */
		string m_sTime,m_sTotalTime;  // Only used by the update timecode loop
		list< pair<int,Message *> > m_listPendingGotoScreens; // Goto Screens we have ignored because of not interrupting the user that should be executed later

#ifdef ENABLE_MOUSE_BEHAVIOR
		class MouseBehavior *m_pMouseBehavior;  // Class to handle special logic such as locking mouse movements, speed bumps, etc.
#endif
#ifdef HID_REMOTE
		PlutoHIDInterface *m_pHIDInterface;
		GenericHIDInterface *m_pGenericHIDInterface;
		vector <string> m_vectGyroDevices;	// Gyro devices to map to GenericHIDInterface
		pthread_t m_HidThreadID;
		pthread_t m_GenericHidThreadID;
#endif
		//<-dceag-const-b->!

	public:

		/**
		* @brief constructor, assignes values to member data
		*/
		Orbiter( int DeviceID,  int PK_DeviceTemplate, string ServerAddress,  string sLocalDirectory,  
			bool bLocalMode,  int iImageWidth,  int iImageHeight, pluto_pthread_mutex_t* pExternalScreenMutex = NULL, bool bFullScreen = false );
		/**
		* @brief destructor
		*/
		virtual ~Orbiter(  );

		/**
		* @brief Sets up the class right after the constructor is called
		*/
		virtual bool GetConfig();

		/**
		* This function will only be used if this device is loaded into the DCE Router's memory space
		* as a plug-in. Otherwise Connect() will be called from the main()
		*/
		virtual bool Register(  );
		virtual void PostConnect();

		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);

		time_t GetLastScreenChangedTime();
		string GetCurrentScreenID();

		virtual void SetImageQuality(unsigned long ulImageQuality) {};

		//callback; should be forwarded to the renderer
		virtual void RenderFrame(void *data);

		/**
		* @brief Returns true if version 2 of the UI is being used
		*/
		virtual bool UsesUIVersion2();

		/**
		* @brief creates a screen handler
		*/
		virtual ScreenHandler *CreateScreenHandler();

		/**
		* @brief executes a screen handler's registered callback
		*/
		bool ExecuteScreenHandlerCallback(CallBackType aCallBackType); 

		/**
		* @brief searches for updates, returns true if an update was found
		* @todo ask
		*/
		virtual bool SelfUpdate() { return false; };


		virtual void HideWindow(string sWindowName) {}

		pluto_pthread_mutex_t m_MaintThreadMutex;  // This will also protect the callback map
		pluto_pthread_mutex_t m_ScreenMutex; /** < Anything that should not be done during a screen render, change, etc. Blocking this will prevent screen changes */
		pluto_pthread_mutex_t m_VariableMutex; /** < Short mutex to protect members like strings and maps */
		pluto_pthread_mutex_t m_DatagridMutex; /** < Don't allow 2 threads to operate on datagrids at the same time */
		pluto_pthread_mutex_t m_TimeCodeMutex; /** < Protects the communication with xine for time code */
		pthread_cond_t m_MaintThreadCond;
		pthread_t m_TimeCodeID;

		/**
		* This is an internal counter. When we start doing stuff that may include lots of embedded commands which want to update the screen, we can create
		* a NeedToRender object on the stack. Each one will increment this counter, and decrement in the destructor. When it's 0, it will call RedrawObjects automatically
		* @todo ask if I placed this comment where it should be
		*/

		int m_dwIDataGridRequestCounter;  // For testing purposes we can log all datagrid requests

		//<-dceag-const-e->

		map<int, PendingCallBackInfo *> m_mapPendingCallbacks; //The map with pending callbacks
		unsigned int m_nCallbackCounter; //a pending callback counter

		bool m_bShiftDown;
		bool m_bShiftDownOnScreenKeyboard;
		bool m_bControlDown;
		bool m_bAltDown;
		bool m_bCapsLock;
		bool m_bUpdateTimeCodeLoopRunning; /** < True when the updatetimecodeloop is running */
		Event::EventType m_TypeToIgnore; // If the screen saver is woken up by a button down or region down, we'll set this to ignore the next button/region up


		class DesignObj_Orbiter *m_pObj_LastSelected;   // The last object we selected.  Used by floorplans to toggle states
		int m_dwPK_DesignObj_Toolbar_Last; // Used by floorplans
		int m_iRow_Floorplan_LastSelected;  // If we're using datagrids as floorplans, this is the last row
		class DesignObj_Orbiter *m_pObj_SelectedLastScreen;   // The last object we selected.  Used by floorplans to toggle states

		int m_iLastX,m_iLastY; // For some reason we keep getting move events with the same coordinates over and over.  Keep track of the last ones

		// '0' - no item is selected until the user moves the arrows
		// '1' - automatically highlight the first object when the screen changes
		// '2' - selection/hightlighing routines disabled
		int m_nSelectionBehaviour;
		bool m_bMemoryManagementEnabled;
		bool m_bWeCanRepeat; /** < true if the rendering device we're using gives us Region Up Messages */

		class DesignObj_Orbiter *m_pObj_Highlighted,*m_pObj_Highlighted_Last; /** < The current object highlighted, changed with the scrolling functions */
		void m_pObj_Highlighted_set(DesignObj_Orbiter *pObj) { m_pObj_Highlighted=pObj; }
		vector < class DesignObj_DataGrid * > m_vectObjs_GridsOnScreen; /** < All the grids currently on the screen */

		string GetLocalDirectory()
		{
			return m_sLocalDirectory;
		}

		class ScreenHistory *m_pScreenHistory_Current;		/** < The currently visible screen */

	protected:

		class ScreenHistory *m_pScreenHistory_Pivot;		/** < The head for the list of designobj of the new screen */
		class ScreenHistory *m_pScreenHistory_New;			/** < The screen history for the new screen. Might be the same with the current if there is only a goto designobj */

		int m_dwPK_Users; /** < The current user */
		int m_dwPK_DeviceTemplate;  /** < This is running as a specific device template */
		class LocationInfo *m_pLocationInfo; /** < The current location */

		string m_sLocalDirectory; /** < A directory to get files from */
		class ScreenHistory *m_pContextToBeRestored; // We need to restore objects visibility status after the user does a go back
		int m_iLastEntryInDeviceGroup; // Used by floorplans to go through a selected device group
		map<int,class DeviceGroup *> m_mapDeviceGroups;
		ScreenHandler *m_pScreenHandler;//Used to change screens
		class LocationInfo *m_pLocationInfo_Initial; // The initial location
		DeviceData_Impl *m_pDevice_ScreenSaver;
		bool m_bScreenSaverActive; // true if the photo screen saver app is running.  not necessarily if the menu is at the screen saver because in ui2 the gallery viewer runs anytime there is no media
		time_t m_tLastMouseMove;
		int m_iLastVideoObjectRendered; // If there are multiple ones on screen, don't block the maint thread while polling all of them, use this as a counter to process one a time

		AskXine_Socket *m_pAskXine_Socket;


		/** flags */
		bool m_bCaptureKeyboard_OnOff; /** < flag for capture keyboard */
		bool m_bCaptureKeyboard_Reset; /** < flag for capture keyboard */
		bool m_bStartingUp; /** < True while the system hasn't yet finished starting */
		int m_iCaptureKeyboard_EditType; /** < 0 = normal, 1 = numbers only, 2 = pin */
		bool m_bCaptureKeyboard_DataGrid; /** < flag for capture keyboard */
		bool m_bRestrictedOp_IsUser;
		int m_iRestrictedOp_ID;
		bool m_bShowShortcuts; /** < if true, the shortcuts for each object rendered will be rendered too. */
		bool m_bUsingExternalScreenMutex;
		bool m_bForward_local_kb_to_OSD; // true if this is orbiter should be used a remote keyboard for another

		int  m_iCaptureKeyboard_PK_Variable; /** < capture keyboard variable (coresponds to primary key) @todo ask */
		string m_sCaptureKeyboard_Text; /** < text for capture keyboard @todo ask */
		string m_sCaptureKeyboard_InternalBuffer; /** < capture keyboard internal buffer */
		PlutoSize m_sScreenSize;

		string m_sLastSelectedDatagrid; /** < The contents of the last selected datagrid cell */

		// The remotes for the current media
		int m_iPK_Screen_Remote,m_iPK_DesignObj_Remote_Popup,m_iPK_Screen_FileList,m_iPK_Screen_RemoteOSD,m_iPK_Screen_OSD_Speed,m_iPK_Screen_OSD_Track;
		string m_sNowPlaying_MediaType,m_sNowPlaying,m_sNowPlaying_Section,m_sNowPlaying_TimeShort,m_sNowPlaying_TimeLong,m_sNowPlaying_Speed,m_sNowPlaying_Angle,m_sNowPlaying_Audio,m_sNowPlaying_Subtitle,m_sDefaultRippingName,m_sApplicationName,m_sNowPlaying_Window; /** < set by the media engine, this is whatever media is currently playing */
		int m_nDefaultStorageDeviceForRipping;
		string m_sDefaultStorageDeviceForRippingName;
		int m_iPK_MediaType,m_iStreamID;
		int m_dwPK_Device_NowPlaying,m_dwPK_Device_NowPlaying_Video,m_dwPK_Device_NowPlaying_Audio,m_dwPK_Device_CaptureCard;  /** < set by the media engine, this is whatever media device is currently playing.  Capture Card is non null if we're displaying media via this card */
		bool m_bPK_Device_NowPlaying_Audio_DiscreteVolume,m_bContainsVideo,m_bUsingLiveAVPath;

		int m_iTimeoutScreenSaver,m_iTimeoutBlank;  /** < When we're not on the screen saver screen how long to timeout before going to it, and when we are, how long before blacking the screen */
		time_t m_tTimeoutTime;  /** < On the screen saver screen, this is the time when the display will go blank */
		timespec m_tButtonDown;  
		string m_sCacheFolder; /** < Where to store graphic files for caching */
		int m_iCacheSize; /** < The maximum size of the graphics files stored (MB) */
		int m_iVideoFrameInterval; /** < The interval between two frame requested from the router */

		map<int,Message *> m_mapHardKeys;  /** < Messages to fire when keys are pressed */
		map< char, Message *> m_mapShortcut; // Map characters held down with message to send	

		DesignObjText *m_pCaptureKeyboard_Text; /** < @todo ask */
		map<int,  vector<PlutoGraphic*> *> m_mapUserIcons; /** < user icons */
		map<int,FloorplanObjectVectorMap *> m_mapFloorplanObjectVector;
		FloorplanObjectVectorMap *m_mapFloorplanObjectVector_Find(int Page)	{ map<int,FloorplanObjectVectorMap *>::iterator it = m_mapFloorplanObjectVector.find(Page); return it==m_mapFloorplanObjectVector.end() ? NULL : (*it).second; }
		string m_mapVariable_Find(int PK_Variable) { PLUTO_SAFETY_LOCK( vm, m_VariableMutex ); VariableMap ::iterator it = m_mapVariable.find(PK_Variable); return it==m_mapVariable.end() ? "" : (*it).second; }

		DesignObj_DataGrid *FindGridOnScreen(string sGridID);

		time_t m_LastActivityTime; /** < The last activity time */
		bool m_bDisplayOn; /** < False if the screen has blanked for the screen saver */
		bool m_bIgnoreFirstEvent; /** < A copy of DATA_Get_Ignore_First_Event for quick access */
		bool m_bYieldScreen; /** < True if the orbiter should make the application desktop full screen ( hide itself ) */
		bool m_bYieldInput; /** < True if the orbiter should yield all input, like keyboard and mouse. This is useful when running the Orbiter as Linux desktop */
		bool m_bBypassScreenSaver; /** < True if we don't want the screen to blank */
		bool m_bRepeatingObject; /** < True if we're currently holding down a repeating button */
		bool m_bRerenderScreen; /** <  Set to true means ignore the objects to redraw, and just redraw the whole screen */
		string m_sActiveApplication_Description,m_sActiveApplication_Window; /** The name of any actively running application */
		int m_PK_Screen_ActiveApp_OSD,m_PK_Screen_ActiveApp_Remote;  /** The remotes to use for the running application */


		OrbiterFileBrowser_Collection *m_pOrbiterFileBrowser_Collection;

		DesignObj_OrbiterMap m_mapObj_All; /** < All objects with the object ID in x.y.z.a.b format */

		map < string, DesignObj_DataList * > m_mapObj_AllNoSuffix; /** < All object with the object ID as a string */
		list < class ScreenHistory * > m_listScreenHistory; /** < A history of the screens we've visited */
		map<int,class DeviceData_Base *> m_mapDevice_Selected;  /** < We can select multiple devices on the floorplan to send messages to, instead of the usual one */
		map<int,class FloorplanObject *> m_mapFloorplanObject_Selected;  /** < The selected floorplan objects.  Note these are copies, not originals, so they must be deleted */

		string m_sObj_Popop_RemoteControl,m_sObj_Popop_FileList;
		int m_Popop_RemoteControl_X,m_Popop_RemoteControl_Y,m_Popop_FileList_X,m_Popop_FileList_Y;

		map < string, DesignObj_DataList * > m_mapObj_Bound; /** < All objects bound with the Bind Icon command */
		DesignObj_DataList *m_mapObj_Bound_Find(string PK_DesignObj) { map<string,DesignObj_DataList *>::iterator it = m_mapObj_Bound.find(PK_DesignObj); return it==m_mapObj_Bound.end() ? NULL : (*it).second; }

		vector < class DesignObj_Orbiter * > m_vectObjs_TabStops; /** < All the tab stops presently on the screen */
		vector < class DesignObj_Orbiter * > m_vectObjs_Selected; /** < All the objects currently selected */
		vector < class DesignObj_Orbiter * > m_vectObjs_VideoOnScreen; /** < All the video on screen */
		bool m_bAlreadyQueuedVideo; // We only put 1 GetVideFrame in the queue
		class DesignObj_Orbiter *m_pObj_NowPlayingOnScreen,*m_pObj_NowPlaying_MT_OnScreen,*m_pObj_NowPlaying_Section_OnScreen,*m_pObj_NowPlaying_TimeShort_OnScreen,*m_pObj_NowPlaying_TimeLong_OnScreen,*m_pObj_NowPlaying_Speed_OnScreen,*m_pObj_NowPlaying_Angle_OnScreen,*m_pObj_NowPlaying_Audio_OnScreen,*m_pObj_NowPlaying_Subtitle_OnScreen; /** < The objects showing 'now playing' on screen */

		list<class PlutoPopup*> m_listPopups;
		map<PlutoPopup *, unsigned long> m_mapPopupDialogs;

		map< string, class DesignObj_DataGrid * > m_mapObjs_AllGrids; /** < All the datagrids */
		map< string, class DesignObj_Orbiter * > m_mapFloorplan;

		map< string, string> m_mapBoundIconValues;  /** < Values for all bound icons */

		/** for threads and shared memory control @todo ask */

		pthread_mutexattr_t m_MutexAttr; /** < @todo ask */
		//	pthread_cond_t m_MessageQueueCond; /** < @todo ask */
		pthread_t m_MaintThreadID;

		CacheImageManager *m_pCacheImageManager;

		virtual void GetWindowPosition(PlutoPoint& point) { point.X = point.Y = 0; }

		// Be sure we're only reading the correct version
		virtual bool OkayToDeserialize(int iSC_Version);

		virtual bool OnReplaceHandler(string sIP);

		/** these methods are general purpose, and will call Orbiter-specific methods to do the work */

		/**
		* @brief Add a popup to the list.  Will return false if the identical popup was already there
		*/
		bool AddPopup(list<class PlutoPopup*> &listPopups,class PlutoPopup *pPopup);

		/**
		* @brief Get the popup with the id or name specified
		*/
		PlutoPopup *FindPopupByName(DesignObj_Orbiter *pObj,string sPopupName);

		/**
		* @brief Forwards a RefreshScreen to renderer
		* 
		*/
		virtual void RealRedraw( void *iData );

		// Handle displaying any alert messages on the screen
		void ServiceAlerts( void *iData );

		// Don't show shortcuts anymore
		void RemoveShortcuts( void *iData );

		void RedrawObject( void *iData );

		// Reselect the given object, used for repeating buttons
		void ReselectObject( void *data );

		//helper functions for the timer mechanism in screen handlers
		void ServiceScreenHandler(void *data);
		void StartScreenHandlerTimer(int nInterval /*in miliseconds*/);

		// If in a screen's handler you are going to redirect to another screen, but want this one added to the history for go back's,
		// call the base handler and then this function, like: 	ScreenHandler::SCREEN_This_Room(PK_Screen,bAlways); m_pOrbiter->ForceCurrentScreenIntoHistory();
		void ForceCurrentScreenIntoHistory();

	public: // temp - remove this

		PlutoGraphic *m_pGraphicBeforeHighlight;
		PlutoRectangle m_rectLastHighlight;

		/**
		* @brief Starts caching the grid in the background
		* 
		*/
		 void StartCachingGrid( void *iData );

		/**
		* @brief Timeout the object, which is data
		*/
		void Timeout( void *data );

		/**
		* @brief Handle the screen saver
		*/
		void ScreenSaver( void *data );
		void StartScreenSaver(bool bGotoScreenSaverDesignObj=true);
		void StopScreenSaver();
		void SendOffToSS( void *data ); // Send the actual on/off in a separate thread in case pss isn't registered
		void SendOnToSS( void *data );  // Send the actual on/off in a separate thread in case pss isn't registered

		/**
		* @brief renders a floorplan
		* @todo ask
		*/
		virtual void RenderFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter, DesignObj_Orbiter *pDesignObj_Orbiter_Screen, PlutoPoint point = PlutoPoint(0, 0));

		/**
		* @brief The framework will call this when it's time to change screens
		* This immediately calls RenderScreen. Normally we add the current screen to the history list so we can go back again. However,
		* if we're changing screens because of a GO BACK command, we won't do that
		*/
		virtual void NeedToChangeScreens( class ScreenHistory *pScreenHistory);

		void HandleNewObjectsOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter);

		/**
		* @brief returnes the video frame from the DesignObj_Orbiter object pointed by iData
		* @param iData points to the DesignObj_Orbiter object we want the video frame from
		*/
		virtual void GetVideoFrame( void *iData );

		/**
		* @brief Polls the media player for the current timecode and self-updates.  Used only for Xine
		*/
		void UpdateTimeCodeLoop();

		/**
		* @brief
		* @param if X and Y are -1, the object was selected by pushing a keyboard key, rather than touching the screen
		*/
		void SelectedObject( DesignObj_Orbiter *pDesignObj_Orbiter, SelectionMethod selectionMethod, int iX = -1, int iY = -1);

		/**
		* @brief
		* @todo ask
		*/
		bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, int iX, int iY, SelectionMethod selectionMethod );
		bool Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction);

		/**
		* @brief
		* @todo ask
		*/
		bool SelectedGrid( DesignObj_DataGrid *pDesignObj_DataGrid, class DataGridCell *pCell, SelectionMethod selectionMethod, int iRow, int iColumn );

		/**
		* @brief
		* @todo An object that requires special handling was selected
		*/
		virtual void SpecialHandlingObjectSelected(DesignObj_Orbiter *pDesignObj_Orbiter);

		/**
		* @brief
		* @todo A floorplan object was selected
		*/
		virtual void SelectedFloorplan(DesignObj_Orbiter *pDesignObj_Orbiter,int Row=-1);

		/** The above methods are called by the orbiter-specific class to indicate input, they will call these following methods: */

		/**
		* @brief returns true if the specified button was clicked
		* @param dwPK_Button specifies the id for the button
		* @todo ask
		*/
		bool ClickedButton( DesignObj_Orbiter *pDesignObj_Orbiter, int dwPK_Button );

		/**
		* @brief returns true if the specified region was clicked
		* @todo ask
		*/
		bool ClickedRegion( DesignObj_Orbiter *pDesignObj_Orbiter, int iX, int iY, DesignObj_Orbiter *&pTopMostAnimatedObject );

		/**
		* Highlighting is used to show the currently 'highlighted' object, particularly with a remote that has up/down/left/right/enter controls
		* These functions move between objects that have their 'tab stop' property set to true
		*/

		/**
		* @brief Render selected state for this object
		*/
		virtual void SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point = PlutoPoint(0, 0) );

		/**
		* @brief Returns false if a screen was specified and it's not the current one
		*/
		bool TestCurrentScreen(string &sPK_DesignObj_CurrentScreen);

		/**
		* @brief A "DoMaint" callback when we need to select an object after a delay
		*/
		void DelayedSelectObject( void *data );

		/**
		* If a ScreenHandler needs to intercept a message it can register using this function
		*/
		bool ScreenHandlerMsgInterceptor( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		/**
		* @brief A "DoMaint" callback to continually refresh the screen
		*/
		void ContinuousRefresh( void *data );

	bool IsYieldScreen() const;
		bool IsYieldInput() const;
		/**
		* @brief Kill the maintenance thread that handles tasks like redraws.  This function will block until the thread exits
		*/
		void KillMaintThread();

		void StopSimulatorThread();

		/**
		* @brief handle when Orbiter Plugin tells us it's not ok to load, passing in the response.
		* If this returns 0, we will terminate, 1 we will load anyway, 2 we will try again to get an OK status
		*/
		virtual int HandleNotOKStatus(string sStatus,string sRegenStatus,int iRegenPercent);
		virtual bool RouterNeedsReload();
		virtual int DeviceIdInvalid();
		virtual int PickOrbiterDeviceID();
		virtual int SetupNewOrbiter();
		virtual int MonitorRegen(int PK_Device);
		virtual bool RegenOrbiter(); // Send Orbiter plugin a command to regen this orbiter
		virtual void ShowProgress(int nPercent);

		/**
		* @brief Convert a virtual device (a negative device that corresponds to the VirtDev entries in DeviceTemplate) into the real device ID
		*/
		void TranslateVirtualDevice(int PK_DeviceTemplate,long &PK_Device);

		/**
		*	INITIALIZATION
		*/

		/**
		* @brief initializises the orbiter with the specified graphic type
		* @todo ask
		*/
		virtual void Initialize( GraphicType Type, int iPK_Room=0, int iPK_EntertainArea=0 );


        /**
		* @brief initializises the specified Grid
		* @todo ask
		*/
		void InitializeGrid( DesignObj_DataGrid *pObj );

		/**
		* @brief parses the configuration data based on the graphic type
		* @todo ask
		*/
		bool ParseConfigurationData( GraphicType Type );

		/**
		* @brief parses an object
		* @todo ask
		*/
		void ParseObject( DesignObj_Orbiter *pObj, DesignObj_Orbiter *pObj_Screen, DesignObj_Orbiter *pObj_Parent, GraphicType Type, int iLev );

		/**
		* We'll parse the grids only on the first use since it takes time and it will make startup faster
		*/
		void ParseGrid(DesignObj_DataGrid *pObj_Datagrid);

		/**
		* Parse info about hard keys
		*/
		virtual void ParseHardKeys();

		/**
		* @brief renders desktop
		* Not all controllers will be able to do this. Mainly the 'iX' nested
		* @todo ask
		*/
		virtual bool RenderDesktop( class DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, PlutoPoint point = PlutoPoint(0, 0) );

		/**
		* @brief The derived class should implement this if it can.  It moves the mouse pointer to the given coordinates
		*/
		virtual void SetMousePointer(int X, int Y) {}

		virtual void PlayMNG_CallBack(void *data);

		virtual void GetRepeatedKeysForScreen(DesignObj_Orbiter* pObj, string& sKeysList);

		virtual bool IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button, bool bDown);


		/**
		* @brief sets the time to the one specified by the parameter
		*/
		virtual void SetTime( char *pcServerTimeString ) {};

		/**
		*	FUNCTIONS A DERIVED ORBITER MAY WANT TO IMPLEMENT
		*/

	public:

		virtual bool PreprocessEvent( Orbiter::Event &event );
		virtual bool ProcessEvent( Orbiter::Event &event );

		void QueueEventForProcessing( void *eventData );

		virtual bool HandleButtonEvent(int PK_Button);

	protected:
		/**
		*	ACCEPT OUTSIDE INPUT
		*/

		/**
		* @brief the specified button was pressed
		*/
		virtual bool ButtonDown( int iPK_C_Button );

		/**
		* @brief a button was relesed
		* @todo ask
		*/
		virtual bool ButtonUp(int iPK_C_Button);

		/**
		* @brief forward the keypress event to an OSD
		* @todo ask
		*/
		virtual void GenerateSimulateKeypress(int iPK_C_Button);

		virtual void StopRepeatRelatedEvents();

	public:
		/**
		* @brief Something happened, like a touch or a button, reset any timeouts or screen saver.  Returns false if it should be ignored
		*/
		virtual bool GotActivity( int PK_Button );  // 0=it's a mouse click

		virtual void SimulateMouseClick(int x, int y);

		virtual void SimulateMouseMovements(int x, int y);

		virtual void SimulateKeyPress(long key);

		bool OkayToInterrupt( int iInterruption );  // Returns true if it's ok to interrupt the user with interrupt level=iInterruption
		void ServiceInterruptionQueue(); // Handling any pending goto screens that were ignored so as to not interrupt the user
		char *ReadFileIntoBuffer( string sFileName, size_t &Size ); // Get the file locally if it's an OSD, otherwise reqest from server

		/**
		* @brief the point belongs to a region that was clicked
		* @todo ask
		*/
		virtual bool RegionDown( int iX, int iY );

		/**
		* @brief
		* @todo ask
		* @warning We don't handle this here
		*/
		virtual bool RegionUp(int iX, int iY);


		/**
		*	UTILITIES
		*/

		/**
		* @brief the text identified by it's primary key is found in an object
		* @return the object containing the text
		* @todo ask
		*/
		virtual DesignObjText *FindText( DesignObj_Orbiter *pObj, int iPK_Text );

		/**
		* @brief Reset the selected state of an object because it's coming on screen
		*/
		void ResetState(DesignObj_Orbiter *pObj, bool bDontResetState=false);

		/**
		* @brief Redraw any objects that change during execution of a command
		*/
		virtual ReceivedMessageResult ReceivedMessage( class Message *pMessageOriginal );

		/**
		* @brief sets the capture keyboard parameters
		*/
		bool BuildCaptureKeyboardParams( string sPK_DesignObj, int iPK_Variable, string sOnOff, string sType, string sReset, int iPK_Text, bool bDataGrid );

		/**
		* @brief Get all the buttons that this object and its children will want to trap for
		*/
		void GetButtonsInObject( DesignObj_Data *pObj, vector<int> &vectButtons );

		/**
		* @brief deletes the last char from the capture keyboard variable (from the variable map)
		* @todo ask what's this used for?
		*/
		bool CaptureKeyboard_EditText_DeleteLastChar();

		/**
		* @brief adds the char to the capture keyboard variable (from the variable map)
		* @todo ask what's this used for?
		*/
		bool CaptureKeyboard_EditText_AppendChar( char ch );

		/**
		* @brief sets the value of the specified variable in the variable map
		*/
		void CaptureKeyboard_UpdateVariableAndText( int iVariable, string sVariable );

		/**
		* @brief Find an object.  If pDesignObj_Orbiter is past and the ObjectID is not fully qualified, it will look for sPK_DesignObj as a child of pDesignObj_Orbiter
		*/
		DesignObj_Orbiter *FindObject( string sPK_DesignObj, class DesignObj_Orbiter *pDesignObj_Orbiter = NULL );
		DesignObj_Orbiter *FindObject( int PK_DesignObj ) { return FindObject(StringUtils::itos(PK_DesignObj)); }

		/**
		* @brief finds an object identified by a single numeber by searching in the object hierarchi
		* @todo ask
		*/
		DesignObj_Orbiter *FindSingleNumberObject( int iPK_ID_Object, DesignObj_Orbiter *pObj );

		/**
		* @brief executes the commands in the command list.  If there is a goto screen, we don't want to execute it inline--it should be executed last
		* after setting all the variables and such.  So, rather than executing a goto screen, if one is found, it will be set in pMessage_GotoScreen
		* and the caller is expected to pass it to ReceiveMessage after execute commands in list has finished.  We don't just put it at the end of
		* execute commands, because we could be calling ExecuteCommands more than 1 at a time, such as if multiple objects were selected.

		AB 5/29/2005 -- This means that if a button does a goto screen, then a show object for a button by default hidden,
		it won't be unhidden because the goto screen (which resets the hidden flag) is executed last.  This concept of 
		holding up the goto is probably no longer needed since we now use NeedToRender, which serves the same purpose
		*/
		void ExecuteCommandsInList( DesignObjCommandList *pDesignObjCommandList, DesignObj_Orbiter *pDesignObj_Orbiter, SelectionMethod selectionMethod, int iX = -1, int iY = -1, int Repeat=0 ); // Execute commands

		/**
		* @brief substitutes some defined markers with values taken from the class data
		* @todo ask
		*/
		string SubstituteVariables( string Input, DesignObj_Orbiter *pDesignObj_Orbiter, int iX, int iY );

		/**
		* @brief get the lists with visible and hidden children of an object
		*/
		void GetChildrenVisibilityContext(DesignObj_Orbiter *pObj, 
			map<DesignObj_Orbiter *, bool>& mapVisibilityContext);

		/**
		* @brief get the lists with visible and hidden children of an object
		*/
		void SetChildrenVisibilityContext(DesignObj_Orbiter *pObj, 
			const map<DesignObj_Orbiter *, bool>& mapVisibilityContext);

		/**
		* @brief A helper function we can call internally rather than the full CMD_GotoScreen
		*/
		void GotoDesignObj( string sDesignObj, string sID = "" ) { CMD_Goto_DesignObj( 0, sDesignObj, sID, "", false, false ); }
		void CMD_Goto_Screen(string sID,int PK_Screen) { CMD_Goto_Screen(sID,PK_Screen,interuptAlways,false,false); } // Helper so we don't need to type out the default values

		/**
		* @brief does a custom comparation (you ca also specify the operand to use) with the value from the variable identified by the key and the other parmeter
		* @param sComparisson specifies the operand to use
		*/
		bool DoComparisson( int PK_Variable, string sComparisson, string sValue );

		/**
		* @brief
		* @todo ask
		*/
		void FindDGArrows( DesignObj_Orbiter *pObj, DesignObj_DataGrid *pDGObj );

		/**
		* @brief returns the selected identifiing it by the row number
		* @todo ask
		*/
		bool SelectedGrid( int iDGRow );

		/**
		* @brief We need the maintenance function to be called in this many clock ticks
		*/
		void CallMaintenanceInMiliseconds( time_t miliseconds, OrbiterCallBack fnCallBack, void *iData, ePurgeExisting e_PurgeExisting, bool bPurgeTaskWhenScreenIsChanged = true );
		void PurgeCallBack(OrbiterCallBack fnCallBack,void *pVoidPtr=NULL); // Purge a pending callback

		// Report our current EntArea/Room to orbiter plugin
		void FireEntAreaRoomCommands();

		bool HasActivePopups() { return m_listPopups.empty()==false; }

		// Get the coordinates for the highlighted cell within this grid
		void GetDataGridHighlightCellCoordinates(DesignObj_DataGrid *pGrid,PlutoRectangle &rect);
		DataGridCell *GetDataGridHighlightCell(DesignObj_DataGrid *pGrid);
		DataGridCell *GetDataGridSelectedCell(DesignObj_DataGrid *pGrid) { return NULL; }

		void GotoMainMenu();

		/**
		*	MAINTENANCE CALL BACKS
		*
		*	When you need something to be done at a particular time, like popup buttons, call 'CallMaintenance', passing in the time you need the function
		*	called, and any iData you want the function to get, as a void *. The call backs must be void Function( void *iData ) syntax.
		*/

		/**
		* @brief
		* @param iData is a pointer to an object to deselect
		*/
		void DeselectObjects( void *iData );

		virtual void OnQuit();
		virtual void OnReload();

		// Wait for related devices to startup before we let the user go
		virtual bool WaitForRelativesIfOSD();

		bool PendingCallbackScheduled(OrbiterCallBack fnCallBack);

		//Returns true if Orbiter's config file has preseeded install values.
		bool IsSelfInstallable();

		//Used to redirect game media types to MEDIATYPE_LMCE_Game_CONST
		int PreprocessMediaType(int iPK_MediaType);

		//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Path();
	int DATA_Get_PK_Users();
	string DATA_Get_Current_Screen();
	void DATA_Set_Current_Screen(string Value,bool bUpdateDatabase=false);
	int DATA_Get_PK_Distro();
	bool DATA_Get_Development();
	bool DATA_Get_No_Effects();
	int DATA_Get_PK_Skin();
	int DATA_Get_PK_Size();
	int DATA_Get_PK_Language();
	string DATA_Get_FK_EntertainArea();
	string DATA_Get_Update_Name();
	string DATA_Get_Communication_file();
	string DATA_Get_Timeout();
	string DATA_Get_CacheFolder();
	int DATA_Get_CacheSize();
	bool DATA_Get_Use_OCG_Format();
	int DATA_Get_VideoFrameInterval();
	int DATA_Get_ImageQuality();
	void DATA_Set_ImageQuality(int Value,bool bUpdateDatabase=false);
	bool DATA_Get_Leave_Monitor_on_for_OSD();
	bool DATA_Get_Ignore();
	bool DATA_Get_Dont_Auto_Jump_to_Remote();
	int DATA_Get_ScreenWidth();
	int DATA_Get_ScreenHeight();
	int DATA_Get_Rotation();
	int DATA_Get_PK_UI();
	string DATA_Get_Hard_Keys_mapping();
	int DATA_Get_Using_Infrared();
	string DATA_Get_Remote_Phone_IP();
	int DATA_Get_Listen_Port();
	int DATA_Get_PK_Screen();
	string DATA_Get_Spacing();
	void DATA_Set_Spacing(string Value,bool bUpdateDatabase=false);
	string DATA_Get_Offset();
	void DATA_Set_Offset(string Value,bool bUpdateDatabase=false);
	bool DATA_Get_Get_Time_Code_for_Media();
	string DATA_Get_Shortcut();
	bool DATA_Get_Expert_Mode();
	bool DATA_Get_Enable_Memory_Management();
	int DATA_Get_Border_Size();
	string DATA_Get_Alert_Filter_Level();
	bool DATA_Get_Ignore_First_Event();
	void DATA_Set_Ignore_First_Event(bool Value,bool bUpdateDatabase=false);
	bool DATA_Get_Automatically_Go_to_Remote();
	bool DATA_Get_Queue_Instead_of_Instant_Play();
	void DATA_Set_Queue_Instead_of_Instant_Play(bool Value,bool bUpdateDatabase=false);

			*****EVENT***** accessors inherited from base class
	void EVENT_Touch_or_click(int iX_Position,int iY_Position);

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { string sCMD_Result; CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,NULL);};
	virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #2 - Orbiter Beep */
	/** Make the orbiter beep */

	virtual void CMD_Orbiter_Beep() { string sCMD_Result; CMD_Orbiter_Beep(sCMD_Result,NULL);};
	virtual void CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #3 - Display On/Off */
	/** Turn the display on or off */
		/** @param #8 On/Off */
			/** 0=Off, 1=On */
		/** @param #125 Already processed */
			/** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

	virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed) { string sCMD_Result; CMD_Display_OnOff(sOnOff.c_str(),bAlready_processed,sCMD_Result,NULL);};
	virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #4 - Go back */
	/** Make the orbiter go back to the prior screen, like the back button in a web browser */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** If this is specified, the orbiter will ignore the command unless this is the current screen */
		/** @param #21 Force */
			/** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce) { string sCMD_Result; CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { string sCMD_Result; CMD_Goto_DesignObj(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,bCant_Go_Back,sCMD_Result,NULL);};
	virtual void CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { string sCMD_Result; CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #7 - Terminate Orbiter */
	/** Causes the Orbiter application to exit */

	virtual void CMD_Terminate_Orbiter() { string sCMD_Result; CMD_Terminate_Orbiter(sCMD_Result,NULL);};
	virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #8 - Remove Screen From History */
	/** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
		/** @param #10 ID */
			/** If specified, only screens that match this ID will be removed */
		/** @param #159 PK_Screen */
			/** The screen to remove */

	virtual void CMD_Remove_Screen_From_History(string sID,int iPK_Screen) { string sCMD_Result; CMD_Remove_Screen_From_History(sID.c_str(),iPK_Screen,sCMD_Result,NULL);};
	virtual void CMD_Remove_Screen_From_History(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #9 - Scroll Grid */
	/** Scroll a datagrid */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to scroll the grid */

	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #10 - Move Highlight */
	/** Move the current highlight pointer */
		/** @param #1 Relative Level */
			/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
		/** @param #3 PK_DesignObj */
			/** The grid to scroll.  If not specified, any currently visible grids will scroll */
		/** @param #30 PK_Direction */
			/** The direction to move the highlight */

	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
	virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #13 - Play Sound */
	/** Plays a sound file on the orbiter */
		/** @param #19 Data */
			/** A pointer to a block of memory representing the sound file to play */
		/** @param #20 Format */
			/** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat) { string sCMD_Result; CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #14 - Refresh */
	/** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
		/** @param #15 DataGrid ID */
			/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

	virtual void CMD_Refresh(string sDataGrid_ID) { string sCMD_Result; CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #15 - Regen Screen */
	/** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

	virtual void CMD_Regen_Screen() { string sCMD_Result; CMD_Regen_Screen(sCMD_Result,NULL);};
	virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #16 - Requires Special Handling */
	/** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

	virtual void CMD_Requires_Special_Handling() { string sCMD_Result; CMD_Requires_Special_Handling(sCMD_Result,NULL);};
	virtual void CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { string sCMD_Result; CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #18 - Set Graphic To Display */
	/** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
		/** @param #3 PK_DesignObj */
			/** The object to set */
		/** @param #10 ID */
			/** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #20 - Set Object Parameter */
	/** changes one of the object's DesignObjParameters */
		/** @param #3 PK_DesignObj */
			/** The object to change */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #27 PK_DesignObjParameter */
			/** The parameter */

	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { string sCMD_Result; CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #21 - Set Object Position */
	/** Change an objects's position on the screen */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #22 - Set Object Size */
	/** Change an object's size */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #23 - Set Pos Rel To Parent */
	/** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/**  */
		/** @param #12 Position Y */
			/**  */

	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #24 - Set Size Rel To Parent */
	/** Change an object's size, relative to it's parent object */
		/** @param #3 PK_DesignObj */
			/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #11 Position X */
			/** The percentage of the parent object's width.  100=the parent's full width. */
		/** @param #12 Position Y */
			/** The percentage of the parent object's height.  100=the parent's full height. */

	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #25 - Set Text */
	/** Change the text within a text object on the fly */
		/** @param #3 PK_DesignObj */
			/** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
		/** @param #9 Text */
			/** The text to assign */
		/** @param #25 PK_Text */
			/** The text object in which to store the current input */

	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text) { string sCMD_Result; CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,NULL);};
	virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #26 - Set Bound Icon */
	/** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
		/** @param #5 Value To Assign */
			/** The value corresponding to an alt graphic. */
		/** @param #9 Text */
			/** Text for the binding */
		/** @param #14 Type */
			/** The type of bound icon. */

	virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType) { string sCMD_Result; CMD_Set_Bound_Icon(sValue_To_Assign.c_str(),sText.c_str(),sType.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #27 - Set Variable */
	/** Change the value of a variable */
		/** @param #4 PK_Variable */
			/** The variable to change */
		/** @param #5 Value To Assign */
			/** The value to assign */

	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign) { string sCMD_Result; CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #29 - Simulate Mouse Click */
	/** Simulates a mouse click or touch on the indicated x & y coordinates */
		/** @param #11 Position X */
			/** position X */
		/** @param #12 Position Y */
			/** position Y */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #30 - Stop Sound */
	/** If a sound file is being played, it will be stopped. */

	virtual void CMD_Stop_Sound() { string sCMD_Result; CMD_Stop_Sound(sCMD_Result,NULL);};
	virtual void CMD_Stop_Sound(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #31 - Store Variables */
	/** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

	virtual void CMD_Store_Variables() { string sCMD_Result; CMD_Store_Variables(sCMD_Result,NULL);};
	virtual void CMD_Store_Variables(string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { string sCMD_Result; CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #58 - Set Current User */
	/** Sets what user is currently using the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

	virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** If you don't know the location, you can also set just the entertainment area */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #66 - Select Object */
	/** The same as clicking on an object. */
		/** @param #3 PK_DesignObj */
			/** The object to select. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** Will only happen if this is the current screen. */
		/** @param #102 Time */
			/** If specified, rather than happening immediately it will happen in x seconds. */

	virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { string sCMD_Result; CMD_Select_Object(sPK_DesignObj.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach) { string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),sSendOnSuccess.c_str(),bShow_logo,bRetransmit,bExclusive,bDetach,sCMD_Result,NULL);};
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #72 - Surrender to OS */
	/** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
		/** @param #8 On/Off */
			/** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
		/** @param #54 Fully release keyboard */
			/** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys.  True also releases the mouse. */
		/** @param #225 Always */
			/** If true, the mouse will always be ignored */

	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways) { string sCMD_Result; CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,bAlways,sCMD_Result,NULL);};
	virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #77 - Set Current Room */
	/** If you don't know the location, you can also set just the room */
		/** @param #57 PK_Room */
			/** The room */

	virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #85 - Reset Highlight */
	/** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */
		/** @param #3 PK_DesignObj */
			/** If specified, this object will be highlighted.  Otherwise the first detected one. */

	virtual void CMD_Reset_Highlight(string sPK_DesignObj) { string sCMD_Result; CMD_Reset_Highlight(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset_Highlight(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #88 - Set Current Location */
	/** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
		/** @param #65 LocationID */
			/** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

	virtual void CMD_Set_Current_Location(int iLocationID) { string sCMD_Result; CMD_Set_Current_Location(iLocationID,sCMD_Result,NULL);};
	virtual void CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #126 - Guide */
	/** Go to the media guide if applicable, otherwise forward to media plugin. */

	virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
	virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Select the highlighted item */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #192 - On */
	/** Turn the device on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn the device off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #194 - Toggle Power */
	/** First time stops any media playing.  Second time turns the display off.  Third time powers off the media director. */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

	virtual void CMD_Toggle_Power(string sOnOff) { string sCMD_Result; CMD_Toggle_Power(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlight up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlight down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlight left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlight right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #238 - Continuous Refresh */
	/** Continuously refresh the current page.  Used when the page contains constantly changing data. */
		/** @param #102 Time */
			/** The interval time in seconds */

	virtual void CMD_Continuous_Refresh(string sTime) { string sCMD_Result; CMD_Continuous_Refresh(sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** If at a remote control, forward to media plugin.  Otherwise go back 1 screen. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit) { string sCMD_Result; CMD_Set_Now_Playing(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),sText.c_str(),iPK_MediaType,iStreamID,iValue,sName.c_str(),sList_PK_Device.c_str(),bRetransmit,sCMD_Result,NULL);};
	virtual void CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #254 - Bind Icon */
	/** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
		/** @param #3 PK_DesignObj */
			/** The object which contains the icon, or whose child objects contain the icon. */
		/** @param #14 Type */
			/** The type of binding, like "housemode", "userstatus_39288", etc. */
		/** @param #104 Child */
			/** If true, it will set the property for the child object(s), rather than the designated object. */

	virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild) { string sCMD_Result; CMD_Bind_Icon(sPK_DesignObj.c_str(),sType.c_str(),bChild,sCMD_Result,NULL);};
	virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #258 - Clear Selected Devices */
	/** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
		/** @param #3 PK_DesignObj */
			/** If specified, the object referenced here will be invalidated and redrawn. */

	virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj) { string sCMD_Result; CMD_Clear_Selected_Devices(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #260 - Set Main Menu */
	/** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
		/** @param #9 Text */
			/** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

	virtual void CMD_Set_Main_Menu(string sText) { string sCMD_Result; CMD_Set_Main_Menu(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #265 - Quit */
	/** Exits the orbiter application */

	virtual void CMD_Quit() { string sCMD_Result; CMD_Quit(sCMD_Result,NULL);};
	virtual void CMD_Quit(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #324 - Set Timeout */
	/** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
		/** @param #3 PK_DesignObj */
			/** The screen to set the timeout on.  If blank the current screen. */
		/** @param #102 Time */
			/** The timeout in seconds.  0 or blank means no timeout. */

	virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime) { string sCMD_Result; CMD_Set_Timeout(sPK_DesignObj.c_str(),sTime.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #325 - Keep Screen On */
	/** Allow or don't allow the screen to blank with the screen saver. */
		/** @param #8 On/Off */
			/** If other than "0", the screen saver will be disabled. */

	virtual void CMD_Keep_Screen_On(string sOnOff) { string sCMD_Result; CMD_Keep_Screen_On(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
	/** Positions the on-screen mouse pointer centered over a certain object */
		/** @param #3 PK_DesignObj */
			/** The object to center the mouse over. */

	virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj) { string sCMD_Result; CMD_Set_Mouse_Pointer_Over_Object(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #354 - Show Mouse Pointer */
	/** Shows or hides the mouse pointer. */
		/** @param #8 On/Off */
			/** 1=show it, 0=hide it. */

	virtual void CMD_Show_Mouse_Pointer(string sOnOff) { string sCMD_Result; CMD_Show_Mouse_Pointer(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #363 - Back / Clear Entry */
	/** If at a remote control, forward to media plugin.  Otherwise clear typing */

	virtual void CMD_Back_Clear_Entry() { string sCMD_Result; CMD_Back_Clear_Entry(sCMD_Result,NULL);};
	virtual void CMD_Back_Clear_Entry(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #366 - Activate Window */
	/** ActivateApplication - Used by Linux On Screeen Orbiters only */
		/** @param #50 Name */
			/** Name as known by ratpoison. */

	virtual void CMD_Activate_Window(string sName) { string sCMD_Result; CMD_Activate_Window(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #389 - Send Message */
	/** Sends a message stored in a parameter as a text object. */
		/** @param #9 Text */
			/** The message in command line-style format */
		/** @param #144 Go Back */
			/** Go back after sending the command if it does not contain another goto screen or go back */

	virtual void CMD_Send_Message(string sText,bool bGo_Back) { string sCMD_Result; CMD_Send_Message(sText.c_str(),bGo_Back,sCMD_Result,NULL);};
	virtual void CMD_Send_Message(string sText,bool bGo_Back,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { string sCMD_Result; CMD_Show_Popup(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),bExclusive,bDont_Auto_Hide,sCMD_Result,NULL);};
	virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #398 - Remove Popup */
	/** Hides a popup. */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** (optional).  The screen on which it's a local popup */
		/** @param #50 Name */
			/** The name of the popup.  If not specified all popups will be removed */

	virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName) { string sCMD_Result; CMD_Remove_Popup(sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #399 - Show Shortcuts */
	/** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

	virtual void CMD_Show_Shortcuts() { string sCMD_Result; CMD_Show_Shortcuts(sCMD_Result,NULL);};
	virtual void CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #401 - Show File List */
	/** Shows the file list */
		/** @param #29 PK_MediaType */
			/** The type of media the user wants to browse. */

	virtual void CMD_Show_File_List(int iPK_MediaType) { string sCMD_Result; CMD_Show_File_List(iPK_MediaType,sCMD_Result,NULL);};
	virtual void CMD_Show_File_List(int iPK_MediaType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #402 - Use Popup Remote Controls */
	/** If this command is executed the remote controls will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen on which to put the popup */

	virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_Remote_Controls(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #403 - Use Popup File List */
	/** If this command is executed the file lists will be displayed as popups. */
		/** @param #11 Position X */
			/** The location of the popup */
		/** @param #12 Position Y */
			/** The location of the popup */
		/** @param #16 PK_DesignObj_CurrentScreen */
			/** The screen to put the popup on */

	virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_File_List(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #405 - Scale this object */
	/** If you add this command to the startup list of an object it will cause Orbiter Gen to scale this object and all it's children. */
		/** @param #48 Value */
			/** The value to scale to.  100=full size, 50=half size */

	virtual void CMD_Scale_this_object(int iValue) { string sCMD_Result; CMD_Scale_this_object(iValue,sCMD_Result,NULL);};
	virtual void CMD_Scale_this_object(int iValue,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse) { string sCMD_Result; CMD_Set_Floorplan(sPK_DesignObj.c_str(),sType.c_str(),bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #408 - Show Floorplan */
	/** Shows the floorplan */
		/** @param #11 Position X */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #12 Position Y */
			/** If the floorplan is not full screen, the location where it should be displayed */
		/** @param #14 Type */
			/** The type of floorplan */

	virtual void CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType) { string sCMD_Result; CMD_Show_Floorplan(iPosition_X,iPosition_Y,sType.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #413 - Forward local k/b to OSD */
	/** Means this orbiter's keyboard should be controlling the application running on the media director. */
		/** @param #119 True/False */
			/** If 1, do it.  It 0, stop */

	virtual void CMD_Forward_local_kb_to_OSD(bool bTrueFalse) { string sCMD_Result; CMD_Forward_local_kb_to_OSD(bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Forward_local_kb_to_OSD(bool bTrueFalse,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #415 - Set Mouse Position Relative */
	/** Move the mouse relative to its current position */
		/** @param #11 Position X */
			/** The X Position to move */
		/** @param #12 Position Y */
			/** The Y Position to move */

	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Mouse_Position_Relative(iPosition_X,iPosition_Y,sCMD_Result,NULL);};
	virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #416 - Simulate Mouse Click At Present Pos */
	/** Simulates clicking the mouse button */
		/** @param #14 Type */
			/** If L or empty, the left button.  If R the right button. */

	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType) { string sCMD_Result; CMD_Simulate_Mouse_Click_At_Present_Pos(sType.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #548 - Menu */
	/** Shows the main menu */
		/** @param #9 Text */
			/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Menu(string sText,int iStreamID) { string sCMD_Result; CMD_Menu(sText.c_str(),iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { string sCMD_Result; CMD_Update_Time_Code(iStreamID,sTime.c_str(),sTotal.c_str(),sSpeed.c_str(),sTitle.c_str(),sSection.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue) { string sCMD_Result; CMD_Goto_Screen(sID.c_str(),iPK_Screen,iInterruption,bTurn_On,bQueue,sCMD_Result,NULL);};
	virtual void CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection) { string sCMD_Result; CMD_Set_Mouse_Behavior(sPK_DesignObj.c_str(),sOptions.c_str(),bExclusive,sDirection.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #801 - Set Mouse Sensitivity */
	/** 1=Sensitive, 3=not sensitive */
		/** @param #48 Value */
			/** 1=sensitive, 3=least sensitive */

	virtual void CMD_Set_Mouse_Sensitivity(int iValue) { string sCMD_Result; CMD_Set_Mouse_Sensitivity(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Mouse_Sensitivity(int iValue,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption) { string sCMD_Result; CMD_Display_Alert(sText.c_str(),sTokens.c_str(),sTimeout.c_str(),iInterruption,sCMD_Result,NULL);};
	virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo) { string sCMD_Result; CMD_Set_Active_Application(sName.c_str(),iPK_Screen,sIdentifier.c_str(),iPK_Screen_GoTo,sCMD_Result,NULL);};
	virtual void CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo) { string sCMD_Result; CMD_Get_Active_Application(sName,iPK_Screen,sIdentifier,iPK_Screen_GoTo,sCMD_Result,NULL);};
	virtual void CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #834 - Execute Shortcut */
	/** Execute the shortcut associated with a key.  Called when a key is held down. */
		/** @param #48 Value */
			/** The ascii value of the key (ie 65='A').  Valid are 0-9,*,#,A-Z */

	virtual void CMD_Execute_Shortcut(int iValue) { string sCMD_Result; CMD_Execute_Shortcut(iValue,sCMD_Result,NULL);};
	virtual void CMD_Execute_Shortcut(int iValue,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #838 - Bind to Wireless Keyboard */
	/** If the USB RF dongle is attached it causes it to do a bind request and add a remote. */

	virtual void CMD_Bind_to_Wireless_Keyboard() { string sCMD_Result; CMD_Bind_to_Wireless_Keyboard(sCMD_Result,NULL);};
	virtual void CMD_Bind_to_Wireless_Keyboard(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #912 - Activate PC Desktop */
	/** Activate or de-activate the PC-desktop */
		/** @param #119 True/False */
			/** If true, switch to the last PC desktop.  If false, switch to Orbiter's desktop */

	virtual void CMD_Activate_PC_Desktop(bool bTrueFalse) { string sCMD_Result; CMD_Activate_PC_Desktop(bTrueFalse,sCMD_Result,NULL);};
	virtual void CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To) { string sCMD_Result; CMD_Assisted_Make_Call(iPK_Users,sPhoneExtension.c_str(),sPK_Device_From.c_str(),iPK_Device_To,sCMD_Result,NULL);};
	virtual void CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #975 - XPromptReload */
	/** This command displays a message using custom drawn X primitives and asks the user if they want to reload the router. */
		/** @param #9 Text */
			/** Text to display on the screen */

	virtual void CMD_XPromptReload(string sText) { string sCMD_Result; CMD_XPromptReload(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_XPromptReload(string sText,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

		/** temporary solution only */


		/**
		* @brief calculates the new current row after the specified number of cells have been skiped (upwards)
		* @todo ask
		*/
	};

	extern int g_iDontRender; /** < @todo ask */

	class NeedToRender
	{
		class Orbiter *m_pOrbiter; /** <  */
		const char *m_pWhere; /** <  */
		static class ScreenHistory *m_pScreenHistory;

	public:

		NeedToRender( class Orbiter *pOrbiter, const char *pWhere );
		~NeedToRender();

		static void NeedToChangeScreens( class Orbiter *pOrbiter, class ScreenHistory *pScreenHistory);
		static class ScreenHistory *m_pScreenHistory_get();
	};

	//------------------------------------------------------------------------
	// CallBackInfo
	//------------------------------------------------------------------------
	// Stuff the callback routines need
	class PendingCallBackInfo
	{
	public:
		PendingCallBackInfo(unsigned int nCallbackID, bool bPurgeTaskWhenScreenIsChanged = true)
		{
			m_nCallbackID = nCallbackID;
			m_bStop = false;
			m_bPurgeTaskWhenScreenIsChanged = bPurgeTaskWhenScreenIsChanged;
		}

		Orbiter *m_pOrbiter;
		OrbiterCallBack m_fnCallBack;
		timespec m_abstime;
		void *m_pData;
		unsigned int m_nCallbackID; // A unique ID
		bool m_bStop; // Don't execute after all, we've decided to stop it (probably started another one of same type)
		bool m_bPurgeTaskWhenScreenIsChanged;
	};

	//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
