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
#ifndef __MouseBehavior_H__
#define __MouseBehavior_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
using namespace DCE;

#define AXIS_LOCK_NONE	0
#define AXIS_LOCK_X		1
#define AXIS_LOCK_Y		2
#define AXIS_LOCK_BOTH	3

class OSDScreenHandler;

namespace DCE
{
	class MouseBehavior;
	
	// A pure virtual base class for the call-backs
	class MouseHandler
	{
		friend class DCE::MouseBehavior;
		friend class DatagridMouseHandlerHelper;

	public:
		int m_iLastNotch,m_iCancelLevel;  // Notch = last position on a locked vert/horiz bar.  CancelLevel = if doing discrete movement, cancel returns to this value
		string m_sCancelLevel; // Same as m_i for cases where a string is better
		bool m_bTapAndRelease; // The user is adjusting to a discrete/absolute level
		bool m_bLockAxis; // True, the default, means the framework will report Move's with the locked axis fixed over the locked object
		DesignObj_Orbiter *m_pObj;
		MouseBehavior *m_pMouseBehavior;
		bool m_bStartedMovement;  // Means the user has started moving
		string m_sOptions;

		MouseHandler(DesignObj_Orbiter *pObj, string sOptions, MouseBehavior *pMouseBehavior);
		virtual ~MouseHandler();


		virtual void Start() {}  // This is now active (ie the user started moving in the direction this is locked onto)
		virtual void Stop() {}  // The user is now moving in another direction using a different mouse handler

		virtual bool ButtonDown(int PK_Button) { return false; }     // Return true means don't process this anymore
		virtual bool ButtonUp(int PK_Button) { return false; }   // Return true means don't process this anymore
		virtual void Move(int X,int Y,int PK_Direction) {}
		virtual void RelativeMove(int DeltaX, int DeltaY) {}
		virtual void Notch(int PK_Direction,int iRepeat) {} // The user moved a 'notch' in the given direction
		virtual bool MovedOutside(int PK_Direction) { return false; } // Override this and return true if you don't want the framework to automatically reposition the pointer inside the object
		virtual bool SlowDrift(int &X,int &Y) { return false; } // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move
#ifndef WIN32
		typedef
#endif
		enum EMouseHandler { mh_Locked, mh_Speed, mh_Light, mh_Volume, mh_Media, mh_Keyboard, mh_MediaBrowser, mh_HorizMenu };
		virtual EMouseHandler TypeOfMouseHandler()=0;
	};

	#define	NUM_SAMPLES		30
	class MouseSensitivity
	{
	public:
		int SampleInterval;  
		int DiscardSamplesOlderThanSec; 
		// Threshold 1 represents the fastest, most clear movement, requiring a high ratio, for users that are very precise
		int Threshhold_1_Minimum;  // Minimum number of pixels to move
		int Threshhold_1_Ratio; // Minimum ratio : 100=baseline, 200=twice as much on the directional axis as the non-directional, 500=5x as much
		int Threshhold_2_Minimum;
		int Threshhold_2_Ratio;
		int Threshhold_2_Samples; // How many samples must be taken before the ratio/minimum's are lowered to threshhold 2
		int Threshhold_3_Minimum;
		int Threshhold_3_Ratio;
		int Threshhold_3_Samples;

		int IgnoreMovesLessThanThisPerSample;  // This is considered drift, not move
		int NumNotchesForExternalApp; // Determines sensitivity when sending move commands to an external app

		MouseSensitivity() { SetSensitivity(2); }
		void SetSensitivity(int Level); // 1=most sensitive, 3=least sensitive
	};

	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior, such as locking axis's, ramping and acceleration, etc.
	*/
	//-----------------------------------------------------------------------------------------------------
	class MouseBehavior
	{
		friend class Orbiter;
		friend class ScreenHandler;
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
		friend class ::OSDScreenHandler;
		

	protected:
		Orbiter *m_pOrbiter;
		class MouseGovernor *m_pMouseGovernor;
		class MouseIterator *m_pMouseIterator;
		MouseSensitivity m_MouseSensitivity;
		DesignObj_Orbiter *m_pObj_Locked;
		MouseHandler *m_pMouseHandler;
	    unsigned long m_dwSamples[NUM_SAMPLES];
		unsigned long m_dwLastSampleShift;
		PlutoPoint m_pSamples[NUM_SAMPLES];
		PlutoPoint m_pLastPosition;
		PlutoPoint m_pStartMovement;
		PlutoRectangle m_rMouseConstrained;
		bool m_bMouseKilled; // If true, the user wants to reposition the mouse without moving the cursor
		string m_sOptions;
		int m_iPK_Button_Mouse_Last; // The last mouse button
		unsigned long m_iTime_Last_Mouse_Down,m_iTime_Last_Mouse_Up; // When it was pressed
		unsigned long m_dwTime_Last_Notch;
		bool m_bMouseConstrained,m_bMouseVisible;
		bool m_bHasTimeline; // True if the last set media position indicated we could directly set media position (ie a timeline)
		DesignObj_Orbiter *m_pObj_Previously_Highlighted;

