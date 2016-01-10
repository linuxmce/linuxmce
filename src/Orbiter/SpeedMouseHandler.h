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
#ifndef __SpeedMouseHandler_H__
#define __SpeedMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
using namespace DCE;

#define MAX_SPEEDS		17

#define SPEED_STYLE_TIMELINE_IDLE	1
#define SPEED_STYLE_TIMELINE_SPEED	2
#define SPEED_STYLE_TIMELINE_SEEK	3
#define SPEED_STYLE_SPEED_ONLY		4

// One screen has a popup object for both the relative and absolute positioning, alternate versions of the same object
// CMD_Update_Time_Code will call m_pMouseBehavior->SetMediaInfo if there is a time received

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class SpeedMouseHandler : public MouseHandler
	{
		friend class MouseBehavior;

		static const int m_iSpeeds[MAX_SPEEDS];
		int m_CurrentMedia_Start,m_CurrentMedia_Stop,m_CurrentMedia_Pos;  // The start and stop values in seconds of the media, and where we are now
		int m_iLastGoodPosition;

	public:
		SpeedMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		~SpeedMouseHandler();
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Speed; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		bool SlowDrift(int &X,int &Y);  // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move

		bool ParsePosition(string sMediaPosition);

		void DrawInfo();
		string FormatTime(int Time);
		int ParseTime(string sTime);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