		// After changing the OSD with UI2 it may be a couple seconds before the media player settles down and we don't want
		// it displaying speed bars until then
		time_t m_tIgnoreSpeedChangesUntil;

	public:
		typedef enum { mcs_Normal, mcs_LeftRight, mcs_UpDown, mcs_AnyDirection, mcs_LeftRightUpDown } MouseCursorStyle;
		typedef enum { smb_Default, smb_TurnOffRemote, smb_TurnOnRemote, smb_LeaveRemoteUnchanged } SetMouseBehaviorRemote;
		MouseBehavior(Orbiter *pOrbiter);
		virtual ~MouseBehavior();

		void Set_Mouse_Behavior(string sOptions,bool bExclusive,string sDirection,string sDesignObj);
		bool ButtonDown(int PK_Button);     // Return true means don't process this anymore
		bool ButtonUp(int PK_Button);   // Return true means don't process this anymore
		void Move(int X,int Y);
		void RelativeMove(int DeltaX, int DeltaY);

		// If pObj_Furthest is specified, then whether or not there is an exact match, the object furthest in the PK_Direction will be assigned to it
		DesignObj_Orbiter *FindChildObjectAtPosition(DesignObj_Orbiter *pObj_Parent,int X,int Y,DesignObj_Orbiter **pObj_Furthest=NULL, int PK_Direction=0);

		void ResetSamples() { for(int i=0;i<NUM_SAMPLES;++i) m_dwSamples[i]=0; }
		void ShiftSamples() { for(int i=NUM_SAMPLES-2;i>=0;--i) { m_dwSamples[i+1]=m_dwSamples[i]; m_pSamples[i+1]=m_pSamples[i]; } m_pSamples[0].X=m_pSamples[0].Y=0; m_dwSamples[0]=0; }
		void Clear(bool bGotoMainMenu=false);
		void HighlightObject(DesignObj_Orbiter *pObj);
		PlutoRectangle GetHighlighedObjectCoordinates();
		void SelectFirstObject(char cDirection='1',DesignObj_Orbiter *pObj_Parent=NULL);
		int GetDirectionAwayFromHighlight(int X,int Y);
		void PositionMouseAtObjectEdge(int PK_Direction);

		// Override these for OS specific handling
		virtual void SetMousePosition(int X,int Y) { m_pLastPosition.X=X; m_pLastPosition.Y=Y; /*LoggerWrapper::GetInstance()->Write(LV_FESTIVAL,"SetMousePosition %d,%d",X,Y);*/ }
		virtual void SetMousePosition(DesignObj_Orbiter *pObj) { SetMousePosition( pObj->m_rPosition.X + pObj->m_pPopupPoint.X + pObj->m_rPosition.Width/2 , pObj->m_rPosition.Y + pObj->m_pPopupPoint.Y + pObj->m_rPosition.Height/2); }
		virtual void GetMousePosition(PlutoPoint *p) { *p = m_pLastPosition; }
		virtual void ShowMouse(bool bShow, SetMouseBehaviorRemote setMouseBehaviorRemote=smb_Default) { }
		void m_pLastPosition_set(int X,int Y) { m_pLastPosition.X=X; m_pLastPosition.Y=Y; }

		void SetMediaInfo(string sTime,string sTotal,string sSpeed,string sTitle,string sSection,bool bHasTimeline);
		void MediaStopped();

		const MouseHandler* GetMouseHandler() const;

        // have a platform-specific implementation
        virtual bool ConstrainMouse(const PlutoRectangle &rect) { return false; }
        virtual bool ConstrainMouse() { PlutoRectangle rect; return ConstrainMouse(rect); }
		virtual void SetMouseCursorStyle(MouseCursorStyle mouseCursorStyle) {}
		virtual bool SetMouseCursorImage(const string &sPath, const string &sPathMask) { return false;}
		virtual void CaptureRelativeMovements();
		virtual void ReleaseRelativeMovements();
    };

}
//-----------------------------------------------------------------------------------------------------
#endif
